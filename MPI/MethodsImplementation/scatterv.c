#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int MPI_scatterv(const void *sendbuf, const int *sendcounts, const int *displs,
                 MPI_Datatype sendtype, void *recvbuf, int recvcount,
                 MPI_Datatype recvtype,
                 int root, MPI_Comm comm)
{
    int noOfProcesses, rank, counter = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        const void *sendbufcopy = sendbuf;
        MPI_Request request;
        MPI_Irecv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, &request);
        for (counter = 0; counter < noOfProcesses; counter++)
        {
            sendbuf = (void *)(((int *)sendbufcopy) + displs[counter]);
            if (counter != root)
            {
                MPI_Send(sendbuf, sendcounts[counter], sendtype, counter, counter + 1, comm);
            }
            else
            {
                MPI_Send(sendbuf, sendcounts[counter], sendtype, counter, counter + 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            //printf("send: %d\n", *(int *)sendbuf);
        }
        // MPI_Recv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
}
int main(int argc, char *argv[])
{
    int i, rank, nproc;
    int *isend;
    int *irecv = (int *)(malloc(sizeof(int) * 17));
    int displ[4] = {0, 5, 9, 12};
    int counts[4] = {5, 3, 2, 4};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Master process prepare message
    if (rank == 0)
    {
        isend = (int *)(malloc(sizeof(int) * 17));
        for (i = 0; i < 17;)
            isend[i] = ++i;
    }

    // Everyone executes this, process 0 scaters data
    MPI_scatterv(isend, counts, displ, MPI_INT, irecv, counts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    for (i = 0; i < counts[rank]; i++)
        printf("Process with rank %d has received data %d\n", rank, irecv[i]);

    MPI_Finalize();
    printf("Bye from %d\n", rank);

    return 0;
}