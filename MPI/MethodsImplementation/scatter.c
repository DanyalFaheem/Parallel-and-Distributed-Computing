#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
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
            //printf("send: %d\n", *(int *)sendbuf);
            sendbuf = (void *)(((int *)sendbuf) + sendcount);
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
int i,rank,nproc,irecv;
	int* isend;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	//Master process prepare message
	if (rank==0) {
		isend = (int*)(malloc(sizeof(int)*nproc));
		for (i=0;i<nproc;)
			isend[i]= ++i;
	}
	
	//Everyone executes this, process 0 scaters data
	MPI_scatter(isend,1,MPI_INT,&irecv,1,MPI_INT,0,MPI_COMM_WORLD);
        printf("Process with rank %d has received data %d\n",rank,irecv);

	MPI_Finalize();
	printf("Bye from %d\n",rank);

return 0;
}