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
                // printf("send: %d\n", *(int *)sendbuf);
                MPI_Recv(recvbuf, recvcount, recvtype, counter, 1, comm, MPI_STATUS_IGNORE);
                // printf("recv: %d\n", *(int *)recvbuf);
            }
            else
            {
                MPI_Send(sendbuf, sendcount, sendtype, root, 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            // printf("recv: %d\n", *(int *)recvbuf);
            recvbuf = (void *)(((int *)recvbuf) + recvcount);
        }
        // MPI_Send(sendbuf, sendcount, sendtype, rank, rank + 1, comm);
    }
    else
    {
        MPI_Send(sendbuf, sendcount, sendtype, root, 1, comm);
    }
}

int MPI_scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
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
                MPI_Send(sendbuf, sendcount, sendtype, counter, counter + 1, comm);
            }
            else
            {
                MPI_Send(sendbuf, sendcount, sendtype, counter, counter + 1, comm);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            // printf("send: %d\n", *(int *)sendbuf);
            sendbuf = (void *)(((int *)sendbuf) + sendcount);
        }
        // MPI_Recv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
}

int MPI_alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 MPI_Comm comm)
{
    MPI_Request request;

    int noOfProcesses, rank, counter = 0, counts = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);
    MPI_Barrier(comm);
    for (counts = 0; counts < noOfProcesses; counts++)
    {
        if (rank == counts)
        {
            MPI_Irecv(recvbuf, recvcount, recvtype, counts, MPI_ANY_TAG, comm, &request);
            for (counter = 0; counter < noOfProcesses; counter++)
            {
                if (counter != counts)
                {
                    MPI_Send(sendbuf, sendcount, sendtype, counter, counter + 1, comm);
                }
                else
                {
                    MPI_Send(sendbuf, sendcount, sendtype, counter, counter + 1, comm);
                    MPI_Wait(&request, MPI_STATUS_IGNORE);
                    recvbuf = (void *)(((int *)recvbuf) + recvcount);
                }
                // printf("send: %d\n", *(int *)sendbuf);
                sendbuf = (void *)(((int *)sendbuf) + sendcount);
            }
            // MPI_Recv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
        }
        else
        {
            MPI_Recv(recvbuf, recvcount, recvtype, counts, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
            recvbuf = (void *)(((int *)recvbuf) + recvcount);
        }
        MPI_Barrier(comm);
    }
    MPI_Barrier(comm);

}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    // Get number of processes and check that 3 processes are used
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get my rank
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Define my value
    int my_values[4];
    int i;
    for (i = 0; i < size; i++)
    {
        my_values[i] = my_rank + i * 100;
    }
    for (i = 0; i < size; i++)
    {
        printf("Process %d, my values = %d.\n", my_rank, my_values[i]);
    }
    int buffer_recv[4];
    MPI_alltoall(&my_values, 1, MPI_INT, buffer_recv, 1, MPI_INT, MPI_COMM_WORLD);
    for (i = 0; i < size; i++)
    {
        printf("Values collected on process %d: %d.\n", my_rank, buffer_recv[i]);
    }
    MPI_Finalize();
    printf("\nBye from %d\n", my_rank);
    return 0;
}