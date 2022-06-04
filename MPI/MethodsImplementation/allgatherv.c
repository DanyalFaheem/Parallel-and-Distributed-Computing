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

int MPI_allgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                   void *recvbuf, const int *recvcounts, const int *displs,
                   MPI_Datatype recvtype, MPI_Comm comm)

{
    int noOfProcesses, rank, counter = 0, root = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        void *recvbufcopy = recvbuf;
        recvbuf = (void *)(((int *)recvbufcopy) + displs[root]);
        MPI_Request request;
        MPI_Irecv(recvbuf, recvcounts[root], recvtype, root, MPI_ANY_TAG, comm, &request);
        for (counter = 0; counter < noOfProcesses; counter++)
        {
            if (counter != root)
            {
                MPI_Recv(recvbuf, recvcounts[counter], recvtype, counter, counter + 1, comm, MPI_STATUS_IGNORE);
            }
            else
            {
                MPI_Send(sendbuf, sendcount, sendtype, counter, counter + 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            //printf("recv: %d\n", *(int *)recvbuf);
            recvbuf = (void *)(((int *)recvbufcopy) + displs[counter]);
        }
        recvbuf = recvbufcopy;
    }
    else
    {
        MPI_Send(sendbuf, sendcount, sendtype, root, rank + 1, comm);
    }
    MPI_Barrier(comm);
    // recvbuf = (void *)(((int *)recvbuf) - ((recvcount * noOfProcesses) - recvcount));
    MPI_BroadCast(recvbuf, recvcounts[noOfProcesses - 1] + displs[noOfProcesses - 1], recvtype, root, comm);
}
int main(int argc, char *argv[])
{
    int i, j, k, rank, nproc;
    int *isend, *irecv;
    irecv = (int *)(malloc(sizeof(int) * 30));
    int displ[4] = {0, 5, 9, 12};
    int counts[4] = {5, 3, 2, 4};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    isend = (int *)(malloc(sizeof(int) * counts[rank]));
    for (i = 0; i < counts[rank]; i++)
        isend[i] = rank;
    // Everyone executes this, process 0 scaters data
    MPI_allgatherv(isend, counts[rank], MPI_INT, irecv, counts, displ, MPI_INT, MPI_COMM_WORLD);

    for (i = 0; i < nproc; i++)
    {
        for (j = displ[i], k = 0; k < counts[i]; k++, j++)
        {
            printf("Process with rank %d has sent data %d at index %d\n", i, irecv[j], j);
        }
    }
    MPI_Finalize();
    printf("Bye from %d\n", rank);
}