#ifndef CUTS_HH
#define CUTS_HH

#include <vector>

class FourVector;

namespace IO {
    class Settings;
}

namespace ResBos {
    class Cuts {
        public:
            // Initialize Default cuts
            Cuts(IO::Settings*);
            virtual ~Cuts() {};

            // Functions that preform cuts
            bool operator()(const std::vector<int>& p, const std::vector<FourVector>& f) const {
                return MakeCuts(p,f);
            }
            bool MakeCuts(const std::vector<int>&, const std::vector<FourVector>&) const;
            bool UserCuts(const std::vector<int>&, const std::vector<FourVector>&) const;

        private:
            // Cuts variables
            double qMin, qMax, qtMin, qtMax, yMin, yMax;
            double ptlMin, ptlMax, ylMin, ylMax, METMin, METMax;
            double drllMin, drllMax;
            double ptl1Min, ptl2Min, ptl1Max, ptl2Max;
    };
}

#endif
