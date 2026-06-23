#include "Utility_test.hh"
#include "gtest/gtest.h"

TEST(UtilityTest, Power) {
    EXPECT_DOUBLE_EQ(Utility::ipow<double>(2, 2), 4);
    EXPECT_DOUBLE_EQ(Utility::ipow<double>(3, 3), 27);
    EXPECT_DOUBLE_EQ(Utility::ipow<double>(M_PI, 20), pow(M_PI, 20));
}

TEST(UtilityTest, Factorial) {
    EXPECT_DOUBLE_EQ(Utility::factorial<double>(1), 1);
    EXPECT_DOUBLE_EQ(Utility::factorial<double>(2), 2);
    EXPECT_DOUBLE_EQ(Utility::factorial<double>(3), 6);
    EXPECT_DOUBLE_EQ(Utility::factorial<double>(4), 24);
    EXPECT_DOUBLE_EQ(Utility::factorial<double>(10), 3628800);
}

TEST(UtilityTest, DivDif) {
     
}

TEST(UtilityTest, Zeta) {   
    EXPECT_LT(fabs(Utility::zeta(2)-M_PI*M_PI/6.0),1E-8);
    EXPECT_LT(fabs(Utility::zeta(4)-pow(M_PI,4)/90.0),1E-8);
    EXPECT_LT(fabs(Utility::zeta(12)-691*pow(M_PI,12)/638512875.0),1E-8);
    EXPECT_LT(fabs(Utility::zeta(14)-2*pow(M_PI,14)/18243225.0),1E-8);
}

TEST(UtilityTest, PolyLog) {
    EXPECT_EQ(Utility::xLi(-1,0.5),2);
    EXPECT_EQ(Utility::xLi(0,0.5),1);
    EXPECT_LT(fabs(Utility::xLi(2,0.5)-0.5822405264650127),1E-8);
    EXPECT_LT(fabs(Utility::xLi(7,0.5)-0.5020145633247085),1E-8);
    EXPECT_LT(fabs(Utility::xLi(15,0.5)-0.5000076381652628),1E-8);
    EXPECT_LT(fabs(Utility::xLi(3,0.995)-1.193896987191249),1E-8);
    EXPECT_LT(fabs(Utility::xLi(3,0.5)-0.5372131936080403),1E-8);

    double y = 0.5;
    EXPECT_EQ(Utility::xLi(3,1.0-y),Utility::xLi(3,y));
    EXPECT_EQ(1-y,y);
}

TEST(UtilityTest, Polint) {
    std::function<double(double)> func = [](double x) {return x*x*x+3.0;};
    std::vector<double> x_ = {0,1,2,3,4,5,6,7,8,9};
    std::vector<double> y_;
    for(auto x : x_) {
        y_.push_back(func(x));
    }

    for(int i = 0; i < 100; ++i) {
        double x = 9.0/100.0*i;
        EXPECT_LT(fabs(Utility::Polint(x_,y_,4,x)-func(x)),1E-5);
    }
}

TEST(UtilityTest, Logspace) {
    const size_t N = 20;
    auto result = Utility::Logspace(-1, 0, N); 

    EXPECT_EQ(result.size(), N);
    for(size_t i = 0; i < N; ++i)
        EXPECT_DOUBLE_EQ(result[i], pow(10, -1+1.0/(N-1)*static_cast<double>(i)));
    EXPECT_DOUBLE_EQ(result[0], 0.1);
    EXPECT_DOUBLE_EQ(result[N-1], 1.0);
}

TEST(UtilityTest, Linspace) {
    const size_t N = 100;
    auto result = Utility::Linspace(0, N, N+1);

    EXPECT_EQ(result.size(), N+1);
    for(size_t i = 0; i < N; ++i)
        EXPECT_DOUBLE_EQ(result[i], static_cast<double>(i));
    EXPECT_DOUBLE_EQ(result[0], 0);
    EXPECT_DOUBLE_EQ(result[N], static_cast<double>(N));
}
