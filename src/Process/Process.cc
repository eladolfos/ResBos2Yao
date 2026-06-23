#include "ResBos/ResBos.hh"
#include "ResBos/Process.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Enums.hh"
#include "ResBos/QCDConst.hh"
#include "ResBos/Beam.hh"

namespace ResBos {

Process::Process(std::shared_ptr<ResBos> resbos_, const IO::Settings &settings) : resbos(resbos_) {
    ew = std::unique_ptr<Electroweak>(new Electroweak(&settings));
    ps = nullptr;
}

Process::~Process() {}

double Process::NonPert(const std::array<double, 2>&, const std::vector<double> &calc, bool) const {
    return calc[0];
}

std::array<double, 4> Process::CalcCorrection(double, const std::vector<double>&, bool) const {
    return {1, 1, 1, 1};
}

}
