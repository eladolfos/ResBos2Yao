#ifndef WMA_HH
#define WMA_HH 

#include <vector>
#include <cmath>
#include <functional>

#include "ResBos/Resummation.hh"
#include "ResBos/Asymptotic.hh"
#include "ResBos/Enums.hh"

namespace IO {
    class Settings;
}

namespace Utility {
    class Grid2D;
}

namespace ResBos {
    typedef std::map<size_t,Utility::Grid3D*> MapGrid3D;
    typedef std::map<size_t,Utility::Grid2D*> MapGrid2D;
    class ResBos;

    class WmA : public Resummation, public Asymptotic {
        public:
            static std::unique_ptr<Calculation> Create() {
                return std::unique_ptr<WmA>(new WmA());
            }
            void Initialize(IO::Settings*,std::shared_ptr<ResBos>);

            static std::string GetName() {return "WmA";}
            bool IsRegistered() {return registered;}

            double CalcCrossing(const size_t&,  const double&,  const double&);
            double GetCalcCrossing(const double&, const double&, const double&) const;
            virtual std::vector<double> GetCalc(const double&, const double&, const double&);
            std::vector<Conv> GetNeededConvs() const;
            std::vector<double> GetPoint(const double&, const double&, const double&);
            double GetPointMode(const double&, const double&, const double&, const size_t&);

            void GridGen();

            bool LoadGrid();
            bool SaveGrid();
            void MergeGrid();

        protected:
            WmA() {};
            static bool registered;

        private:
            virtual std::vector<double> GetResummation(const double&, const double&,
                                                       const double&);
            virtual std::vector<double> GetAsymptotic(const double&, const double&, const double&);
            bool needResum, needAsym;
            MapGrid3D gridResum, gridAsym;
            MapGrid2D gridCrossing;
    };
}

#endif
