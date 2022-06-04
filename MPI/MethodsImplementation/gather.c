#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int MPI_gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
               MPI_Comm comm)
{
    int noOfProcesses, rank, counter = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
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
    }
    else
    {
        MPI_Send(sendbuf, sendcount, sendtype, root, 1, comm);
    }
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
        sendBuf[i] = rank;

    // Everyone executes this, process 0 gathers from all
    MPI_gather(sendBuf, 3, MPI_INT, recvBuf, 3, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (i = 0; i < 12; i++)
        {
            printf("\nProcess number %d send data : %d", i / 3, recvBuf[i]);
        }
    }

    MPI_Finalize();
    printf("\nBye from %d\n", rank);
    return 0;
}