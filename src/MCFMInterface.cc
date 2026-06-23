#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/loguru.hpp"
#include "ResBos/MCFMInterface.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"

std::shared_ptr<RESBOS> CreateResbos(char* fname, size_t len) {
    // Prevent Logging
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;

    std::string filename(fname, 0, len+1);

    std::shared_ptr<RESBOS> resbos = std::shared_ptr<RESBOS>(new RESBOS()); 

    // Load Settings
    IO::Settings settings(fname);
    resbos -> SetSettings(&settings);

    // Setup beams
    Utility::PDF* pdf = new Utility::PDF(&settings);
    std::map<std::thread::id, Utility::PDF*> pdfs;
    pdfs[std::this_thread::get_id()] = pdf;
    Utility::Hoppet *hoppet = new Utility::Hoppet(pdf -> GetPDF());

    std::pair<Beam::Beam*, Beam::Beam*> beams = Beam::Beam::MakeBeams(
            settings.GetSetting<std::string>("beam"), settings.GetSetting<double>("ecm"));
    beams.first -> SetPDF(pdf);
    beams.second -> SetPDF(pdf);
    beams.first -> SetHoppet(hoppet);
    beams.second -> SetHoppet(hoppet);
    resbos -> SetBeams(beams);
    
    // Setup process
    std::unique_ptr<ResBos::Process> process = ResBos::ProcessFactory::Instance().Create(
            settings.GetSetting<std::string>("Process"),resbos,settings);
    if(process == nullptr) throw std::runtime_error("ResBos: Invalid Process type");
    resbos -> SetProcess(std::move(process));
    
    // Setup calculation
    std::unique_ptr<ResBos::Calculation> calc = ResBos::CalculationFactory::Instance().Create(
            settings.GetSetting<std::string>("XSec"));
    if(calc == nullptr) throw std::runtime_error("ResBos: Invalid calculation mode");
    calc -> Initialize(&settings,resbos);
    calc -> GridSetup(settings);
    resbos -> SetCalculation(std::move(calc));
    
    // Check if calculation grid exists already and not in fitting mode
    if(!resbos -> GetCalculation() -> LoadGrid() && settings.GetSetting<std::string>("Mode") != "Fit") { 

        // Setup the convolutions
        bool gridGen;
        if(settings.GetSetting<std::string>("GridGen") == "true") gridGen = true;
        else gridGen = false;
        Beam::Convolution *conv = new Beam::Convolution(resbos, pdf, hoppet, gridGen,
                resbos -> GetCalculation() -> GetCOrder(), resbos -> GetCalculation() -> GetScheme(), resbos -> GetCalculation() -> GetC1(),
                resbos -> GetCalculation() -> GetC2(), resbos -> GetCalculation() -> GetC3());
        conv -> SetPDFs(pdfs);
        std::vector<Conv> convEnums = {
            Conv::C1, Conv::C1P1, Conv::C1P1P1, Conv::C1P2, Conv::C2, Conv::C2P1
        };
        
        // If using Convolution interpolation grids, then try to load.
        // If it fails to load, it means it doesn't exist and needs to be generated
        // After generating, save the grid so it can be loaded next time
        if(gridGen) {
            for(auto convEnum : convEnums) {
                if(!conv -> LoadGrid(convEnum)) {
                    conv -> GenerateGrid(convEnum);
                    conv -> SaveGrid(convEnum);
                }
            }
        }

        // Generate C grid if in Resummation mode
        if(settings.GetSetting<std::string>("XSec") == "Resummation" ||
           settings.GetSetting<std::string>("XSec") == "WmA") 
            conv -> GenerateGrid(Conv::C);

        beams.first -> SetConvolution(conv);
        beams.second -> SetConvolution(conv);

        // Setup the calculation grid
        calc -> GridGen();
        calc -> SaveGrid();

    }

    // If WmA grid, need to combine Resummed and Asymptotic grid
    if(settings.GetSetting<std::string>("XSec") == "WmA")
        calc -> MergeGrid();

    return resbos;
}

void InitElectroweak(std::shared_ptr<RESBOS> resbos, double mZ, double mW, double GF) {
    resbos -> InitElectroweak(mZ,mW,GF);
}

void DeleteResbos(std::shared_ptr<RESBOS>) {}

double MatrixElement(const std::shared_ptr<RESBOS> resbos, double* x) {
    return resbos -> GetMatrixElement(x);
}
