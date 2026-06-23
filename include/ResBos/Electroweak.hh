#ifndef ELECTROWEAK_HH
#define ELECTROWEAK_HH

#include <array>
#include <complex>
#include <map>

#include "ResBos/Enums.hh"

namespace IO {
    class Settings;
}

class Electroweak {
    public:
        // Constructors and Destructors
        Electroweak() {};
        Electroweak(const IO::Settings *s);
        virtual ~Electroweak() {};

        // Init functions
        virtual void initAlpha(const IO::Settings*);

        // MCFM Init
        virtual void MCFMInit(double mzIn, double mwIn, double GFin);

        // Getters
        virtual inline std::complex<double> GetAlpha0() const {return alpha0;}
        virtual inline std::complex<double> GetSW2() const {return sws;}
        virtual inline std::complex<double> GetCW2() const {return cws;}
        virtual inline double GetGF() const {return GF;}

        virtual inline std::complex<double> GetZCoupl() const {return zcoupl;}
        virtual inline std::complex<double> GetACoupl() const {return acoupl;}
        virtual inline std::complex<double> GetWCoupl() const {return wcoupl;}

        virtual inline std::complex<double> GetMzComplex() const {return mZ;}
        virtual inline double GetMzReal() const {return real(mZ);}
        virtual inline double GetGamZ() const {return gamZ;}

        virtual inline std::complex<double> GetMwComplex() const {return mW;}
        virtual inline double GetMwReal() const {return real(mW);}
        virtual inline double GetGamW() const {return gamW;}

        virtual inline std::complex<double> GetMhComplex() const {return mH;}
        virtual inline double GetMhReal() const {return real(mH);}
        virtual inline double GetGamH() const {return gamH;}

        virtual std::complex<double> GetMtComplex() const {return mT;}
        virtual double GetMtReal() const {return real(mT);}
        virtual double GetGamT() const {return gamT;}

        virtual inline double GetCKM(int pid1, int pid2) const {return CKM.at(std::abs(pid1*pid2));}
        virtual double GetAlphaEM(double) const;

    private:
        void CalculateParams();

        std::complex<double> sws, cws, mW, mZ, alpha0, alphaMz, sw, cw, mH, mT;
        std::complex<double> acoupl, zcoupl, wcoupl;
        EWMassScheme massScheme;
        EWInputs inputs;
        double GF, gamW, gamZ, gamH, gamT;
        double alphaEM0, alphaEMMz;
        int alphaEMOrder;
        const std::array<double,5> Q2Step{{0.26e-6, 0.011, 0.25, 3.5, 90.}};
        std::array<double,5> betaEM{{0.1061, 0.2122, 0.460, 0.700, 0.725}};
        std::array<double,5> alphaEMStep;
        std::map<int,double> CKM;
};

#endif
