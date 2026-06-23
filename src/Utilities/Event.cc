#include <stdexcept>

#include "ResBos/Event.hh"
#include "ResBos/Particle.hh"

bool Event::Check() {
    if(scales.size() != xsec_vars.size()) return false;
    return true;
}

void Event::Clear() {
    particles.resize(0);
    xsec = 0;
    xsec_vars.resize(0);
}

void Event::Reset() {
    Clear();
    scales.resize(0);
}

void Event::AddParticle(const Particle& part) {
    particles.push_back(part);
}

void Event::AddParticle(const int& pid, const FourVector& mom, const int& status, const std::pair<int,int>& cols=std::make_pair(0,0)) {
    Particle p{pid,mom,status,cols};
    particles.push_back(p);
}

void Event::AddScales(const Scales& scales_) {
    // Validate that the scales are all positive and non-zero
    for(auto scale_ : scales_) {
        if(scale_ <= 0) 
            throw std::runtime_error("Scales must be greater than 0. Given " 
                                     + std::to_string(scale_) + " as a scale.");
    }
}
