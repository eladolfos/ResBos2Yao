#ifndef CALCULATION_HH
#define CALCULATION_HH

#include <vector>
#include <map>
#include <functional> 
#include <mutex>
#include <cmath>
#include <unordered_map>
#include <memory>

#include <iostream>

#include "ResBos/Beam.hh"
#include "ResBos/Enums.hh"
#include "ResBos/QCDConst.hh"

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
    typedef std::map<size_t,Utility::Grid3D*> MapGrid3D;
    class ResBos;
    
    class Calculation {
        public:
            // Initialize the ResBos class for preforming the calculations
            Calculation() {
                resbos = NULL;
            }
            virtual ~Calculation();
            virtual void Initialize(IO::Settings*, std::shared_ptr<ResBos>);
            virtual bool IsRegistered() = 0;

            // Getters
            virtual inline size_t GetAsymOrder() const {return AsymOrder;}
            virtual inline size_t GetPertOrder() const {return PertOrder;}
            virtual inline size_t GetDeltaSigmaOrder() const {return DeltaSigmaOrder;}
            virtual inline size_t GetAOrder() const {return AOrder;}
            virtual inline size_t GetBOrder() const {return BOrder;}
            virtual inline size_t GetCOrder() const {return COrder;}
            virtual inline size_t GetHOrder() const {return HOrder;}

            virtual inline double GetA1() const {return m_A1;}
            virtual inline double GetA2(const size_t &nf, const double &C1_=QCD::B0) const {
                if(C1_ == QCD::B0) return m_A2[nf-3];
                return m_A2[nf-3]-2*QCD::beta0(nf)*m_A1*log(QCD::B0/C1_);
            }
            virtual inline double GetA3(const size_t &nf, const double &C1_=QCD::B0) const {
                if(C1_ == QCD::B0) return m_A3[nf-3];
                return m_A3[nf-3]+4*pow(QCD::beta0(nf)*log(QCD::B0/C1_),2)*m_A1
                     - 2*log(QCD::B0/C1_)*(QCD::beta1(nf)*m_A1+2*QCD::beta0(nf)*m_A2[nf-3]);
            }
            virtual inline double GetA4(const size_t &nf, const double& C1_=QCD::B0) const {
                if(C1_ == QCD::B0) return m_A4[nf-3];
                const double logc1 = log(QCD::B0/C1_);
                return m_A4[nf-3]-8*m_A1*pow(logc1*QCD::beta0(nf), 3)
                                 +2*pow(logc1, 2)*QCD::beta0(nf)*(5*m_A1*QCD::beta1(nf)
                                                                  +6*m_A2[nf-3]*QCD::beta0(nf))
                                 -2*logc1*(m_A1*QCD::beta2(nf)+2*m_A2[nf-3]*QCD::beta1(nf)+3*m_A3[nf-3]*QCD::beta0(nf));
            }
            virtual inline double GetA5(const size_t &nf, const double& /*C1_=QCD::B0*/) const {
                return m_A5[nf-3];
            }

            virtual inline double GetB1(const size_t &nf, const double& C1_=QCD::B0,
                                        const double &C2_=1.0) const {
                if(C1_ == QCD::B0 && C2_ == 1.0) return m_B1[nf-3];
                return m_B1[nf-3]-2*m_A1*log(QCD::B0*C2_/C1_);
            }
            virtual inline double GetB2(const size_t &nf, const double& C1_=QCD::B0,
                                        const double &C2_=1.0) const {
                if(C1_ == QCD::B0 && C2_ == 1.0) return m_B2[nf-3];
                return m_B2[nf-3]-2*m_A2[nf-3]*log(QCD::B0*C2_/C1_)
                       +QCD::beta0(nf)*(2*m_A1*pow(log(QCD::B0/C1_),2)
                                       -2*m_A1*pow(log(C2_),2)+2*m_B1[nf-3]*log(C2_));
            }
            virtual inline double GetB3(const size_t &nf, const double& C1_=QCD::B0,
                                        const double &C2_=1.0) const {
                if(C1_ == QCD::B0 && C2_ == 1.0) return m_B3[nf-3];
                return m_B3[nf-3]-2*m_A3[nf-3]*log(QCD::B0*C2_/C1_)
                    +2*QCD::beta1(nf)*(m_A1*pow(log(QCD::B0/C1_),2)+log(C2_)
                            *(m_B1[nf-3]-m_A1*log(C2_)))
                    -4.0/3.0*pow(QCD::beta0(nf),2)*(2*m_A1*pow(log(QCD::B0/C1_),3)
                            +pow(log(C2_),2)*(2*m_A1*log(C2_)-3*m_B1[nf-3]))
                    +4.0*QCD::beta0(nf)*(m_A2[nf-3]*pow(log(QCD::B0/C1_),2)
                            +log(C2_)*(m_B2[nf-3]-m_A2[nf-3]*log(C2_)));
            }

            virtual inline double GetC1() const {return C1;}
            virtual inline double GetC2() const {return C2;}
            virtual inline double GetC3() const {return C3;}
            virtual inline double GetMuR() const {return muR;}
            virtual inline double GetMuF() const {return muF;}

            virtual inline Scheme GetScheme() const { return scheme; }
            virtual std::vector<Conv> GetNeededConvs() const = 0;
            virtual std::vector<double> GetPoint(const double&, const double&, const double&);

            // Setters
            virtual inline void SetAsymOrder(const size_t &order){ AsymOrder = order; }
            virtual inline void SetPertOrder(const size_t &order){ PertOrder = order; }
            virtual inline void SetDeltaSigmaOrder(const size_t &order){ DeltaSigmaOrder = order; }
            virtual inline void SetAOrder(const size_t &AOrder_) { AOrder = AOrder_; }
            virtual inline void SetBOrder(const size_t &BOrder_) { BOrder = BOrder_; }
            virtual inline void SetCOrder(const size_t &COrder_) { COrder = COrder_; }
            virtual inline void SetHOrder(const size_t &HOrder_) { HOrder = HOrder_; }
            virtual inline void SetC1(const double &C1_) { C1 = C1_; }
            virtual inline void SetC2(const double &C2_) { C2 = C2_; }
            virtual inline void SetC3(const double &C3_) { C3 = C3_; }
            virtual inline void SetMuR(const double &muR_) { muR = muR_; }
            virtual inline void SetMuF(const double &muF_) { muF = muF_; }
            virtual inline void SetResumScales(const double &C1_, const double &C2_,
                                               const double &C3_) {
                SetC1(C1_); SetC2(C2_); SetC3(C3_);
            }
            virtual inline void SetScales(const double &C1_, const double &C2_, const double &C3_,
                                          const double &muR_, const double &muF_) {
                SetResumScales(C1_, C2_, C3_); SetMuR(muR_); SetMuF(muF_);
            }
            virtual void SetScheme(Scheme s);
            virtual void SetNonPertCoefficients(const std::vector<double>&) {}      
            virtual void SetBMax(const double&) {}//yfu
            double NonPertYMax = 5.0;
            virtual void SetYMax(const double&) {}//yfu

            // Other
            virtual double GetYMax(const double&, const double&) const;
            virtual double GetYMax(const double&) const;
            virtual double GetQTMax(const double&, const double&) const;
            virtual std::vector<double> GetCalc(const double&, const double&, const double&) = 0;
            const bool& IncludeY() const { return includeY; }
            bool& IncludeY() { return includeY; }

            // Grid Functions
            void GridSetup(const IO::Settings&);
            virtual void GridGen();
            virtual void MergeGrid() {};
            void WriteInp(const std::string&, const std::vector<double>&) const;

            // Grid I/O
            virtual bool SaveGrid();
            virtual bool LoadGrid();

#ifdef FITTING
            void ClearMap() {}
#endif

        protected:
            CentralScale scale;
            std::vector<double> qVals, qTVals, yVals;
            GridType gType; 
            MapGrid3D grid;
            std::shared_ptr<ResBos> resbos;
            bool KinCorr;
            double qMin, qMax, qTMin, qTMax, yMin, yMax;

        private:
            A5Choice A5Err;
            Scheme scheme;
            size_t AsymOrder, PertOrder, DeltaSigmaOrder;
            double C1, C2, C3, muR, muF;
            size_t AOrder, BOrder, COrder, HOrder;
            double m_A1;
            std::array<double,3> m_A2, m_A3, m_A4, m_A5;
            std::array<double,3> m_B1, m_B2, m_B3;
            bool includeY{true};
    };

    class CalculationFactory {
        public:
            using TCreateMethod = std::unique_ptr<Calculation>(*)();
            TCreateMethod CreateFunc;

            static CalculationFactory& Instance();

            bool Register(const std::string, TCreateMethod);
            std::unique_ptr<Calculation> Create(const std::string&);

        private:
            CalculationFactory() : methods() {};
            std::map<std::string, TCreateMethod> methods;
    };

#define REGISTER_CALC(calculation) \
    bool calculation::registered = CalculationFactory::Instance().Register(calculation::GetName(), \
            calculation::Create);

}

#endif
