#include <cmath>
#include <exception>
#include <map>
#include <stdexcept>

#include "ResBos/Beam.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/Settings.hh"

#include "ResBos/loguru.hpp"

namespace Beam {

Beam::Beam() : energy{0}, beamType{BeamType::p} {
    pdf = nullptr;
    hoppet = nullptr;
    convolution = nullptr;
}

Beam::Beam(int, IO::Settings) {
    pdf = nullptr;
    hoppet = nullptr;
    convolution = nullptr;
}

Beam::Beam(BeamType type, FourVector mom) : beamType{type}, beamMom{mom} {
    energy = mom.E();
    pdf = nullptr;
    hoppet = nullptr;
    convolution = nullptr;
}

Beam::~Beam() {
    if(convolution != nullptr) delete convolution;
    if(pdf != nullptr) delete pdf;
    if(hoppet != nullptr) delete hoppet;
}

std::pair<Beam*,Beam*> Beam::MakeBeams(const std::string beamStr, const double ecm) {
    Beam *b1, *b2;
    if(beamStr.compare("ppb") == 0) {
        DLOG_F(INFO,"Making ppb beams with center of mass energy = %f",ecm);
        b1 = new Beam(BeamType::p,FourVector(0,0,ecm/2.0,ecm/2.0)); 
        b2 = new Beam(BeamType::pb, FourVector(0,0,-ecm/2.0,ecm/2.0));
    } else if(beamStr.compare("pp") == 0) {
        DLOG_F(INFO,"Making pp beams with center of mass energy = %f",ecm);
        b1 = new Beam(BeamType::p,FourVector(0,0,ecm/2.0,ecm/2.0)); 
        b2 = new Beam(BeamType::p, FourVector(0,0,-ecm/2.0,ecm/2.0));
    } else if(beamStr.compare("pCu") == 0) {
        DLOG_F(INFO,"Making pCU beams with center of mass energy = %f",ecm);
        b1 = new Beam(BeamType::p,FourVector(0,0,ecm/2.0,ecm/2.0)); 
        b2 = new Beam(BeamType::Cu, FourVector(0,0,-ecm/2.0,ecm/2.0));
    } else if(beamStr.compare("pPb") == 0) {
        DLOG_F(INFO,"Making pPB beams with center of mass energy = %f",ecm);
        b1 = new Beam(BeamType::p,FourVector(0,0,ecm/2.0,ecm/2.0)); 
        b2 = new Beam(BeamType::Pb, FourVector(0,0,-ecm/2.0,ecm/2.0));
    } else if(beamStr.compare("pn") == 0) {
        DLOG_F(INFO,"Making pn beams with center of mass energy = %f",ecm);
        b1 = new Beam(BeamType::p,FourVector(0,0,ecm/2.0,ecm/2.0)); 
        b2 = new Beam(BeamType::n, FourVector(0,0,-ecm/2.0,ecm/2.0));
    } else if(beamStr.compare("pd") == 0) {
        DLOG_F(INFO,"Making pn beams with center of mass energy = %f",ecm);
        b1 = new Beam(BeamType::p,FourVector(0,0,ecm/2.0,ecm/2.0));
        b2 = new Beam(BeamType::d, FourVector(0,0,-ecm/2.0,ecm/2.0));
    } else 
        throw std::runtime_error("Invalid beam choice. Check manual for valid options.");

    return std::make_pair(b1,b2);
}

double Beam::GetQMin() const {return pdf -> qMin();}

BDFMap Beam::BeamMap(const size_t &order, const int &pid, const double &x, const double &Q) const {
    BDFMap result;
    // Needed at alpha_s for Asymptotic and leading for DeltaSigma
    result[BDF::PDF] = PDF(BDF::PDF,pid,x,Q);
    result[BDF::P1] = PDF(BDF::P1,pid,x,Q);

    // Needed at alpha_s^2 for Asymptotic and alpha_s for DeltaSigma
    if(order > 0) {
        result[BDF::P2] = PDF(BDF::P2,pid,x,Q);
        result[BDF::P1P1] = PDF(BDF::P1P1,pid,x,Q);
        result[BDF::C1] = PDF(BDF::C1,pid,x,Q);
        result[BDF::C1P1] = PDF(BDF::C1P1,pid,x,Q);

        // Gluon Inital states only
        if(pid == 0) {
            result[BDF::G1] = PDF(BDF::G1,pid,x,Q);
        }

        // Needed at alpha_s^3 for Asymptotic and alpha_s^2 for DeltaSigma
        if(order > 1) {
            result[BDF::P3] = PDF(BDF::P3,pid,x,Q); 
            result[BDF::P1P2] = PDF(BDF::P1P2,pid,x,Q);
            result[BDF::P2P1] = PDF(BDF::P2P1,pid,x,Q);
            result[BDF::P1P1P1] = PDF(BDF::P1P1P1,pid,x,Q);
            result[BDF::C1P2] = PDF(BDF::C1P2,pid,x,Q); 
            result[BDF::C1P1P1] = PDF(BDF::C1P1P1,pid,x,Q);
            result[BDF::C2] = PDF(BDF::C2,pid,x,Q);
            result[BDF::C2P1] = PDF(BDF::C2P1,pid,x,Q);

            // Gluon Initial states only
            if(pid == 0) {
                result[BDF::G1P1] = PDF(BDF::G1P1,pid,x,Q);
            }
        }
    }

    return result;
}

double Beam::PDF(const BDF bdf, const int pid, const double x, const double Q) const {
    switch(bdf) {
        case BDF::PDF:
            return BeamPDF(pid,x,Q); 
        case BDF::C:
        case BDF::C1:
        case BDF::C2:
        case BDF::C1P1:
        case BDF::C1P1P1:
        case BDF::C1P2:
        case BDF::C2P1:
        case BDF::G1:
        case BDF::G1P1:
            return BeamConv(pid,x,Q,BDF2Conv.at(bdf));
        case BDF::P1:
        case BDF::P2:
        case BDF::P3:
        case BDF::P1P1:
        case BDF::P1P1P1:
        case BDF::P1P2:
        case BDF::P2P1:
            return BeamHoppet(pid,x,Q,BDF2Splitting.at(bdf));
        default:
            throw std::runtime_error("Invalid beam distribution function");
    }
}

double Beam::BeamPDF(const int pid, const double x, const double Q) const {
    if(beamType == BeamType::p) 
        return pdf -> Apdf(pid,x,Q);
    else if(beamType == BeamType::pb) 
        return pdf -> Apdf(-pid,x,Q);
    else if(beamType == BeamType::n) {
        return pdf -> Apdf(Neutron(pid),x,Q);
    } else if(beamType == BeamType::Cu) {
        return 0.46*pdf -> Apdf(pid,x,Q) + 0.54*pdf -> Apdf(Neutron(pid),x,Q);
    } else if(beamType == BeamType::d) {
        return 0.5*pdf -> Apdf(pid,x,Q) + 0.5*pdf -> Apdf(Neutron(pid),x,Q);
    } else {
        throw std::runtime_error("Beam: Invalid beam type");
    }
}

double Beam::BeamHoppet(const int pid, const double x, const double Q, const Splitting split) const {
    if(beamType == BeamType::p) 
        return hoppet -> GetConvolution(pid,x,Q,split);
    else if(beamType == BeamType::pb) 
        return hoppet -> GetConvolution(-pid,x,Q,split);
    else if(beamType == BeamType::n) {
        return hoppet -> GetConvolution(Neutron(pid),x,Q,split);
    } else if(beamType == BeamType::Cu) {
        return 0.46*hoppet -> GetConvolution(pid,x,Q,split) 
             + 0.54*hoppet -> GetConvolution(Neutron(pid),x,Q,split);
    } else if(beamType == BeamType::d) {
        return 0.5*hoppet -> GetConvolution(pid,x,Q,split)
             + 0.5*hoppet -> GetConvolution(Neutron(pid),x,Q,split);
    } else {
        throw std::runtime_error("Beam: Invalid beam type");
    }
}

double Beam::BeamConv(const int pid, const double x, const double Q, const Conv conv) const {
    if(beamType == BeamType::p)
        return convolution -> GetConv(pid,x,Q,conv);
    else if(beamType == BeamType::pb) 
        return convolution -> GetConv(-pid,x,Q,conv);
    else if(beamType == BeamType::n) {
        return convolution -> GetConv(Neutron(pid),x,Q,conv);
    } else if(beamType == BeamType::Cu) {
        return 0.46*convolution -> GetConv(pid,x,Q,conv) 
             + 0.54*convolution -> GetConv(Neutron(pid),x,Q,conv);
    } else if(beamType == BeamType::Pb) {
        int pid2 = Neutron(pid);
        return 0.4*convolution -> GetConv(pid,x,Q,conv)
             + 0.6*convolution -> GetConv(pid2,x,Q,conv);
    } else if(beamType == BeamType::d) {
        int pid2 = Neutron(pid);
        return 0.5*convolution -> GetConv(pid,x,Q,conv)
             + 0.5*convolution -> GetConv(pid2,x,Q,conv);
    } else {
        throw std::runtime_error("Beam: Invalid beam type");
    }
}

int Beam::Neutron(const int pid) const {
    int result;
    if(pid == 1) result = 2;
    else if(pid == -1) result = -2;
    else if(pid == 2) result = 1; 
    else if(pid == -2) result = -1; 
    else result = pid;
    return result;
}

}
