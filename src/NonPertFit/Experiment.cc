#include "NonPertFit/Experiment.hh"
#include "NonPertFit/NonPertFit.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Utility.hh"
#include "ResBos/Brent.hh"

using namespace std::placeholders;

std::string getTime()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

double Simpson(const size_t& nx, const double& dx, const std::vector<double>& f, double& err) {
    double simp, errd, trpz;
    size_t ms = 0;
    if(dx <= 0) return 0;
    if(nx <= 1) simp = 0;
    else if(nx == 2) {
        simp = (f[0]+f[1])/2.0;
        errd = (f[0]-f[1])/2.0;
    } else {
        ms = nx % 2; 

        double add = 0;
        size_t nz = nx;
        if(ms == 0) {
            add = (9.0*f[nx-1]+19.0*f[nx-2]-5*f[nx-3]+f[nx-4])/24.0;
            nz--;
        }

        if(nz == 3) {
            simp = (f[0]+4*f[1]+f[2])/3.0;
            trpz = (f[0]+2*f[1]+f[2])/2.0;
        } else {
            double se = f[1];
            double s0 = 0;
            size_t nm1 = nz - 1;
            for(size_t i = 3; i < nm1; i += 2) {
                se += f[i];
                s0 += f[i-1];
            }
            simp = (f[0]+4*se+2*s0+f[nz-1])/3.0;
            trpz = (f[0]+2*se+2*s0+f[nz-1])/2.0;
        }

        errd = trpz - simp;
        simp = simp + add;
    }

    if(fabs(simp) > 1E-10) err = errd/simp;
    else err = 0.0;

    simp *= dx;

    return simp;
}

Experiment::Experiment() {
    resbos = std::shared_ptr<ResBos::ResBos>(new ResBos::ResBos());
}

void Experiment::Init() {
    // Setup PDFs
    Utility::PDF* pdf = new Utility::PDF(NonPertFit::global_settings.get()); 
    std::map<std::thread::id,Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    Utility::Hoppet* hoppet = new Utility::Hoppet(pdf -> GetPDF());

    // Setup beams
    std::pair<Beam::Beam*, Beam::Beam*> beams = Beam::Beam::MakeBeams(data.Beam(),data.ECM());
    beams.first -> SetPDF(pdf);
    beams.second -> SetPDF(pdf);
    beams.first -> SetHoppet(hoppet);
    beams.second -> SetHoppet(hoppet);
    resbos -> SetBeams(beams);

    // Setup the process
    std::string processName;
    switch(experiment) {
        case ExpType::Collider:
            processName = "DrellYan";
            break;
        case ExpType::Fixed:
        case ExpType::Fixed2:
            processName = "A0";
            break;
    }
    std::unique_ptr<ResBos::Process> process = ResBos::ProcessFactory::Instance().Create(
        processName,resbos,*NonPertFit::global_settings);
    process -> Initialize(*NonPertFit::global_settings);
    resbos -> SetProcess(std::move(process));


    // Setup the calculation class
    auto resum = ResBos::CalculationFactory::Instance().Create("Resummation");
    asym = ResBos::CalculationFactory::Instance().Create("Asymptotic");
    pert = ResBos::CalculationFactory::Instance().Create("Perturbative");
    resum -> Initialize(NonPertFit::global_settings.get(), resbos);
    asym -> Initialize(NonPertFit::global_settings.get(), resbos);
    pert -> Initialize(NonPertFit::global_settings.get(), resbos);
    resbos -> SetCalculation(std::move(resum));
    //if(processName == "A0") asym -> SetAsymOrder(1);

    // Setup the convolutions only for the first experiment since the rest can reuse
    static Beam::Convolution *conv;
    if(!conv) {
        conv = new Beam::Convolution(resbos, pdf, hoppet, true,
                resbos -> GetCalculation() -> GetCOrder(), 
                resbos -> GetCalculation() -> GetScheme(), 
                resbos -> GetCalculation() -> GetC1(),
                resbos -> GetCalculation() -> GetC2(), 
                resbos -> GetCalculation() -> GetC3());
        conv -> SetPDFs(pdfs);
        std::vector<Conv> convEnums = {
            Conv::C1, Conv::C1P1, Conv::C1P1P1, Conv::C1P2, Conv::C2
        };

        for(auto convEnum : convEnums) {
            if(!conv -> LoadGrid(convEnum)) {
                conv -> GenerateGrid(convEnum);
                conv -> SaveGrid(convEnum);
            }
        }

        conv -> GenerateGrid(Conv::C);
    }

    // Add convolution results to the beam class
    beams.first -> SetConvolution(conv);
    beams.second -> SetConvolution(conv);
    yResults.resize(data.NPoints());
    yMean.resize(data.NPoints());
    QMean.resize(data.NPoints());
    qtMean.resize(data.NPoints());
    correction.resize(data.NPoints());
}

void Experiment::CalculateCorrection(const std::function<std::vector<double>(double, double, double)> &calc, 
                                     std::array<double, 2> &corr, double pt, double y,
                                     double qmin, double qmax, size_t nsteps, bool pert) const {
    std::vector<double> coeff0(nsteps), coeff1(nsteps), denom0(nsteps), denom1(nsteps);
    double dq = (qmax - qmin)/(nsteps - 1);
    for(size_t i = 0; i < nsteps; ++i) {
        double Q = qmin + i*dq;

        auto calculation = calc(Q, pt, y);
        auto result = resbos -> GetProcess() -> CalcCorrection(Q, calculation, pert);
        coeff0[i] = result[0];
        coeff1[i] = result[1];
        denom0[i] = result[2];
        denom1[i] = result[3];
    }

    double err;
    corr[0] = Simpson(nsteps, dq, coeff0, err) / Simpson(nsteps, dq, denom0, err);
    corr[1] = Simpson(nsteps, dq, coeff1, err) / Simpson(nsteps, dq, denom1, err);

    coeff0.clear();
    coeff1.clear();
    denom0.clear();
    denom1.clear();
}

double Experiment::GetPrediction(size_t idx, bool reset) {
    double result = 0;
    auto point = data.GetDataPoint(idx);

    // Set yk to zero if yk is set to false
    double ykVal = 1.0;
    if(yk) ykVal = point.yk;

    const double pTmin = point.pTmin;
    const double pTmax = point.pTmax;
    const double QMax = point.Qmax;
    const double QMin = point.Qmin;

    if(experiment == ExpType::Collider) {
        const double pTmin = point.pTmin;
        const double pTmax = point.pTmax;
        const double DelpT = pTmax - pTmin;
        const size_t qtrange = 20;
        const double dqt = (pTmax - pTmin) / (qtrange-1);
        const double ptavg = (pTmin + pTmax)/2.0;
        const double ymax = std::min(resbos -> GetCalculation() -> GetYMax(QMax, ptavg), point.ymax);
        const double ymin = std::max(0.0, point.ymin);
        const double yavg = (ymax + ymin)/2.0;
        const double DelY = ymax - ymin;
        const size_t yrange = 20;
        const double dy = DelY/(yrange-1);
        const double Qavg = (QMax + QMin)/2.0;
        double err;
        // Only calculate Y Piece if it hasn't been calculated yet
        if(ptavg > ptYInc) {
            if(yResults[idx] != 0) {
                result += yResults[idx];
            } else {
                std::array<double, 2> correctionA, correctionP;
                auto pertCalc = std::function<std::vector<double>(double, double, double)>(
                        std::bind(&ResBos::Calculation::GetCalc, pert.get(), _1, _2, _3));
                auto asymCalc = std::function<std::vector<double>(double, double, double)>(
                        std::bind(&ResBos::Calculation::GetCalc, asym.get(), _1, _2, _3));
                CalculateCorrection(pertCalc, correctionP, 4.0, 0.0, QMin, QMax, 50, true);
                CalculateCorrection(asymCalc, correctionA, 4.0, 0.0, QMin, QMax, 50);
                if(data.Name() == "CDF1" || data.Name() == "CDF2" || data.Name() == "D01" || data.Name() == "D02"){
                  correctionP[0] = 1.0;
                  correctionP[1] = 1.0;
                  correctionA[0] = 1.0;
                  correctionA[1] = 1.0;
                }
                std::vector<double> SigA(qtrange);
                std::vector<double> SigP(qtrange);
                for(size_t i = 0; i < qtrange; ++i) {
                    std::vector<double> SigA1(yrange);
                    std::vector<double> SigP1(yrange);
                    double pT = pTmin + static_cast<int>(i)*dqt;
                    for(size_t j = 0; j < yrange; ++j) {
                        double y = ymin + static_cast<int>(j)*dy;
                        std::vector<double> tmpPert = pert -> GetCalc(Qavg,pT,y);
                        std::vector<double> tmpAsym = asym -> GetCalc(Qavg,pT,y);
                        SigA1[j] = resbos -> GetProcess() -> NonPert(correctionA, tmpAsym);
                        SigP1[j] = resbos -> GetProcess() -> NonPert(correctionP, tmpPert, true);
                        tmpPert.clear();
                        tmpAsym.clear();
                    }
                    SigA[i] = Simpson(yrange, dy, SigA1, err);
                    SigP[i] = Simpson(yrange, dy, SigP1, err);
                    SigA1.clear();
                    SigP1.clear();
                }
                //std::cout << "Integration y " << getTime() << std::endl;
                double asym = Simpson(qtrange, dqt, SigA, err)/DelpT;
                double pert = Simpson(qtrange, dqt, SigP, err)/DelpT*ykVal;
                yResults[idx] = pert-asym;
                std::cout << data.Name() << " idx: " << idx << " y = " << yResults[idx] << " pert = " << pert << " asym = " << asym << std::endl;
                //std::cout << "y end " << getTime() << std::endl;
                result += yResults[idx];
                SigA.clear();
                SigP.clear();
            }
        }

        //std::cout << "Start calculate W: idx: " << idx << " correction: " << correction[idx][0] << " " << getTime() << std::endl;

        if((yMean[idx] == 0 || qtMean[idx] == 0 || correction[idx][0] == 0) || reset) {
            // CalculateCorrection(correction[idx], ptavg, yavg, QMin, QMax, 50);
            auto resCalc = std::function<std::vector<double>(double, double, double)>(
                    std::bind(&ResBos::Calculation::GetCalc, resbos->GetCalculation(), _1, _2, _3));
            CalculateCorrection(resCalc, correction[idx], 4.0, 0.0, QMin, QMax, 50);
            if(data.Name() == "CDF1" || data.Name() == "CDF2" || data.Name() == "D01" || data.Name() == "D02"){
              correction[idx][0] = 1.0;
              correction[idx][1] = 1.0;
            }
            //std::cout << "correction W " << correction[idx][0] << " " << getTime() << std::endl;
            double yavg = 0;
            std::vector<double> SigW(qtrange);
            for(size_t i = 0; i < qtrange; ++i) {
                std::vector<double> SigW1(yrange);
                double pT = pTmin + static_cast<int>(i)*dqt;
                for(size_t j = 0; j < yrange; ++j) {
                    auto y = ymin + static_cast<int>(j)*dy;
                    std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Qavg,pT,y);
                    SigW1[j] = resbos -> GetProcess() -> NonPert(correction[idx], tmp);
                    tmp.clear();
                    //std::cout << "Calculate W pT: " << pT << " y: " << y << " " << getTime() << std::endl;
                }
                SigW[i] = Simpson(yrange, dy, SigW1, err);

                auto yCrossing = [&](double yGuess) {
                    auto tmp = resbos -> GetCalculation() -> GetCalc(Qavg, pT, yGuess);
                    return resbos -> GetProcess() -> NonPert(correction[idx], tmp)*DelY - SigW[i];
                };

                Brent brent{yCrossing};
                try{
                    double tmp = brent.CalcRoot(ymin, ymax);
                    if(!std::isnan(tmp))
                        yavg += tmp;
                } catch (std::runtime_error &e) {
                    continue;
                }
                SigW1.clear();
            }
            //std::cout << "Integration W " << getTime() << std::endl;

            auto resum = Simpson(qtrange, dqt, SigW, err) / DelpT;
            yMean[idx] = yavg/qtrange;

            auto qtCrossing = [&](double qtGuess) {
                auto tmp = resbos -> GetCalculation() -> GetCalc(Qavg, qtGuess, yMean[idx]);
                return resbos -> GetProcess() -> NonPert(correction[idx], tmp)*DelY - resum;
            };
            Brent brent{qtCrossing};
            try{
                qtMean[idx] = brent.CalcRoot(pTmin, pTmax);
                if(qtMean[idx] != qtMean[idx]) {
                    qtMean[idx] = 0;
                }
            } catch (std::runtime_error &e) {
                qtMean[idx] = 0;
            }

            std::cout << "resum = " << resum << std::endl;
            std::cout << data.Name() << " idx: " << idx << " yMean: " << yMean[idx] << " qtMean: " << qtMean[idx] << " " << getTime() << std::endl;
            result += resum;
            SigW.clear();
        } else {
            auto resum = resbos -> GetCalculation() -> GetCalc(Qavg, qtMean[idx], yMean[idx]);
            result += resbos -> GetProcess() -> NonPert(correction[idx], resum)*DelY;
            resum.clear();
        }

        return 2.0*result;
    } else if(experiment == ExpType::Fixed) {
        double pT = point.pT;
        constexpr size_t NQQ = 10;
        const double DelQQ = QMax*QMax - QMin*QMin;
        const double dQQ = DelQQ/(NQQ);
        double energy, y, err;
        // Only calculate Y Piece if it hasn't been calculated yet
        if(pT > ptYInc) {
            if(yResults[idx] != 0) 
                result += yResults[idx];
            else {
                std::vector<double> SigY(NQQ);
                std::vector<double> SigA(NQQ);
                std::vector<double> SigP(NQQ);
                for(size_t i = 0; i < NQQ; i++) {
                    double Q = sqrt(QMin*QMin+static_cast<int>(i)*dQQ);
                    if(Id() == 2) {
                        energy = sqrt(pT*pT+Q*Q+point.ymin*point.ymin);
                        y = 0.5*log((energy+point.ymin)/(energy-point.ymin));
                    } else {
                        y = point.ymin;
                    }
                    std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
                    std::vector<double> tmpAsym = asym -> GetCalc(Q,pT,y);
                    SigY[i] = tmpPert[0] - tmpAsym[0];
                    SigA[i] = tmpAsym[0];
                    SigP[i] = tmpPert[0];
                    tmpPert.clear();
                    tmpAsym.clear();
                }
                double asym = Simpson(NQQ, dQQ, SigA, err);
                double pert = Simpson(NQQ, dQQ, SigP, err)*ykVal;
                yResults[idx] = (pert-asym)/2.0/M_PI/pT;
                std::cout << data.Name() << " idx: " << idx << " y = " << yResults[idx] << " pert = " << pert/ykVal << " pert*yk = " << pert << " asym = " << asym << std::endl;

                //yResults[idx] = Simpson(NQQ,dQQ,SigY,err)/2.0/M_PI/pT*ykVal;
                //std::cout << data.Name() << " idx: " << idx << " y = " << yResults[idx] << std::endl;
                result += yResults[idx];
                SigA.clear();
                SigP.clear();
                SigY.clear();
            }
        }
        
        if(QMean[idx] == 0 || reset) {
            std::vector<double> SigW(NQQ);
            for(size_t i = 0; i < NQQ; i++) {
                double Q = sqrt(QMin*QMin+static_cast<int>(i)*dQQ);
                if(Id() == 2) {
                    energy = sqrt(pT*pT+Q*Q+point.ymin*point.ymin);
                    y = 0.5*log((energy+point.ymin)/(energy-point.ymin));
                } else {
                    y = point.ymin;
                }
                std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Q,pT,y);
                SigW[i] = tmp[0];
                tmp.clear();
            }
            auto resum = Simpson(NQQ, dQQ, SigW, err);
            auto yCrossing = [&](double QGuess) {
                auto tmp = resbos -> GetCalculation() -> GetCalc(QGuess, pT, y);
                return tmp[0]*DelQQ - resum;
            };

            Brent brent{yCrossing};
            try {
                QMean[idx] = brent.CalcRoot(QMin, QMax);
                if(QMean[idx] != QMean[idx])
                    QMean[idx] = 0;
            } catch (std::runtime_error &e) {
                QMean[idx] = 0.0;
            }
            result += resum/2.0/M_PI/pT;
            std::cout << data.Name() << " idx: " << idx << " resum: " << resum/2.0/M_PI/pT << " " << getTime() << std::endl;
            SigW.clear();
        } else {
            const double Q = QMean[idx];
            if(Id() == 2) {
                energy = sqrt(pT*pT+Q*Q+point.ymin*point.ymin);
                y = 0.5*log((energy+point.ymin)/(energy-point.ymin));
            } else {
                y = point.ymin;
            }
            auto resum = resbos -> GetCalculation() -> GetCalc(Q, pT, y);
            result += resum[0]*DelQQ/2.0/M_PI/pT;
            resum.clear();
        }

        return result;
    } else if(experiment == ExpType::Fixed2) {
        double pT = point.pT;
        constexpr size_t Ny = 10;
        constexpr size_t NQQ = 10;
        const double ymax = std::min(resbos -> GetCalculation() -> GetYMax(QMax, pT), point.ymax);
        const double ymin = std::max(0.0, point.ymin);
        const double DelY = ymax - ymin;
        const double dy = DelY/(Ny-1);
        // const double yrange = Ny;
        const double DelQQ = QMax*QMax-QMin*QMin;
        const double dQQ = DelQQ/(NQQ-1);
        double err;
        // Only calculate Y Piece if it hasn't been calculated yet
        if(pT > ptYInc) {
            if(yResults[idx] != 0) 
                result += yResults[idx];
            else {
                std::vector<double> SigY(NQQ);
                for(size_t i = 0; i < NQQ; i++) {
                    std::vector<double> SigY1(Ny);
                    double Q = sqrt(QMin*QMin+static_cast<int>(i)*dQQ);
                    for(size_t j = 0; j < Ny; j++) {
                        double y = ymin + static_cast<int>(j)*dy;
                        std::vector<double> tmpPert = pert -> GetCalc(Q,pT,y);
                        std::vector<double> tmpAsym = asym -> GetCalc(Q,pT,y);
                        SigY[i] = tmpPert[0] - tmpAsym[0];
                        tmpPert.clear();
                        tmpAsym.clear();
                    }
                    SigY[i] = Simpson(Ny,dy,SigY1,err);
                    SigY1.clear();
                }
                yResults[idx] = Simpson(NQQ,dQQ,SigY,err)/pT*ykVal;
                result += yResults[idx];
                SigY.clear();
            }
        }
        std::vector<double> SigW(NQQ);
        if((QMean[idx] == 0 && yMean[idx] == 0) || reset) {
            for(size_t i = 0; i < NQQ; i++) {
                std::vector<double> SigW1(Ny);
                double Q = sqrt(QMin*QMin+static_cast<int>(i)*dQQ);
                for(size_t j = 0; j < Ny; j++) {
                    double y = ymin + static_cast<int>(j)*dy;
                    std::vector<double> tmp = resbos -> GetCalculation() -> GetCalc(Q,pT,y);
                    SigW1[j] = tmp[0];
                    tmp.clear();
                }
                SigW[i] = Simpson(Ny,dy,SigW1,err);

                auto QCrossing = [&](double yGuess) {
                    auto tmp = resbos -> GetCalculation() -> GetCalc(Q, pT, yGuess);
                    return tmp[0]*DelY - SigW[i];
                };

                Brent brent{QCrossing};
                try {
                    yMean[idx] = brent.CalcRoot(ymin, ymax);
                    if(yMean[idx] != yMean[idx])
                        yMean[idx] = 0;
                } catch (std::runtime_error &e) {
                    yMean[idx] = 0;
                }
                SigW1.clear();
            }
            auto resum = Simpson(NQQ, dQQ, SigW, err);
            auto yCrossing = [&](double QGuess) {
                auto tmp = resbos -> GetCalculation() -> GetCalc(QGuess, pT, yMean[idx]);
                return tmp[0]*DelQQ - resum;
            };

            Brent brent{yCrossing};
            try {
                QMean[idx] = brent.CalcRoot(QMin, QMax);
                if(QMean[idx] != QMean[idx])
                    QMean[idx] = 0;
            } catch (std::runtime_error &e) {
                QMean[idx] = 0.0;
            }
            result += resum/pT;
            SigW.clear();
        } else {
            const double Q = QMean[idx];
            const double y = yMean[idx];
            auto resum = resbos -> GetCalculation() -> GetCalc(Q, pT, y);
            result += resum[0]*DelQQ*DelY/pT;
            resum.clear();
        }
        return result;
    } else {
        throw std::runtime_error("Experiment type non-implemented");
    }
}

double Experiment::Chi2(bool reset, bool print) {
    Eigen::VectorXd theory(data.NPoints());
    for(size_t j = 0; j < data.NPoints(); ++j) {
        auto point = data.GetDataPoint(j);
        theory[j] = GetPrediction(j, reset)*point.efficiency;
    }

    return data.Chi2(theory, print);
}
