#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int MPI_reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
               MPI_Op op, int root, MPI_Comm comm)
{
    int noOfProcesses, rank, counter = 0, i = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        int *result = (calloc(count, sizeof(int)));
        MPI_Request request;
        MPI_Irecv(recvbuf, count, datatype, root, MPI_ANY_TAG, comm, &request);
        for (counter = 0; counter < noOfProcesses; counter++)
        {
            if (counter != root)
            {
                //printf("send: %d\n", *(int *)sendbuf);
                MPI_Recv(recvbuf, count, datatype, counter, 1, comm, MPI_STATUS_IGNORE);
                //printf("recv: %d\n", *(int *)recvbuf);
            }
            else
            {
                MPI_Send(sendbuf, count, datatype, root, 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            for (i = 0; i < count; i++)
            {
                result[i] += *(((int *)recvbuf) + i);
            }
            //printf("recv: %d\n", *(int *)recvbuf);
            // recvbuf = (void *)(((int *)recvbuf) + recvcount);
        }
        // MPI_Send(sendbuf, sendcount, sendtype, rank, rank + 1, comm);
        for (i = 0; i < count; i++)
        {
            *(((int *)recvbuf) + i) = result[i];
        }
        free(result);
    }
    else
    {
        MPI_Send(sendbuf, count, datatype, root, 1, comm);
    }
}
int main(int argc, char *argv[])
{
    int sendBuf[3];
    int recvBuf[3];
    int nrank, nproc, i;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &nrank);
    for (i = 0; i < 3; i++)
    {
        sendBuf[i] = nrank + i;
    }
    MPI_reduce(sendBuf, recvBuf, 3, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (nrank == 0)
    {

        for(i = 0; i < 3; i++) 
        printf("irecv: %d\n", recvBuf[i]);
    }
    MPI_Finalize();
    printf("\nBye from %d\n", nrank);
    return 0;
}