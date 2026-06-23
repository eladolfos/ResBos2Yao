#ifndef RESUMMATION_HH
#define RESUMMATION_HH 

#include <vector>
#include <cmath>
#include <unordered_map>
#include <array>

#include "ResBos/Calculation.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Utility.hh"
#include "ResBos/OgataQuad.hh"

#ifdef FITTING
#include "ResBos/Grid3D.hh"

namespace Fitting {

using key_t = std::array<double, 3>;

struct key_hash {
    std::size_t operator()(const key_t &key) const noexcept {
        std::size_t hash{};
        for(const auto &elm : key)
            hash = hash ^ (std::hash<double>{}(elm) << 1);
        return hash;
    }
};

using map_t = std::unordered_map<key_t,std::vector<double>,key_hash>;

}
#endif

namespace IO {
    class Settings;
}

namespace ResBos {
    typedef std::vector<std::pair<int,int>> Partons;
    class Process;
    class ResBos;

    // Contain the first n zeroes of the Bessel funciton, plus the origin
    const std::vector<double> besselZero = {
                 0, 2.4048256, 5.5200781, 8.6537279,11.7915344,14.9309177,18.0710640,
        21.2116366,24.3524715,27.4934791,30.6346065,33.7758202,36.9170984,40.0584258,
        43.1997917,46.3411884,49.4826099,52.6240518,55.7655108,58.9069839,62.0484692,
        65.1899648,68.3314693,69.9022245,73.6145006,77.7560256,80.8975559,84.0390908,
        87.1806298,90.3221726,93.4637188,96.6052680,99.7468199,102.888374,106.029931,
        109.171490,112.313050,115.454613,118.596177,121.737742,124.879309,128.020877,
        131.162446,134.304017,137.445588,140.587160,143.728734,146.870308,150.011883,
        153.153458,156.295034,159.436611,162.578189,165.719767,168.861345,172.002924,
        175.144504,178.286084,181.427665,184.569246,187.710827};
    const size_t nBesselJ = besselZero.size();

    class Resummation : virtual public Calculation {
        public:
            // Initialize Resummation class
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<Resummation>(new Resummation());
            }
            void Initialize(IO::Settings*, std::shared_ptr<ResBos>);

            static std::string GetName() {return "Resummation";}
            std::vector<Conv> GetNeededConvs() const;
            bool IsRegistered() {return registered;}

            // public function to get the result of the resummation calculation at a fixed
            // value of Q, qt, and y
            virtual std::vector<double> GetCalc(const double&, const double&, const double&);
            virtual void BSpacePlot(const double&, const double&);

            // Setters
            void SetNonPert(const NonPertEnum &iNonPert_) {iNonPert = iNonPert_;}
            void SetNonPertCoefficients(const std::vector<double> &g_) {g = g_;}      
            void SetBMax(const double &bMax_) {bMax = bMax_;}
            void SetYMax(const double &yMax_) {NonPertYMax = yMax_;}//yfu
            void SetQ0(const double &Q0_) {Q0 = Q0_;}

#ifdef FITTING
            void ClearMap() { SudPertMap.clear(); SudPertMap.rehash(0); }
#endif

        protected:
            Resummation() { ogata = Utility::OgataQuad(); }
            static bool registered;

        private:
            // Private functions
            double FTransform(const double&, const double&, const double&,
                              const double&, const size_t&) const;
#ifndef FITTING
            std::vector<double> FTransformVec(const double&, const double&, 
                                              const double&, const double&) const;
#else
            std::vector<double> FTransformVec(const double&, const double&, 
                                              const double&, const double&);
            Fitting::map_t SudPertMap;
#endif
            double FTransformHigh(const double,const double,const double,
                    const double,const double,const int) const;
            double Sudakov(const double&,const double&) const;
            double SudInt(const double&,const double&) const;
            double CxFCxF(const double,const double,const double,const int) const;
            double NonPert(const double&,const double&,const double&,const double&) const;
            inline double GetBStar(const double b, const double mui=bMax) const 
                {return b/sqrt(1+pow(b/mui,2));}
            double GetA(const size_t&, const double&, const double&) const;
            double GetB(const size_t&, const double&, const double&, const double&) const;

            // Static private variables
            static constexpr double p00=1.0,p02=-1.0/9.0/2.0/(8.0*8.0),
                             p04=9.0*25.0*49.0/24.0/Utility::ipow(8.0,4);
            static constexpr double q01=-1.0/8.0,q03=9.0*25.0/6.0/Utility::ipow(8.0,3);
            static constexpr double sudErr=1E-4;
            Utility::OgataQuad ogata;

        protected:
            // Protected variables
            NonPertEnum iNonPert;
            std::vector<double> g;
            static double bMax, Q0, rerr, aerr;
    };

}

#endif
