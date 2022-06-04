#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MPI_BroadCast(void *buf, int count, MPI_Datatype dtype, int root, MPI_Comm comm)
{
    int *buf2[count];
    int noOfProcesses, rank, counter = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        MPI_Request request;
        MPI_Irecv(buf2, count, dtype, root, MPI_ANY_TAG, comm, &request);
        for (counter = 0; counter < noOfProcesses; counter++)
        {
            if (counter != root)
            {
                MPI_Send(buf, count, dtype, counter, counter + 1, comm);
            }
            else
            {
                MPI_Send(buf, count, dtype, counter, counter + 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
                memcpy(buf, buf2, count);
                // MPI_Recv(buf, count, dtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
            }
        }
        // MPI_Recv(buf, count, dtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(buf, count, dtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
}

int MPI_allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
    int noOfProcesses, rank, counter = 0, root = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        void *recvbufcopy = recvbuf;
        MPI_Request request;
        MPI_Irecv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, &request);
        for (counter = 0; counter < noOfProcesses; counter++)
        {
            if (counter != root)
            {
                //printf("send: %d\n", *(int *)sendbuf);
                MPI_Recv(recvbuf, recvcount, recvtype, counter, 1, comm, MPI_STATUS_IGNORE);
                //printf("recv: %d\n", *(int *)recvbuf);
            }
            else
            {
                MPI_Send(sendbuf, sendcount, sendtype, root, 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            //printf("recv: %d\n", *(int *)recvbuf);
            recvbuf = (void *)(((int *)recvbuf) + recvcount);
        }
        // MPI_Send(sendbuf, sendcount, sendtype, rank, rank + 1, comm);
        recvbuf = recvbufcopy;
    }
    else
    {
        MPI_Send(sendbuf, sendcount, sendtype, root, 1, comm);
    }
    MPI_Barrier(comm);
    // recvbuf = (void *)(((int *)recvbuf) - ((recvcount * noOfProcesses) - recvcount));
    MPI_BroadCast(recvbuf, recvcount * noOfProcesses, recvtype, root, comm);
}
int main(int argc, char *argv[])
{
    int i, rank, nproc;
    int sendBuf[3];
    int recvBuf[12]; // for 4 processes, 3 ints for each
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Each process prepare send buff data
    for (i = 0; i < 3; i++)
    {
        sendBuf[i] = rank;
        recvBuf[i] = 0;
    }
    // Everyone executes this, process 0 gathers from all
    MPI_allgather(sendBuf, 3, MPI_INT, recvBuf, 3, MPI_INT, MPI_COMM_WORLD);

    for (i = 0; i < 12; i++)
    {
        printf("\nProcess number %d sent data to rank %d: %d", i / 3, rank, recvBuf[i]);
    }

    MPI_Finalize();
    printf("\nBye from %d\n", rank);
    return 0;
}