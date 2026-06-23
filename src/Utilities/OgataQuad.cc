#include "ResBos/Utility.hh"
#include "ResBos/OgataQuad.hh"
#include "ResBos/loguru.hpp"
#include "ResBos/Brent.hh"

namespace Utility {
    constexpr std::array<double, OgataQuad::maxN> OgataQuad::jnZeros;

    void OgataQuad::init() {
        for(size_t i = 0; i < maxN; ++i) {
            xi[i] = jnZeros[i]/M_PI;
            double jp1 = BesJ1(M_PI*xi[i]);
            w[i] = BesY0(M_PI*xi[i]) / jp1;
        }
    }

    double OgataQuad::OgataT(const func& f, const double &h, const double &rerr,
                             const double &aerr) const {
        double result = 0;
        size_t i;
        const size_t N = static_cast<size_t>(M_PI/h);
        double err = 0;
        for(i = 0; i < N; ++i) {
            double knot = M_PI/h*GetPsi(h*xi[i]);
            double jnu = BesJ0(knot);
            double psip = GetPsiP(h*xi[i]);
            if(psip != psip) psip = 1.0;
            err = M_PI*w[i]*jnu*psip*f(knot);
            result += err;

            if(std::abs(err/result) < rerr && std::abs(err) < aerr) break;

            if(i > 5 && result == 0) {
                DLOG_F(INFO,"OgataQuad: Result seems to be zero");
                err = 0;
                return 0;
            }
        }
      
        err = std::abs(err);
        LOG_IF_F(WARNING, i==N,
                 "OgataQuad: Precision of %e reached, requested rerr = %e, aerr = %e",
                 err, rerr, aerr);

        DLOG_F(INFO,"OgataQuad: Result = %e +/- %e", result, err);
        DLOG_F(INFO,"Number of function evals: %zu", i);

        return result;
    }

    std::vector<double> OgataQuad::OgataT(const funcVec& f, const double &h, const double &rerr,
                                          const double &aerr) const {
        const size_t fsize = f(1.0).size();
        std::vector<double> err(fsize);
        std::vector<double> aerr_(fsize);
        static size_t iwarn = 0;
        static constexpr size_t nwarn = 10;

        std::vector<double> result(fsize, 0.0);
        size_t i;
        size_t N = static_cast<size_t>(M_PI/h);
        N = N > maxN ? maxN : N;
        double max_rerr = 0, max_aerr = 0;
        for(i = 0; i < N; ++i) {
            double knot = static_cast<double>(N)*GetPsi(h*xi[i]);
            double psip = GetPsiP(h*xi[i]);
            if(psip != psip) psip = 1.0;
            double wgt = M_PI*w[i]*BesJ0(knot)*psip;

            // Calculate results for the whole vector
            err = f(knot);
            for(size_t j = 0; j < fsize; ++j) {
                err[j] *= wgt;
                result[j] += err[j];
            }

            // Find max relative and absolute errors to use
            max_rerr = *std::max_element(err.begin(), err.end(), AbsCompare);
            std::transform(err.begin(), err.end(), result.begin(), 
                           aerr_.begin(), std::divides<double>());
            max_aerr = *std::max_element(aerr_.begin(),aerr_.end(), AbsCompare);
            if(std::abs(max_rerr) < rerr && std::abs(max_aerr) < aerr) break;
        }

        max_aerr = std::abs(max_aerr);
        max_rerr = std::abs(max_rerr);
        if(iwarn < nwarn && i==N) {
            LOG_IF_F(WARNING, i == N,
                     "OgataQuad: Precision of %e and %e reached, requested rerr = %e, aerr = %e",
                     max_rerr, max_aerr, rerr, aerr);
            iwarn++;
        } else if(iwarn == nwarn && i==N) {
            LOG_F(WARNING, "OgataQuad: Suppressing remaning precision warnings");
            iwarn++;
        }
        return result;
    }

    double OgataQuad::OgataU(const func &f, const double &h, const double &rerr,
                              const double &aerr) const {
        
        double result = 0;
        size_t i;
        double err;
        for(i = 0; i < maxN; ++i) {
            double knot = xi[i]*h;
            err = h*w[i]*f(knot)*BesJ0(knot);
            result += err;

            if(std::abs(err/result) < rerr && std::abs(err) < aerr) break;

            if(i > 5 && result == 0) {
                DLOG_F(INFO,"OgataQuad: Result seems to be zero");
                err = 0;
                return 0;
            }
        }
        
        err = std::abs(err);
        LOG_IF_F(WARNING, i==maxN,
                 "OgataQuad: Precision of %e reached, requested rerr = %e, aerr = %e",
                 err, rerr, aerr);

        DLOG_F(INFO,"OgataQuad: Result = %e +/- %e", result, err);
        DLOG_F(INFO,"Number of function evals: %zu", i);

        return result;
    }

    double OgataQuad::GetHu(const func &f, const double &q, const std::pair<double, double> &Q) const {
        auto h = [&](const double &x) { return -std::abs(x*f(x/q)); };
        Brent brent(h);
        double b = brent.Minimize(Q.first, Q.second);
        if(b > Q.second) return Q.second/jnZeros[0];
        if(b < Q.first) return Q.first/jnZeros[0];
        return b/jnZeros[0];
    }

    double OgataQuad::GetHu(const funcVec &f, const double &q, const std::pair<double, double> &Q) const {
        auto h = [&](const double &x) { 
            auto tmp = f(x/q); 
            double max = *std::max_element(tmp.begin(), tmp.end());
            return -std::abs(x*max);
        };
        Brent brent(h);
        double b = brent.Minimize(Q.first, Q.second);
        if(b > Q.second) return Q.second/jnZeros[0];
        if(b < Q.first) return Q.first/jnZeros[0];
        return b/jnZeros[0];
    }

    double OgataQuad::GetHt(const double& hu) const {
        auto f = [&](const double &h) { 
            return hu - M_PI*tanh(M_PI/2*sinh(h*jnZeros[15]/M_PI)); 
        };
        Brent brent(f);
        try {
            return brent.CalcRoot(2*hu/M_PI/jnZeros[1], 2*hu/M_PI/jnZeros[50]);
        } catch (std::exception &e) {
            return hu;
        }
    }

    double OgataQuad::FBTU(const func &g, const double &q, const std::pair<double, double> &Q,
                            const double &rerr, const double &aerr) const {
        double hu = GetHu(g, q, Q);
        auto f = [&](const double &x) { return g(x/q)/q; };
        return OgataU(f, hu, rerr, aerr);
    }

    double OgataQuad::FBT(const func &g, const double &q, const std::pair<double, double> &Q,
                           const double &rerr, const double &aerr) const {
        double hu = GetHu(g, q, Q);
        double ht = GetHt(hu);
        if(ht > hDefault) ht = hDefault;
        auto f = [&](const double &x) { return g(x/q)/q; };
        return OgataT(f, ht, rerr, aerr);
    }

    std::vector<double> OgataQuad::FBT(const funcVec &g, const double &q, double y,
                                       const std::pair<double, double> &Q,
                                       const double &rerr, const double &aerr) {
#ifdef FITTING
        double ht;
        auto close = htMap.lower_bound({q, y});
        if(close == htMap.end()) {
            double hu = GetHu(g, q, Q);
            ht = GetHt(hu);
            if(ht > hDefault) ht = hDefault;
            htMap[{q,y}] = ht;
        } else {
            if(std::abs(close -> first.first - q) > 0.1 ||
               std::abs(close -> first.second - y) > 0.1) {
                double hu = GetHu(g, q, Q);
                ht = GetHt(hu);
                if(ht > hDefault) ht = hDefault;
                htMap[{q,y}] = ht;
            } else {
                ht = close -> second;
            }
        }
#else
        double hu = GetHu(g, q, Q);
        double ht = GetHt(hu);
        if(ht > hDefault) ht = hDefault;
#endif
        auto f = [&](const double &x) { 
            auto results = g(x/q);
            for(auto& result : results) result /= q;
            return results;
        };
        return OgataT(f, ht, rerr, aerr);
    }
}
