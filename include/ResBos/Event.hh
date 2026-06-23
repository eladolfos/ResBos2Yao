#ifndef EVENT_HH
#define EVENT_HH

#include <tuple>
#include <vector>
#include <map>

#include "ResBos/Particle.hh"

#include "LHEF/LHEF3.h"

typedef std::array<double,5> Scales;

class FourVector;
class Particle;

class Event {
    public:
        // Constructor
        Event() = default;

        // Destructor
        ~Event() = default;

        // Check to ensure event is built correctly
        bool Check();

        // Setters
        void Clear();
        void Reset();
        void AddParticle(const Particle&);  
        void AddParticle(const int&, const FourVector&, const int&, const std::pair<int,int>&);
        void AddScales(const Scales&);
        void SetXSec(const double& xsec_) {xsec = xsec_; xsec_vars.push_back(xsec_);}
        void SetXSecErr(const double& xerr_) {xerr = xerr_;}
        void AddXSecVar(const double& xsec_) {xsec_vars.push_back(xsec_);}
        void SetBeams(const double& beamA_, const double& beamB_) {
            beamA = beamA_;
            beamB = beamB_;
        }
        
        // Getters
        std::vector<Particle> Particles() const {return particles;}
        size_t size() const {return particles.size();}
        double XSec() const {return xsec;}
        double XErr() const {return xerr;}
        std::vector<double> XSecVars() const {return xsec_vars;}
        Scales GetScales(const size_t& i=0) const {return scales[i];}
        std::vector<Scales> GetScalesVec() const {return scales;}
        double BeamA() const {return beamA;}
        double BeamB() const {return beamB;}
        double Scale() const {return scale;}
        double AlphaEM() const {return alphaEM;}
        double AlphaS() const {return alphaS;}
        std::map<std::string,double>* WgtDetails() const {return weights_detailed;}
        std::vector<double>* WgtCompressed() const {return weights_compressed;}
        LHEF::LHAscales* LHAScales() const {return lha_scales;}
        LHEF::LHAweights* Weights() const {return weights;}
        LHEF::LHArwgt* RWeights() const {return rwgt;}
        std::map<std::string,std::string>* EventAttributes() const {return eventAttributes;}

        // Access Operators
        Particle& operator[](const size_t &i) {return particles[i];}
        const Particle& operator[](const size_t &i) const {return particles[i];}

        // Conatiner iterators for particles
        Particle& front() {return particles.front();}
        Particle& at(const size_t &i) {return particles.at(i);}
        Particle& back() {return particles.back();}

    private:
        double beamA, beamB;
        double xsec, xerr;
        std::vector<Scales> scales;
        std::vector<Particle> particles;
        std::vector<double> xsec_vars;
        double scale, alphaEM, alphaS;

        // TODO
        // Contents of the LHAinitrwgt tag
        LHEF::LHAinitrwgt *initrwgt;

        // Contents of the LHAgenerator tags.
        std::vector<LHEF::LHAgenerator > *generators;

        // A map of the LHAweightgroup tags, indexed by name.
        std::map<std::string,LHEF::LHAweightgroup > *weightgroups;

        // A map of the LHAweight tags, indexed by name.
        std::map<std::string,LHEF::LHAweight > *init_weights;

        // Store current-event Les Houches event tags.
        std::map<std::string, std::string > *eventAttributes;

        // The weights associated with this event, as given by the LHAwgt tags
        std::map<std::string,double > *weights_detailed;

        // The weights associated with this event, as given by the LHAweights tags
        std::vector<double > *weights_compressed;

        // Contents of the LHAscales tag.
        LHEF::LHAscales *lha_scales;

        // Contents of the LHAweights tag (compressed format)
        LHEF::LHAweights *weights;

        // Contents of the LHArwgt tag (detailed format)
        LHEF::LHArwgt *rwgt;

        // Vectorized version of LHArwgt tag, for easy and ordered access.
        std::vector<double> weights_detailed_vector;
};

#endif
