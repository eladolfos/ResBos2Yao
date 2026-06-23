#ifndef ADAPTIVE_MAP_HH
#define ADAPTIVE_MAP_HH

#include <array>
#include <limits>
#include <mutex>
#include <string>
#include <vector>

namespace Utility {

using Vector2D = std::vector<std::vector<double>>;
using Vector = std::vector<double>;
using lim = std::numeric_limits<double>;

class AdaptiveMap {
    public:
        AdaptiveMap() = default;
        AdaptiveMap(const size_t&);
        AdaptiveMap(const Vector2D&, const size_t& nbins_=0);
        AdaptiveMap(const AdaptiveMap& other) {
            nbins = other.nbins;
            ndims = other.ndims;
            bins = other.bins;
            grid = other.grid;
            sumF = other.sumF;
            nF = other.nF;
            jacobian = other.jacobian;
        }
        AdaptiveMap(const AdaptiveMap&& other) {
            nbins = other.nbins;
            ndims = other.ndims;
            bins = other.bins;
            grid = other.grid;
            sumF = other.sumF;
            nF = other.nF;
            jacobian = other.jacobian;
        }
        virtual ~AdaptiveMap() {};

        void MakeUniform(const size_t& nbins_=0);
        std::vector<double> Jacobian(const std::vector<double>&);
        void AddTrainingData(const Vector2D&, const Vector&);
        void AddTrainingData(const Vector&, const double&);
        void Adapt(const double&, const size_t& nbins_=0);
        void Map(Vector2D&, Vector2D&, Vector&);

        std::string Settings(const size_t& ngrid_=5) const;
        std::vector<std::array<double,2>> AllRegion() const;
        std::array<double,2> Region(const size_t&) const;
        size_t GetBins() const {return bins[0].size();}
        size_t GetDims() const {return grid.size();}

        std::vector<double> Random();
        std::vector<double> operator ()(const std::vector<double>&);
        void MPISync();

    private:
        void InitGrid(const Vector2D&, const bool& init=false);

        size_t nbins, ndims;
        double jacobian;
        Vector2D bins, grid, sumF, nF;
};

}

#endif
