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
#include "ResBos/WmA.hh"

#include <chrono>

namespace ResBos{
    using namespace std::placeholders;
    REGISTER_CALC(WmA)

    // Initialize the WmA class
    void WmA::Initialize(IO::Settings *s, std::shared_ptr<ResBos> r) {
        gType = GridType::WmA;
        needResum = false;
        needAsym = false;

        // Initialize needed params for Resummation
        SetAOrder(s->GetSettingSizet("AOrder"));
        SetBOrder(s->GetSettingSizet("BOrder"));
        SetCOrder(s->GetSettingSizet("COrder"));
        SetHOrder(s->GetSettingSizet("HOrder"));
        Q0 = s->GetSettingDouble("Q0");
        bMax = s->GetSettingDouble("bMax");
        SetC1(s->GetSettingDouble("C1")*QCD::B0);
        SetC2(s->GetSettingDouble("C2"));
        SetC3(s->GetSettingDouble("C3")*QCD::B0);
        
        std::string gees = s -> GetSettingString("g");
        std::stringstream ss(gees);
        double i;
        while(ss>>i) {
            g.push_back(i);
            if(ss.peek() == ',' || ss.peek() == ' ') ss.ignore();
        }

        std::string nonPertStr = s->GetSettingString("NonPert");
        if(nonPertStr.compare("Gaussian") == 0) iNonPert = NonPertEnum::Gaussian;
        else if(nonPertStr.compare("BLNY") == 0) iNonPert = NonPertEnum::BLNY;
        else if(nonPertStr.compare("SIYY") == 0) iNonPert = NonPertEnum::SIYY;
        else throw std::runtime_error("Process: Invalid Non-pert function choice. Valid options can be found in the manual");

        // Initialize needed params for Asymptotic
        SetAsymOrder(s -> GetSettingSizet("AsymOrder"));
        SetMuF(s -> GetSettingDouble("muF"));
        SetMuR(s -> GetSettingDouble("muR"));

        Calculation::Initialize(s,r);
    }

    bool WmA::LoadGrid() {
        // Check for Resum grid
        gType = GridType::Resum;
        if(Calculation::LoadGrid()) gridResum = grid;
        else {
            needResum = true;
        }

        // Check for Asym grid
        gType = GridType::Asym;
        if(Calculation::LoadGrid()) gridAsym = grid;
        else {
            needAsym = true;
        }

        // Change grid type back, and return false if one grid is needed
        gType = GridType::WmA;
        return !(needResum || needAsym); 
    }

    bool WmA::SaveGrid() {
        // Save Resum grid
        gType = GridType::Resum;
        grid = gridResum;
        bool resumSave = Calculation::SaveGrid();

        // Save Asym grid
        gType = GridType::Asym;
        grid = gridAsym;
        bool asymSave = Calculation::SaveGrid();

        // Change back to WmA grid type
        gType = GridType::WmA;
        return resumSave && asymSave;
    }

    double WmA::CalcCrossing(const size_t &mode, const double &Q, const double &y) {
        // Find all W-A=0 points for given Q and y
        auto func = std::function<double(double)>(std::bind(&WmA::GetPointMode,this,Q,_1,y,mode));
        std::vector<std::pair<double,double>> ranges;
        double current = 0, past = 0;
        double qt0=0, qti;
        for(int i = 0; i < GetQTMax(Q,y)/2; ++i) {
            qti = 2.0*i;
            if(qti > qTMax) break;
            current = func(qti);
            if(current*past < 0 && current < 0) {
                ranges.push_back(std::make_pair(qt0,qti));
            }
            past = current;
            qt0 = qti;
        }
        Brent crossing(func);
        LOG_IF_F(INFO, ranges.size() == 0,
                "No crossing found for Q=%f, y=%f, in range qt=[%f,%f] for mode: %zu",
                Q,y,2.0,qTMax,mode);
        double result = qTMax;
        for(auto range : ranges) {
            double min = range.first;
            double max = range.second;
            result = crossing.CalcRoot(min,max);
            double resum = gridResum[mode] -> Interpolate(Q,y,result);
            double asym = gridAsym[mode] -> Interpolate(Q,y,result);
            LOG_F(INFO,"Crossing(%zu,%f,%f)=%f, difference = %e, qtMax = %e",mode,Q,y,result,resum-asym,GetQTMax(Q,y));
        }
        return result;
    }

    std::vector<Conv> WmA::GetNeededConvs() const{
        std::vector<Conv> neededGrids;
        if(GetAsymOrder()>1 || GetCOrder()>0) {
            neededGrids.push_back(Conv::C1);
            neededGrids.push_back(Conv::C1P1);
            if(GetAsymOrder()>2 || GetCOrder()>1) {
                neededGrids.push_back(Conv::C2);
                neededGrids.push_back(Conv::C1P1P1);
                neededGrids.push_back(Conv::C1P2);
            }
        }
        return neededGrids;
    }

    void WmA::GridGen() {
        if(qTMin == 0) qTMin += 1E-1;
        int nQT = static_cast<int>(10*log(qTMax/qTMin)+3);
        int nY = static_cast<int>((yMax-yMin)/0.1+1);

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

        if(needResum) {
            std::vector<double> test = GetResummation(qVals[0],qTVals[0],yVals[0]);
            for(size_t i = 0; i < test.size(); i++) {
                Utility::Grid3D *g_ = new Utility::Grid3D(qVals,yVals,qTVals);
                gridResum[i] = g_;
            }
        }

        if(needAsym) {
            std::vector<double> test = GetAsymptotic(qVals[0],qTVals[0],yVals[0]);
            for(size_t i = 0; i < test.size(); i++) {
                Utility::Grid3D *g_ = new Utility::Grid3D(qVals,yVals,qTVals);
                gridAsym[i] = g_;
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
                    if(needResum) {
                        std::vector<double> resultResum = GetResummation(q,qT,y);
                        for(size_t i = 0; i < resultResum.size(); i++) {
                            gridResum[i] -> AddPoint(q,y,qT,resultResum[i]);
                        }
                    }
                    if(needAsym) {
                        std::vector<double> resultAsym = GetAsymptotic(q,qT,y);
                        for(size_t i = 0; i < resultAsym.size(); i++) {
                            gridAsym[i] -> AddPoint(q,y,qT,resultAsym[i]);
                        }
                    }
                }
            }
        }
        pb.Done();
        LOG_F(INFO,"Finished generating calculation grid");
    }

    double WmA::GetCalcCrossing(const double &Q, const double &qT, const double &y) const {
        double res = gridResum.at(0) -> Interpolate(Q,y,qT);
        double asym = gridAsym.at(0) -> Interpolate(Q,y,qT);
        return res-asym;
    }

    void WmA::MergeGrid() {
        // Reset vectors
        qVals.clear();
        yVals.clear();

        int nY = static_cast<int>((yMax-yMin)/0.1+1);

        double QVal = qMin;
        while(QVal <= qMax) { 
            qVals.push_back(QVal);
            if(QVal < 200) QVal += 10;
            else if(QVal < 500) QVal += 50;
            else if(QVal < 1000) QVal += 100;
            else QVal += 500;
        } 
        for(int i = 0; i < nY; i++) yVals.push_back(yMin+i*0.1);


        // Merge grids
        LOG_F(INFO,"Creating crossing grid");
        for(size_t i = 0; i < gridResum.size(); ++i) {
            gridCrossing[i] = new Utility::Grid2D(qVals,yVals);
            for(auto q : qVals) {
                for(auto y : yVals) {
                    double crossing = CalcCrossing(i,q,y);
                    DLOG_F(3,"Crossing(%zu,%f,%f)=%e",i,q,y,crossing);
                    gridCrossing[i] -> AddPoint(q,y,crossing);
                }
            }
        }
        LOG_F(INFO,"Finished crossing grid");
    }

    std::vector<double> WmA::GetPoint(const double &Q, const double &qt, const double &y) {
        std::vector<double> result(gridResum.size());
        for(size_t i = 0; i < gridResum.size(); ++i) {
            if(qt < 2.0) result[i] = gridResum[i] -> Interpolate(Q,y,qt);
            else if(qt  < gridCrossing[i] -> Interpolate(Q,y)) {
                result[i] = gridResum[i] -> Interpolate(Q,y,qt);
                result[i] -= gridAsym[i] -> Interpolate(Q,y,qt);
            } else {
                result[i] = 0;
            }
        }

        return result;
    }

    double WmA::GetPointMode(const double &Q, const double &qt, const double &y,
                             const size_t &mode) {
        return gridResum[mode] -> Interpolate(Q,y,qt) - gridAsym[mode] -> Interpolate(Q,y,qt);
    }

    std::vector<double> WmA::GetCalc(const double &Q, const double &qt, const double &y) {
        if(qt < 2.0) return GetResummation(Q,qt,y);
        else {
            std::vector<double> resum = GetResummation(Q,qt,y);
            std::vector<double> asym = GetAsymptotic(Q,qt,y);
            std::vector<double> result(resum.size());
            for(size_t i = 0; i < resum.size(); ++i) {
                result[i] = resum[i] - asym[i];
                if(qt > gridCrossing[i] -> Interpolate(Q,y)) 
                    result[i] = 0;
            }
            return result;
        }
    }

    std::vector<double> WmA::GetResummation(const double &Q, const double &qt, const double &y) {
        return Resummation::GetCalc(Q,qt,y);
    }

    std::vector<double> WmA::GetAsymptotic(const double &Q, const double &qt, const double &y) {
        return Asymptotic::GetCalc(Q,qt,y);
    }
}
