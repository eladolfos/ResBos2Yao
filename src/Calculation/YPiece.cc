#include "ResBos/Brent.hh"
#include "ResBos/Grid2D.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/loguru.hpp"
#include "ResBos/Process.hh"
#include "ResBos/ProgressBar.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Utility.hh"
#include "ResBos/YPiece.hh"

#include <chrono>

namespace ResBos{
    using namespace std::placeholders;
    REGISTER_CALC(YPiece);

    // Initialize the YPiece class
    void YPiece::Initialize(IO::Settings *s, std::shared_ptr<ResBos> r) {
        gType = GridType::Y;
        needAsym = false;
        needPert = false;

        // Initialize needed params for Asymptotic and Perturbative
        SetAsymOrder(s -> GetSettingInt("AsymOrder"));
        SetPertOrder(s -> GetSettingInt("PertOrder"));
        SetMuF(s -> GetSettingDouble("muF"));
        SetMuR(s -> GetSettingDouble("muR"));

        Calculation::Initialize(s,r);
    }

    bool YPiece::LoadGrid() {
        // Check for Asym grid
        gType = GridType::Asym;
        if(Calculation::LoadGrid()) gridAsym = grid;
        else {
            needAsym = true;
        }

        // Check for Pert grid
        gType = GridType::Pert;
        if(Calculation::LoadGrid()) gridPert = grid;
        else {
            needPert = true;
        }

        // Change grid type back, and return false if one grid is needed
        gType = GridType::Y;
        return !(needAsym || needPert); 
    }

    bool YPiece::SaveGrid() {
        // Save Asym grid
        gType = GridType::Asym;
        grid = gridAsym;
        bool asymSave = Calculation::SaveGrid();

        // Save Pert grid
        gType = GridType::Pert;
        grid = gridPert;
        bool pertSave = Calculation::SaveGrid();

        // Change back to YPiece grid type
        gType = GridType::Y;
        return asymSave && pertSave;
    }

    std::vector<Conv> YPiece::GetNeededConvs() const{
        std::vector<Conv> neededGrids;
        if(GetAsymOrder()>1) {
            neededGrids.push_back(Conv::C1);
            neededGrids.push_back(Conv::C1P1);
            if(GetAsymOrder()>2) {
                neededGrids.push_back(Conv::C2);
                neededGrids.push_back(Conv::C1P1P1);
                neededGrids.push_back(Conv::C1P2);
            }
        }
        return neededGrids;
    }

    void YPiece::GridGen() {
        if(qTMin == 0) qTMin += 1E-1;
        int nQT = 10*log(qTMax/qTMin)+3;
        int nY = (yMax-yMin)/0.1+1;

        double QVal = qMin;
        while(QVal <= qMax) { 
            qVals.push_back(QVal);
            if(QVal < 200) QVal += 10;
            else if(QVal < 500) QVal += 50;
            else if(QVal < 1000) QVal += 100;
            else QVal += 500;
        } 
        for(int i = 0; i < nQT; i++) qTVals.push_back(qTMax*exp(-0.1*(nQT-i-1)));
        for(int i = 0; i < nY; i++) yVals.push_back(yMin+i*0.1);

        if(needAsym) {
            std::vector<double> test = GetAsymptotic(qVals[0],qTVals[0],yVals[0]);
            for(size_t i = 0; i < test.size(); i++) {
                Utility::Grid3D *g = new Utility::Grid3D(qVals,yVals,qTVals);
                gridAsym[i] = g;
            }
        }

        if(needPert) {
            std::vector<double> test = GetPerturbative(qVals[0],qTVals[0],yVals[0]);
            for(size_t i = 0; i < test.size(); i++) {
                Utility::Grid3D *g = new Utility::Grid3D(qVals,yVals,qTVals);
                gridPert[i] = g;
            }
        }

        std::cout << "Generating the grid for the calculation: " << std::endl;
        LOG_F(INFO,"Begin generating calculation grid");
        ProgressBar pb(qVals.size()*qTVals.size()*yVals.size());
        for(auto q : qVals) {
            for(auto y : yVals) {
                for(auto qT : qTVals) {
                    pb.Update(1);
                    pb.Display();
                    if(needAsym) {
                        std::vector<double> resultAsym = GetAsymptotic(q,qT,y);
                        for(size_t i = 0; i < resultAsym.size(); i++) {
                            gridAsym[i] -> AddPoint(q,y,qT,resultAsym[i]);
                        }
                    }
                    if(needPert) {
                        std::vector<double> resultPert = GetPerturbative(q,qT,y);
                        for(size_t i = 0; i < resultPert.size(); i++) {
                            gridPert[i] -> AddPoint(q,y,qT,resultPert[i]);
                        }
                    }
                }
            }
        }
        pb.Done();
        LOG_F(INFO,"Finished generating calculation grid");
    }

    std::vector<double> YPiece::GetPoint(double Q, double qt, double y) {
        std::vector<double> result(gridPert.size());
        const size_t nAngular = resbos -> GetProcess() -> GetNAngular();
        const size_t nChannels = resbos -> GetProcess() -> GetChannels(); 
        const size_t nAsym = gridAsym.size()/nChannels;
        for(size_t i = 0; i < nChannels; ++i) {
            for(size_t j = 0; j < nAsym; ++j) {
                if(qt > 1) result[i*nAngular + j] = -gridAsym[i*nAsym + j] -> Interpolate(Q,y,qt);
            }
            for(size_t j = 0; j < nAngular; ++j) {
                if(qt > 1) result[i*nAngular+j] += gridPert[i*nAngular+j] -> Interpolate(Q,y,qt);
            }
        }

        return result;
    }

    std::vector<double> YPiece::GetCalc(double Q, double qt, double y) {
        if(qt > 1) {
            std::vector<double> asym = GetAsymptotic(Q,qt,y);
            std::vector<double> pert = GetPerturbative(Q,qt,y);
            std::vector<double> result(pert.size());
            for(size_t i = 0; i < pert.size(); ++i) {
                result[i] = pert[i] - asym[i];
            }
            return result;
        }
        return std::vector<double>(resbos->GetProcess()->GetNAngular()*resbos->GetProcess()->GetChannels(),0);
    }

    std::vector<double> YPiece::GetAsymptotic(double Q, double qt, double y) {
        return Asymptotic::GetCalc(Q,qt,y);
    }

    std::vector<double> YPiece::GetPerturbative(double Q, double qt, double y) {
        return Perturbative::GetCalc(Q,qt,y);
    }
}
