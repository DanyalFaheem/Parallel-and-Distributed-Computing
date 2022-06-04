//=================Importing all the important libraries=========
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"
#include <unistd.h> // for sleep()
//=============================================================

// =========Stucture that will store information about a matrix
struct Matrix
{
    int row;
    int col;
    int **matValues;
};
//=============================================================

//==============Fuction to create to allocate and set values of matrix
struct Matrix Create_Matrix(struct Matrix m)
{
    m.matValues = calloc(m.row, sizeof(int *));       // contigious allocation for number of rows
    for (int counter = 0; counter < m.row; counter++) // run a loop for number of rows
    {
        m.matValues[counter] = calloc(m.col, sizeof(int)); // contigious allocation fro number of columns
        for (int count = 0; count < m.col; count++)        // run a loop for number of columns
        {
            m.matValues[counter][count] = rand() % 3; // set a random value between 0(inclusive) and 3(exclusive)
        }
    }
    return m; // return a matrix
}
//=============================================================
//==============Function to print the matrix ==================
void PrintMat(struct Matrix m)
{
    printf("rows: %d , columns : %d \n", m.row, m.col); // printing the number of rows and columns
    for (int counter = 0; counter < m.row; counter++)   // run a loop for number of rows
    {
        printf("| ");
        for (int count = 0; count < m.col; count++) // run a loop for number of columns
        {
            printf("%d ", m.matValues[counter][count]); // printing the number
        }
        printf("|\n");
    }
    printf("\n");
}
//=============================================================

//==========Function to get rows and columns from a line while reading from the file============
struct Matrix getDimensionsFromStr(char *line)
{
    char *tokenized;
    char delimiter[2] = " ";             // char through which the string will be tokenized
    tokenized = strtok(line, delimiter); // tokenizing the string using the deliminter
    int i = 0;
    struct Matrix m;          // creating a new matrix object
    while (tokenized != NULL) // run a loop for all the tokens
    {
        if (i == 0)                  // get the rows
            m.row = atoi(tokenized); // set the value of rows to tokenized
        else if (i == 2)             // get the columns
            m.col = atoi(tokenized); // set the value of columns to tokenized
        i++;
        tokenized = strtok(NULL, delimiter); // get the next token
    }
    return m; // return the matrix
}
//=============================================================

// =======================Checking if matrixes are square or have same dimensions
int checkingForSameDimensions(struct Matrix *mats, int len)
{
    for (int i = 0; i < len; i++) // run a loop from 0 to number of matrixes
    {
        if (mats[i].row != mats[i].col) // checking if matrix is not a square
        {
            for (int j = 0; j < len; j++) // run a loop from 0 to number of matrixes
            {
                if ((mats[i].row == mats[j].row && mats[i].col == mats[j].col) && i != j) // checking for same dimensions
                {
                    return 1; // if matrixes are same
                }
            }
        }
        else
        {

            return 1; // return true if square
        }
    }
    return 0;
}

//=============================================================
//==========Function to get the matrix Dimensions for optical order===========
int *getMatrixDimensions(struct Matrix *mats, int size)
{
    int *dim = malloc(size + 1 * sizeof(int)); // allocating for size+1
    dim[0] = mats[0].row;                      // set the first to row of first matrix
    for (int i = 1; i < size + 1; i++)         // run a loop from i=1 to number of matrixes +1
    {
        dim[i] = mats[i - 1].col; // get the matrix column and save to dim
    }
    return dim;
}
//=============================================================
// ==============Function to add brackets to optimal order=============
char *addBrackets(int i, int j, int n, int *otheTable)
{
    static int name = 0;
    static char arr[100];
    static int index = 0;
    if (i == j)
    {
        arr[index] = name + '0'; // save the index of the matrix to the arr
        name++;                  // increment the index of matrix
        index++;                 // increment the index of the arr
        return arr;
    }
    arr[index] = '(';                                         // add ( to the arr
    index++;                                                  // increment the index of the arr
    addBrackets(i, *((otheTable + i * n) + j), n, otheTable); // call the function recursively

    addBrackets(*((otheTable + i * n) + j) + 1, j, n, otheTable); // call the function recursively
    arr[index] = ')';                                             // add ( to the arr
    index++;                                                      // increment the index of the arr
    return arr;
}
//=============================================================
//===============get the optical order of multiplication========
char *leastCostMultiplication(int dimensions[], int n)
{
    int table[n][n];      // create a table for cost
    int otherTable[n][n]; // create a table for order

    for (int i = 1; i < n; i++) // run a loop for i=1 to size of matrixes
    {
        table[i][i] = 0; // set digonal values to 0
    }
    for (int i = 2; i < n; i++) // run a loop from 2 to number of matrixes
    {
        for (int j = 1; j < n - i + 1; j++) // run a loop from 1 to n-i+1
        {
            int x = j + i - 1;               // get the index
            table[j][x] = 99999999;          // set it to maximum
            for (int k = j; k <= x - 1; k++) // run the loop from j to x-1
            {
                int y = table[j][k] + table[k + 1][x] + dimensions[j - 1] * dimensions[k] * dimensions[x]; // set value to table
                if (y < table[j][x])                                                                       // if new cost is smaller than existing table value
                {
                    table[j][x] = y;      // set the table cost to new cost
                    otherTable[j][x] = k; // set the index in the order table
                }
            }
        }
    }
    return addBrackets(1, n - 1, n, (int *)otherTable); // call the function to brackets and return the array
}
//=============================================================

//==============Function to multiply the matrix using blocking calls==================
void blockingMatrixmultiplication(struct Matrix *one, struct Matrix *two, int rank, int nproc, int offset, int rows)
{
    int extra[one->row][two->col]; // create a new matrix with rows of first and columns of second matrix
    if (rank == 0)                 // if rank is zero, master processor
    {
        offset = 0;                     // set the offset of rows to  0
        rows = one->row / (nproc - 1);  // get the rows distribution
        for (int i = 1; i < nproc; i++) // run  aloop from 1 to number of processor
        {
            if (i == nproc - 1) // if it is the last processor
            {
                rows += one->row % (nproc - 1); // add remaining rows to last processor
            }
            MPI_Send(&rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);                                    // send rows to processor i
            MPI_Send(&offset, 1, MPI_INT, i, 0, MPI_COMM_WORLD);                                  // send offset to processor i
            MPI_Send(&one->matValues[offset][0], rows * one->col, MPI_INT, i, 1, MPI_COMM_WORLD); // send matrix to processor i
            offset += rows;                                                                       // add rows to offset
        }

        for (int i = 1; i < nproc; i++) // run a loop from i=1 to number of processors
        {
            MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);                           // recieve rows
            MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);                         // recieve offset
            MPI_Recv(&extra[offset][0], rows * two->col, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieve resulatant matrix chunk
        }
        printf("Blocking\n");
        // =================Printing the resultant matrix===================
        for (int i = 0; i < one->row; i++)
        {
            for (int j = 0; j < two->col; j++)
            {
                printf("%d ", extra[i][j]);
            }
            printf("\n");
        }
        //==================================================================
    }
    else
    {
        MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieve rows
        printf("%d rank process will compute %d rows\n", rank, rows);
        MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieve offset
        printf("%d rank process will start from %d rows\n", rank, offset);

        //================Multiplying matrix=============================
        for (int i = offset, l = 0; i < (offset + rows); i++, l++)
        {
            for (int j = 0; j < two->col; j++)
            {
                extra[l][j] = 0;
                for (int k = 0; k < two->row; k++)
                {
                    extra[l][j] += (one->matValues[i][k] * two->matValues[k][j]);
                }
            }
        }
        //=======================================================
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);                      // send rows to master
        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);                    // send offset to master
        MPI_Send(&extra[0][0], rows * two->col, MPI_INT, 0, 2, MPI_COMM_WORLD); // send resultant matrix to master
    }
}
//===================================================================

// Function to validate results of 2 matrices passed by comparing each value
int validateResults(float **matC, float **matD, int size)
{
    for (int counter = 0; counter < size; counter++) // run a loop from counter 0 to size
    {
        for (int count = 0; count < size; count++) // run a loop for number of columns
        {
            if (matC[counter][count] != matD[counter][count]) // if value does not match
            {
                printf("Invalid: %d, %d, %.0f, %.0f\n", counter, count, matC[counter][count], matD[counter][count]);
                return 0; // return false if values do not match
            }
        }
    }
    return 1; // return true if matrixes are same
}
//===================================================================

//=============Fucnction to multiply using blociking and non blocking calls===============
struct Matrix nonBlockingMatrixMultiplication(struct Matrix *one, struct Matrix *two, int rank, int nproc, int *offset, int *rows)
{
    MPI_Request request = MPI_REQUEST_NULL; // setting the request to null
    int extras[one->row][two->col];         // creating a matirx with first's rows and second matrix's colums
    if (rank == 0)                          // if rank is 0 (master processor)
    {
        rows = (int *)calloc(nproc, sizeof(int));       // allocate memory to rows
        offset = (int *)calloc(nproc + 1, sizeof(int)); // allocate memory to offset
        MPI_Request requests[nproc], req;               // create
        offset[1] = 0;
        int remainingRows = one->row % (nproc - 1); // get the mod of rows with number of slave processors
        for (int i = 1; i < nproc; i++)             // run a loop from i=1 to number of proces
        {
            rows[i] = one->row / (nproc - 1); // get the number of rows to be calculated by each processor
            if (remainingRows > 0)            // if remaining rows are greator than 0
            {
                rows[i]++;       // add one more row to each processor's apointed rows
                remainingRows--; // decrement remaining rows
            }

            MPI_Isend(&rows[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &req);   // send rows to processor i
            MPI_Isend(&offset[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &req); // send offset to processor i
            MPI_Isend(&one->matValues[offset[i]][0], rows[i] * one->col, MPI_INT, i, 1, MPI_COMM_WORLD, &req);
            // send matrix to processor i

            for (int j = i; j < nproc; j++) // update offset to all other processors
            {
                offset[j + 1] += rows[i];
            }
        }
        MPI_Status status;
        for (int i = 1; i < nproc; i++) // run a loop to recieve resultant matrix from all processors
        {
            MPI_Irecv(&extras[offset[i]][0], rows[i] * two->col, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &requests[i]);
        }
        int flag = 0;
        for (int i = 1; i < nproc;)
        {
            MPI_Test(&requests[i], &flag, &status);
            if (flag)
            {
                i++;
                flag = 0;
            }
        }
        // printf("Non blocking \n");
        // for (int i = 0; i < one->row; i++)
        // {
        //     for (int j = 0; j < two->col; j++)
        //     {
        //         printf("%d ", extras[i][j]);
        //     }
        //     printf("\n");
        // }
    }
    else
    {
        int row, offsets;
        MPI_Recv(&row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieve rows from processor 0
        // printf("%d rank process will compute %d rows\n", rank, row);
        MPI_Recv(&offsets, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // recieve offset from processor 0
        // printf("%d rank process will start from %d rows\n", rank, offsets);

        //======================================================
        for (int i = offsets, l = 0; i < (offsets + row); i++, l++) // run a loop to multiply the matrixes
        {
            for (int j = 0; j < two->col; j++)
            {
                extras[l][j] = 0;
                for (int k = 0; k < two->row; k++)
                {
                    extras[l][j] += (one->matValues[i][k] * two->matValues[k][j]);
                }
            }
        }

        //======================================================
        MPI_Request req;
        MPI_Isend(&extras[0][0], row * two->col, MPI_INT, 0, 2, MPI_COMM_WORLD, &req); // send the resultant matrix
    }

    MPI_Wait(&request, MPI_STATUS_IGNORE); // call MPI-wait to wait for non blocking calls to finish

    MPI_Bcast(&extras[0][0], one->row * two->col, MPI_INT, 0, MPI_COMM_WORLD); // broadcast the resultant matrix from rank 0 to all processors

    MPI_Wait(&request, MPI_STATUS_IGNORE);

    struct Matrix m; // create a new matrix to return
    m.row = one->row;
    m.col = two->col;
    m.matValues = calloc(m.row, sizeof(int *)); // allocate space for new matrix rows
    for (int counter = 0; counter < m.row; counter++)
    {
        m.matValues[counter] = calloc(m.col, sizeof(int)); // allocate space for new matrix columns
        for (int count = 0; count < m.col; count++)
        {
            m.matValues[counter][count] = extras[counter][count]; // copy the values
        }
    }
    return m; // return the matrix
}

//=============================================================
// struct to hold the information for next new matrixes indexes to be multiplied
struct indexes
{
    int left;
    int right;
    int index1;
    int index2;
};
//=============================================================

// =============Function to get the next matrixes to be multiplited and their indexes
struct indexes getNextMatrixes(char *abc)
{
    struct indexes I1;
    I1.index1 = -1; // set all the variables to -1
    I1.index2 = -1;
    I1.left = -1;
    I1.right = -1;
    for (int i = 0; i < strlen(abc); i++) // run a loop for the length of the char array
    {
        if (abc[i] == '(') // if char is opening bracket, update the first non bracket to -1
        {
            I1.left = i;    // set index for opening matrix to i
            I1.index1 = -1; // set the left matrix index to -1
        }
        if (abc[i] == ')' && I1.index1 == -1) // if char is closing bracket
        {
            I1.left = -1;   // set index for opening matrix to -1
            I1.index1 = -1; // set the left matrix index to -1
        }
        else if (abc[i] != '(' && abc[i] != ')' && I1.index1 != -1)
        {
            I1.index2 = abc[i] - '0'; // set the right matrix
            I1.right = i + 1;         // set index for closing matrix to i +1
            break;
        }
        else if (abc[i] != '(' && abc[i] != ')' && I1.index1 == -1)
        {
            I1.index1 = abc[i] - '0'; // set the left matrix index
        }
    }

    return I1;
}
//===========================================================================
int main(int argc, char **argv)
{
    srand(time(NULL));                               // set the seed from current time
    FILE *fileToRead = fopen("dimensions.txt", "r"); // opening the file for reading

    if (fileToRead == NULL) // if file is not present or cannot be opened
    {
        printf("Error opening file");
        return 0;
    }
    else
    {
        char *SingleLine = malloc(30 * sizeof(char));                 // allocating the memory for line
        int size = 5;                                                 // minimum number of matrixes
        struct Matrix *AllMat = malloc(size * sizeof(struct Matrix)); // allocating the memory for matrixes
        int i = 0;
        while (fgets(SingleLine, sizeof(SingleLine), fileToRead)) // read line by line
        {
            if (size > 7) // if matrixes is greator than 7
            {
                printf("The number of Matrices can not be greator than 7\n");
                return 0;
            }
            if (strchr(SingleLine, 'X') != NULL) // if line does not contain 'X' (removing garbage lines)
            {
                if (i == size) // if number of matrixes is equal to size
                {
                    size++;
                    AllMat = realloc(AllMat, size * sizeof(struct Matrix)); // reallocate more memory for new matrixes
                }
                AllMat[i] = getDimensionsFromStr(SingleLine); // get the matrix rows and columns
                i++;
            }
        }
        if (SingleLine) // if line is not empty
        {
            free(SingleLine); // free the memory
        }

        if (i < 5) // if number of matrixes is less than 5
        {
            printf("Number of matrixes can not be less than 5\n");
            return 0;
        }

        if (checkingForSameDimensions(AllMat, size) == 1) // if matrixes are square or have same matrixes
        {
            printf("Error! some matrices have same dimensions or are square matrices\n");
            return 0;
        }

        for (i = 0; i < size; i++) // run a loop from 0 to number of matrixes
        {
            AllMat[i] = Create_Matrix(AllMat[i]); // setting random values to the matrixes
        }
        int *dim = getMatrixDimensions(AllMat, size);         // get the dimensions for optical order
        char *order = leastCostMultiplication(dim, size + 1); // get optical order in char array

        int rank, nproc;
        int offset, *offsets;
        int rows, *row;
        MPI_Init(&argc, &argv);                // initalize MPI
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // get rank of the processor
        MPI_Comm_size(MPI_COMM_WORLD, &nproc); // get number of processors
        struct Matrix abc;
        double time_spent = 0.0;
        clock_t begin = clock();
        for (int i = 0; i < size - 1; i++) // run a loop for i=0 to number of multiplicans
        {
            struct indexes I1;
            I1 = getNextMatrixes(order); // get next matrix indexes to be multiplied
            // if(rank==0)
            // printf("%s\n",order);
            char *newStr = malloc(strlen(order) - 4 * sizeof(char)); // create a new char array
            int j = 0;
            for (int i = 0; i < strlen(order); i++) // run aloop to update the order
            {
                if (i < I1.left || i > I1.right - 1) // copy the order without the innermost parenthisis
                {
                    newStr[j] = order[i];
                    j++;
                }
            }

            abc = nonBlockingMatrixMultiplication(&AllMat[I1.index1], &AllMat[I1.index2], rank, nproc, &offset, &rows); // multily matrix using blocking and non blocking
            AllMat[I1.index1] = abc;                                                                                    // update the matrixes array to store the result
            newStr[I1.left] = (I1.index1 + '0');                                                                        // update the new order
            order = newStr;                                                                                             // order is updated
            MPI_Barrier(MPI_COMM_WORLD);                                                                                // MPI barrier is called so that all processors are blocked until all processors  have reached here
        }
        // if (rank == 0) // print resulatant matrixes
            // PrintMat(abc);

        MPI_Finalize(); // terminate MPI
        if (rank == 0)
        {
            clock_t end = clock();
            time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

            printf("The elapsed time is %f seconds\n", time_spent);
        }
    }
    return 0;
}