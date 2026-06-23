#include "gtest/gtest.h"
#include "Process_test.hh"
#include "../ResBos_test.hh"
#include "../Utilities/Settings_test.hh"

TEST_F(ProcessTest, Registration) {
    for(auto process_str : processes) {
        auto process = ResBos::ProcessFactory::Instance().Create(process_str,resbos,settings);
        EXPECT_TRUE(process -> IsRegistered());
    }
}

TEST_F(DrellYanTest, HardPart) {
    EXPECT_LT(fabs(processDY -> H1()-1.246402934059572),1E-8); 
    EXPECT_LT(fabs(processDY -> H2()-18.522702637331573),1E-8); 

    EXPECT_LT(fabs(processZU -> H1()-1.246402934059572),1E-8); 
    EXPECT_LT(fabs(processZU -> H2()-18.522702637331573),1E-8); 

    EXPECT_LT(fabs(processZD -> H1()-1.246402934059572),1E-8); 
    EXPECT_LT(fabs(processZD -> H2()-18.522702637331573),1E-8); 
}

TEST_F(A0Test, HardPart) {
    EXPECT_LT(fabs(process -> H1()-1.246402934059572),1E-8); 
    EXPECT_LT(fabs(process -> H2()-18.522702637331573),1E-8); 
}
