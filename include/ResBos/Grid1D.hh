#ifndef GRID1D_HH
#define GRID1D_HH

#include <vector>
#include <algorithm>

namespace Utility {

    class Grid1D {
        public:
            Grid1D() {}
            Grid1D(const std::vector<double> &x) :
                x_{x} {
                    xSize = x.size(); 
                    grid.reserve(x.size());
                };

            Grid1D(const std::vector<double> &x, const std::vector<double> &g) :
                x_(x), grid(g) {
                    xSize = x.size();
                };

            virtual ~Grid1D() {}

            virtual void AddPoint(const size_t &i, const double &value);
            virtual void AddPoint(const double &x, const double &value);

            // Find... Finds the nearest point to the value input
            virtual std::vector<double>::const_iterator FindX(const double &x) const; 
            virtual double FindPoint(const size_t &i) const;
            virtual double FindPoint(const double &x) const;
            virtual size_t FindPointLocation(const double &x) const;

            //Get returns point at a given grid point
            virtual double GetX(const size_t &i) const {return x_[i];}
            virtual size_t GetXDim() const {return x_.size();}

            // Preforms a 1D interpolation of the grid to find the result at the desired input point
            virtual double Interpolate(const double &x, const size_t &xOrder=4) const;

            void PrintGrid() const;
        private:
            std::vector<double> x_, grid;
            size_t xSize;
    };

}


#endif

