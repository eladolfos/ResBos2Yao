#ifndef CALCULATION_TEST_HH
#define CALCULATION_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Calculation.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"
#include "../Beam/Beam_test.hh"
#include "../Process/Process_test.hh"

using ::testing::Return;
using ::testing::ReturnPointee;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::An;

class MockCalc : public ResBos::Calculation {
    public:
        // Mock Getters
        MOCK_CONST_METHOD0(GetAsymOrder, int()); 
        MOCK_CONST_METHOD0(GetDeltaSigmaOrder, int());
        MOCK_CONST_METHOD0(GetAOrder, int());
        MOCK_CONST_METHOD0(GetBOrder, int());
        MOCK_CONST_METHOD0(GetCOrder, int());
        MOCK_CONST_METHOD0(GetHOrder, int());
        MOCK_CONST_METHOD0(GetA1, double());
        MOCK_CONST_METHOD1(GetA2, double(int));
        MOCK_CONST_METHOD1(GetA3, double(int));
        MOCK_CONST_METHOD1(GetB1, double(int));
        MOCK_CONST_METHOD1(GetB2, double(int));
        MOCK_CONST_METHOD1(GetB3, double(int));
        MOCK_CONST_METHOD0(GetC1, double());
        MOCK_CONST_METHOD0(GetC2, double());
        MOCK_CONST_METHOD0(GetC3, double());
        MOCK_CONST_METHOD0(GetMuR, double());
        MOCK_CONST_METHOD0(GetMuF, double());
        MOCK_CONST_METHOD0(GetScheme, Scheme());
        MOCK_CONST_METHOD0(GetNeededConvs, std::vector<Conv>());
        MOCK_METHOD3(GetPoint, std::vector<double>(double,double,double));

        // Mock Setters
        MOCK_METHOD1(SetAsymOrder, void(int));
        MOCK_METHOD1(SetDeltaSigmaOrder, void(int));
        MOCK_METHOD1(SetAOrder, void(int));
        MOCK_METHOD1(SetBOrder, void(int));
        MOCK_METHOD1(SetCOrder, void(int));
        MOCK_METHOD1(SetHOrder, void(int));
        MOCK_METHOD1(SetC1, void(double));
        MOCK_METHOD1(SetC2, void(double));
        MOCK_METHOD1(SetC3, void(double));
        MOCK_METHOD1(SetMuR, void(double));
        MOCK_METHOD1(SetMuF, void(double));
        MOCK_METHOD3(SetResumScales, void(double,double,double));
        MOCK_METHOD5(SetScales, void(double,double,double,double,double));
        MOCK_METHOD1(SetScheme, void(Scheme));

        // Mock Other
        MOCK_METHOD2(GetYMax, double(double,double));
        MOCK_METHOD1(GetYMax, double(double));
        MOCK_METHOD3(GetCalc, std::vector<double>(double,double,double));
        MOCK_METHOD0(GridGen, void());

};

#endif
