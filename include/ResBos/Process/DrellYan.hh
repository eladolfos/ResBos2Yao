#ifndef PROCESS_DRELLYAN_HH
#define PROCESS_DRELLYAN_HH

#include "ResBos/Process.hh"

namespace ResBos {

class DrellYan : public Process {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<DrellYan>(new DrellYan(resbos_,settings));
        }

        static inline std::string GetName() { return "DrellYan"; }
        bool inline IsRegistered() { return registered; }
        void Initialize(const IO::Settings&);
        void InitializeMCFM();

        // Information about the calculation
        virtual Partons GetPartons(const size_t &i = 0) const;
        virtual inline size_t GetChannels() const { return nChannels; }
        inline size_t GetNAngular() const { return nAngular;}
        inline size_t GetResumAngular() const { return nResumAngular; }
        inline InitialState GetInitState() const { return initState; }
        inline const std::vector<int>& PIDs() const { return pids; }

        // Information pertaining to the hard part and matrix element
        inline size_t GetNDim() const { return nDim; }
        inline int GetAlphasCoupling() const { return 0; }
        double H1() const;
        double H2(const double &Q) const;
        double H2(const size_t &nf = 5) const;
        double ME(const std::vector<double>&, const std::vector<double>&) const;
        inline double GetCoupling(const double&) const { return coupling; }
        virtual double CxFCxF(const double&, const double&, const double&, const size_t&) const;
        double GetPhaseSpacePoint(const std::vector<double>&, std::vector<double>&,
                                  std::vector<FourVector>&) const;
        double GetPert(const double&, const double&, const double&, const double&, const double&,
                       const double&, const double&, const double&, const size_t&) const;
        double NonPert(const std::array<double, 2>&, const std::vector<double>&, bool) const;
        std::array<double, 4> CalcCorrection(double, const std::vector<double>&, bool) const;

        // Function to generate an event to be passed to LHE
        void MakeEvent(Event&, const std::vector<FourVector>&);

    protected:
        // Factory variables
        DrellYan(std::shared_ptr<ResBos>,const IO::Settings&);
        static bool registered;

        // Perturbative functions
        double Amp2QQ(const double&, const double&, const double&, const double&,
                      const double&, const size_t&) const;
        double Amp2GQ(const double&, const double&, const double&, const double&,
                      const double&, const size_t&) const;

        const size_t nChannels = 2, nDim = 5, nAngular = 5, nResumAngular = 2;
        const double coupling = 0.5;
        const InitialState initState = InitialState::qq;

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
        double QL_Z1, QL_Z2, QR_Z1, QR_Z2;
        double branching_ratio;

        // Particles produced
        const std::vector<int> pids{23,11,-11};
};

class ZU : public DrellYan {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<ZU>(new ZU(resbos_,settings));
        }

        static inline std::string GetName() { return "ZU"; }
        bool inline IsRegistered() { return registered; }

        // Information about the calculation
        Partons GetPartons(const size_t &i = 0) const;
        inline size_t GetChannels() const { return nChannels; }

        // Information pertaining to the hard part and matrix element
        virtual double CxFCxF(const double&, const double&, const double&, const size_t&) const;

    private:
        // Factory variables
        ZU(std::shared_ptr<ResBos> resbos_, const IO::Settings& settings) : DrellYan(resbos_, settings) {
            name = "ZU";
        };
        static bool registered;

        const size_t nChannels = 1;
};

class ZD : public DrellYan {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<ZD>(new ZD(resbos_,settings));
        }

        inline static std::string GetName() { return "ZD"; }
        inline bool IsRegistered() { return registered; }

        // Information about the calculation
        Partons GetPartons(const size_t &i = 0) const;
        inline size_t GetChannels() const { return nChannels; }

        // Information pertaining to the hard part and matrix element
        virtual double CxFCxF(const double&, const double&, const double&, const size_t&) const;

    private:
        // Factory variables
        ZD(std::shared_ptr<ResBos> resbos_, const IO::Settings& settings) : DrellYan(resbos_, settings) {
            name = "ZD";
        };
        static bool registered;

        const size_t nChannels = 1;
};

class A0 : public Process {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<A0>(new A0(resbos_,settings));
        }

        static std::string GetName() { return "A0"; }
        bool IsRegistered() { return registered; }
        void Initialize(const IO::Settings&);
        void InitializeMCFM() { return; }

        // Information about the calculation
        int GetAlphasCoupling() const { return 0; }
        Partons GetPartons(const size_t &i = 0) const;
        size_t GetChannels() const { return nChannels; }
        size_t GetNAngular() const { return nAngular; }
        size_t GetResumAngular() const { return nResumAngular; }
        InitialState GetInitState() const { return initState; }
        const std::vector<int>& PIDs() const { return pids; }
        double Charge(const int&, const size_t&) const;

        // Information pertaining to the hard part and matrix element
        size_t GetNDim() const { return nDim; }
        double H1() const;
        double H2(const double &Q) const;
        double H2(const size_t &nf = 5) const;
        double ME(const std::vector<double>&, const std::vector<double>&) const;
        double GetCoupling(const double&) const;
        double CxFCxF(const double&, const double&, const double&, const size_t&) const;
        double GetPhaseSpacePoint(const std::vector<double>&, std::vector<double>&,
                                          std::vector<FourVector>&) const;
        double GetPert(const double&, const double&, const double&, const double&, const double&,
                       const double&, const double&, const double&, const size_t&) const;

        // Function to generate an event to be passed to LHE
        void MakeEvent(Event&, const std::vector<FourVector>&);

    private:
        // Factory variables
        A0(std::shared_ptr<ResBos> resbos_, const IO::Settings& settings);
        static bool registered;

        // Perturbative functions
        double Amp2QQ(const double&, const double&, const double&, const double&,
                      const double&, const size_t&) const;
        double Amp2GQ(const double&, const double&, const double&, const double&,
                      const double&, const size_t&) const;

        const size_t nChannels = 1, nDim = 5, nAngular = 5, nResumAngular = 2;
        const double coupling = 0.5;
        const InitialState initState = InitialState::qq;

        double C_E[2],C_O[2];
        double alpha(const double&) const; 
        const std::map<int,double> charge = {
            {1,1.0/9.0},
            {2,4.0/9.0},
            {3,1.0/9.0},
            {4,4.0/9.0},
            {5,1.0/9.0}
        };

        // Particles produced
        const std::vector<int> pids{22,11,-11};
};

class Z0 : public Process {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<Z0>(new Z0(resbos_,settings));
        }

        inline static std::string GetName() { return "Z0"; }
        inline bool IsRegistered() { return registered; }
        void Initialize(const IO::Settings&);
        void InitializeMCFM() { return; };

        // Information about the calculation
        inline int GetAlphasCoupling() const { return 0; }
        Partons GetPartons(const size_t &i = 0) const;
        inline size_t GetChannels() const { return nChannels; }
        inline size_t GetNAngular() const { return nAngular; }
        inline size_t GetResumAngular() const { return nResumAngular; }
        inline InitialState GetInitState() const { return initState; }
        inline const std::vector<int>& PIDs() const { return pids; }
        double Charge(const int&, const size_t&) const;

        // Information pertaining to the hard part and matrix element
        inline size_t GetNDim() const { return nDim; }
        double H1() const;
        double H2(const double &Q) const;
        double H2(const size_t &nf = 5) const;
        double ME(const std::vector<double>&, const std::vector<double>&) const;
        double GetCoupling(const double&) const;
        double CxFCxF(const double&, const double&, const double&, const size_t&) const;
        double GetPhaseSpacePoint(const std::vector<double>&, std::vector<double>&,
                                  std::vector<FourVector>&) const;
        double GetPert(const double&, const double&, const double&, const double&, const double&,
                       const double&, const double&, const double&, const size_t&) const;

        // Function to generate an event to be passed to LHE
        void MakeEvent(Event&, const std::vector<FourVector>&);

    private:
        // Factory variables
        Z0(std::shared_ptr<ResBos> resbos_, const IO::Settings& settings);
        static bool registered;

        // Perturbative functions
        double Amp2QQ(const double&, const double&, const double&, const double&,
                      const double&, const size_t&) const;
        double Amp2GQ(const double&, const double&, const double&, const double&,
                      const double&, const size_t&) const;

        const size_t nChannels = 1, nDim = 5, nAngular = 5, nResumAngular = 2;
        const double coupling = 0.5;
        const InitialState initState = InitialState::qq;

        double C_E[2],C_O[2];
        std::map<int,double> ZFFA, ZFFV;

        // Particles produced
        const std::vector<int> pids{23,11,-11};
};

}

#endif
