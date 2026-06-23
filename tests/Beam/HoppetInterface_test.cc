#include "gtest/gtest.h"
#include "HoppetInterface_test.hh"
#include "ResBos/DoubleExponential.hh"

TEST_F(HoppetTest, P1) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        auto vec = this -> hoppet -> GetConvolution(x,91.18,1);
        for(int i = -5; i < 6; ++i) {
            result += vec[i];
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
        std::cout << momSum << std::endl;
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-1));
}

TEST_F(HoppetTest, P1P1) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> hoppet -> GetConvolution(i,x,91.18,1,1);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-2));
}

TEST_F(HoppetTest, P1P1P1) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> hoppet -> GetConvolution(i,x,91.18,1,1,1);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-3));
}

TEST_F(HoppetTest, P2) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> hoppet -> GetConvolution(i,x,91.18,2);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-2));
}

TEST_F(HoppetTest, P2P1) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> hoppet -> GetConvolution(i,x,91.18,2,1);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-3));
}

TEST_F(HoppetTest, P1P2) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> hoppet -> GetConvolution(i,x,91.18,1,2);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-3));
}

TEST_F(HoppetTest, P3) {
    std::function<double(double)> func = [this](double x) {
        double result = 0;
        for(int i = -5; i < 6; ++i) {
            result += this -> hoppet -> GetConvolution(i,x,91.18,3);
        }
        return x*result;
    };

    Utility::DoubleExponential integral(func);

    std::vector<double> xBins = {1E-8,1E-7,1E-6,1E-5,1E-4,1E-3,1E-2,1E-1,1};
    double momSum = 0;
    double rerr=1E-8, aerr=1E-16;
    for(size_t i = 0; i < xBins.size()-1; ++i) {
        momSum += integral.Integrate(xBins[i],xBins[i+1],rerr,aerr);
    }

    EXPECT_LT(fabs(momSum),1E-4*pow(0.118/(2*M_PI),-3));
}

TEST_F(HoppetTest, GetConv) {
    EXPECT_DOUBLE_EQ(hoppet -> GetConvolution(1,0.3,91.18,1,1,1), 
                     hoppet -> GetConvolution(1,0.3,91.18,Splitting::P1P1P1));
}

TEST_F(HoppetTest, SwitchSet) {
    double result1 = hoppet -> GetConvolution(1,0.3,91.18,1,1,1);
    LHAPDF::PDF* pdf = LHAPDF::mkPDF("CT14nnlo",0);
    hoppet -> SwitchSet(pdf);
    double result2 = hoppet -> GetConvolution(1,0.3,91.18,1,1,1);
    EXPECT_DOUBLE_EQ(result1,result2);
}
