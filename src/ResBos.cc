#include "ResBos/ResBos.hh"
#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Cuts.hh"
#include "ResBos/Electroweak.hh"
#include "ResBos/Event.hh"
#include "ResBos/Histogram.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Process.hh"

#include "LHEF/LHEFResBos.h"

namespace ResBos {

ResBos::ResBos() {
    settings = nullptr;
    process = nullptr;
    calc = nullptr;
    writeHists_ = false;
    writeEvent_ = false;
}

ResBos::ResBos(IO::Settings* _settings, std::unique_ptr<Process> _process, 
        std::unique_ptr<Calculation> _calc, std::pair<Beam::Beam*,Beam::Beam*> _beams) 
    : settings(_settings), process(std::move(_process)), calc(std::move(_calc)), beams(_beams) {
        cuts = std::unique_ptr<Cuts>(new Cuts(settings));
        hists = std::unique_ptr<HistogramCollection>(new HistogramCollection(settings));
        writeHists_ = false;
        writeEvent_ = false;
}

ResBos::~ResBos(){}

void ResBos::SetSettings(IO::Settings *s) {
    settings = s;
    cuts = std::unique_ptr<Cuts>(new Cuts(settings));
    hists = std::unique_ptr<HistogramCollection>(new HistogramCollection(settings));
}

std::function<double(const std::vector<double>&, const double&)> ResBos::GetXSect() {
    auto func = [this](const std::vector<double> &x, const double &wgt) {return XSect(x,wgt);};
    return func;
}

double ResBos::XSect(const std::vector<double> &x, const double &wgt) {
    std::vector<double> Point(process -> GetNDim());
    std::vector<FourVector> momenta(4);
    double PSwt = process -> GetPhaseSpacePoint(x,Point,momenta);
    double Q = Point[0], qt = Point[1], y = Point[2];
    if(PSwt == 0) return 0;
    if(!cuts->MakeCuts(process->PIDs(),momenta)) return 0;
    std::vector<double> result = calc -> GetPoint(Q,qt,y);
    double MEwt = process -> ME(Point,result);

    if(WriteHists()) {
        double histwgt = wgt*MEwt*PSwt/settings->GetSettingInt("itmax2");    
        hists->FillHists(process->PIDs(),momenta,histwgt);
    }

    if(WriteEvent()) {
        std::lock_guard<std::mutex> LHEWriteLock(LHEWriteMutex);
        double eventwgt = wgt*MEwt*PSwt/settings->GetSettingInt("itmax2");    
        event -> Clear();
        process -> MakeEvent(*event, momenta);
        event -> SetXSec(eventwgt);
        lhe -> SetEvent(*event);
    }

    return MEwt*PSwt;
}

double ResBos::GetMatrixElement(double *x) const {
    std::vector<double> Point = {x[0], x[1], x[2], x[3], 0};
    double Q = Point[0], qt = Point[1], y = Point[2];
    std::vector<double> result = calc -> GetPoint(Q,qt,y);
    return process -> ME(Point, result);
}

bool ResBos::SetupMCFMInterface(std::string /*filename*/) {
    return true;
}

void ResBos::InitElectroweak(double mZ, double mW, double GF) {
    GetProcess() -> GetEW() -> MCFMInit(mZ, mW, GF); 
    GetProcess() -> InitializeMCFM();
}

bool ResBos::WriteHists() const {
    return writeHists_;
}

bool ResBos::WriteEvent() const {
    return writeEvent_;
}

bool ResBos::SetupLHEOut() {
    event = std::shared_ptr<Event>(new Event());
    event -> SetBeams(GetECM(),GetECM());
    event -> SetXSec(1.0);

    std::fstream lhefile("resbos.lhe");
    lhe = std::unique_ptr<LHEF3FromResBos>(new LHEF3FromResBos(event));
    lhe -> OpenLHEF("resbos.lhe");
    lhe -> SetInit();

    writeEvent_ = true;
    return true;
}

bool ResBos::CloseLHEOut(const double& xsec, const double& error) {
    event -> SetXSec(xsec);
    event -> SetXSecErr(error);

    lhe -> CloseLHEF(true);

    return true;
}

bool ResBos::FinalizeHist() {
    return hists -> Finalize();   
}

bool ResBos::SaveHists() const {
    return hists -> SaveHists();
}

//bool ResBos::FillHists(const std::vector<FourVector>& momenta, const double& wgt) const {
//    return true;
//}

double ResBos::H(const size_t& order, const double& Q, const double &muR) const {
    using namespace QCD;
    double value = 0;
    int db = GetProcess() -> GetAlphasCoupling();
    double logMuR = 2.0*log(muR/Q);
    size_t nf = GetNF(Q);
    switch(order) {
        case 2:
            value += process -> H2(muR);
            // Taken from 1705.09127 Eq. 3.32
            if(Q != muR) {
                value += 4.0*db*((1+db)/2.0*pow(M_PI*beta0(nf)*logMuR,2)+pi2*beta1(nf)*logMuR)
                      +  2*(1+db)*M_PI*beta0(nf)*logMuR*process->H1();
            }
            value *= GetAlpi(muR);
            value += process -> H1();
            // Taken from 1705.09127 Eq. 3.31
            if(Q != muR) value += 2*db*M_PI*beta0(nf)*logMuR;
            value *= GetAlpi(muR);
            break;
        case 1:
            value += process -> H1();
            // Taken from 1705.09127 Eq. 3.31
            if(Q != muR) value += 2*db*M_PI*beta0(nf)*logMuR;
            value *= GetAlpi(muR);
            break;
    }

    return value + 1;
}

double ResBos::GetECM() const { 
    if(beams.first -> Energy() != beams.second -> Energy()) 
        throw std::runtime_error("Beam: Beams have different center of mass energies. Check input file.");

    return 2*beams.first -> Energy();
}

double ResBos::GetAlpi(const double &Q) const {
    return beams.first -> GetPDF() -> Alpi(Q);
}

size_t ResBos::GetNF(const double &Q) const {
    return beams.first -> GetPDF() -> NF(Q);
}

std::string ResBos::GetPDFName() const {
    return beams.first -> GetPDF() -> GetName();
}

int ResBos::GetISet() const {
    return beams.first -> GetPDF() -> GetISet();
}

}
