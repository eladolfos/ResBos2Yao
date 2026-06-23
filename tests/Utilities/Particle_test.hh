#ifndef PARTICLE_TEST_HH
#define PARTICLE_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Particle.hh"

class MockParticle : public Particle {
    public:
        // Mock getter functions
        MOCK_CONST_METHOD0(PID,int()); 
        MOCK_CONST_METHOD0(Status,int()); 
        MOCK_CONST_METHOD0(Mom,FourVector()); 
        MOCK_CONST_METHOD0(Cols,std::pair<int,int>()); 
        MOCK_CONST_METHOD0(Col,int()); 
        MOCK_CONST_METHOD0(Acol,int()); 
        MOCK_CONST_METHOD0(isFinal,bool()); 
        MOCK_CONST_METHOD0(Px,double()); 
        MOCK_CONST_METHOD0(Py,double()); 
        MOCK_CONST_METHOD0(Pz,double()); 
        MOCK_CONST_METHOD0(E, double()); 
        MOCK_CONST_METHOD0(M, double()); 
        MOCK_CONST_METHOD0(Mother1,size_t()); 
        MOCK_CONST_METHOD0(Mother2,size_t()); 
        MOCK_CONST_METHOD0(Daughter1,size_t()); 
        MOCK_CONST_METHOD0(Daughter2,size_t()); 

        // Mock setter functions
        MOCK_METHOD1(SetPID,void(const int&));
        MOCK_METHOD1(SetStatus,void(const int&));
        MOCK_METHOD1(SetMom,void(const FourVector&));
        MOCK_METHOD1(SetCols,void(const std::pair<int,int>&));
        MOCK_METHOD2(SetMothers,void(const size_t&, const size_t&));
        MOCK_METHOD2(SetDaughters,void(const size_t&, const size_t&));
};

#endif
