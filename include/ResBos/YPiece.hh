#ifndef TOTAL_HH
#define TOTAL_HH 

#include <vector>
#include <cmath>
#include <functional>

#include "ResBos/Asymptotic.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Perturbative.hh"

namespace IO {
    class Settings;
}

namespace Utility {
    class Grid2D;
}

namespace ResBos {
    typedef std::map<int,Utility::Grid3D*> MapGrid3D;
    typedef std::map<int,Utility::Grid2D*> MapGrid2D;
    class ResBos;

    class YPiece : public Asymptotic, public Perturbative {
        public:
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<YPiece>(new YPiece());
            }
            void Initialize(IO::Settings*,std::shared_ptr<ResBos>);

            static std::string GetName() {return "YPiece";}
            bool IsRegistered() {return registered;}

            virtual std::vector<double> GetCalc(double,double,double);
            std::vector<Conv> GetNeededConvs() const;
            std::vector<double> GetPoint(double,double,double);

            void GridGen();

            bool LoadGrid();
            bool SaveGrid();

        protected:
            YPiece() {};
            static bool registered;

        private:
            virtual std::vector<double> GetAsymptotic(double,double,double);
            virtual std::vector<double> GetPerturbative(double,double,double);
            bool needAsym, needPert;
            MapGrid3D gridAsym, gridPert;
    };
}

#endif
