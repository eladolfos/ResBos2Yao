#include "ResBos/Utility.hh"
#include "OgataQuad_test.hh"

double rfunc(const double &r, const double &a=1) {
    return r*exp(-pow(a*r, 2)/2);
}

double kfunc(const double &k, const double &a=1) {
    return exp(-pow(k/a, 2)/2)/pow(a, 2);
}

TEST_P(OgataQuadTest, Functions) {
    double k = GetParam();
    auto func = [&](const double &b) -> double {
        return b*exp(-b);
    };
    auto qfunc = [&](const double &qt) -> double {
        return pow(1+qt*qt, -1.5);
    };

    double result = ogata -> FBT(func, k, std::make_pair(k, 10*k), 1E-10, 1E-10);
    EXPECT_NEAR(result, qfunc(k), 1E-8);
}

INSTANTIATE_TEST_SUITE_P(EvaluationRange, OgataQuadTest, ::testing::Range(0.1,10.0,0.1));
