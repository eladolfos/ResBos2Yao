#ifndef PHASESPACE_HH
#define PHASESPACE_HH

#include <vector>
#include <map>
#include <iostream>

#include "ResBos/Random.hh"

class FourVector;

class PhaseSpace {
    public:
        // Constructors and Destructors
        PhaseSpace();
        PhaseSpace(const double& S, const size_t& iMonte=2, 
                   const size_t& iQt=0, const size_t& nPrimaryPart=2);
        virtual ~PhaseSpace() {
            delete rand;
        }

        // Phase Space generators
        virtual double PhaseGen(const std::vector<double> &x, std::vector<FourVector> &p) const;
        virtual double PhaseSpace3(const std::vector<double>  &x, double &lx1, 
                                   double &lx2, std::vector<FourVector> &p);
        virtual double x2Phase(double*, const double&, const double&,
                               const double&, FourVector&, FourVector&) const;
        virtual double Wresph(const std::vector<double>&, const double&,
                              std::vector<double>&, std::vector<FourVector>&);
//        std::vector<double> GetPSPoint() { return phaseSpace; };
//        std::vector<FourVector> GetMomentum() { return p;};

        //Setters
        virtual void SetECM(const double &ECM) {S = ECM*ECM;}
        virtual void SetNPrimaryPart(const size_t &nPrimaryPart_) {
            nPrimaryPart = nPrimaryPart_;
        }
        virtual void SetNSecondaryPart(const size_t &nSecondaryPart_) {
            nSecondaryPart = nSecondaryPart_;
        }
        virtual void SetIMonte(const size_t &iMonte_) {iMonte = iMonte_;}
        virtual void SetIQt(const size_t &iQt_) {iQt = iQt_;}
        virtual void SetPhiDep(const bool &phiDep_) {phiDep = phiDep_;}
        virtual void SetMass(const std::vector<double> &mass_) {mass = mass_;}
        virtual void SetQRange(const double &Qmin, const double &Qmax) {qMin = Qmin; qMax = Qmax;}
        virtual void SetQtRange(const double &Qtmin, const double &Qtmax) {
            qtMin = Qtmin; qtMax = Qtmax;
        }
        virtual void SetRandomGenerator(Random* random) {rand = random;}

    private:
        double YMaximum(const double &q, const double &qT) const;
        double FillMomenta3(const std::array<double,5>  &r, std::vector<FourVector> &p) const;
        FourVector GetYVect(const double &dQ, const FourVector &Q, 
                            const FourVector &X, const FourVector &Z) const;
        void AdditionalDecays() {return;}

        double S, qMin, qMax, qtMin, qtMax;
        size_t iMonte, iQt, nPrimaryPart, nSecondaryPart;
//        std::vector<FourVector> p;
        std::vector<double> mass;//, phaseSpace;
        bool phiDep;
        Random *rand;

};

#endif
