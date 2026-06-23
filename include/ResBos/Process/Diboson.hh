#ifndef PROCESS_DIBOSON_HH
#define PROCESS_DIBOSON_HH

#include "ResBos/Process.hh"

namespace ResBos {

class Diboson : public Process {
    public:
        Diboson(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new Diboson(r,s);
        }

        // Information about the calculation
        virtual Partons GetPartons(int i = 0) const;
        virtual int GetChannels() const;
        virtual InitialState GetInitState() const;

        // Information pertaining to the hard part and matrix element
        virtual int GetNDim() const;
        virtual int GetAlphasCoupling() const {return 0;}
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

class ZZ : public Diboson {
    public:
        ZZ(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new ZZ(r,s);
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

class WW : public Diboson {
    public:
        WW(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new WW(r,s);
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

class AA : public Diboson {
    public:
        AA(ResBos::ResBos*, const IO::Settings &);
        static Process* create(ResBos::ResBos* r, const IO::Settings &s) {
            return new AA(r,s);
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

}

#endif
