#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Our main function which all processes run 
void winnerGame(int *buffer, int arrSize, int searchValue, char *sendbuf, char *recvbuf)
{
    int i, rank, size, counter;
    MPI_Status status;
    MPI_Request request = MPI_REQUEST_NULL;
    int root = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == root)
    {
        // If root process then will wait for a recv call from each process
        for (int counter = 0; counter < size - 1; counter++)
        {
            MPI_Irecv(recvbuf, 6, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            // Get source of recv
            int source = status.MPI_SOURCE;
            if (status.MPI_TAG == 1)
            {
                // If Tag is 1, meaning data found by slave, abort all other processes
                for (int counter = 0; counter < size; counter++)
                {
                    if (counter != rank && counter != source)
                    {
                        strcpy(sendbuf, "Abort");
                        MPI_Send(sendbuf, 6, MPI_CHAR, counter, 0, MPI_COMM_WORLD);
                    }
                }
                printf("\nValue %d found at process %d\n\n", searchValue, source);
                return;
            }
            else if (status.MPI_TAG == 0)
            {
                /* code */
            }
            // If tag is 2, meaning processes exited without finding value
            else if (status.MPI_TAG == 2)
            {
                printf("Process %d exiting after completing search\n", source);
            }
        }
        printf("\nValue not found at any slave process\n");
    }
    else
    {
        // Start a recv call to abort from root
        MPI_Irecv(recvbuf, 6, MPI_CHAR, root, 0, MPI_COMM_WORLD, &request);
        int flag = 0;
        for (int counter = 0; counter < arrSize; counter++)
        {
            // Check if recv call completed
            MPI_Test(&request, &flag, &status);
            if (flag)
            {
                // If tag is 0 meaning process has to abort
                if (status.MPI_TAG == 0)
                {
                    printf("Process %d aborting\n", rank);
                    return;
                }
            }
            // Check if value found
            if (buffer[counter] == searchValue)
            {
                strcpy(sendbuf, "Found");
                // Tell root value has been foudn
                MPI_Send(sendbuf, 6, MPI_CHAR, root, 1, MPI_COMM_WORLD);
                return;
            }
        }
        // Tell root I did not find value
        strcpy(sendbuf, "Nound");
        MPI_Send(sendbuf, 6, MPI_CHAR, root, 2, MPI_COMM_WORLD);
    }
    // MPI_Barrier(MPI_COMM_WORLD);
}

int main(int argc, char *argv[])
{
    // Initialize our variables
    srand(time(NULL));
    int *sendBuffer, *recvBuffer;
    char sendbuf[10], recvbuf[10];
    int *displs, *sendCounts;
    int i, rank, size, counter;
    MPI_Status status;
    int root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int searchValue;
    if (rank == root)
    {
        // Initialize sender buffer if root process
        sendBuffer = (int *)calloc((size * size) - size, sizeof(int));
        // Randomize values in buffer
        for (int counter = 0; counter < (size * size) - size; counter++)
        {
            sendBuffer[counter] = rand() % ((size * size) - size);
        }
        // Randomize value to search
        searchValue = rand() % ((size * size) - size);
        recvBuffer = (int *)calloc(0, sizeof(int));
        printf("\nValue to serch is %d\n", searchValue);
    }
    else
    {
        // If rank is not root, allocate recv buffer
        recvBuffer = (int *)calloc(size, sizeof(int));
    }
    // Broadcast search value to all slaves
    MPI_Bcast(&searchValue, 1, MPI_INT, root, MPI_COMM_WORLD);
    // Using scatterv to scatter array to all processes but not 0 process
    displs = (int *)calloc(size, sizeof(int));
    sendCounts = (int *)calloc(size, sizeof(int));
    int index = 0;
    displs[0] = sendCounts[0] = 0;
    for (int counter = 1; counter < size; counter++, index += size)
    {
        displs[counter] = index;
        sendCounts[counter] = size;
    }
    MPI_Scatterv(sendBuffer, sendCounts, displs, MPI_INT, recvBuffer, size, MPI_INT, root, MPI_COMM_WORLD);
    // Print all values here
    if (rank != 0)
    {
        printf("Values at tid %d are : ", rank);
        for (int counter = 0; counter < size; counter++)
        {
            printf("%d  ", recvBuffer[counter]);
        }
        printf("\n");
    }
    winnerGame(recvBuffer, size, searchValue, sendbuf, recvbuf);
    MPI_Finalize();
    // printf("\n");
    return 0;
}
