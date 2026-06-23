#include "ResBos/Grid3D.hh"
#include "ResBos/Utility.hh"

#include "ResBos/loguru.hpp"

template class std::vector<double>;

namespace Utility {

    void Grid3D::AddPoint(const size_t &i, const size_t &j, const size_t &k, const double &value) {
        grid[i+xSize*j+xSize*ySize*k] = value; 
    }

    void Grid3D::AddPoint(const double &x, const double &y,
                          const double &z, const double &value) {
        size_t point = FindPointLocation(x,y,z);
        grid[point] = value;
    }

    std::vector<double>::const_iterator Grid3D::FindX(const double &x) const {
        return std::lower_bound(x_.begin(), x_.end(), x);
    }

    std::vector<double>::const_iterator Grid3D::FindY(const double &y) const {
        return std::lower_bound(y_.begin(), y_.end(), y);
    }

    std::vector<double>::const_iterator Grid3D::FindZ(const double &z) const {
        return std::lower_bound(z_.begin(), z_.end(), z);
    }

    double Grid3D::FindPoint(const size_t &i, const size_t &j, const size_t &k) const {
        return grid[i+xSize*j+xSize*ySize*k];
    }

    double Grid3D::FindPoint(const double &x, const double &y, const double &z) const {
        return grid[FindPointLocation(x,y,z)];
    }

    size_t Grid3D::FindPointLocation(const double &x, const double &y, const double &z) const {
        size_t i = static_cast<size_t>(std::distance(x_.begin(),FindX(x)));
        size_t j = static_cast<size_t>(std::distance(y_.begin(),FindY(y)));
        size_t k = static_cast<size_t>(std::distance(z_.begin(),FindZ(z)));

        return i+xSize*j+xSize*ySize*k;
    }


    double Grid3D::Interpolate(const double &x, const double &y, const double &z, 
                               const size_t &xOrder, const size_t &yOrder, const size_t &zOrder){
        // Obtain a vector of the nearest xOrder*yOrder*zOrder points and ensure it doesn't go over a boundary
        // Begin with the x direction
        // 1) Check if it is within xOrder/2 of an edge (if the order is odd, add additional point to the end)
        // 2) Fill with the surrounding xOrder elements
        std::vector<double>::iterator itX = std::lower_bound(x_.begin(), x_.end(), x);
        while(static_cast<size_t>(std::distance(x_.begin(),itX)) < xOrder/2) ++itX;
        while(static_cast<size_t>(std::distance(itX,x_.end())) < xOrder/2+xOrder%2) --itX;
        std::vector<double> xInterp(itX-static_cast<long int>(xOrder/2),
                                    itX+static_cast<long int>(xOrder/2+xOrder%2)); 

        // Repeat for y direction
        std::vector<double>::iterator itY = std::lower_bound(y_.begin(), y_.end(), y);
        while(static_cast<size_t>(std::distance(y_.begin(),itY)) < yOrder/2) ++itY;
        while(static_cast<size_t>(std::distance(itY,y_.end())) < yOrder/2+yOrder%2) --itY;
        std::vector<double> yInterp(itY-static_cast<long int>(yOrder/2),
                                    itY+static_cast<long int>(yOrder/2+yOrder%2)); 

        // Repeat for z direction
        std::vector<double>::iterator itZ = std::lower_bound(z_.begin(), z_.end(), z);
        while(static_cast<size_t>(std::distance(z_.begin(),itZ)) < zOrder/2) ++itZ;
        while(static_cast<size_t>(std::distance(itZ,z_.end())) < zOrder/2+zOrder%2) --itZ;
        std::vector<double> zInterp(itZ-static_cast<long int>(zOrder/2),
                                    itZ+static_cast<long int>(zOrder/2+zOrder%2)); 

        std::vector<double> tmp(yOrder), tmp2(xOrder), tmp3(zOrder);

        for(size_t i = 0; i < xOrder; i++) {
            for(size_t j = 0; j < yOrder; j++) {
                for(size_t k = 0; k < zOrder; k++) {
                    tmp3[k] = grid[FindPointLocation(xInterp[i],yInterp[j],zInterp[k])];
                }
                tmp[j] = Polint(zInterp,tmp3,zOrder,z);
            }
            tmp2[i] = Polint(yInterp,tmp,yOrder,y);
        }
        double result = Polint(xInterp,tmp2,xOrder,x);
        DLOG_F(3,"Interpolation result: x = %f y = %f z = %f f(x,y,z) = %f",x,y,z,result);
        return result;
    }

    void Grid3D::PrintGrid() {
        for(size_t i = 0; i < xSize; i++) {
            for(size_t k = 0; k < zSize; k++) {
                for(size_t j = 0; j < ySize; j++) {
                    std::cout << x_[i] << "\t" << y_[j] << "\t" << z_[k] << "\t" << FindPoint(i,j,k) << std::endl;
                }
            }
        }
    }

}

