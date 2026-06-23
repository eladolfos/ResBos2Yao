#ifndef PROCESS_WPM_HH
#define PROCESS_WPM_HH

#include "ResBos/Process.hh"

namespace ResBos {

class Wpm : public Process {
    public:
        void Initialize(const IO::Settings&);
        void InitializeMCFM() { return; }

        // Information about the calculation
        inline InitialState GetInitState() const { return initState; }
        inline size_t GetChannels() const { return nChannels; }
        inline size_t GetNAngular() const { return nAngular; }
        inline size_t GetResumAngular() const { return nResumAngular; }
        inline virtual const std::vector<int>& PIDs() const { return pids; }

        // Information pertaining to the hard part and matrix element
        inline size_t GetNDim() const { return nDim; }
        inline int GetAlphasCoupling() const { return 0; }
        double H1() const;
        double H2(const double &Q) const;
        double H2(const size_t &nf = 5) const;
        double ME(const std::vector<double>&, const std::vector<double>&) const;
        inline double GetCoupling(const double&) const { return coupling; }
        double CxFCxF(const double&, const double&, const double&, const size_t&) const;
        double GetPhaseSpacePoint(const std::vector<double>&, std::vector<double>&,
                                  std::vector<FourVector>&) const;
        double GetPert(const double&, const double&, const double&, const double&, const double&,
                       const double&, const double&, const double&, const size_t&) const;

        // Function to generate an event to be passed to LHE
        void MakeEvent(Event&, const std::vector<FourVector>&);

    protected:
        // Factory variables
        Wpm(std::shared_ptr<ResBos>,const IO::Settings&);

        // Perturbative functions
        double Amp2QQ(const double&, const double&, const double&, const double&, const double&,
                      const size_t&) const;
        double Amp2GQ(const double&, const double&, const double&, const double&, const double&,
                      const size_t&) const;

        const size_t nChannels = 1, nDim = 5, nAngular = 5, nResumAngular = 2;
        double coupling;
        const InitialState initState = InitialState::qq;

        double fL2;
        int sign;
        std::vector<int> pids;
};

class WPlus : public Wpm {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<WPlus>(new WPlus(resbos_,settings));
        }

        inline static std::string GetName() { return "WPlus"; }
        inline bool IsRegistered() { return registered; }

        // Information about the calculation
        Partons GetPartons(const size_t &i = 0) const;

    private:
        // Factory variables
        WPlus(std::shared_ptr<ResBos> resbos_, const IO::Settings& settings) : Wpm(resbos_,settings) {
            name = "WPlus";
            sign = 1;
            pids = {24,-11,12};
        };
        static bool registered;

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};

class WMinus : public Wpm {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<WMinus>(new WMinus(resbos_,settings));
        }

        inline static std::string GetName() { return "WMinus"; }
        inline bool IsRegistered() { return registered; }

        // Information about the calculation
        Partons GetPartons(const size_t &i = 0) const;

    private:
        // Factory variables
        WMinus(std::shared_ptr<ResBos> resbos_, const IO::Settings& settings) : Wpm(resbos_,settings) { 
            name = "WMinus";
            sign = -1;
            pids = {-24,11,-12};
        };
        static bool registered;

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};

}

#endif
