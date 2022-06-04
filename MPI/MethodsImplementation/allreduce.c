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

int MPI_allreduce(const void *sendbuf, void *recvbuf, int count,
                  MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
    int noOfProcesses, rank, counter = 0, i = 0, root = 0;
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
    MPI_Barrier(comm);
    MPI_BroadCast(recvbuf, count, datatype, root, comm);
}
int main(int argc, char *argv[])
{
    int sendBuf[2];
    int recvBuf[2];
    int nrank, nproc, i;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &nrank);
    for (i = 0; i < 2; i++)
    {
        sendBuf[i] = nrank + i;
    }
    MPI_allreduce(sendBuf, recvBuf, 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    for (i = 0; i < 2; i++)
        printf("irecv: %d\n", recvBuf[i]);
    MPI_Finalize();
    printf("\nBye from %d\n", nrank);
    return 0;
}