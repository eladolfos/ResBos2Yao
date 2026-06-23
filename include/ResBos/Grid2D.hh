#ifndef GRID2D_HH
#define GRID2D_HH

#include <vector>
#include <algorithm>

namespace Utility {

    class Grid2D {
        public:
            Grid2D(const std::vector<double> &x, const std::vector<double> &y) :
                x_{x}, y_{y} {
                    xSize = x.size(); ySize = y.size();
                    grid.reserve(x.size()*y.size());
                };

            Grid2D(const std::vector<double> &x, const std::vector<double> &y,
                   const std::vector<double> &g) :
                x_(x), y_(y), grid(g) {
                    xSize = x.size(); ySize = y.size();
                };

            virtual ~Grid2D() {}

            virtual void AddPoint(const size_t &i, const size_t &j, const double &value);
            virtual void AddPoint(const double &x, const double &y, const double &value);

            // Find... Finds the nearest point to the value input
            virtual std::vector<double>::const_iterator FindX(const double &x) const; 
            virtual std::vector<double>::const_iterator FindY(const double &y) const; 
            virtual double FindPoint(const size_t &i, const size_t &j) const;
            virtual double FindPoint(const double &x, const double &y) const;
            virtual size_t FindPointLocation(const double &x, const double &y) const;

            //Get returns point at a given grid point
            virtual double GetX(const size_t &i) const {return x_[i];}
            virtual double GetY(const size_t &i) const {return y_[i];}
            virtual size_t GetXDim() const {return x_.size();}
            virtual size_t GetYDim() const {return y_.size();}

            // Preforms a 2D interpolation of the grid to find the result at the desired input point
            virtual double Interpolate(const double &x, const double &y,
                                       const size_t &xOrder=5, const size_t &yOrder=4);

            void PrintGrid();
        private:
            std::vector<double> x_, y_, grid;
            size_t xSize, ySize;
    };

}


#endif
