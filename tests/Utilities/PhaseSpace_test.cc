#include "PhaseSpace_test.hh"
#include "ResBos/FourVector.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::Values;

TEST_P(PhaseSpaceTest, PhaseGenMassless) {
    PhaseSpace ps;

    // Parameters
    const size_t nParticles = GetParam();
    particles.resize(nParticles);
    const std::vector<double> masses(nParticles,0);
    const double ECM = 8000;
    std::vector<double> randNum(56);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    randNum[0] = rand->Get();
    randNum[1] = rand->Get();
    randNum[2] = rand->Get();
    randNum[3] = rand->Get();
    for(size_t i = 1; i < nParticles-1; i++) {
        randNum[3*(i-1)+4] = rand->Get();
        randNum[3*(i-1)+5] = rand->Get();
        randNum[3*(i-1)+6] = rand->Get();
    }

    ps.PhaseGen(randNum, particles);
    double px = 0, py = 0, pz = 0, E = 0;
    for(auto particle : particles) {
        px += particle.Px();
        py += particle.Py();
        pz += particle.Pz();
        E += particle.E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT((E-ECM),1E-12);
}

TEST_P(PhaseSpaceTest, PhaseGenMassive) {
    PhaseSpace ps;

    // Parameters
    const size_t nParticles = GetParam();
    particles.resize(nParticles);
    const std::vector<double> masses(nParticles,10);
    const double ECM = 8000;
    std::vector<double> randNum(56);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    randNum[0] = rand->Get();
    randNum[1] = rand->Get();
    randNum[2] = rand->Get();
    randNum[3] = rand->Get();
    for(size_t i = 1; i < nParticles-1; i++) {
        randNum[3*(i-1)+4] = rand->Get();
        randNum[3*(i-1)+5] = rand->Get();
        randNum[3*(i-1)+6] = rand->Get();
    }

    ps.PhaseGen(randNum, particles);
    double px = 0, py = 0, pz = 0, E = 0;
    for(auto particle : particles) {
        px += particle.Px();
        py += particle.Py();
        pz += particle.Pz();
        E += particle.E();
        EXPECT_LT(fabs(particle.M()-10),1E-9);
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT((E-ECM),1E-12);
}

TEST_F(PhaseSpaceTest, PhaseSpace3) {
    PhaseSpace ps;

    // Parameters
    const size_t nParticles = 5;
    const double ECM = 8000;
    particles.resize(nParticles);
    std::vector<double> masses = {125,0,0,0};
    std::vector<double> randNum(7);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);
    ps.SetQRange(200,300);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    for(size_t i = 0; i < 7; i++) {
        randNum[i] = rand->Get();
    }

    double x1, x2;

    ps.PhaseSpace3(randNum, x1, x2, particles);
    double px = 0, py = 0, pz = 0, E = 0;
    for(size_t i = 0; i < nParticles; ++i) {
        px += particles[i].Px();
        py += particles[i].Py();
        pz += particles[i].Pz();
        E += particles[i].E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT((E-2*ECM),1E-12);
}

TEST_F(PhaseSpaceTest, WresphIMonte0IQt0) {
    PhaseSpace ps;

    // Parameters
    const int nParticles = 3;
    const double ECM = 8000;
    particles.resize(nParticles);
    std::vector<double> masses = {91.18,0,0};
    std::vector<double> randNum(5);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);
    ps.SetQRange(100,200);
    ps.SetQtRange(0,100);
    ps.SetIQt(0);
    ps.SetIMonte(0);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    for(size_t i = 0; i < 5; i++) {
        randNum[i] = rand->Get();
    }
   
    std::vector<double> phaseSpacePt;
    double Yg = 2.51;
    ps.Wresph(randNum, Yg, phaseSpacePt, particles);
    double px = particles[0].Px(), py = particles[0].Py(), pz = particles[0].Pz();
    double E = particles[0].E();
    for(size_t i = 1; i < nParticles; ++i) {
        px -= particles[i].Px();
        py -= particles[i].Py();
        pz -= particles[i].Pz();
        E -= particles[i].E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT(E,1E-12);
}

TEST_F(PhaseSpaceTest, WresphIMonte1IQt1) {
    PhaseSpace ps;

    // Parameters
    const int nParticles = 3;
    const double ECM = 8000;
    particles.resize(nParticles);
    std::vector<double> masses = {91.18,0,0};
    std::vector<double> randNum(5);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);
    ps.SetQRange(100,200);
    ps.SetQtRange(2,100);
    ps.SetIQt(1);
    ps.SetIMonte(1);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    for(size_t i = 0; i < 5; i++) {
        randNum[i] = rand->Get();
    }
    
    std::vector<double> phaseSpacePt;
    double Yg = 2.51;
    ps.Wresph(randNum, Yg, phaseSpacePt, particles);
    double px = particles[0].Px(), py = particles[0].Py(), pz = particles[0].Pz();
    double E = particles[0].E();
    for(size_t i = 1; i < nParticles; ++i) {
        px -= particles[i].Px();
        py -= particles[i].Py();
        pz -= particles[i].Pz();
        E -= particles[i].E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT(E,1E-12);
}

TEST_F(PhaseSpaceTest, WresphIMonte2IQt2) {
    PhaseSpace ps;

    // Parameters
    const size_t nParticles = 3;
    const double ECM = 8000;
    particles.resize(nParticles);
    std::vector<double> masses = {91.18,0,0};
    std::vector<double> randNum(5);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);
    ps.SetQRange(100,200);
    ps.SetQtRange(2,100);
    ps.SetIQt(2);
    ps.SetIMonte(2);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    for(size_t i = 0; i < 5; i++) {
        randNum[i] = rand->Get();
    }
    
    std::vector<double> phaseSpacePt;
    double Yg = 2.51;
    ps.Wresph(randNum, Yg, phaseSpacePt, particles);
    double px = particles[0].Px(), py = particles[0].Py(), pz = particles[0].Pz();
    double E = particles[0].E();
    for(size_t i = 1; i < nParticles; ++i) {
        px -= particles[i].Px();
        py -= particles[i].Py();
        pz -= particles[i].Pz();
        E -= particles[i].E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT(E,1E-12);
}

TEST_F(PhaseSpaceTest, WresphIMonte3IQt3) {
    PhaseSpace ps;

    // Parameters
    const size_t nParticles = 3;
    const double ECM = 8000;
    particles.resize(nParticles);
    std::vector<double> masses = {91.18,0,0};
    std::vector<double> randNum(5);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);
    ps.SetQRange(100,200);
    ps.SetQtRange(0,100);
    ps.SetIQt(3);
    ps.SetIMonte(3);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    for(size_t i = 0; i < 5; i++) {
        randNum[i] = rand->Get();
    }
    
    std::vector<double> phaseSpacePt;
    double Yg = 2.51;
    ps.Wresph(randNum, Yg, phaseSpacePt, particles);
    double px = particles[0].Px(), py = particles[0].Py(), pz = particles[0].Pz();
    double E = particles[0].E();
    for(size_t i = 1; i < nParticles; ++i) {
        px -= particles[i].Px();
        py -= particles[i].Py();
        pz -= particles[i].Pz();
        E -= particles[i].E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT(E,1E-12);
}

TEST_F(PhaseSpaceTest, WresphPhiDep) {
    PhaseSpace ps;

    // Parameters
    const size_t nParticles = 3;
    const double ECM = 8000;
    particles.resize(nParticles);
    std::vector<double> masses = {91.18,0,0};
    std::vector<double> randNum(4);

    // Set parameters in Phase Space Generator
    ps.SetRandomGenerator(rand);
    ps.SetECM(ECM);
    ps.SetNPrimaryPart(nParticles);
    ps.SetMass(masses);
    ps.SetQRange(100,200);
    ps.SetQtRange(0,100);
    ps.SetIQt(0);
    ps.SetIMonte(0);
    ps.SetPhiDep(false);

    EXPECT_CALL(*rand,Get())
        .WillRepeatedly(Return(0.5));

    // Set random numbers
    for(size_t i = 0; i < 4; i++) {
        randNum[i] = rand->Get();
    }
    
    std::vector<double> phaseSpacePt;
    double Yg = 2.51;
    ps.Wresph(randNum, Yg, phaseSpacePt, particles);
    double px = particles[0].Px(), py = particles[0].Py(), pz = particles[0].Pz();
    double E = particles[0].E();
    for(size_t i = 1; i < nParticles; ++i) {
        px -= particles[i].Px();
        py -= particles[i].Py();
        pz -= particles[i].Pz();
        E -= particles[i].E();
    }

    EXPECT_LT(px,1E-12);
    EXPECT_LT(py,1E-12);
    EXPECT_LT(pz,1E-12);
    EXPECT_LT(E,1E-12);
}

INSTANTIATE_TEST_SUITE_P(NParticlePhaseGen,PhaseSpaceTest,::testing::Range<size_t>(2,15));
