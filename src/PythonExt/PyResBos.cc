#include <thread>

#include "ResBos/loguru.hpp"

#include "ResBos/main.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"
#include "ResBos/PyResBos.hh"
#include "ResBos/ResBos.hh"

using namespace ResBos;

void PrintSplash() {
    LOG_S(INFO) << "****************************************************";
    LOG_S(INFO) << "****************************************************";
    LOG_S(INFO) << "**                           ///////              **";
    LOG_S(INFO) << "**     ____            ____  /////           ___  **";
    LOG_S(INFO) << "**    / __ \\___  _____/ __ )____  _____     |__ \\ **";
    LOG_S(INFO) << "**   / /_/ / _ \\/ ___/ __  / __ \\/ ___/_______/ / **";
    LOG_S(INFO) << "**  / _, _/  __(__  ) /_/ / /_/ (__  )_____/ __/  **";
    LOG_S(INFO) << "** /_/ |_|\\___/____/_____/\\____/____/     /____/  **";
    LOG_S(INFO) << "**                        /                       **";
    LOG_S(INFO) << "**   ---------->---------+---------<----------    **";
    LOG_S(INFO) << "**                      /|                        **";
    LOG_S(INFO) << "**                     / |                        **";
    LOG_S(INFO) << "**                    /  |                        **";
    LOG_S(INFO) << "**                                                **";
    LOG_S(INFO) << "****************************************************";
    LOG_S(INFO) << "****************************************************";
    LOG_S(INFO) << "ResBos: Version 2.0.0";
    LOG_S(INFO) << "Author: Joshua Isaacson";
    std::cout << "****************************************************" << std::endl; 
    std::cout << "****************************************************" << std::endl; 
    std::cout << "**                           ///////              **" << std::endl;
    std::cout << "**     ____            ____  /////           ___  **" << std::endl;
    std::cout << "**    / __ \\___  _____/ __ )____  _____     |__ \\ **" << std::endl;
    std::cout << "**   / /_/ / _ \\/ ___/ __  / __ \\/ ___/_______/ / **" << std::endl;
    std::cout << "**  / _, _/  __(__  ) /_/ / /_/ (__  )_____/ __/  **" << std::endl;
    std::cout << "** /_/ |_|\\___/____/_____/\\____/____/     /____/  **" << std::endl;
    std::cout << "**                        /                       **" << std::endl;
    std::cout << "**   ---------->---------+---------<----------    **" << std::endl;
    std::cout << "**                      /|                        **" << std::endl;
    std::cout << "**                     / |                        **" << std::endl;
    std::cout << "**                    /  |                        **" << std::endl;
    std::cout << "**                                                **" << std::endl;
    std::cout << "**         Written by: Joshua Isaacson            **" << std::endl;
    std::cout << "**              Version: 2.0.0                    **" << std::endl;
    std::cout << "****************************************************" << std::endl;
    std::cout << "****************************************************" << std::endl; 
    std::cout << std::endl;
}

pyResbos::pyResbos(int argc, char* argv[]) {

    // Startup logger
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
    loguru::init(argc,argv);
    std::string logfile = "resbos.log";
    loguru::add_file(logfile.c_str(),loguru::Truncate,loguru::Verbosity_INFO);
    PrintSplash();

    // Default config file to be used and output files to be stored
    std::string cfgFile = "resbos.config";
    std::string outputFolder = ".";

    // Command line options
    std::string seed, nthreads, itmax1, itmax2, ncall1, ncall2;

// Ensure that if ROOT is used that it is thread safe
#ifdef HAVE_ROOT
#ifdef ROOT6
    ROOT::EnableThreadSafety();
#endif
#ifdef ROOT5
    TThread::Initialize();
#endif
#endif

    // Setup the ResBos class to hold all the needed peices
    LOG_F(INFO, "Starting ResBos");
    resbos = std::make_shared<ResBos>();

    LOG_S(INFO) << "Configuration File: " << cfgFile;

    // Load the settings files
    LOG_F(INFO,"Loading the settings files");
    IO::Settings settings(cfgFile);

    resbos -> SetSettings(&settings);

    // PDF Initialization
    LOG_F(INFO,"Initializing PDF");
    Utility::PDF* pdf = new Utility::PDF(&settings);
    std::map<std::thread::id,Utility::PDF*> pdfs;
    //int nThreads = 0;
    //pool = new ThreadPool(nThreads);
    //pool -> GetIDs();
    //std::vector<std::thread::id> ids(nThreads);
    //for(int i = 0; i < nThreads; i++) {
    //    pdfs[pool->GetID(i)] = new Utility::PDF(&settings);
    //}
    pdfs[std::this_thread::get_id()] = pdf;
    LOG_F(INFO,"Initializing Hoppet");
    Utility::Hoppet* hoppet = new Utility::Hoppet(pdf -> GetPDF(), settings.GetSetting<double>("mc"),
            settings.GetSetting<double>("mb"), settings.GetSetting<double>("mt"));

    // Setup the beams
    LOG_F(INFO,"Setting up the Beams");
    std::pair<Beam::Beam*,Beam::Beam*> beams = Beam::Beam::MakeBeams(settings.GetSetting<std::string>("beam"),settings.GetSetting<double>("ecm"));
    beams.first -> SetPDF(pdf);
    beams.second -> SetPDF(pdf);
    beams.first -> SetHoppet(hoppet);
    beams.second -> SetHoppet(hoppet);
    resbos -> SetBeams(beams);

    // Setup the process class
    LOG_F(INFO,"Setting up the Process");
    std::unique_ptr<Process> process = ProcessFactory::Instance().Create(
            settings.GetSetting<std::string>("Process"),resbos,settings);
    if(process == nullptr) throw std::runtime_error("ResBos: Invalid Process type");
    process -> Initialize(settings);
    resbos -> SetProcess(std::move(process));

    // Setup the calculation class
    LOG_F(INFO,"Setting up the Calculation"); 
    std::unique_ptr<Calculation> calc = CalculationFactory::Instance().Create(
            settings.GetSetting<std::string>("XSec"));
    if(calc == nullptr) throw std::runtime_error("ResBos: Invalid calculation mode");
    calc -> Initialize(&settings,resbos);
    calc -> GridSetup(settings);
    resbos -> SetCalculation(std::move(calc));

    // Setup the convolutions
    LOG_F(INFO,"Setting up the Convolutions");
    bool gridGen;
    if(settings.GetSetting<std::string>("GridGen") == "true") gridGen = true;
    else gridGen = false;
    Beam::Convolution *conv = new Beam::Convolution(resbos, pdf, hoppet, gridGen,
            resbos -> GetCalculation() -> GetCOrder(), resbos -> GetCalculation() -> GetScheme(), resbos -> GetCalculation() -> GetC1(),
            resbos -> GetCalculation() -> GetC2(), resbos -> GetCalculation() -> GetC3());
    conv -> SetPDFs(pdfs);
    // TODO: Add in so only need ones are generated (improve speed for lower orders, especially not including C2P1)
    std::vector<Conv> convEnums = {
        Conv::C1, Conv::C1P1, Conv::C1P1P1, Conv::C1P2, Conv::C2, Conv::C2P1, Conv::G1, Conv::G1P1
    };

    if(gridGen) {
        for(auto convEnum : convEnums) {
            if(!conv -> LoadGrid(convEnum)) {
                conv -> GenerateGrid(convEnum);
                conv -> SaveGrid(convEnum);
            }
        }
    }

    conv -> GenerateGrid(Conv::C);

    // Add convolution results to the beam class
    beams.first -> SetConvolution(conv);
    beams.second -> SetConvolution(conv);
}

double pyResbos::MatrixElement(std::vector<double> x) {
    return resbos -> XSect(x,1.0);
}

std::vector<double> pyResbos::MatrixElementVec(int nevents, int vars, double* array) {
    std::vector<double> results(nevents);
    for(int ievent = 0; ievent < nevents; ++ievent) {
        std::vector<double> x(vars);
        for(int ivar = 0; ivar < vars; ++ivar) {
            x[ivar] = array[ievent*vars+ivar];
        }
        results[ievent] = MatrixElement(x);
    }
    return results;
}

//std::vector<double> pyResbos::GetCalculationVec(int nevents, int vars, double* array) {
//    std::vector<double> results(nevents);
//    for(int ievent = 0; ievent < nevents; ++ievent) {
//        double Q = array[ievent*vars];
//        double qt = array[ievent*vars+1];
//        double y = array[ievent*vars+2];
//        results[ievent] = resbos -> GetCalculation() -> GetCalc(Q,qt,y);
//    }
//
//    return results;
//}

std::vector<double> pyResbos::GetResummation(int nevents, int nvars, double* array, int mode) {
    std::vector<double> result(nevents);
    for(int ievent = 0; ievent < nevents; ++ievent) {
        double Q = array[ievent*nvars];
        double b = array[ievent*nvars+1];
        double qt = array[ievent*nvars+2];
        double y = array[ievent*nvars+3];
        double ecm = resbos -> GetECM();
        double x1 = Q/ecm*exp(y);
        double x2 = Q/ecm*exp(-y);

        result[ievent] = resbos -> GetCalculation() -> FTransform(b,Q,qt,x1,x2,mode);
    }

    return result;
}
