#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int MPI_gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, const int *recvcounts, const int *displs,
                MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int noOfProcesses, rank, counter = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    if (rank == root)
    {
        const void *recvbufcopy = recvbuf;
        recvbuf = (void *)(((int *)recvbufcopy) + displs[root]);
        MPI_Request request;
        MPI_Irecv(recvbuf, recvcounts[root], recvtype, root, MPI_ANY_TAG, comm, &request);
        for (counter = 0; counter < noOfProcesses; counter++)
        {
                        recvbuf = (void *)(((int *)recvbufcopy) + displs[counter]);
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

        }
    }
    else
    {
        MPI_Send(sendbuf, sendcount, sendtype, root, rank + 1, comm);
    }
}
int main(int argc, char *argv[])
{
    int i, j, k, rank, nproc;
    int *isend;
    int *irecv = (int *)(malloc(sizeof(int) * 30));
    int displ[4] = {0, 5, 9, 12};
    int counts[4] = {5, 3, 2, 4};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    
    isend = (int *)(malloc(sizeof(int) * 5));
    for (i = 0; i < counts[rank];i++)
        isend[i] = rank;

    // Everyone executes this, process 0 gathers data
    MPI_gatherv(isend, counts[rank], MPI_INT, irecv, counts, displ, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = displ[i], k = 0; k < counts[i]; k++, j++)
            {
                printf("Process with rank %d has sent data %d at index %d\n", i, irecv[j], j);
            }
        }
    }
    MPI_Finalize();
    printf("Bye from %d\n", rank);
}