#ifndef RESBOS_HH
#define RESBOS_HH

#include <functional>
#include <vector>
#include <memory>
#include <mutex>

#include "ResBos/Calculation.hh"
#include "ResBos/Enums.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/Settings.hh"

class Event;
class LHEF3FromResBos;

namespace Beam {
    class Beam;
}

namespace IO {
    class Settings;
}

namespace ResBos {

class Process;
class Cuts;
class HistogramCollection;

class ResBos {
    public:
        // ResBos constructors and destructors
        ResBos();
        ResBos(IO::Settings*,std::unique_ptr<Process>,
               std::unique_ptr<Calculation>,std::pair<Beam::Beam*,Beam::Beam*>);
        virtual ~ResBos();

        // ResBos functions
        virtual std::function<double(const std::vector<double>&, const double&)> GetXSect();
        virtual double XSect(const std::vector<double>&, const double&);
        virtual void ToggleWrite() {writeHists_ = !writeHists_;}
        virtual void ToggleWriteLHE() {writeEvent_ = !writeEvent_;}
        virtual bool SetupLHEOut();
        virtual bool CloseLHEOut(const double&, const double&);

        // Process interface
        virtual void SetProcess(std::unique_ptr<Process> p) {process = std::move(p);}
        virtual Process* GetProcess() const {return process.get();} 
        virtual double H(const size_t&, const double&, const double&) const;

        // Setting interface
        virtual void SetSettings(IO::Settings*);
        virtual IO::Settings* GetSettings() const {return settings;}
        template<typename SettingsType>
        SettingsType GetSetting(const std::string& key) const {
            return settings -> GetSetting<SettingsType>(key);
        }

        // Beam interface
        virtual double GetECM() const;
        virtual void SetBeams(std::pair<Beam::Beam*,Beam::Beam*> b) {beams = b;}
        virtual std::pair<Beam::Beam*, Beam::Beam*> GetBeams() const {return beams;} 
        virtual double GetAlpi(const double&) const;
        virtual size_t GetNF(const double&) const;
        virtual std::string GetPDFName() const;
        virtual int GetISet() const;

        // Calculation interface
        virtual void SetCalculation(std::unique_ptr<Calculation> c) {calc = std::move(c);}
        virtual Calculation* GetCalculation() const {return calc.get();} 

        // MCFM Interface pieces
//        static ResBos& MCFMInstance();
        virtual bool SetupMCFMInterface(std::string);
        virtual double GetMatrixElement(double*) const;
        virtual void InitElectroweak(double mZ, double mW, double GF);

        // Histogram Interface
        virtual bool FinalizeHist(); 
        virtual bool SaveHists() const;

    private:
        // Helper functions for event generation
//        bool Cuts(const std::vector<FourVector>&) const;
        bool WriteHists() const;
        bool WriteEvent() const;
//        bool FillHists(const std::vector<FourVector>&,const double&) const;

        // Variables
        bool writeHists_, writeEvent_;
        IO::Settings* settings; 
        std::unique_ptr<Process> process;
        std::unique_ptr<Calculation> calc;
        std::pair<Beam::Beam*,Beam::Beam*> beams;
        std::unique_ptr<Cuts> cuts;
        std::unique_ptr<HistogramCollection> hists;
        std::shared_ptr<Event> event;
        std::unique_ptr<LHEF3FromResBos> lhe;

        // Mutexes
        std::mutex LHEWriteMutex;
};

}

#endif
