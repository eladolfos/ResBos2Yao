#ifndef NEWTONCOTES_HH
#define NEWTONCOTES_HH

#include "ResBos/BaseIntegrator.hh"

namespace Utility {

class NewtonCotes : public BaseIntegrator {
    public:
        NewtonCotes(const size_t &order, bool closed=true);
        NewtonCotes(const size_t &order, bool closed=true, bool cache=true);
        NewtonCotes(const size_t &order, const FunctionD &func,
                    bool closed=true, bool cache=true);
        NewtonCotes(const size_t &order, const FunctionVD &func,
                    bool closed=true, bool cache=true);

        double Integrate(const double&, const double&, double&, double&);
        std::vector<double> IntegrateVec(const double&, const double&,
                                         double&, double&);

    private:
        void SetupClosed();
        void SetupOpen();

        size_t m_order;
        std::function<double(const double&, const double&)> baseFunc, errFunc;
        FunctionVD baseFuncV, errFuncV;
};

}

#endif
