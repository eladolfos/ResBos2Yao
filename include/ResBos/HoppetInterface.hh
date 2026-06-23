#ifndef HOPPETINTERFACE_HH
#define HOPPETINTERFACE_HH

#include <mutex>

#include "LHAPDF/LHAPDF.h"
#include "ResBos/Enums.hh"

// Header file to contain the interface between the C++ portion of the code, and the fortran of HOPPET

namespace Utility {

    class Hoppet{
        public:
            Hoppet() {};
            Hoppet(LHAPDF::PDF *p);
            virtual ~Hoppet(); 

            virtual double GetConvolution(const int&, const double&, const double&, 
                                          const int&, const int &j = 0, const int &k = 0);
            virtual double GetConvolution(const int&, const double&, const double&,
                                          const Splitting&);
            virtual std::map<int,double> GetConvolution(const double&, const double&, const int&,
                                                        const int &j=0, const int &k=0);
            void SwitchSet(LHAPDF::PDF *p);
        private:
            static std::mutex hoppetMutex;
    };

}

#endif
