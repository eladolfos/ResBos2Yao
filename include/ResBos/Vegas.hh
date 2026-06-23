#ifndef VEGAS_HH
#define VEGAS_HH

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <future>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ResBos/AdaptiveMap.hh"
#include "ResBos/Random.hh"

#include "fmt/format.h"

using lim = std::numeric_limits<double>;

namespace IO {
    class Settings;
}

class KBNSummation{
    public:
        KBNSummation(): sum{0}, correction{0} { };
        inline double GetSum() noexcept {return sum + correction;};
        void AddTerm(double value) noexcept;
        inline void Reset() noexcept {sum = 0; correction = 0;} ;
    private:
        double sum, correction;
};

using VegasPt = std::array<double,2>;
using Batch = std::vector<double>;
using BatchInt = std::vector<size_t>;
using Batch2D = std::vector<std::vector<double>>;
using Func = std::function<double(const std::vector<double>&, const double&)>;

class VegasResult {
    public:
        VegasResult(bool weighted_ = true);
        void Update(double, double);
        VegasPt GetResult() const {return {mean, std::sqrt(var)};}
        bool Converged(const double&, const double&) const;
        std::string Summary() const;

    private:
        std::vector<double> sMean, sVar;
        double sum, sum2, sumVar;
        double mean, mean2, meanDivVar, mean2DivVar;
        double var, var2, OneDivVar;
        size_t size, n;
        bool weighted;
        double chi2() const;
        double dof() const { return static_cast<double>(sMean.size() - 1); }
};

class Vegas {
    public:
        Vegas(const Utility::AdaptiveMap&, const IO::Settings&);
        void Set(const IO::Settings&);
        void Set(const std::map<std::string,std::string>&);
        void Clear() {result = VegasResult(adapt);}
        std::string Settings(const size_t& ngrid=0);
        void RandomBatch(size_t, size_t, Batch2D&, Batch2D&, Batch&, BatchInt&);
        VegasPt operator ()(const Func&);
        
    private:
        static void SynchronizeMPI();

        void Setup();
//        void FillSigF(const std::function<double(std::vector<double>,double)>&,
//                      size_t cubeBase, size_t cubeBatch);
        Batch MakeBatch(const Func&, Batch2D, Batch);
        int GetMPIRank();
        int mpiRank;
        std::vector<double> sigF;
        Batch jac, fdv2;
        std::vector<size_t> nEvalCube;
        double sumSigF;
        Utility::AdaptiveMap map;
        size_t neval, nitn, maxinc, nCubeBatch, maxCube, maxEvalCube, minEvalCube;
        size_t ndims, nstrats, nincrements, nCube, lastNEval;
        double alpha, beta, rtol, atol;
        bool adapt, adaptErrors, sync;
        VegasResult result;
        Random* rand;
};

/*
class Vegas{
    public:
        Vegas(const size_t&, const int&, const int&, const int&, const int&) noexcept;
        virtual ~Vegas();

        void SetRange(std::vector<double>,std::vector<double>);
        void SetInit(int init_) noexcept {init = init_;}
        void SetItmx(int itmx_) noexcept {itmx = itmx_;}
        inline void SetNCalls(int nCalls) noexcept {ncall = nCalls;};

        double VEGAS(std::function<double(double*,double)> f) noexcept;
        double GetResult() {return result;}
        double GetError() {return sd;}

    private:
        std::mutex GridRefine, RandomNumber;
        static const int nGrid = 10;
        static const int ndmx = 100;
        void rebin(double ra, int j) noexcept;
        void threadIntegrate(int calls, int iThread) noexcept;
        std::function<double(double*,double)> func;
        std::vector<Random> rand;
        Random random;
        int ndo, nd, ng, npg, kd[ndmx];
        const size_t nDim;
        int init, mds, itmx, ncall, nprn, refine;
        double alph, wgt, chi2a, avgi;
        double dxg, dv2g, xjac, dx[ndmx], xu[ndmx], xl[ndmx];
        double ti, tsi, D[ndmx][ndmx], Di[ndmx][ndmx], fb, f2b, calls, ti2;
        double cd[ndmx][ndmx];
        double x[ndmx], xi[ndmx][ndmx], f, f2, sd, xo, xn, dt[ndmx], rc, r[ndmx];
        int ia[ndmx], it, ndm, xnd, kg[ndmx];
        double si, si2, schi, swgt;
        double inCall, fun2;
        double sint2, sint, result;
        unsigned nCubes, nCubeNpg;
        int nThreads, ndThread;
        std::vector<double> tsiThreads, tiThreads;
        std::vector<std::vector<std::vector<double>>> DThreads;
        int totCalls;

        KBNSummation sfun, sfun2, d[ndmx][ndmx], di[ndmx][ndmx];
        
};

uint64_t next(uint64_t x);
*/
#endif
