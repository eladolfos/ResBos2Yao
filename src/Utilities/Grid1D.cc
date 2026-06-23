#include "ResBos/Grid1D.hh"
#include "ResBos/Utility.hh"

#include "ResBos/loguru.hpp"

template class std::vector<double>;

namespace Utility {

    void Grid1D::AddPoint(const size_t &i, const double &value) {
        grid[i] = value; 
    }

    void Grid1D::AddPoint(const double &x, const double &value) {
        size_t point = FindPointLocation(x);
        grid[point] = value;
    }

    std::vector<double>::const_iterator Grid1D::FindX(const double &x) const {
        return std::lower_bound(x_.begin(), x_.end(), x);
    }

    double Grid1D::FindPoint(const size_t &i) const {
        return grid[i];
    }

    double Grid1D::FindPoint(const double &x) const {
        return grid[FindPointLocation(x)];
    }

    size_t Grid1D::FindPointLocation(const double &x) const {
        return static_cast<size_t>(std::distance(x_.begin(),FindX(x)));
    }


    double Grid1D::Interpolate(const double &x, const size_t &xOrder) const{
        // Obtain a vector of the nearest xOrder points and ensure it doesn't go over a boundary
        // 1) Check if it is within xOrder/2 of an edge (if the order is odd, add additional point to the end)
        // 2) Fill with the surrounding xOrder elements
        auto itX = std::lower_bound(x_.begin(), x_.end(), x);
        while(static_cast<size_t>(std::distance(x_.begin(),itX)) < xOrder/2) itX++;
        while(static_cast<size_t>(std::distance(itX,x_.end())) < xOrder/2+xOrder%2) itX--;
        std::vector<double> xInterp(itX-static_cast<long int>(xOrder/2),
                                    itX+static_cast<long int>(xOrder/2+xOrder%2)); 

        std::vector<double> tmp(xOrder);

        for(size_t i = 0; i < xOrder; i++) {
                tmp[i] = grid.at(FindPointLocation(xInterp[i]));
        }
        double result = Polint(xInterp,tmp,xOrder,x);
        DLOG_F(3,"Interpolation result: x = %f f(x) = %f",x,result);
        return result;
    }

    void Grid1D::PrintGrid() const {
        for(size_t i = 0; i < xSize; i++) {
            std::cout << x_[i] << "\t" << grid[i] << std::endl;
        }
    }

}

