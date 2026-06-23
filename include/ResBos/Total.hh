#ifndef TOTAL_HH
#define TOTAL_HH 

#include <vector>
#include <cmath>
#include <functional>

#include "ResBos/Asymptotic.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Perturbative.hh"
#include "ResBos/Resummation.hh"

namespace IO {
    class Settings;
}

namespace Utility {
    class Grid2D;
}

namespace ResBos {
    typedef std::map<size_t,Utility::Grid2D*> MapGrid2D;
    class ResBos;

    class Total : public Resummation, public Asymptotic, public Perturbative {
        public:
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<Total>(new Total());
            }
            void Initialize(IO::Settings*,std::shared_ptr<ResBos>);

            static std::string GetName() {return "Total";}
            bool IsRegistered() {return registered;}

            double CalcCrossing(int, double, double);
            double GetCalcCrossing(double,double,double) const;
            virtual std::vector<double> GetCalc(double,double,double);
            std::vector<Conv> GetNeededConvs() const;
            std::vector<double> GetPoint(double,double,double);
            double GetPointMode(double,double,double,int);

            void GridGen();

            bool LoadGrid();
            bool SaveGrid();
            void MergeGrid();

        protected:
            Total() {};
            static bool registered;

        private:
            virtual std::vector<double> GetResummation(double,double,double);
            virtual std::vector<double> GetAsymptotic(double,double,double);
            virtual std::vector<double> GetPerturbative(double,double,double);
            bool needResum, needAsym, needPert;
            MapGrid3D gridResum, gridAsym, gridPert;
            MapGrid2D gridCrossing;
    };
}

#endif
