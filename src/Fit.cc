#include <BAT/BCAux.h>
#include <BAT/BCLog.h>
#include <BAT/BCHistogramFitter.h>

#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>

void Fitter(std::vector<double>& parMin, std::vector<double>& parMax) {
    // Open log file
    BCLog::OpenLog("log.txt");
    BCLog::SetLogLevel(BCLog::detail);

    // Set nicer style for drawing than the ROOT default
    BCAux::SetStyle();

    // -----------------------
    // Load data
    // -----------------------
}

Experiment::Experiment(std::string name, Settings *s) {
    // PDF Initialization
    pdf = new PDF(s);

    std::fstream data;
    std::cout << "Initializing data for: " << name << std::endl;
    name.insert(0,"./data12e/");
    data.open(name.c_str());
    std::string dummy;

    if(name.find("E288") != std::string::npos) {
        id = 1;
        frac_n = 0.6;
        p = ProcessFactory::Get() -> CreateProcess("A0");
        iBeam = 1;
    } else if (name.find("E605") != std::string::npos) {
        id = 2;
        frac_n = 0.6;
        p = ProcessFactory::Get() -> CreateProcess("A0");
        iBeam = 1;
    } else if (name.find("R209") != std::string::npos) {
        id = 3;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("A0");
        iBeam = 1;
    } else if (name.find("D0z1") != std::string::npos) {
        id = 4;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("Z0");
        iBeam = -1;
    } else if (name.find("D0z2") != std::string::npos) {
        id = 5;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("Z0");
        iBeam = -1;
    } else if (name.find("CDFz1") != std::string::npos) {
        id = 6;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("Z0");
        iBeam = -1;
    } else if (name.find("CDFz2") != std::string::npos) {
        id = 7;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("Z0");
        iBeam = -1;
    } else if (name.find("ATLAS") != std::string::npos) {
        id = 8;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("Z0");
        iBeam = 1;
    } else if (name.find("CMS") != std::string::npos) {
        id = 9;
        frac_n = 0;
        p = ProcessFactory::Get() -> CreateProcess("Z0");
        iBeam = 1;
    }

    int nPoint;
    // Read in the data for each experiment
    if(data.is_open()) {
        // First four lines are not needed
        for(int i = 0; i < 4; i++) getline(data,dummy);
        // Read in the fifth line and parse it based on which experiment we are reading in for
        getline(data,dummy);
        if(id < 3) {
            std::vector<std::string> tokens;
            std::istringstream iss(dummy);
            copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    back_inserter(tokens));
            ecm = atof(tokens[2].c_str());
            yPz = atof(tokens[3].c_str());
        } else if(id >= 8) {
            std::vector<std::string> tokens;
            std::istringstream iss(dummy);
            copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    back_inserter(tokens));
            ecm = atof(tokens[2].c_str());
            xsec = atof(tokens[3].c_str());
        } else {
            std::vector<std::string> tokens;
            std::istringstream iss(dummy);
            copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    back_inserter(tokens));
            ecm = atof(tokens[2].c_str());
        }
        // Next line is just a line to label the table, and is not needed here
        getline(data,dummy);
        // Read in each experimental data point until the end of the file is reached
        // The data is parsed, and stored into the proper location for the class
        while(getline(data,dummy)) {
            std::vector<std::string> tokens;
            std::istringstream iss(dummy);
            copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    back_inserter(tokens));
            Qmin.push_back(atof(tokens[0].c_str()));
            Qmax.push_back(atof(tokens[1].c_str()));
            pT.push_back(atof(tokens[2].c_str()));
            dataPt.push_back(atof(tokens[3].c_str()));
            statError.push_back(atof(tokens[4].c_str()));
            sysError.push_back(atof(tokens[5].c_str()));
            corrErr.push_back(atof(tokens[7].c_str())/100.0);
            err.push_back(sqrt(pow(statError.back(),2)+pow(sysError.back(),2)));
            yk.push_back(atof(tokens[tokens.size()-1].c_str()));
        }
    } else {
        throw std::runtime_error("Fitting: Invalid experiment: " + name);
    }

    // Ensure that the vector for the theory predicitons is the same size as the number of data points
    theory.resize(dataPt.size());

    SetupProcess(s);
}

void Experiment::SetupProcess(Settings *s) {
    p -> SetScheme(s -> GetSetting<std::string>("Scheme"));
    p -> Initialize(s);
    p -> fitting = true;
    p -> SetProcessType();
    p -> ABSetup();
    p -> SetPDF(pdf);
    p -> SetiBeam(iBeam);
    p -> SetECM(ecm);
    p -> SetFractN(frac_n);
}

void Experiment::CalcChi2() {
    // Function to calculate the Chi^2 of an experiment, including the shifted data from correlated systematic errors
    chi2 = 0;
    CalcShift();
    for(int i = 0; i < theory.size(); i++) {
        chi2 += pow((theory[i]-dataPt[i]+rk*corrErr[i]*dataPt[i])/err[i],2)+rk*rk;        
        //        std::cout << dataPt[i] << "\t" << theory[i] << "\t" << N*theory[i] << "\t" << err[i] << "\t" << pow((N*theory[i]-dataPt[i])/err[i],2) << std::endl;
    }
}

void Experiment::CalcShift() {
    // Funciton to calculate the shift in the data due to systematic errors, from the CTEQ paper (NEEDS TO BE EXTENDED TO MORE THAN ONE VARIABLE!!!)
    A = 1; B = 0;
    for(int i = 0; i < theory.size(); i++) {
        A += corrErr[i]*corrErr[i]*dataPt[i]*dataPt[i]/pow(err[i],2);
        B += corrErr[i]*dataPt[i]*(dataPt[i]-theory[i])/pow(err[i],2);
    }
    rk = B/A;
}

void Experiment::CalcTheory() {
    // Function to calculate the theory prediction for each experiment, based off the type of experiment
    for(int i = 0; i < dataPt.size(); i++) {
        std::vector<double> dsig, dsigb;
        std::vector<double> SigW, SigA, SigP;
        double SigY;
        std::vector<double> Point(3);
        double dsigAve, err;
        const int Ny = 10;
        const int NQQ = 10;
        if( id == 1 || id == 2) { //E288 and E605 Data Sets
            double DelQQ = Qmax[i]*Qmax[i] - Qmin[i]*Qmin[i];
            double dQQ = DelQQ/(NQQ-1);
            double energy;
            for(int j = 0; j < NQQ; j++) {
                double Q = sqrt(Qmin[i]*Qmin[i]+j*dQQ);
                if(id == 2) {
                    energy = sqrt(pT[i]*pT[i]+Q*Q+yPz*yPz);
                    Point[2] = 0.5*log((energy+yPz)/(energy-yPz));
                } else {
                    Point[2] = yPz;
                }
                Point[0] = Q;
                Point[1] = pT[i];
                SigW = p -> Resum(Point);
                std::map<std::vector<double>,double>::iterator it = yPiece.find(Point);
                if(it == yPiece.end()) {
                    SigA = p -> Asym(Point);
                    SigP = p -> Pert(Point);
                    SigY = SigP[0] - SigA[0];
                    yPiece[Point] = SigY;
                } else {
                    SigY = it -> second;
                }
                dsig.push_back(SigW[0]+SigY*yk[i]);
            }
            dsigAve = smpsna(NQQ,dQQ,dsig,err);
            theory[i] = dsigAve/2.0/M_PI/pT[i];
        } else if(id == 3) { //R209 Data Set
            double ymax = 2.5;
            double ymin = 0.0;
            double DelY = ymax - ymin;
            double dy = DelY/(Ny-1);
            double yrange = Ny;
            double DelQQ = Qmax[i]*Qmax[i]-Qmin[i]*Qmin[i];
            double dQQ = DelQQ/(NQQ-1);
            for(int j = 0; j < NQQ; j++) {
                double Q = sqrt(Qmin[i]*Qmin[i]+j*dQQ);
                dsig.resize(0);
                for(int k = 0; k < Ny; k++) {
                    Point[0] = Q;
                    Point[1] = pT[i];
                    Point[2] = ymin + k*dy;
                    SigW = p -> Resum(Point);
                    std::map<std::vector<double>,double>::iterator it = yPiece.find(Point);
                    if(it == yPiece.end()) {
                        SigA = p -> Asym(Point);
                        SigP = p -> Pert(Point);
                        SigY = SigP[0] - SigA[0];
                        yPiece[Point] = SigY;
                    } else {
                        SigY = it -> second;
                    }
                    dsig.push_back(SigW[0]+SigY*yk[i]);
                }
                dsigb.push_back(smpsna(Ny,dy,dsig,err));
            }
            dsigAve = smpsna(NQQ,dQQ,dsigb,err);
            theory[i] = dsigAve/pT[i];
        } else if(id > 3 && id < 8) { //All dsig/dpT data sets
            double Q = Qmin[i];
            double ymax = 4.0;
            double ymin = 0.0;
            double DelY = ymax - ymin;
            double dy = DelY/(Ny-1);
            double yrange = Ny;
            if(pT[i] < 1.5) yk[i] = 0.0;
            for(int k = 0; k < Ny; k++) {
                Point[0] = Q;
                Point[1] = pT[i];
                Point[2] = ymin + k*dy;
                SigW = p -> Resum(Point);
                std::map<std::vector<double>,double>::iterator it = yPiece.find(Point);
                if(it == yPiece.end()) {
                    SigA = p -> Asym(Point);
                    SigP = p -> Pert(Point);
                    SigY = SigP[0] - SigA[0];
                    yPiece[Point] = SigY;
                } else {
                    SigY = it -> second;
                }
                dsig.push_back(SigW[0]+SigY*yk[i]);
            }
            dsigAve = smpsna(Ny, dy, dsig, err);
            theory[i] = dsigAve*2.0;
        } else if(id > 7) { //All 1/sig dsig/dpT data sets
            double Q = Qmin[i];
            double ymax = 4.0;
            if(id==8) ymax = 2.4;
            double ymin = 0.0;
            double DelY = ymax - ymin;
            double dy = DelY/(Ny-1);
            double yrange = Ny;
            double DelPt;
            if(i!=0) {
                DelPt = pT[i]-pT[i-1];
            } else {
                DelPt = pT[1]-pT[0];
            }
            int Nqt = 10;
            double dPt = DelPt/(Nqt-1);
            for(int j = 0; j < Nqt; j++) {
                dsig.resize(0);
                for(int k = 0; k < Ny; k++) {
                    Point[0] = Q;
                    Point[1] = pT[i]-DelPt/2.0+j*dPt;
                    if(Point[1] == 0) Point[1] = 1E-3;
                    Point[2] = ymin + k*dy;
                    SigW = p -> Resum(Point);
                    std::map<std::vector<double>,double>::iterator it = yPiece.find(Point);
                    if(it == yPiece.end()) {
                        SigA = p -> Asym(Point);
                        SigP = p -> Pert(Point);
                        SigY = SigP[0] - SigA[0];
                        yPiece[Point] = SigY;
                    } else {
                        SigY = it -> second;
                    }
                    dsig.push_back((SigW[0]+SigY)*yk[i]);
                }
                dsigb.push_back(smpsna(Ny,dy,dsig,err));
            }
            dsigAve = smpsna(Nqt, dPt, dsigb, err)/DelPt;
            theory[i] = dsigAve*2.0;
            if(id>7) theory[i] /= xsec*N;
            std::cout << theory[i] << "\t" << dataPt[i] << std::endl;
        }
    }
}

void fcn(int &npar, double *deriv, double &f, double par[], int flag) {
    f = 0;

    for(auto e : experimentList) {
        e -> SetG(par[0],par[1],par[2]);
        e -> SetNorm(par[e -> GetID() + 2]);
    }

    std::vector<std::future<void>> futures;
    for(auto e : experimentList) {
        futures.push_back(std::async(std::launch::async,&Experiment::CalcTheory,e));
    }

    for(int i = 0; i < futures.size(); ++i) {
        futures[i].wait();
    }

    for(auto e: experimentList) {
        e -> CalcChi2();
        f += e -> GetChi2();
        std::cout << e -> GetID() << "\t" << e -> GetChi2() << std::endl;
    }
    // ----------------------
    // Define fit function with 3 parameters
    // ----------------------
    TF1 f1("f",fptr,&Fit::Evaluate,0,100,npar,"MyFunction","Evaluate");
}
