#include "BaseIntegrator_test.hh"
#include "ResBos/Utility.hh"
#include "ResBos/AdaptiveIntegrator.hh"
#include "ResBos/Adzint.hh"
#include "ResBos/DoubleExponential.hh"
#include "ResBos/GaussKronrod.hh"
#include "ResBos/NewtonCotes.hh"

double func1(const double &x) { return x*x + 1; }
double func2(const double &x) { return sin(1.0/x); }
double func3(const double &x) { return exp(-x*x); }
double func4(const double &x) { return 1.0/(1+x*x); }
double func5(const double &x) { return 1.0/(2+cos(x)); }
double func6(const double &x) { return Utility::BesJ0(x); }

Utility::BaseIntegrator* AdaptiveIntegratorGKBuild() {
    auto tmp = std::unique_ptr<Utility::BaseIntegrator>(new Utility::GaussKronrod());
    return new Utility::AdaptiveIntegrator(std::move(tmp), 5000);
}

Utility::BaseIntegrator* AdaptiveIntegratorNCBuild(const size_t &order, const bool &closed) {
    auto tmp = std::unique_ptr<Utility::BaseIntegrator>(new Utility::NewtonCotes(order, closed, false));
    return new Utility::AdaptiveIntegrator(std::move(tmp), 30000);
}

Utility::BaseIntegrator* AdzintBuild(const int &iacta, const int &iactb) {
    return new Utility::Adzint(iacta, iactb);
}

Utility::BaseIntegrator* NewtonCotesBuild(const size_t &order, const bool &closed,
                                          const bool &cache = false) {
    return new Utility::NewtonCotes(order, closed, cache);
}

TEST_P(IntegratorTest, Integration) {
    double rerr = 1e-10;
    double aerr = 1e-16;

    integrator -> SetFunction(func1); 
    double result = integrator -> Integrate(0.0, 4.0, rerr, aerr);
    if(mode)
        EXPECT_NEAR(result, 76.0/3.0, 5*rerr);
    else
        EXPECT_NEAR(result, 76.0/3.0, 1E-8);

    integrator -> SetFunction(func2); 
    result = integrator -> Integrate(0.1, 2*M_PI, rerr, aerr);
    if(mode)
        EXPECT_NEAR(result, 2.2717166014, 6*rerr);
    else
        EXPECT_NEAR(result, 2.2717166014, 1E-8);

    integrator -> SetFunction(func3);
    result = integrator -> Integrate(0.0, 1.0, rerr, aerr);
    if(mode)
        EXPECT_NEAR(result, 0.746824132812427, 5*rerr);
    else
        EXPECT_NEAR(result, 0.746824132812427, 1E-8);

    integrator -> SetFunction(func4);
    result = integrator -> Integrate(0.0, 1.0, rerr, aerr);
    if(mode)
        EXPECT_NEAR(result, M_PI_4, 5*rerr);
    else
        EXPECT_NEAR(result, M_PI_4, 1E-8);

    integrator -> SetFunction(func5);
    result = integrator -> Integrate(0.0, M_PI, rerr, aerr);
    if(mode)
        EXPECT_NEAR(result, M_PI/sqrt(3.0), 5*rerr);
    else
        EXPECT_NEAR(result, M_PI/sqrt(3.0), 1E-8);
}

INSTANTIATE_TEST_SUITE_P(Adaptive, IntegratorTest,
    ::testing::Values(std::make_tuple(new Utility::DoubleExponential, false),
                      std::make_tuple(AdzintBuild(0, 0), false),
                      std::make_tuple(AdzintBuild(1, 1), false),
                      std::make_tuple(AdzintBuild(2, 2), false),
                      std::make_tuple(AdaptiveIntegratorGKBuild(), false),
                      std::make_tuple(AdaptiveIntegratorNCBuild(2, true), false),
                      std::make_tuple(AdaptiveIntegratorNCBuild(3, true), false),
                      std::make_tuple(AdaptiveIntegratorNCBuild(4, true), false),
                      std::make_tuple(AdaptiveIntegratorNCBuild(3, false), false),
                      std::make_tuple(AdaptiveIntegratorNCBuild(4, false), false),
                      std::make_tuple(AdaptiveIntegratorNCBuild(5, false), false)));

INSTANTIATE_TEST_SUITE_P(WithError, IntegratorTest,
    ::testing::Values(std::make_tuple(new Utility::GaussKronrod, true),
                      std::make_tuple(NewtonCotesBuild(2, true), true),
                      std::make_tuple(NewtonCotesBuild(3, true), true),
                      std::make_tuple(NewtonCotesBuild(4, true), true),
                      std::make_tuple(NewtonCotesBuild(3, false), true),
                      std::make_tuple(NewtonCotesBuild(4, false), true),
                      std::make_tuple(NewtonCotesBuild(5, false), true)));
