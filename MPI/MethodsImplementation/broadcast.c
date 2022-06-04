#include <mpi.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
int MPI_BroadCast(void* buf, int count, MPI_Datatype dtype, int root, MPI_Comm comm){
	char* buf2[20];
    int noOfProcesses, rank, counter = 0;
    MPI_Comm_size(comm, &noOfProcesses);
    MPI_Comm_rank(comm, &rank);  
    if (rank == root) {
    MPI_Request request;
    MPI_Irecv(buf2, count, dtype, root, MPI_ANY_TAG, comm, &request);
        for(counter = 0; counter < noOfProcesses; counter++) {
        		if (counter != root) {
              		 MPI_Send(buf, count, dtype, counter, counter + 1, comm);
               }
               else {
               		MPI_Send(buf, count, dtype, counter, counter + 1, comm);
               		MPI_Wait(&request, MPI_STATUS_IGNORE);
               		memcpy(buf, buf2, 13);
               		//MPI_Recv(buf, count, dtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
               		}
        }
        // MPI_Recv(buf, count, dtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Recv(buf, count, dtype, root, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char *argv[])
{
    char message[20];
	int i, rank, size;
	MPI_Status status;
	int root = 0;
	MPI_Init(&argc, &argv) ;
	MPI_Comm_size (MPI_COMM_WORLD,&size);
	MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    if (rank==root)
		strcpy(message,"Hello, world");
	
	//All will execute this
	MPI_BroadCast(message,13,MPI_CHAR,root,MPI_COMM_WORLD);

	printf("\nMessage from %d process : %s",rank, message);
    MPI_Finalize();
printf("\n");	
    return 0;
}


// MPI_Recv(void* data,int count,MPI_Datatype type, int source,int tag,MPI_Comm comm,MPI_Status* status)
// MPI_Recv(rdata, 100, MPI_CHAR, 0, 20, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
// MPI_Send(void* data, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm)
// MPI_Send(sdata, 9, MPI_CHAR, 1, 20, MPI_COMM_WORLD);