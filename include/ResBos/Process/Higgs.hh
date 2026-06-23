#ifndef PROCESS_HIGGS_HH
#define PROCESS_HIGGS_HH

#include "ResBos/Grid1D.hh"
#include "ResBos/Process.hh"

namespace ResBos {

enum class HiggsDecay {WW, ZZ, AA, ZA, GG, BB, TauTau, MuMu, SS, CC, TT, Width};

class Higgs : public Process {
    public:
        static std::unique_ptr<Process> Create(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) {
            return std::unique_ptr<Higgs>(new Higgs(resbos_,settings));
        }

        inline static std::string GetName() { return "Higgs"; }
        inline bool IsRegistered() { return registered; }
        void Initialize(const IO::Settings&);
        void InitializeMCFM();

        // Information about the calculation
        Partons GetPartons(const size_t &i = 0) const;
        inline size_t GetChannels() const { return nChannels; }
        inline size_t GetResumAngular() const { return nResumAngular; }
        inline size_t GetNAngular() const { return nAngular; }
        inline InitialState GetInitState() const { return initState; }
        inline const std::vector<int>& PIDs() const { return pids; }

        // Information pertaining to the hard part and matrix element
        inline size_t GetNDim() const { return nDim; }
        inline int GetAlphasCoupling() const { return 2; }
        double H1() const;
        double H2(const double &Q) const;
        double H2(const size_t&) const { return 0; }
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
        Higgs(std::shared_ptr<ResBos>, const IO::Settings &);
        static bool registered;

        // Perturbative functions
        double Amp2GG(const double&, const double&, const double&, const double&) const;
        double Amp2GQ(const double&, const double&, const double&, const double&) const;
        double Amp2QQ(const double&, const double&, const double&, const double&) const;

        // Process detail variables
        const size_t nChannels = 1, nAngular = 1, nResumAngular = 1;
        size_t nDim;
        const InitialState initState = InitialState::gg;
        HiggsDecay higgsDecay;        
        double MassCorrections(const double&) const;
        double BrH2AA(const double&) const;

        std::map<HiggsDecay,Utility::Grid1D> BrHGrid;

        // Particles produced
        std::vector<int> pids;
};

/*
class HiggsJet : public Process {
    public:
        HiggsJet(ResBos*, const IO::Settings &);

        // Information about the calculation
        virtual Partons GetPartons(int i = 0) const;
        virtual int GetChannels() const;
        virtual InitialState GetInitState() const;

        // Information pertaining to the hard part and matrix element
        virtual int GetNDim() const;
        virtual double H1() const;
        virtual double H2(const double Q) const;
        virtual double H2(const int nf = 5) const;
        virtual double ME(std::vector<double>,std::vector<double>) const;
        virtual double GetCoupling(const double) const;
        virtual double CxFCxF(double,double,double,int) const;
        virtual double GetPhaseSpacePoint(double *x, const std::vector<double>&,
                                          std::vector<FourVector>&) const;

    private:
        // Factory variables

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};

class HiggsAssociated : public Process {
    public:
        HiggsAssociated(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new HiggsAssociated(r,s);
        }

        // Information about the calculation
        virtual Partons GetPartons(int i = 0) const;
        virtual int GetChannels() const;
        virtual InitialState GetInitState() const;

        // Information pertaining to the hard part and matrix element
        virtual int GetNDim() const;
        virtual double H1() const;
        virtual double H2(const double Q) const;
        virtual double H2(const int nf = 5) const;
        virtual double ME(std::vector<double>,std::vector<double>) const;
        virtual double GetCoupling(const double) const;
        virtual double CxFCxF(double,double,double,int) const;
        virtual double GetPhaseSpacePoint(double *x, const std::vector<double>&,
                                          std::vector<FourVector>&) const;

    private:
        // Factory variables

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};

class HiggsW : public HiggsAssociated {
    public:
        HiggsW(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new HiggsW(r,s);
        }

        // Information about the calculation
        virtual Partons GetPartons(int i = 0) const;
        virtual int GetChannels() const;
        virtual InitialState GetInitState() const;

        // Information pertaining to the hard part and matrix element
        virtual int GetNDim() const;
        virtual double H1() const;
        virtual double H2(const double Q = 0) const;
        virtual double ME(std::vector<double>,std::vector<double>) const;
        virtual double GetCoupling(const double) const;
        virtual double CxFCxF(double,double,double,int) const;
        virtual double GetPhaseSpacePoint(double *x, const std::vector<double>&,
                                          std::vector<FourVector>&) const;

    private:
        // Factory variables

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};

class HiggsZ : public HiggsAssociated {
    public:
        HiggsZ(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new HiggsZ(r,s);
        }

        // Information about the calculation
        virtual Partons GetPartons(int i = 0) const;
        virtual int GetChannels() const;
        virtual InitialState GetInitState() const;

        // Information pertaining to the hard part and matrix element
        virtual int GetNDim() const;
        virtual double H1() const;
        virtual double H2(const double Q = 0) const;
        virtual double ME(std::vector<double>,std::vector<double>) const;
        virtual double GetCoupling(const double) const;
        virtual double CxFCxF(double,double,double,int) const;
        virtual double GetPhaseSpacePoint(double *x, const std::vector<double>&,
                                          std::vector<FourVector>&) const;

    private:
        // Factory variables

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};

class HiggsBbar : public Process {
    public:
        HiggsBbar(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new HiggsBbar(r,s);
        }

        // Information about the calculation
        virtual Partons GetPartons(int i = 0) const;
        virtual int GetChannels() const;
        virtual InitialState GetInitState() const;

        // Information pertaining to the hard part and matrix element
        virtual int GetNDim() const;
        virtual double H1() const;
        virtual double H2(const double Q = 0) const;
        virtual double ME(std::vector<double>,std::vector<double>) const;
        virtual double GetCoupling(const double) const;
        virtual double CxFCxF(double,double,double,int) const;
        virtual double GetPhaseSpacePoint(double *x, const std::vector<double>&,
                                          std::vector<FourVector>&) const;

    private:
        // Factory variables

        double CAA_E[2],CAA_O[2],CZA_E[2],CZA_O[2],CZZ_E[2],CZZ_O[2];
};
*/
}

#endif
