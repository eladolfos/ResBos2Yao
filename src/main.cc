#include "ResBos/main.hh"
#include "ResBos/Random.hh"
#include "ResBos/Histogram.hh"
#include "ResBos/MPI.hh"
// #include "ResBos/GitSHA1.hh"


#include "ResBos/loguru.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "CLI/CLI.hpp"
#pragma GCC diagnostic pop

#ifdef HAVE_ROOT
#ifdef ROOT6
#include "TROOT.h"
#endif
#ifdef ROOT5
#include "TThread.h"
#endif
#endif

// Function to print out a splash to remind the user of the code and to let them know what to cite
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
//#ifndef NO_GIT
//    std::cout << "GIT SHA INFO: " << GitSHA1 << std::endl << std::endl;
//    LOG_S(INFO) << "GIT SHA INFO: " << GitSHA1;
//    if(GitModified.size() != 0) {
//        std::cout << "GIT LOCAL CHANGES IN:" << std::endl;
//        LOG_S(INFO) << "GIT LOCAL CHANGES IN:";
//        for(auto file : GitModified) {
//            LOG_F(INFO,"\t- %s",file.c_str()); 
//            std::cout << "\t- " << file << std::endl; 
//        }
//        std::cout << std::endl;
//    }
//#endif
}

// Main function
int main(int argc, char* argv[]) {
    // Run MPI startup if MPI is available
#ifdef USING_MPI
    MPI_Init(&argc,&argv);
#endif 
    ResBos::mpi = new ResBos::MPI();

    // Startup logger
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
    loguru::init(argc,argv);
#ifdef USING_MPI
    std::string logfile = "resbos_" + std::to_string(ResBos::mpi -> Rank()) + ".log";
#else
    std::string logfile = "resbos.log";
#endif
    loguru::add_file(logfile.c_str(),loguru::Truncate,loguru::Verbosity_INFO);
    PrintSplash();

    // Default config file to be used and output files to be stored
    std::string cfgFile = "resbos.config";
    std::string outputFolder = ".";

    // Command line options
    std::string seed, nthreads, itmax1, itmax2, ncall1, ncall2;
    bool writeEvents, gridGen;

    // Parse the command line options
    CLI::App app{"ResBos2: A Monte Carlo Resummation code"};

    app.add_option("-i,--input",cfgFile,"Input file name to be used for the ResBos run parameters"); 
    app.add_option("-o,--output",outputFolder,"Output folder to store the output histograms and files from the code");
    app.add_option("-s,--seed",seed,"Seed for the random number generator. Default is read from input file.");
    app.add_option("-n,--nthreads",nthreads,"Number of threads to run the ResBos code using");
    app.add_option("--itmax1",itmax1,"Number of iterations for warm-up run of VEGAS");
    app.add_option("--itmax2",itmax2,"Number of iterations for final run of VEGAS");
    app.add_option("--ncall1",ncall1,"Number of event calls per iteration for warm-up run of VEGAS");
    app.add_option("--ncall2",ncall2,"Number of event calls per iteration for final run of VEGAS");
    app.add_flag("-w,--write",writeEvents,"Output the events generated in LHE format");
    app.add_flag("-g,--gridGen",gridGen,"If true, generate grids for interpolation to speed up calculation.");
    app.add_option("-v","Change verbosity of logging. Takes an integer or options found in manual.");

    CLI11_PARSE(app, argc, argv);

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
    std::shared_ptr<ResBos::ResBos> resbos(std::make_shared<ResBos::ResBos>());

    LOG_S(INFO) << "Configuration File: " << cfgFile;

    // Load the settings files
    LOG_F(INFO,"Loading the settings files");
    IO::Settings settings(cfgFile);

    // Add command line settings to the settings loaded from files
    if(app.count("--nthreads") == 1) 
        settings.AddSettingCmdLine("nThreads",nthreads); 
    if(app.count("--seed") == 1) 
        settings.AddSettingCmdLine("iSeed",seed); 
    if(app.count("--itmax1") == 1) 
        settings.AddSettingCmdLine("itmax1",itmax1); 
    if(app.count("--itmax2") == 1) 
        settings.AddSettingCmdLine("itmax2",itmax2); 
    if(app.count("--ncall1") == 1) 
        settings.AddSettingCmdLine("ncall1",ncall1); 
    if(app.count("--ncall2") == 1) 
        settings.AddSettingCmdLine("ncall2",ncall2); 

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
    Utility::Hoppet* hoppet = new Utility::Hoppet(pdf -> GetPDF());

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
    std::unique_ptr<ResBos::Process> process = ResBos::ProcessFactory::Instance().Create(
            settings.GetSetting<std::string>("Process"),resbos,settings);
    if(process == nullptr) throw std::runtime_error("ResBos: Invalid Process type");
    process -> Initialize(settings);
    resbos -> SetProcess(std::move(process));

    // Setup the calculation class
    LOG_F(INFO,"Setting up the Calculation"); 
    std::unique_ptr<ResBos::Calculation> calc = ResBos::CalculationFactory::Instance().Create(
            settings.GetSetting<std::string>("XSec"));
    if(calc == nullptr) throw std::runtime_error("ResBos: Invalid calculation mode");
    calc -> Initialize(&settings,resbos);
    calc -> GridSetup(settings);
    resbos -> SetCalculation(std::move(calc));

    // Check if calculation grid exists already and not in fitting mode
    if((!resbos -> GetCalculation() -> LoadGrid() 
        && settings.GetSetting<std::string>("Mode") != "Fit")) { 

        // Setup the convolutions
        LOG_F(INFO,"Setting up the Convolutions");
        if(settings.GetSetting<std::string>("GridGen") == "true") gridGen = true;
        else gridGen = false;
        Beam::Convolution *conv = new Beam::Convolution(resbos, pdf, hoppet, gridGen,
                resbos -> GetCalculation() -> GetCOrder(),
                resbos -> GetCalculation() -> GetScheme(),
                resbos -> GetCalculation() -> GetC1(),
                resbos -> GetCalculation() -> GetC2(), resbos -> GetCalculation() -> GetC3());
        conv -> SetPDFs(pdfs);
        // TODO: Add in so only need ones are generated (improve speed for lower orders, especially not including C2P1)
        std::vector<Conv> convEnums = {
            Conv::C1, Conv::C1P1, Conv::C1P1P1, Conv::C1P2, Conv::C2, Conv::C2P1, Conv::G1, Conv::G1P1
        };
        
        // If using Convolution interpolation grids, then try to load.
        // If it fails to load, it means it doesn't exist and needs to be generated
        // After generating, save the grid so it can be loaded next time
        if(gridGen) {
            for(auto convEnum : convEnums) {
                if(!conv -> LoadGrid(convEnum)) {
                    conv -> GenerateGrid(convEnum);
#if USING_MPI
                    if(ResBos::mpi -> Rank() == 0)
#endif
                        conv -> SaveGrid(convEnum);
#if USING_MPI
                    ResBos::mpi -> Barrier();
#endif

                }
            }
        }

        // Generate C grid if in Resummation mode
        if(settings.GetSetting<std::string>("XSec") == "Resummation" ||
           settings.GetSetting<std::string>("XSec") == "WmA" ||
           settings.GetSetting<std::string>("XSec") == "Total") 
            conv -> GenerateGrid(Conv::C);

        // Add convolution results to the beam class
        resbos -> GetBeams().first -> SetConvolution(conv);
        resbos -> GetBeams().second -> SetConvolution(conv);

        // Setup the calculation grid
        LOG_F(INFO,"Setting up the Calculation grid");
        resbos -> GetCalculation() -> GridGen();
#if USING_MPI
        if(ResBos::mpi -> Rank() == 0)
#endif
            resbos -> GetCalculation() -> SaveGrid();
#if USING_MPI
        ResBos::mpi -> Barrier();
        ResBos::mpi -> Rank();
#endif

        delete conv;
    }

    // If WmA or Total grid, need to combine Resummed and Asymptotic grid
    if(settings.GetSetting<std::string>("XSec") == "WmA" ||
       settings.GetSetting<std::string>("XSec") == "Total")
        resbos -> GetCalculation() -> MergeGrid();

    LOG_F(INFO,"Beginning VEGAS integration");
    Utility::AdaptiveMap map(resbos -> GetProcess() -> GetNDim());
    settings.AddSetting("iterations",settings.GetSetting<std::string>("itmax1"));
    settings.AddSetting("evaluations",settings.GetSetting<std::string>("ncall1"));
    Vegas vegas(map, settings);
    vegas(resbos->GetXSect());
    resbos -> ToggleWrite();
    if(settings.GetSetting<bool>("WriteEvent")) {
        LOG_F(INFO,"Writing events to LHEF");
        resbos -> SetupLHEOut();
    }
    vegas.Set({{"iterations",settings.GetSetting<std::string>("itmax2")},
               {"evaluations",settings.GetSetting<std::string>("ncall2")}});
    vegas.Clear();
    std::array<double,2> results = vegas(resbos->GetXSect());

    resbos -> FinalizeHist();
    resbos -> SaveHists();
    if(settings.GetSetting<bool>("WriteEvent")) {
        resbos -> CloseLHEOut(results[0],results[1]);
    }

    // Print out information at the end for completeness and ease for the user
#if USING_MPI
    if(ResBos::mpi -> Rank() == 0) {
#endif
        LOG_S(INFO) << "Total Cross-Section: " << results[0] << " +/- " << results[1];
        std::cout << std::left << std::endl;
        std::cout.fill('=');
        std::cout << std::setw(80) << "=" << std::endl;
        std::cout.fill(' ');
        std::cout << std::setw(10) << "|"  << std::setw(20) << "Process:" << std::setw(49) << resbos -> GetProcess() -> Name() << "|" << std::endl;
        std::cout << std::setw(10) << "|"  << std::setw(20) << "ECM:" << std::setw(49) << std::to_string(resbos -> GetECM()) + " GeV" << "|" <<  std::endl;
        std::cout << std::setw(10) << "|"  << std::setw(20) << "Beams:" << std::setw(49) << settings.GetSetting<std::string>("beam") << "|" << std::endl;
        std::cout << std::setw(10) << "|"  << std::setw(20) << "Calculation:" << std::setw(49) << settings.GetSetting<std::string>("XSec") << "|" << std::endl;
        std::cout << std::setw(10) << "|"  << std::setw(20) << "Cross-Section:" << std::setw(49) << std::to_string(results[0]) + " +/- " + std::to_string(results[1]) + " pb" << "|" << std::endl;
        std::cout.fill('=');
        std::cout << std::setw(80) << "" << std::endl;
        std::cout << std::endl;
#if USING_MPI
    }
#endif

    // Clean-up
    LOG_F(INFO,"Cleaning up");
    delete hoppet;
    for(auto mpdf : pdfs) {
        delete mpdf.second;
    }

    LOG_F(INFO,"ResBos finished successfully");

    loguru::shutdown();

#ifdef USING_MPI
    MPI_Finalize();
#endif 
    return 0;
}

