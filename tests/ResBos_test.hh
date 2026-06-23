#ifndef RESBOS_TEST_HH
#define RESBOS_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/ResBos.hh"
#include "ResBos/Process.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Beam.hh"

using namespace ResBos;

class MockResBos : public ResBos {
    public:
        // Mock ResBos driver functions
        MOCK_METHOD0(GetXSect,std::function<double(const std::vector<double>&,
                                                         const double&)>());
        MOCK_METHOD2(XSect,double(const std::vector<double>&, const double&));

        // Mock Process interface
        MOCK_METHOD1(SetProcess,void(std::unique_ptr<Process>));
        MOCK_CONST_METHOD0(GetProcess,Process*());
        MOCK_CONST_METHOD2(H,double(const size_t&,const double&));
        MOCK_CONST_METHOD3(H,double(const size_t&,const double&,const double&));
        
        // Mock Setting interface
        MOCK_METHOD1(SetSettings,void(IO::Settings*));
        MOCK_CONST_METHOD0(GetSettings,IO::Settings*());

        // Mock Beam interface
        MOCK_METHOD1(SetBeams,void(std::pair<Beam::Beam*,Beam::Beam*>));
        MOCK_CONST_METHOD0(GetBeams,std::pair<Beam::Beam*,Beam::Beam*>());
        MOCK_CONST_METHOD0(GetECM,double());
        MOCK_CONST_METHOD1(GetAlpi,double(const double&));
        MOCK_CONST_METHOD1(GetNF,size_t(const double&));

        // Mock Calculation interface
        MOCK_METHOD1(SetCalculation,void(std::unique_ptr<Calculation>));
        MOCK_CONST_METHOD0(GetCalculation,Calculation*());
};

#endif
