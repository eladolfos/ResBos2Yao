#ifndef GRID3D_HH
#define GRID3D_HH

#include <vector>
#include <algorithm>

namespace Utility {

    class Grid3D {
        public:
            Grid3D(const std::vector<double> &x, const std::vector<double> &y,
                   const std::vector<double> &z) :
                x_(x), y_(y), z_(z) {
                    xSize = x.size(); ySize = y.size(); zSize = z.size();
                    grid.reserve(x.size()*y.size()*z.size());
                };

            Grid3D(const std::vector<double> &x, const std::vector<double> &y,
                   const std::vector<double> &z, const std::vector<double> &g) :
                x_(x), y_(y), z_(z), grid(g) {
                    xSize = x.size(); ySize = y.size(); zSize = z.size();
                };

            virtual ~Grid3D() {}

            virtual void AddPoint(const size_t &i, const size_t &j, 
                                  const size_t &k, const double &value);
            virtual void AddPoint(const double &x, const double &y, 
                                  const double &z, const double &value);

            // Find... Finds the nearest point to the value input
            virtual std::vector<double>::const_iterator FindX(const double &x) const; 
            virtual std::vector<double>::const_iterator FindY(const double &y) const; 
            virtual std::vector<double>::const_iterator FindZ(const double &z) const; 
            virtual double FindPoint(const size_t &i, const size_t &j, const size_t &k) const;
            virtual double FindPoint(const double &x, const double &y, const double &z) const;
            virtual size_t FindPointLocation(const double &x, const double &y,
                                             const double &z) const;

            //Get returns point at a given grid point
            virtual double GetX(const size_t &i) const {return x_[i];}
            virtual double GetY(const size_t &i) const {return y_[i];}
            virtual double GetZ(const size_t &i) const {return z_[i];}
            virtual size_t GetXDim() const {return x_.size();}
            virtual size_t GetYDim() const {return y_.size();}
            virtual size_t GetZDim() const {return z_.size();}

            // Preforms a 3D interpolation of the grid to find the result at the desired input point
            virtual double Interpolate(const double &x, const double &y, const double &z,
                                       const size_t &xOrder=4, const size_t &yOrder=4, 
                                       const size_t &zOrder=4);

            void PrintGrid();
        private:
            std::vector<double> x_, y_, z_, grid;
            size_t xSize, ySize, zSize;
    };

}


#endif

