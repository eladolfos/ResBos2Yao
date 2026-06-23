#ifndef LHEF_TEST_HH
#define LHEF_TEST_HH

#include "gmock/gmock.h"
#include "LHEF/LHEFResBos.h"

class MockLHEF3FromResBos : public LHEF3FromResBos {
    public:
        MOCK_METHOD1(OpenLHEF,bool(std::string));
        MOCK_METHOD0(SetInit,bool());
        MOCK_METHOD1(SetEvent,bool(const Event&));
        MOCK_METHOD1(CloseLHEF,bool(bool));
};

#endif
