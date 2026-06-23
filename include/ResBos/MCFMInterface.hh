#ifndef MCFMINTERFACE_HH
#define MCFMINTERFACE_HH

#include "ResBos/ResBos.hh"

extern "C" {
    typedef ResBos::ResBos RESBOS;

    // Constructor
    std::shared_ptr<RESBOS> CreateResbos(char*, size_t);

    // Destructor
    void DeleteResbos(std::shared_ptr<RESBOS>);

    // Functions
    double MatrixElement(const std::shared_ptr<RESBOS>, double*);
    void InitElectroweak(std::shared_ptr<RESBOS>, double, double, double);
}


#endif
