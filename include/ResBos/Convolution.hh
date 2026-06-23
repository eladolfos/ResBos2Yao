#ifndef CONVOLUTION_HH
#define CONVOLUTION_HH

#include <map>
#include <vector>
#include <thread>
#include <functional>
#include <memory>

#include "ResBos/BaseIntegrator.hh"
#include "ResBos/Enums.hh"
#include "ResBos/QCDConst.hh"

const extern std::thread::id mainThread;

namespace Utility {
    class Grid2D;
    class PDF;
    class Hoppet;
}

namespace ResBos {
    class ResBos;
}

namespace Beam {
    typedef std::map<int,Utility::Grid2D*> ConvGrid;
    typedef std::map<std::thread::id,Utility::PDF*> PDFThreads;

    // Class to preform the convolutions and store grids
    class Convolution {
        public:
            // Constructors and Destructors
            Convolution() {}; // Purely for testing purposes
            Convolution(std::shared_ptr<ResBos::ResBos>, Utility::PDF*, 
                    Utility::Hoppet*,
                    const bool &gridGen=true, const size_t &COrder=2, const Scheme &s=Scheme::CFG, 
                    const double &C1=QCD::B0, const double &C2=1, const double &C3=QCD::B0);
            Convolution(const Convolution&);
            virtual ~Convolution();

            // Setters
            void SetPDFs(const PDFThreads &pdfs_) {pdfs = pdfs_; pdf = pdfs[mainThread];}
            void SetHoppet(Utility::Hoppet* hoppet_) {hoppet = hoppet_;}
            void SetGridGen(const bool &gridGen_) {gridGen = gridGen_;}
            void SetOrder(const size_t &COrder_) {COrder = COrder_;}
            void SetScheme(const Scheme &scheme_) {scheme = scheme_;}
            void SetC1C2C3(const double &C1_, const double &C2_, const double &C3_) {
                C1 = C1_*QCD::B0; C2 = C2_; C3 = C3_*QCD::B0;
            }

            // Getters
            virtual double GetConv(const int&, const double&, const double&, const Conv&);
            virtual std::map<int,double> GetConv(const double&, const double&, const Conv&);
            virtual ConvGrid GetGrid(const Conv&) const;

            // Generate Grid
            void GenerateGrid(PDFThreads, Utility::Hoppet*, Conv);
            void GenerateGrid(const Conv &conv);

            // Search for grids
            bool GetC1Grid();
            bool GetC1P1Grid();
            bool GetC1P1P1Grid();
            bool GetC1P2Grid();
            bool GetC2Grid();
            bool GetC2P1Grid();
            bool GetG1Grid();
            bool GetG1P1Grid();

            // Convolution I/O
            bool SaveGrid(const Conv&) const;
            bool LoadGrid(const Conv&);

        private:
            // Calculate the grids
            void CalcCGrid();
            std::map<int,Utility::Grid2D*> CalcC1Grid(const int &mode = 0);
            std::map<int,Utility::Grid2D*> CalcC2Grid(const int &mode = 0);
            std::map<int,Utility::Grid2D*> CalcG1Grid(const int &mode = 0);

            // Helper functions for the FFT calculation
            double C1qq(const double&) const;
            double C1qg(const double&) const;
            double C1gq(const double&) const;

            double C2qq(const double&) const;
            double C2qqb(const double&) const;
            double C2qqp(const double&) const;
            double C2qg(const double&) const;
    
            // Actual convolution calculations
            double CConv(const int&, const double&, const double&,
                         const std::thread::id &id=mainThread);
            double C1Conv(const int&, const double&, const double&,
                          const int &mode=0, const std::thread::id &id=mainThread);
            double C2Conv(const int&, const double&, const double&,
                          const int &mode=0, const std::thread::id &id=mainThread);
            double G1Conv(const int&, const double&, const double&,
                          const int &mode=0, const std::thread::id &id=mainThread);
            double Convolve(const double&, const std::function<double(const double&)>&) const;

            // Functions for CSS scheme dependence, must be run each time since grids will
            // be stored in CFG format to make more universal
            double SchemeDependence(const int&, const double&, const double&, const Conv&) const;
            double HCxF(const int&, const double&, const double&, const double&, const int&) const;

            // Integrands and plus functions for convolution calculations
            double C1xF(int,double,double,double,int,std::thread::id id=mainThread) const;
            double C2xF(int,double,double,double,int,std::thread::id id=mainThread);
            double SxF(int,double,double,double,int,std::thread::id id=mainThread) const;
            double G1xF(int,double,double,double,int,std::thread::id id=mainThread) const;

            // Scale variation of the C functions
            double ScaleVariation(int,int,double,double);

            // Variables
            bool gridGen;
            size_t COrder;
            std::shared_ptr<ResBos::ResBos> resbos;
            Utility::PDF *pdf;
            ConvGrid Cf, C1f, C2f, C1P1f, C1P2f, C1P1P1f, C2P1f, G1f, G1P1f;
            std::vector<double> xVec, qVec;
            Utility::Hoppet *hoppet;
            std::map<std::thread::id, Utility::PDF*> pdfs;
            double muF, muR;
            Scheme scheme;
            double C1, C2, C3;
            std::unique_ptr<Utility::BaseIntegrator> m_integrator;

            // FFT Convolution variables
            // static constexpr unsigned int nQ = 108;
            static constexpr unsigned int nQ = 108;
            static constexpr double xMin = 1E-8;
            static constexpr double xMax = 1.0;
            static constexpr double uMax = -log(xMin);
            static constexpr double uMin = -log(xMax);
            static constexpr unsigned int N = pow(2,8);
            static constexpr double uStep = (uMax-uMin)/N;
            static constexpr double dtt = 2.3/static_cast<double>(nQ);
            static constexpr double qBase = 0.1;
            static constexpr double t0 = 1.0;
            std::map<int,std::vector<double>> C2qqVals;
            std::map<double,double> CQQMap, CQGMap, CQQBMap, CQQPMap;
            std::map<double,double> CGGMap, CGQMap;
            std::vector<double> C2qgVals, C2qqbVals, C2qqpVals;

    };
}

#endif
