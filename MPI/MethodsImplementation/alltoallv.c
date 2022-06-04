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
int MPI_alltoallv(const void *sendbuf, const int *sendcounts,
                  const int *sdispls, MPI_Datatype sendtype, void *recvbuf,
                  const int *recvcounts, const int *rdispls, MPI_Datatype recvtype,
                  MPI_Comm comm)
{
    MPI_Request request;
                        const void *recvbufcopy = recvbuf;
    int noOfProcesses, rank, counter = 0, counts = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    MPI_Barrier(comm);
    for (counts = 0; counts < noOfProcesses; counts++)
    {
        if (rank == counts)
        {
            const void *sendbufcopy = sendbuf;

                        recvbuf = (void *)(((int *)recvbufcopy) + rdispls[rank]);
            MPI_Irecv(recvbuf, recvcounts[rank], recvtype, counts, MPI_ANY_TAG, comm, &request);
            for (counter = 0; counter < noOfProcesses; counter++)
            {
                sendbuf = (void *)(((int *)sendbufcopy) + sdispls[counter]);
                if (counter != counts)
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
            recvbuf = (void *)(((int *)recvbufcopy) + rdispls[rank]);
            MPI_Recv(recvbuf, recvcounts[rank], recvtype, counts, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
        }
        MPI_Barrier(comm);
    }
    MPI_Barrier(comm);
}
int main(int argc, char *argv[])
{
    
 int i, j, k, rank, nproc;
    int *isend;
    int *irecv = (int *)(malloc(sizeof(int) * 30));
    int sdispl[4] = {0, 5, 9, 12};
    int scounts[4] = {5, 3, 2, 4};
    int rdispl[4] = {0, 5, 9, 12};
    int rcounts[4] = {5, 3, 2, 4};a
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    
    isend = (int *)(malloc(sizeof(int) * 30));
    for (i = 0; i < 30;i++)
        isend[i] = rank;

    // Everyone executes this, process 0 gathers data
    MPI_alltoallv(isend, scounts, sdispl, MPI_INT, irecv, rcounts, rdispl, MPI_INT,  MPI_COMM_WORLD);
        for (i = 0; i < nproc; i++)
        {
            for (j = rdispl[i], k = 0; k < rcounts[i]; k++, j++)
            {
                printf("Process with rank %d has sent data %d at index %d\n", i, irecv[j], j);
            }
        }
    MPI_Finalize();
    printf("\nBye from %d\n", rank);
    return 0;
}