#include "Vegas_test.hh"
#include "gtest/gtest.h"

// TEST_F(VegasTest, TestAll) {
//     std::function<double(double*,double)> func = [](double* x, double) {
//         return x[0]*x[1]*x[2]*x[3];
//     };
// 
//     Vegas v(4,0,4,400000,4);
//     std::vector<double> low = {0,0,0,0}, high = {1,1,1,1};
//     v.SetRange(low,high);
// 
//     double result = v.VEGAS(func);
//     result = v.VEGAS(func);
//     double error = v.GetError();
//     if(error < 1E-5) error = 1E-5;
//     EXPECT_LT(fabs(result-pow(0.5,4)),2*fabs(error));
// }
