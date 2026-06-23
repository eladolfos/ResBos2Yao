// ***************************************************************
// This file was created using the bat-project script.
// bat-project is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include <BAT/BCMath.h>
#include "NonPertFit.h"

#include <iostream>

double pyalem(double q2){
// Returns the running electromagnetic coupling alpha
//PN Stolen shamelessly from ResBos 
//
//...Calculate real part of photon vacuum polarization.
//...For leptons simplify by using asymptotic (q^2 >> m^2) expressions.
//   For hadrons use parametrization of H. Burkhardt et al.
//   See R. Kleiss et al, CERN 89-08, vol. 3, pp. 129-131.
//csb___see also Phys.Lett.B356:398-403,1995

  double pi, alpha0, rpigg, aempi;  
  pi = 3.141592654;
  alpha0=1./137.04;
  aempi= alpha0/(3.*pi);
  
  if (q2 < 2e-6)
    rpigg =0.0;
  else if(q2 < 0.09) 
    rpigg = aempi*(13.4916 + log(q2)) + 0.00835*log(1.+q2);
  else if(q2 < 9.) 
    rpigg=aempi*(16.3200+2.0*log(q2))+ 0.00238*log(1.0+3.927*q2);
  else if(q2 < 1e4) 
    rpigg=aempi*(13.4955+3.0*log(q2))+0.00165+ 0.00299*log(1.0+q2);
  else
    rpigg=aempi*(13.4955+3.0*log(q2))+ 0.00221 + 0.00293*log(1.0+q2);

//...Calculate running alpha_em.
  return alpha0/(1.0-rpigg);
}//pyalem ->

// ---------------------------------------------------------
NonPertFit::NonPertFit(const std::string& name)
    : BCModel(name)
{
    // Define parameters here in the constructor.
    // Also define their priors, if using built-in priors.
    // For example:
    AddParameter("g1", 0, 2, "g_{1}", "");
    AddParameter("g2", 0, 2, "g_{2}", "");
    AddParameter("g3", 0, 2, "g_{3}", "");
    
    // Add in nuisance parameters for the normalization of each experiment
//    AddParameter("N0",-5,5,"ATLAS 7TeV Norm","");
//    AddParameter("N1",-5,5,"ATLAS 7TeV(shape) Norm","");
//    AddParameter("N2",-5,5,"ATLAS 8TeV Norm","");
//    AddParameter("N3",-5,5,"ATLAS 8TeV(shape) Norm","");
//    AddParameter("N4",-5,5,"CDF1 Norm","");
//    AddParameter("N5",-5,5,"CDF2 Norm","");
//    AddParameter("N6",-5,5,"CMS 7TeV Norm","");
//    AddParameter("N7",-5,5,"CMS 7TeV(shape) Norm","");
//    AddParameter("N8",-5,5,"CMS 8TeV Norm","");
//    AddParameter("N9",-5,5,"CMS 8TeV(shape) Norm","");
//    AddParameter("N10",-5,5,"D01 Norm","");
//    AddParameter("N11",-5,5,"D02 Norm","");
    AddParameter("N12",-5,5,"E288200 Norm","");
//    AddParameter("N13",-5,5,"E288300 Norm","");
//    AddParameter("N14",-5,5,"E288400 Norm","");
//    AddParameter("N16",-5,5,"E605 Norm","");
//    AddParameter("N17",-5,5,"R209 Norm","");
    SetPriorConstantAll();

    InitResBos();
    // Define observables here, too. For example:
    // AddObservable("mu_squared", 1, 4, "#mu^{2}", "[GeV^{2}]");
    
    count = 0;
}

// ---------------------------------------------------------
NonPertFit::~NonPertFit()
{
    // destructor
    delete pdf;
    delete hoppet;
    delete process;
    delete resbos;
}

// ---------------------------------------------------------
double NonPertFit::LogLikelihood(const std::vector<double>& pars)
{

    // return the log of the conditional probability p(data|pars).
    // This is where you define your model.
    // BCMath contains many functions you will find helpful.
    std::vector<double> g{pars[0],pars[1],pars[2]};

//    double N0 = 1.0+N0err*pars[4];
//    double N1 = 1.0+N1err*pars[5];
//    double N2 = 1.0+N2err*pars[6];
//    double N3 = 1.0+N3err*pars[7];
//    double N4 = 1.0+N4err*pars[8];
//    double N5 = 1.0+N5err*pars[9];
//    double N6 = 1.0+N6err*pars[10];
//    double N7 = 1.0+N7err*pars[11];
//    double N8 = 1.0+N8err*pars[12];
//    double N9 = 1.0+N9err*pars[13];
//    double N10 = 1.0+N10err*pars[14];
//    double N11 = 1.0+N11err*pars[15];
    double N12 = 1.0+N12err*pars[3];
//    double N13 = 1.0+N13err*pars[17];
//    double N14 = 1.0+N14err*pars[18];
//    double N16 = 1.0+N16err*pars[19];
//    double N17 = 1.0+N17err*pars[20];

//    std::vector<double> Norms{N0,N1,N2,N3,N4,N5,N6,N7,N8,N9,N10,N11,N12,N13,N14,N16,N17};

    resbos -> SetNonPertCoefficients(g);

    double logl = 0;
    double prediction = 0;
    for(size_t i = 0; i < GetNDataPoints(); ++i) {
        // get data point
        std::vector<double>& x = GetDataSet()->GetDataPoint(i).GetValues();

        int type = x[0];
        double Qmin = x[1];
        double Qmax = x[2];
        double pT = x[3];

        if(type != 12) continue;

//        if(type < 12) prediction = GetValueDSigmaDPT(type,Qmin,pT);
        prediction = GetValueDSigmaE288(type,Qmin,Qmax,pT);
//        else GetValueR209(type,Qmin,QMax,pT);
        logl += BCMath::LogGaus(prediction*N12,x[4],x[5]);
    }

    count++;
    if(count == 20) exit(-1);

    std::cout << count << "\t" << logl << std::endl;

    return logl;
}

double NonPertFit::Simpson(const int& nx, const double& dx, const std::vector<double>& f, double& err) {
    double simp, errd, trpz;
    int ms = 0;
    if(dx <= 0) return 0;
    if(nx <= 1) simp = 0;
    else if(nx == 2) {
        simp = (f[0]+f[1])/2.0;
        errd = (f[0]-f[1])/2.0;
    } else {
        ms = nx % 2; 

        double add = 0;
        int nz = nx;
        if(ms == 0) {
            add = (9.0*f[nx-1]+19.0*f[nx-2]-5*f[nx-3]+f[nx-4])/24.0;
            nz--;
        }

        if(nz == 3) {
            simp = (f[0]+4*f[1]+f[2])/3.0;
            trpz = (f[0]+2*f[1]+f[3])/2.0;
        } else {
            double se = f[1];
            double s0 = 0;
            int nm1 = nz - 1;
            for(int i = 3; i < nm1; i = i + 2) {
                int im1 = i - 1;
                se += f[i];
                s0 += f[im1];
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

double NonPertFit::GetValueDSigmaE288(const int& type, const double& Qmin, const double& Qmax, const double& pT) {
    const int NQQ = 10;
    double y = 0;
    process -> SetBeam(Beam::pCu);
    switch(type) {
        case 12:    
            process -> SetECM(19.4);
            y = 0.4;
            break;
        case 13:
            process -> SetECM(23.8);
            y = 0.21;
            break;
        case 14:
            process -> SetECM(27.4);
            y = 0.03;
            break;
        case 16:
            process -> SetECM(38.8);
            y = 0.1;
            break;
    }

    double DelQQ = Qmax*Qmax - Qmin*Qmin;
    double dQQ = DelQQ/(NQQ-1);
    std::vector<double> dsig;
    for(int i = 0; i < NQQ; ++i) {
        double Q = sqrt(Qmin*Qmin+i*dQQ);
        if(type==16) {
            double energy = sqrt(pT*pT+Q*Q+y*y);
            y = 0.5*log((energy+y)/(energy-y));
        }
        std::vector<double> result = resbos->GetCalc(Q,pT,y);
        double tmp = result[0]*4.0/9.0+result[2]*1.0/9.0;
        tmp *= 4*M_PI*M_PI*pow(pyalem(Q*Q),2)/(3.0*3.0*M_PI*Q*Q);
        dsig.push_back(tmp);
    }

    double err = 0;
    double dsigAve = Simpson(NQQ,dQQ,dsig,err)/DelQQ;
    
    return dsigAve*DelQQ/2.0/M_PI/pT;
}

// ---------------------------------------------------------
void NonPertFit::InitResBos() {
    std::vector<std::string> cfgFiles{"resbos.config","standard.dat","cuts.dat"};
    IO::Settings settings(cfgFiles);
    pdf = new Utility::PDF(settings);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    int nThreads = 4;
    pool = new ThreadPool(nThreads);
    pool -> GetIDs();
    std::vector<std::thread::id> ids(nThreads);
    for(int i = 0; i < nThreads; ++i) {
        pdfs[pool->GetID(i)] = new Utility::PDF(settings);
    }
    pdfs[std::this_thread::get_id()] = pdf;
    mainThread = std::this_thread::get_id();
    hoppet = new Utility::Hoppet(pdf -> GetPDF(), settings.GetSetting<double>("mc"), settings.GetSetting<double>("mb"), settings.GetSetting<double>("mt"), 36);

    // Setup the process class
    process = Process::Process::Instantiate(settings.GetSetting<std::string>("Process"),settings);
    if(process == NULL) throw std::runtime_error("Process: Invalid Process type");

    // Setup the calculation class
    resbos = new ResBos::Resummation(settings,process);
    resbos -> SetPDF(pdf);
    resbos -> SetHoppet(hoppet);
    resbos -> Setup(process,settings,pdfs);
    resbos -> GridSetup(process,settings,pdfs);

    process -> SetBeam(Beam::pCu);
    process -> SetECM(19.4);
    // Setup the b-space grid for faster calculations
    const size_t numB = 400, numQ = 10, numY = 1;
    const double bMin = 1E-4, bMax = 187.710827; 
    const double qMin = 4, qMax = 8;
    std::vector<double> b, Q, y;
    for(size_t i = 0; i < numB; ++i) {
        b.push_back(-log(1-(double)i/numB)*bMax/6+bMin);
    }
    const double DelQQ = qMax*qMax-qMin*qMin;
    const double dQQ = DelQQ/(numQ-1);
    for(size_t i = 0; i < numQ; ++i) {
        Q.push_back(sqrt(qMin*qMin+i*dQQ)); 
    }
    y.push_back(0.4);
    resbos -> GenerateBGrid(b,Q,y);

    delete pool;
}

// ---------------------------------------------------------
// double NonPertFit::LogAPrioriProbability(const std::vector<double>& pars)
// {
//     // return the log of the prior probability p(pars)
//     // If you use built-in priors, leave this function commented out.
// }

// ---------------------------------------------------------
// void NonPertFit::CalculateObservables(const std::vector<double>& pars)
// {
//     // Calculate and store obvserables. For example:
//     GetObservable(0) = pow(pars[0], 2);
// }
