#include "ResBos/Cuts.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/ThreeVector.hh"

using namespace ResBos;

bool Cuts::UserCuts(const std::vector<int>& /*pids*/, const std::vector<FourVector>& /*momenta*/) const {
    // Add user defined cuts below
    // For more details, please see the ResBos2 Manual
    return true;
}
