#include "fmt/format.h"

#include "ResBos/Grid2D.hh"
#include "ResBos/Utility.hh"

#include "ResBos/loguru.hpp"

template class std::vector<double>;

namespace Utility {

    void Grid2D::AddPoint(const size_t &i, const size_t &j, const double &value) {
        grid[i+xSize*j] = value; 
    }

    void Grid2D::AddPoint(const double &x, const double &y, const double &value) {
        size_t point = FindPointLocation(x,y);
        grid[point] = value;
    }

    std::vector<double>::const_iterator Grid2D::FindX(const double &x) const {
        return std::lower_bound(x_.begin(), x_.end(), x);
    }

    std::vector<double>::const_iterator Grid2D::FindY(const double &y) const {
        return std::lower_bound(y_.begin(), y_.end(), y);
    }

    double Grid2D::FindPoint(const size_t &i, const size_t &j) const {
        return grid[i+xSize*j];
    }

    double Grid2D::FindPoint(const double &x, const double &y) const {
        return grid[FindPointLocation(x,y)];
    }

    size_t Grid2D::FindPointLocation(const double &x, const double &y) const {
        size_t i = static_cast<size_t>(std::distance(x_.begin(),FindX(x)));
        size_t j = static_cast<size_t>(std::distance(y_.begin(),FindY(y)));

        return i+xSize*j;
    }


    double Grid2D::Interpolate(const double &x, const double &y,
                               const size_t &xOrder, const size_t &yOrder){
        // Find location of point
        //unsigned location = FindPointLocation(x,y);

        // Obtain a vector of the nearest xOrder*yOrder points and ensure it doesn't go over a boundary
        // Begin with the x direction
        // 1) Check if it is within xOrder/2 of an edge (if the order is odd, add additional point to the end)
        // 2) Fill with the surrounding xOrder elements
        std::vector<double>::iterator itX = std::lower_bound(x_.begin(), x_.end(), x);
        while(static_cast<size_t>(std::distance(x_.begin(),itX)) < xOrder/2) itX++;
        while(static_cast<size_t>(std::distance(itX,x_.end())) < xOrder/2+xOrder%2) itX--;
        std::vector<double> xInterp(itX-static_cast<long int>(xOrder/2),
                                    itX+static_cast<long int>(xOrder/2+xOrder%2)); 

        // Repeat for y direction
        std::vector<double>::iterator itY = std::lower_bound(y_.begin(), y_.end(), y);
        while(static_cast<size_t>(std::distance(y_.begin(),itY)) < yOrder/2) itY++;
        while(static_cast<size_t>(std::distance(itY,y_.end())) < yOrder/2+yOrder%2) itY--;
        std::vector<double> yInterp(itY-static_cast<long int>(yOrder/2),
                                    itY+static_cast<long int>(yOrder/2+yOrder%2)); 

        std::vector<double> tmp(yOrder), tmp2(xOrder);

        for(size_t i = 0; i < xOrder; i++) {
            for(size_t j = 0; j < yOrder; j++) {
                tmp[j] = grid[FindPointLocation(xInterp[i],yInterp[j])];
            }
            tmp2[i] = Polint(yInterp,tmp,yOrder,y);
        }
        double result = Polint(xInterp,tmp2,xOrder,x);
        DLOG_F(3,"Interpolation result: x = %f y = %f f(x,y) = %f",x,y,result);
        return result;
    }

    void Grid2D::PrintGrid() {
        for(size_t i = 0; i < xSize; i++) {
            for(size_t j = 0; j < ySize; j++) {
                fmt::print("{0:.4e}\t{1:.4e}\t{2:.4e}\n", x_[i], y_[j], FindPoint(i, j));
            }
        }
    }

}
