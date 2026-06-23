#ifndef PROCESS_TEST_HH
#define PROCESS_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Process.hh"
#include "ResBos/Settings.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"

using ::testing::Return;
using ::testing::_;

typedef std::vector<std::pair<int,int>> Partons;

class MockProcess : public ResBos::Process {
    public:
        MOCK_METHOD1(Initialize,void(const IO::Settings&));
        MOCK_METHOD0(InitializeMCFM,void());
        MOCK_METHOD0(IsRegistered,bool());

        // Mock Information about the calculation
        MOCK_CONST_METHOD0(GetEW, Electroweak*());
        MOCK_CONST_METHOD0(GetPS, PhaseSpace*());
        MOCK_CONST_METHOD0(ECM, double());
        MOCK_METHOD0(InitPartons, void());
        MOCK_CONST_METHOD1(GetPartons, Partons(const size_t&));
        MOCK_CONST_METHOD0(GetChannels, size_t());
        MOCK_CONST_METHOD0(GetInitState, InitialState());
        MOCK_CONST_METHOD0(GetNAngular, size_t());
        MOCK_CONST_METHOD0(GetResumAngular, size_t());
        MOCK_CONST_METHOD0(GetAlphasCoupling, int());
        MOCK_CONST_METHOD0(PIDs, const std::vector<int>&());
        MOCK_METHOD2(MakeEvent, void(Event&, const std::vector<FourVector>&));

        // Mock information pertaining to the hard part and matrix element
        MOCK_CONST_METHOD0(GetNDim, size_t());
        MOCK_CONST_METHOD0(H1, double());
        MOCK_CONST_METHOD1(H2, double(const double&));
        MOCK_CONST_METHOD1(H2, double(const size_t&));
        MOCK_CONST_METHOD2(ME, double(const std::vector<double>&, const std::vector<double>&));
        MOCK_CONST_METHOD1(GetCoupling, double(const double&));
        MOCK_CONST_METHOD4(CxFCxF, double(const double&, const double&,
                                          const double&, const size_t&));
        MOCK_CONST_METHOD3(GetPhaseSpacePoint, 
                           double(const std::vector<double>&, std::vector<double>&,
                                  std::vector<FourVector>&));
        MOCK_CONST_METHOD9(GetPert, double(const double&, const double&, const double&,
                                           const double&, const double&, const double&,
                                           const double&, const double&, const size_t&));
        
};

class ProcessTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            const int nprocesses = static_cast<int>(processes.size());
            EXPECT_CALL(settings,GetSettingString("EWMassScheme"))
                .Times(nprocesses)
                .WillRepeatedly(Return("ComplexMass"));
            EXPECT_CALL(settings,GetSettingDouble("GF"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("Mw"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("Mz"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamW"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamZ"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("mT"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("mH"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamT"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamH"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingInt("AlphaEMOrder"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1));
            EXPECT_CALL(settings,GetSettingDouble("AlphaEM0"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0/137.0));
            EXPECT_CALL(settings,GetSettingDouble("AlphaEMMZ"))
                .Times(nprocesses)
                .WillRepeatedly(Return(1.0/127.0));

        }

        // Clean-up work for each test
        virtual void TearDown() {}

        const std::vector<std::string> processes = {"DrellYan", "A0", "ZU", "ZD","WPlus","WMinus","Higgs"};
        IO::MockSettings settings;
        std::shared_ptr<MockResBos> resbos;
};

class DrellYanTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            resbos = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbos,GetECM())
                .Times(3)
                .WillRepeatedly(Return(8000));
            EXPECT_CALL(*resbos,GetNF(_))
                .WillRepeatedly(Return(5));

            IO::MockSettings settings;
            // Calls to init EW Class
            EXPECT_CALL(settings,GetSettingString("EWMassScheme"))
                .Times(3)
                .WillRepeatedly(Return("ComplexMass"));
            EXPECT_CALL(settings,GetSettingDouble("GF"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("Mw"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("Mz"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamW"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamZ"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("mT"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("mH"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamT"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamH"))
                .Times(3)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingInt("AlphaEMOrder"))
                .Times(3)
                .WillRepeatedly(Return(1));
            EXPECT_CALL(settings,GetSettingDouble("AlphaEM0"))
                .Times(3)
                .WillRepeatedly(Return(1.0/137.0));
            EXPECT_CALL(settings,GetSettingDouble("AlphaEMMZ"))
                .Times(3)
                .WillRepeatedly(Return(1.0/127.0));

            EXPECT_CALL(settings,GetSettingDouble("Qmin"))
                .Times(3)
                .WillRepeatedly(Return(66));
            EXPECT_CALL(settings,GetSettingDouble("Qmax"))
                .Times(3)
                .WillRepeatedly(Return(116));
            EXPECT_CALL(settings,GetSettingDouble("Qtmin"))
                .Times(3)
                .WillRepeatedly(Return(66));
            EXPECT_CALL(settings,GetSettingDouble("Qtmax"))
                .Times(3)
                .WillRepeatedly(Return(116));

            processDY = ResBos::ProcessFactory::Instance().Create("DrellYan",resbos,settings); 
            processZU = ResBos::ProcessFactory::Instance().Create("ZU",resbos,settings); 
            processZD = ResBos::ProcessFactory::Instance().Create("ZD",resbos,settings); 

            processDY->Initialize(settings);
            processZU->Initialize(settings);
            processZD->Initialize(settings);
        }

        // Clean-up work for each test
        virtual void TearDown() {}

        std::unique_ptr<ResBos::Process> processDY, processZU, processZD;
        std::shared_ptr<MockResBos> resbos;
};

class A0Test : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            resbos = std::make_shared<MockResBos>();
            EXPECT_CALL(*resbos,GetECM())
                .WillRepeatedly(Return(8000));
            EXPECT_CALL(*resbos,GetNF(_))
                .WillRepeatedly(Return(5));

            IO::MockSettings settings;
            // Calls to init EW Class
            EXPECT_CALL(settings,GetSettingString("EWMassScheme"))
                .Times(1)
                .WillRepeatedly(Return("ComplexMass"));
            EXPECT_CALL(settings,GetSettingDouble("GF"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("Mw"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("Mz"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamW"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamZ"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("mT"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("mH"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamT"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingDouble("gamH"))
                .Times(1)
                .WillRepeatedly(Return(1.0));
            EXPECT_CALL(settings,GetSettingInt("AlphaEMOrder"))
                .Times(1)
                .WillRepeatedly(Return(1));
            EXPECT_CALL(settings,GetSettingDouble("AlphaEM0"))
                .Times(1)
                .WillRepeatedly(Return(1.0/137.0));
            EXPECT_CALL(settings,GetSettingDouble("AlphaEMMZ"))
                .Times(1)
                .WillRepeatedly(Return(1.0/127.0));

            EXPECT_CALL(settings,GetSettingDouble("Qmin"))
                .WillOnce(Return(66));
            EXPECT_CALL(settings,GetSettingDouble("Qmax"))
                .WillOnce(Return(116));
            EXPECT_CALL(settings,GetSettingDouble("Qtmin"))
                .WillOnce(Return(66));
            EXPECT_CALL(settings,GetSettingDouble("Qtmax"))
                .WillOnce(Return(116));

            process = ResBos::ProcessFactory::Instance().Create("A0",resbos,settings); 
            process->Initialize(settings);
        }

        // Clean-up work for each test
        virtual void TearDown() {}

        std::unique_ptr<ResBos::Process> process;
        std::shared_ptr<MockResBos> resbos;
};


#endif

