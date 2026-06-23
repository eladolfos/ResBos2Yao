#ifndef RESBOS_MPI_HH
#define RESBOS_MPI_HH

#include <iostream>

#ifdef USING_MPI
#include "mpi.h"
#endif
#include "ResBos/loguru.hpp"

namespace ResBos {

class MPI {
    public: 
        MPI();

        void PrintRankInfo();

#ifdef USING_MPI
        void Barrier() {
            MPI_Barrier(mComm);
        }

        int Rank() {
            int rank;
            MPI_Comm_rank(mComm, &rank);
            return rank;
        }

        int Size() {
            int size;
            MPI_Comm_size(mComm, &size);
            return size;
        }

        void Broadcast(void* buffer, int count, MPI_Datatype type, int root=0) {
            MPI_Bcast(buffer, count, type, root, mComm);
        }

        void Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
                     void* recvbuf, int recvcount, MPI_Datatype recvtype,
                     int root=0) {
            MPI_Scatter(sendbuf, sendcount, sendtype,
                        recvbuf, recvcount, recvtype,
                        root, mComm);
        }

        void Scatterv(void* sendbuf, int* sendcount, int* displace, MPI_Datatype sendtype,
                     void* recvbuf, int recvcount, MPI_Datatype recvtype,
                     int root=0) {
            MPI_Scatterv(sendbuf, sendcount, displace, sendtype,
                         recvbuf, recvcount, recvtype,
                         root, mComm);
        }

        void Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
                    void* recvbuf, int recvcount, MPI_Datatype recvtype,
                    int root=0) {
            MPI_Gather(sendbuf, sendcount, sendtype,
                       recvbuf, recvcount, recvtype,
                       root, mComm);
        }

        void ReduceAll(void* buffer, int count, MPI_Datatype type, MPI_Op op) {
            MPI_Allreduce(MPI_IN_PLACE, buffer, count, type, op, mComm);
        }

        void GatherAll(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                             void *recvbuf, int recvcount, MPI_Datatype recvtype) {
            MPI_Allgather(sendbuf, sendcount, sendtype,
                          recvbuf, recvcount, recvtype, mComm);
        }

        void GatherAllV(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                             void *recvbuf, int *recvcount, int* displace,
                             MPI_Datatype recvtype) {
            MPI_Allgatherv(sendbuf, sendcount, sendtype,
                           recvbuf, recvcount, displace, recvtype, mComm);
        }

        void Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag) {
            MPI_Recv(buf, count, datatype, source, tag, mComm, MPI_STATUS_IGNORE);
        }

        int Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag) {
            return MPI_Send(buf, count, datatype, dest, tag, mComm);
        }
#endif

    private:
#ifdef USING_MPI
        MPI_Comm mComm;
#endif
};

extern MPI *mpi;

}


#endif
