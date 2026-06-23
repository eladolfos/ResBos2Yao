#include "ResBos/MPI.hh"

namespace ResBos {

    MPI *mpi(NULL);

    MPI::MPI() {
#ifdef USING_MPI
        mComm = MPI_COMM_WORLD;
#endif
    }

    void MPI::PrintRankInfo() {
#ifdef USING_MPI
        const int size = Size();
        if(size > 1) 
            LOG_F(INFO, "Running on %d ranks.", size);
#endif
    }

}
