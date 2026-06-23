#ifndef GRIDND_HH
#define GRIDND_HH

#include <vector>
#include <algorithm>

namespace Utility {

    template<size_t T>
    class GridND {
        public:
            GridND(std::array<std::vector<double>,T> gridPts) {
                _gridPts = gridPts;
                size_t grid_size = 1;
                for(size_t i = 0; i < T; ++i) {
                    grid_size *= _gridPts[i].size();
                }
                _grid.reserve(grid_size);
            };

            virtual ~GridND() {}

            virtual void AddPoint(std::array<size_t,T>, double value);
            virtual void AddPoint(std::array<double,T>, double value);

            // Find... Finds the nearest point to the value input
            virtual std::vector<double>::const_iterator FindX(double x) const; 
            virtual std::vector<double>::const_iterator FindY(double y) const; 
            virtual std::vector<double>::const_iterator FindZ(double z) const; 
            virtual double FindPoint(unsigned int i, unsigned int j, unsigned int k) const;
            virtual double FindPoint(double x, double y, double z) const;
            virtual unsigned int FindPointLocation(double x, double y, double z) const;

            //Get returns point at a given grid point
            virtual double GetX(int i) const {return x_[i];}
            virtual double GetY(int i) const {return y_[i];}
            virtual double GetZ(int i) const {return z_[i];}
            virtual unsigned int GetXDim() const {return x_.size();}
            virtual unsigned int GetYDim() const {return y_.size();}
            virtual unsigned int GetZDim() const {return z_.size();}

            // Preforms a 3D interpolation of the grid to find the result at the desired input point
            virtual double Interpolate(double x, double y, double z, int xOrder=4, int yOrder=4, int zOrder=4);
            //        double Polint(std::vector<double> x_, std::vector<double> y_, int n, double x) const;

            void PrintGrid();
        private:
            std::array<std::vector<double>,T> _gridPts;
            std::vector<double> _grid;
    };

}


#endif


