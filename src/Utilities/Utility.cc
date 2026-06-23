#include <iomanip>
#include <mutex>

#include "ResBos/Utility.hh"
#include "ResBos/loguru.hpp"

namespace Utility {

    // From this function down to the Polint function were inspired from:
    // https://github.com/Expander/polylogarithm/blob/master/src/Li.cpp
    bool is_close(double a, double b, double eps = 1E-10) {
        return std::abs(a - b) < eps;
    }

    bool is_even(long n) { return n % 2 == 0;}

    double binomial(long n, long k) {
        if(k > n) throw std::runtime_error("Error: Binomial coefficient requires n < k");

        double result = 1.0;
        if( k > n-k) k = n-k;
        for(long i = 0; i < k; ++i) {
            result *= static_cast<double>(n - i);
            result /= static_cast<double>(i + 1);
        }

        return result;
    }

    double zeta(const double &s) {
        static std::map<double,double> cached;
        if(cached.find(s) != cached.end()) return cached[s];
        else {
//#if __cpp_lib_math_special_functions >= 201603
#if 0
            const double result = std::riemann_zeta(s);
            cached[s] = result;
            return result;
#else
            if(s==1) return inf;

            double sum = 0.0, sum_old = 0.0;
            long n = 0;

            if(s >= 12) {
                n = 1;
                do{
                    sum_old = sum;
                    sum += pow(static_cast<double>(n),-s);
                    n++;
                } while(!is_close(sum_old,sum) &&
                        n < std::numeric_limits<long>::max() - 2);
                cached[s] = sum;
                return sum;
            }

            do{
                sum_old = sum;
                double sub_sum = 0.0;

                for(long k = 0; k <= n; ++k) {
                    const long sgn = is_even(k) ? 1 : -1;
                    sub_sum += binomial(n, k)*static_cast<double>(sgn)*std::pow(k+1,-s);
                }

                sum += sub_sum*std::pow(2.,-(n+1));
                n++;
            } while (!is_close(sum_old, sum) &&
                     n < std::numeric_limits<long>::max() - 2);

            cached[s] = sum/(1.0 - std::pow(2.0,1-s));
            return sum/(1.0 - std::pow(2.0,1-s));
        }
#endif
    }

    double eta(long n) {
        return (1.0 - std::pow(2.,1-n))*zeta(static_cast<double>(n));
    }

    std::vector<std::array<double,Npts>> Xn(long p) {
        using TArray = std::array<double,Npts>;
        std::vector<TArray> xn(static_cast<size_t>(p+1));

        {
            TArray ar;
            for(long ni = 0; ni < Npts; ++ni) {
                ar[static_cast<size_t>(ni)] = bernoulli[ni];
            }
            xn[0] = ar;
        }

        for(long pi = 1; pi <= p; ++pi) {
            TArray ar;
            for(long ni = 0; ni < Npts; ++ni) {
                double sum = 0.0;
                for(long k = 0; k <= ni; ++k) {
                    sum += binomial(ni,k)*bernoulli[ni-k]/static_cast<double>(k+1)
                         * xn[static_cast<size_t>(pi-1)][static_cast<size_t>(k)];
                }
                ar[static_cast<size_t>(ni)] = sum;
            }
            xn[static_cast<size_t>(pi)] = ar;
        }

        return xn;
    }

    double xLi_negative(const long &n, const double &x) {
        if(is_close(x,1)) return inf;

        double result = 0.0;
        for(long k = 0; k <= -n; ++k) {
            double sum = 0.0;
            for(long j = 0; j <= k; ++j) {
                const long sgn = is_even(j) ? -1 : 1;
                sum += static_cast<double>(sgn)*binomial(k, j)*std::pow(j+1,-n);
            }

            result += std::pow(-x/(1.-x),k+1)*sum;
        }

        return result;
    }

    double xLi_naive_sum(const long &n, const double &x) {
        double sum = 0, sum_old;
        long k = 0;

        do{
            k++;
            sum_old = sum;
            sum += std::pow(x,k)/std::pow(k,n);
        } while(!is_close(sum, sum_old) &&
                k < std::numeric_limits<long>::max() - 2);

        return sum;
    }

    double H(const long &n) {
        double sum = 0.0;
        for(long h = 1; h <= n; ++h) sum += 1.0/static_cast<double>(h);
        return sum;
    }

    double xLi_around_one(const long &n, const double &x) {
        const double mu = log(x);
        double sum=0.0, sum_old=0.0;
        long k = 0;

        do{
            if(k == n-1) {
                k++;
                continue;
            }
            sum_old = sum;
            sum += zeta(static_cast<double>(n-k))/factorial<double>(static_cast<double>(k))
                 * std::pow(mu,k);
            k++;
        } while(!is_close(sum, sum_old) &&
                k < std::numeric_limits<long>::max() - 2);

        return std::pow(mu,n-1)/factorial<double>(static_cast<double>(n-1))*(H(n-1)-log(-mu))+sum;
    }

    double xLi(const long &n, const double &x) {
        static std::map<std::pair<long,double>,double> cached;
        if(cached.find(std::make_pair(n,x)) != cached.end()) {
            return cached[std::make_pair(n,x)];
        } else {
            if(std::abs(x) - 1 > 1E-16) 
                throw std::runtime_error("Li: x out of range (-1,1) , x=" + std::to_string(x));
            if(n < 0) {
                const double result = xLi_negative(n,x);
                cached[std::make_pair(n,x)] = result;
                return result;
            } else if(n == 0) {
                if(is_close(x,1)) return inf;
                return x/(1.0-x);
            } else if(n == 1) return -log(1-x);
            else if(is_close(x,0)) return 0;
            else if(is_close(x,1)) return zeta(static_cast<double>(n));
            else if(is_close(x,-1)) return -eta(n);
            else if(n >= 12) return xLi_naive_sum(n,x);
            else if(is_close(x,1,1E-2)) {
                const double result = xLi_around_one(n,x);
                cached[std::make_pair(n,x)] = result;
                return result;
            } else {
                double u = -log(1-x);
                double p = 1., sum = 0;
                const auto xn = Xn(n-2);
                for(long k = 0; k < Npts; ++k) {
                    p *= u;
                    sum += xn[static_cast<size_t>(n-2)][static_cast<size_t>(k)]*p*fac_inv[k];
                }
                cached[std::make_pair(n,x)] = sum;
                return sum;
            }
        }
    }

    double Polint(const std::vector<double> &x_, const std::vector<double> &y_,
                  const size_t &n, const double &x) {
        // Function to preform a polynomial interpolation of order n
        int ns=0;
        double dift, dif = std::abs(x-x_[0]);
        std::vector<double> c(n), d(n);
        for(size_t i = 0; i < n; ++i) {
            if((dift=std::abs(x-x_[i])) < dif) {
                ns=static_cast<int>(i);
                dif=dift;
            }
            c[i] = y_[i];
            d[i] = y_[i];
        }
        double y = y_[static_cast<size_t>(ns--)];
        double ho,hp,w,den;
        for(size_t m = 0; m < n-1; m++) {
            for(size_t i = 0; i <  n-m-1; i++) {
                ho = x_[i]-x;
                hp = x_[i+m+1]-x;
                w = c[i+1]-d[i];
                if((den=ho-hp) == 0) 
                    throw std::runtime_error("Polint: Error in interpolation routine");
                den = w/den;
                d[i] = hp*den;
                c[i] = ho*den;
            }
            y += (2*ns+1 < static_cast<int>(n-m-1)
                    ? c[static_cast<size_t>(ns+1)] : d[static_cast<size_t>(ns--)]);
        }
        return y;
    }

    /* No longer needed for now
    double DivDif(double* f, double* a, int nn, double x, int mm){
        double t[20], d[20];
        int n, m, ix, iy, mid, npts, mPlus;
        int ip, l, iSub, i, j;
        int mMax=20;
        if(nn > 2 || mm > 1) {
            n = nn;
            m = std::min(std::min(mm,mMax),n-1);
            mPlus = m+1;
            ix = 0;
            iy = n+1;
            while(iy-ix>1){
                if(a[0] < a[n]) {
                    mid = (ix+iy)/2;
                    if(x>=a[mid-1]) {
                        ix=mid;
                    } else {
                        iy=mid;
                    }
                } else {
                    mid = (ix+iy)/2;
                    if(x<=a[mid-1]) {
                        ix=mid;
                    } else {
                        iy=mid;
                    }
                }
            }
            npts=m+2-m%2;
            ip=0;
            l=0;
            while(ip < npts){
                iSub=ix+l;
                if(!(1<=iSub && iSub<= n)){
                    npts=mPlus;
                } else {
                    ip++;
                    t[ip-1]=a[iSub-1];
                    d[ip-1]=f[iSub-1];
                }
                l=-l;
                if(l>=0) l++;
            }
            bool extra=npts!=mPlus;

            for(l=1; l<m; l++){
                if(extra) {
                    iSub=mPlus-l;
                    d[m+1]=(d[m+1]-d[m-1])/(t[m+1]-t[iSub-1]);
                }
                i=mPlus;
                for(int j=l; j < m; j++) {
                    iSub = i-l;
                    d[i-1]=(d[i-1]-d[i-2])/(t[i-1]-t[iSub-1]);
                    i--;
                }
            }

            double tot = d[mPlus-1];
            if(extra) tot = 0.5*(tot+d[m+1]);
            j=m;
            for(l=1;l<m;l++){
                tot=d[j-1]+(x-t[j-1])*tot;
                j--;
            }
            return tot;
        } else return 0;
    }*/

}
