#ifndef CALCULATION_HH
#define CALCULATION_HH

#include <vector>
#include <map>
#include <functional> 
#include <mutex>
#include <cmath>
#include <unordered_map>

#include "ResBos/Enums.hh"
#include "ResBos/ThreadPool.hh"
#include "ResBos/Beam.hh"

namespace IO {
    class Settings;
}

namespace Utility {
    class Grid3D;
    class Grid2D;
    class PDF;
    class Hoppet;
}

class PhaseSpace;

namespace ResBos {
    typedef std::map<int,Utility::Grid3D*> MapGrid3D;

    class Process;
    class Convolution;
    
    class ResBos {
        public:
            // Initialize the ResBos class for preforming the calculations
            ResBos();
            ResBos(const IO::Settings &s, Process *p);
            void Setup(Process *p, const IO::Settings &s);
            void GridSetup(Process *p, const IO::Settings &s, std::map<std::thread::id,Utility::PDF*> pdfs,Utility::Hoppet*);

            // Getters
            inline int GetAsymOrder() const {return AsymOrder;}
            inline int GetDeltaSigmaOrder() const {return DeltaSigmaOrder;}
            inline int GetAOrder() const {return AOrder;}
            inline int GetBOrder() const {return BOrder;}
            inline int GetCOrder() const {return COrder;}
            inline int GetHOrder() const {return HOrder;}
            inline double GetA1() const {return A1;}
            inline double GetA2(int nf) const {return A2[nf-3];}
            inline double GetA3(int nf) const {return A3[nf-3];}
            inline double GetB1(int nf) const {return B1[nf-3];}
            inline double GetB2(int nf) const {return B2[nf-3];}
            inline double GetB3(int nf) const {return B3[nf-3];}
            inline double GetC1() const {return C1;}
            inline double GetC2() const {return C2;}
            inline double GetC3() const {return C3;}
            inline double GetMuR() const {return muR;}
            inline double GetMuF() const {return muR;}
            inline Scheme GetScheme() const {return scheme;}
            virtual std::vector<Conv> GetNeededConvs() const = 0;
            std::vector<double> GetPoint(double,double,double);
            std::pair<BDFMap,BDFMap> CalcMap(int pid1, int pid2, double x1, double x2, double Q) const;
            double GetConvolution(int hadron, int pid, double x, double Q, Conv c) const;

            // Setters
            inline void SetAsymOrder(int order) {AsymOrder = order;}
            inline void SetDeltaSigmaOrder(int order) {DeltaSigmaOrder = order;}
            inline void SetAOrder(int AOrder) {this -> AOrder = AOrder;}
            inline void SetBOrder(int BOrder) {this -> BOrder = BOrder;}
            inline void SetCOrder(int COrder) {this -> COrder = COrder;}
            inline void SetHOrder(int HOrder) {this -> HOrder = HOrder;}
            inline void SetC1(double C1) {this -> C1 = C1;}
            inline void SetC2(double C2) {this -> C2 = C2;}
            inline void SetC3(double C3) {this -> C3 = C3;}
            inline void SetMuR(double muR) {this -> muR = muR;}
            inline void SetMuF(double muF) {this -> muF = muF;}
            inline void SetResumScales(double C1, double C2, double C3) {
                SetC1(C1); SetC2(C2); SetC3(C3);
            }
            inline void SetScales(double C1, double C2, double C3, double muR, double muF) {
                SetResumScales(C1,C2,C3); SetMuR(muR); SetMuF(muF);
            }
            inline void SetScheme(Scheme s) {scheme = s;}
            inline void SetProcess(Process *p) {process = p;}

            // Other
            double GetYMax(double,double);
            double GetYMax(double);
            std::function<double(double*,double)> GetXSect();
            virtual double XSect(double*,double);
            virtual std::vector<double> GetCalc(double,double,double) = 0;
            virtual void GridGen();

            // Selector for different calculation types
            typedef std::unordered_map<std::string,void*> registryMap;
            virtual ~ResBos() = default;

            static registryMap & registry(){
                static registryMap impl;
                return impl;
            }
            template<typename ...T>
            static ResBos *Instantiate(std::string const &name, T&&...args) {
                auto it = registry().find(name);
                if(it == registry().end()) return 0;
                typedef ResBos* (*createType)(T...);
                auto createFun = reinterpret_cast<createType>(it->second);
                return createFun(args...);
            }

        protected:
            double H(const int, const double);
            Process *process;
            std::vector<double> qVals, qTVals, yVals;
            GridType gType; 
            MapGrid3D grid;
            std::pair<Beam,Beam> beams;

        private:
            Scheme scheme;
            int AsymOrder, DeltaSigmaOrder;
            double C1, C2, C3, muR, muF;
            int AOrder, BOrder, COrder, HOrder;
            double A1;
            std::vector<double> A2, A3, B1, B2, B3;
            double qMin, qMax, qTMin, qTMax, yMin, yMax;
    };

    struct Registrar {
        template<typename F>
        Registrar(std::string name, F func) {
            ResBos::registry()[name] = reinterpret_cast<void*>(func);
        }
    };
}

#endif
