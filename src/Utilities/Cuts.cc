#include <algorithm>
#include <limits>
#include <cmath>

#include "ResBos/Cuts.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/Settings.hh"

namespace ResBos {
    Cuts::Cuts(IO::Settings *settings) {
        // Load in settings from config file
        qMin = settings -> GetSettingDouble("qMin");
        qMax = settings -> GetSettingDouble("qMax");

        qtMin = settings -> GetSettingDouble("qtMin");
        qtMax = settings -> GetSettingDouble("qtMax");

        yMin = settings -> GetSettingDouble("yMin");
        yMax = settings -> GetSettingDouble("yMax");

        ptlMin = settings -> GetSettingDouble("ptlMin");
        ptlMax = settings -> GetSettingDouble("ptlMax");

        ylMin = settings -> GetSettingDouble("ylMin");
        ylMax = settings -> GetSettingDouble("ylMax");

        METMin = settings -> GetSettingDouble("METMin");
        METMax = settings -> GetSettingDouble("METMax");

        drllMin = settings -> GetSettingDouble("drllMin");
        drllMax = settings -> GetSettingDouble("drllMax");

        ptl1Min = settings -> GetSettingDouble("ptl1Min");
        ptl2Min = settings -> GetSettingDouble("ptl2Min");
        ptl1Max = settings -> GetSettingDouble("ptl1Max");
        ptl2Max = settings -> GetSettingDouble("ptl2Max");

        // Ensure that the hardest lepton is always the first
        if(ptl1Min < ptl2Min) std::swap(ptl1Min,ptl2Min);
        if(ptl1Max < ptl2Max) std::swap(ptl1Max,ptl2Max);

        // If negative number for a max cut, set it to a large value
        if(ptlMax < 0) ptlMax = std::numeric_limits<double>::infinity();
        if(ylMax < 0) ylMax = std::numeric_limits<double>::infinity();
        if(METMax < 0) METMax = std::numeric_limits<double>::infinity();
        if(drllMax < 0) drllMax = std::numeric_limits<double>::infinity();
        if(ptl1Max < 0) ptl1Max = std::numeric_limits<double>::infinity();
        if(ptl2Max < 0) ptl2Max = std::numeric_limits<double>::infinity();
    }

    bool Cuts::MakeCuts(const std::vector<int>& pids, 
            const std::vector<FourVector>& momenta) const {
        // Define containers for each type of particle
        std::vector<FourVector> bosons, leptons, neutrinos;

        // Find all the different types of particles
        for(size_t i = 0; i < pids.size(); ++i) {
            if(pids[i] > 21 && pids[i] < 38) { // Find bosons
                bosons.push_back(momenta[i]);
            } else if(std::abs(pids[i]) == 11 || std::abs(pids[i]) == 13) { // Find leptons
                leptons.push_back(momenta[i]);
            } else if(std::abs(pids[i]) == 12 || std::abs(pids[i]) == 14) { // Find neutrinos
                neutrinos.push_back(momenta[i]);
            }
        }

        FourVector q;
        if(bosons.size() == 1) { // Single Vector Boson
            q = bosons[0];
        } else if(bosons.size() == 2) { // Diboson production
            q = bosons[0] + bosons[1];
        } else if(bosons.size() == 3) { // Higgs decays
            q = bosons[1] + bosons[2];
        }

        // Cuts on the intermediate boson
        if((q.M() < qMin || q.M() > qMax) && qMin != qMax) return false;
        if(q.Pt() < qtMin || q.Pt() > qtMax) return false;
        if(std::abs(q.Rapidity()) < yMin || std::abs(q.Rapidity()) > yMax) return false;

        // Cuts on leptons
        std::vector<double> ptl;
        for(size_t i = 0; i < leptons.size(); ++i) {
            if(leptons[i].Pt() < ptlMin || leptons[i].Pt() > ptlMax) return false;
            if(std::abs(leptons[i].Rapidity()) < ylMin || std::abs(leptons[i].Rapidity()) > ylMax) 
                return false;
            for(size_t j = i+1; j < leptons.size(); ++j) {
                double drij = leptons[i].DeltaR(leptons[j]);
                if(drij < drllMin || drij > drllMax) return false;
            }
            ptl.push_back(leptons[i].Pt());
        }

        // Cuts on MET
        FourVector MET(0,0,0,0);
        for(auto neutrino : neutrinos) {
            MET += neutrino;
        }
        if(MET.Pt() < METMin || MET.Pt() > METMax) return false;

        // Sort the lepton pts and apply ordered cuts on them
        if(ptl.size() >= 2) {
            std::sort(ptl.begin(),ptl.end());
            if(ptl[0] < ptl1Min || ptl[0] > ptl1Max) return false;
            if(ptl[1] < ptl2Min || ptl[1] > ptl2Max) return false;
        }

        // Call user defined cuts
        return UserCuts(pids, momenta);
    }
}
