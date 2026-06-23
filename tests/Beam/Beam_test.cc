#include "gtest/gtest.h"
#include "Beam_test.hh"
#include "Convolution_test.hh"
#include "Pdf_test.hh"
#include "HoppetInterface_test.hh"

using ::testing::Return;
using ::testing::_;
using ::testing::ReturnArg;
using ::testing::Invoke;

using Beam::BeamType;
using Beam::BDF;

std::map<std::string,std::pair<BeamType,BeamType>> beamMap = {
    {"ppb",std::make_pair(BeamType::p,BeamType::pb)},
    {"pp",std::make_pair(BeamType::p,BeamType::p)},
    {"pCu",std::make_pair(BeamType::p,BeamType::Cu)},
    {"pn",std::make_pair(BeamType::p,BeamType::n)}
};

std::vector<BeamType> beams{BeamType::p,BeamType::pb,BeamType::Cu,BeamType::n};

TEST_F(BeamTest, MakeBeams) {
    for(auto el : beamMap) {
        std::pair<Beam::Beam*,Beam::Beam*> locBeams = Beam::Beam::MakeBeams(el.first,8000);
        EXPECT_EQ(locBeams.first -> GetBeamType(),el.second.first);
        EXPECT_EQ(locBeams.second -> GetBeamType(),el.second.second);
    }

    EXPECT_THROW(Beam::Beam::MakeBeams("zjfalsk",823),std::runtime_error);
}

TEST_P(BeamTest, BeamPDF) {
    Beam::Beam beam(GetParam(),FourVector(0,0,6500,6500));
    Utility::PDF *pdf = new MockPDF();
    beam.SetPDF(pdf);

    EXPECT_EQ(beam.PDF(BDF::PDF,1,0.2,10),pdf->Apdf(1,0.2,10));
}

TEST_P(BeamTest, BeamHoppet) {
    std::vector<Beam::BDF> pdfs = {
        BDF::P1,BDF::P2,BDF::P3,BDF::P1P1,BDF::P1P1P1,BDF::P1P2,BDF::P2P1
    };
    Beam::Beam beam(GetParam(),FourVector(0,0,6500,6500));

    MockHoppet *hoppet = new MockHoppet();
    if(GetParam() != BeamType::Cu) {
        EXPECT_CALL(*hoppet,GetConvolution(_,_,_,_))
            .Times(2*static_cast<int>(pdfs.size()))
            .WillRepeatedly(Return(0.2));
    } else {
        EXPECT_CALL(*hoppet,GetConvolution(_,_,_,_))
            .Times(3*static_cast<int>(pdfs.size()))
            .WillRepeatedly(Return(0.2));
    }

    beam.SetHoppet(hoppet);

    for(auto pdf : pdfs) 
        EXPECT_EQ(beam.PDF(pdf,3,0.2,10),
                  hoppet->GetConvolution(3,0.2,10,Beam::BDF2Splitting.at(pdf)));
}

TEST_P(BeamTest, BeamConv) {
    std::vector<Beam::BDF> pdfs = {
        BDF::C,BDF::C1,BDF::C2,BDF::C1P1,BDF::C1P1P1,BDF::C1P2,BDF::C2P1,
        BDF::G1,BDF::G1P1
    };
    Beam::Beam beam(GetParam(),FourVector(0,0,6500,6500));

    MockConvolution *conv = new MockConvolution();
    if(GetParam() != BeamType::Cu) {
        EXPECT_CALL(*conv,GetConv(_,_,_,_))
            .Times(2*static_cast<int>(pdfs.size()))
            .WillRepeatedly(Return(0.2));
    } else {
        EXPECT_CALL(*conv,GetConv(_,_,_,_))
            .Times(3*static_cast<int>(pdfs.size()))
            .WillRepeatedly(Return(0.2));
    }

    beam.SetConvolution(conv);
    for(auto pdf : pdfs) 
        EXPECT_EQ(beam.PDF(pdf,2,0.2,10),
                  conv->GetConv(2,0.2,10,Beam::BDF2Conv.at(pdf)));
}

TEST_P(BeamTest, BeamMap) {
    std::vector<Beam::BDF> pdfs = {
        BDF::PDF,BDF::P1,BDF::P2,BDF::P3,BDF::P1P1,BDF::P1P1P1,BDF::P1P2,BDF::P2P1,
        BDF::C1,BDF::C2,BDF::C1P1,BDF::C1P1P1,BDF::C1P2,BDF::C2P1
    };
    Beam::Beam beam(GetParam(),FourVector(0,0,6500,6500));

    Utility::PDF *pdf = new MockPDF();
    MockHoppet *hoppet = new MockHoppet();
    MockConvolution *conv = new MockConvolution();

    if(GetParam() != BeamType::Cu) {
        EXPECT_CALL(*hoppet,GetConvolution(_,_,_,_))
            .Times(14)
            .WillRepeatedly(Return(0.2));
        EXPECT_CALL(*conv,GetConv(_,_,_,_))
            .Times(12)
            .WillRepeatedly(Return(0.2));
    } else {
        EXPECT_CALL(*hoppet,GetConvolution(_,_,_,_))
            .Times(28)
            .WillRepeatedly(Return(0.2));
        EXPECT_CALL(*conv,GetConv(_,_,_,_))
            .Times(24)
            .WillRepeatedly(Return(0.2));
    }

    beam.SetPDF(pdf);
    beam.SetHoppet(hoppet);
    beam.SetConvolution(conv);

    auto locBeamMap = beam.BeamMap(2,1,0.2,10);

    for(auto pdf_ : pdfs) EXPECT_EQ(locBeamMap[pdf_], beam.PDF(pdf_,1,0.2,10));
}

INSTANTIATE_TEST_SUITE_P(AllBeams,BeamTest,::testing::ValuesIn(beams));
