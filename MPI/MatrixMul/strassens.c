#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Matrix
{
  int row;
  int col;
  int **matValues;
};

//==============Fuction to create to allocate and set values of matrix
struct Matrix CreateMatrix(struct Matrix m)
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

struct Matrix *Create_Matrix(struct Matrix *m, int size)   //This function is being used to create matrices
{
  m->col = m->row = size;
  m->matValues = calloc(m->row, sizeof(int *));
  for (int counter = 0; counter < m->row; counter++)   //Outer loop handles the rows of matrix
  {
    m->matValues[counter] = calloc(m->col, sizeof(int));
    for (int count = 0; count < m->col; count++)      //Inner loop handles the columns of matrix
    {
      m->matValues[counter][count] = rand() % 3;   //Assigning random values less than 3
    }
  }
  return m;
}

struct Matrix getDimensionsFromStr(char *line)   //This function is tokenizing the string includes the dimensions for matrices 
{
  char *tokenized;
  char delimiter[2] = " ";
  tokenized = strtok(line, delimiter);
  int i = 0;
  struct Matrix m;
  while (tokenized != NULL)
  {
    if (i == 0)
      m.row = atoi(tokenized);
    else if (i == 2)
      m.col = atoi(tokenized);
    i++; 
    tokenized = strtok(NULL, delimiter);    //strtok is used to tokenize the string for getting dimensions
  }  
  return m;
}

//This function is used to check equal dimensions for matrices to be multiplied
int checkingForSameDimensions(struct Matrix *mats, int len)
{
  for (int i = 0; i < len; i++)
  {
    if (mats[i].row != mats[i].col)
    {
      for (int j = 0; j < len; j++)
      {
        if ((mats[i].row == mats[j].row || mats[i].col == mats[j].col) && i != j)
        {
          return 1;
        }
      }
    }
    else
    {
      printf("here");
      return 1;
    }
  }
  return 0;
}

int *getMatrixDimensions(struct Matrix *mats, int size)
{
  int *dim = malloc(size + 1 * sizeof(int));
  dim[0] = mats[0].row;
  for (int i = 1; i < size + 1; i++)
  {
    dim[i] = mats[i - 1].col;
  }

  return dim;
}

char *addBrackets(int i, int j, int n, int *otheTable)
{
  static int name = 0;
  static char arr[100];
  static int index = 0;
  if (i == j)
  {

    arr[index] = name + '0';
    name++;
    index++;
    return arr;
  }
  arr[index] = '(';
  index++;
  addBrackets(i, *((otheTable + i * n) + j), n, otheTable);

  addBrackets(*((otheTable + i * n) + j) + 1, j, n, otheTable);
  arr[index] = ')';
  index++;
  return arr;
}

char *leastCostMultiplication(int dimensions[], int n)
{
  int table[n][n];
  int otherTable[n][n];

  for (int i = 1; i < n; i++)
  {
    table[i][i] = 0;
  }
  for (int i = 2; i < n; i++)
  {
    for (int j = 1; j < n - i + 1; j++)
    {
      int x = j + i - 1;
      table[j][x] = 99999999;
      for (int k = j; k <= x - 1; k++)
      {
        int y = table[j][k] + table[k + 1][x] + dimensions[j - 1] * dimensions[k] * dimensions[x];
        if (y < table[j][x])
        {
          table[j][x] = y;
          otherTable[j][x] = k;
        }
      }
    }
  }
  return addBrackets(1, n - 1, n, (int *)otherTable);
}

struct indexes
{
  int left;
  int right;
  int index1;
  int index2;
};

struct indexes getNextMatrixes(char *abc)
{
  struct indexes I1;
  I1.index1 = -1;
  I1.index2 = -1;
  I1.left = -1;
  I1.right = -1;
  for (int i = 0; i < strlen(abc); i++)
  {
    if (abc[i] == '(')
    {
      I1.left = i;
      I1.index1 = -1;
    }
    if (abc[i] == ')' && I1.index1 == -1)
    {
      I1.left = -1;
      I1.index1 = -1;
    }
    else if (abc[i] != '(' && abc[i] != ')' && I1.index1 != -1)
    {
      I1.index2 = abc[i] - '0';
      I1.right = i + 1;
      break;
    }
    else if (abc[i] != '(' && abc[i] != ')' && I1.index1 == -1)
    {
      I1.index1 = abc[i] - '0';
    }
  }

  return I1;
}
struct Matrix Add_Padding_Size(struct Matrix *m, int size)
{
  if (size > m->row)
  {
    struct Matrix res;
    int extrarows, extracols;
    res.row = res.col = m->row + size - m->row;
    extrarows = extracols = size - m->row;
    res = *Create_Matrix(&res, res.row);
    for (int i = 0; i < m->row; i++)
    {
      for (int j = 0; j < m->col; j++)
      {
        res.matValues[i][j] = m->matValues[i][j];
      }
    }
    for (int i = res.row - extrarows; i < res.row; i++)
    {
      for (int j = 0; j < res.col; j++)
      {
        res.matValues[i][j] = 0;
      }
    }
    for (int i = res.col - extracols; i < res.col; i++)
    {
      for (int j = 0; j < res.row; j++)
      {
        res.matValues[j][i] = 0;
      }
    }
    for (int i = 0; i < m->row; i++)
    {
      free(m->matValues[i]);
    }
    free(m->matValues);
    return res;
  }
  else
  {
    return *m;
  }
}

struct Matrix Add_Padding(struct Matrix *m)
{
  if (m->row == m->col && m->row % 2 == 0)
  {
    return *m;
  }

  struct Matrix res;
  if (m->row > m->col)
  {
    int extrarows, extracols;
    extrarows = m->row % 2;
    res.row = m->row + extrarows;
    extracols = res.row - m->col;
    res.col = m->col + res.row - m->col;
    res = *Create_Matrix(&res, res.row);
    for (int i = 0; i < m->row; i++)
    {
      for (int j = 0; j < m->col; j++)
      {
        res.matValues[i][j] = m->matValues[i][j];
      }
    }
    for (int i = res.row - extrarows; i < res.row; i++)
    {
      for (int j = 0; j < res.col; j++)
      {
        res.matValues[i][j] = 0;
      }
    }
    for (int i = res.col - extracols; i < res.col; i++)
    {
      for (int j = 0; j < res.row; j++)
      {
        res.matValues[j][i] = 0;
      }
    }
    for (int i = 0; i < m->row; i++)
    {
      free(m->matValues[i]);
    }
    free(m->matValues);
  }
  else
  {
    int extrarows, extracols;
    extracols = m->col % 2;
    res.col = m->col + extracols;
    extrarows = res.col - m->row;
    res.row = m->row + extrarows;
    res = *Create_Matrix(&res, res.row);
    for (int i = 0; i < m->row; i++)
    {
      for (int j = 0; j < m->col; j++)
      {
        res.matValues[i][j] = m->matValues[i][j];
      }
    }
    for (int i = res.row - extrarows; i < res.row; i++)
    {
      for (int j = 0; j < res.col; j++)
      {
        res.matValues[i][j] = 0;
      }
    }
    for (int i = res.col - extracols; i < res.col; i++)
    {
      for (int j = 0; j < res.row; j++)
      {
        res.matValues[j][i] = 0;
      }
    }
    for (int i = 0; i < m->row; i++)
    {
      free(m->matValues[i]);
    }
    free(m->matValues);
  }
  return res;
}

void PrintMat(struct Matrix *m)        //Function which is accepting a pointer of struct Matric for printing the matrix
{
  printf("rows: %d , columns : %d \n", m->row, m->col);
  for (int counter = 0; counter < m->row; counter++)
  {
    printf("| ");
    for (int count = 0; count < m->col; count++)
    {
      printf("%d ", m->matValues[counter][count]);
    }
    printf("|\n");
  }
  printf("\n");
}

//This function is taking 2 sub matrices of size 2*2 
struct Matrix *Strassen(struct Matrix *A, struct Matrix *B)
{

  struct Matrix *C = (struct Matrix *)malloc(sizeof(struct Matrix));   //creating a 2*2 matric for storing the result
  C = Create_Matrix(C, 2);

  //This loop is being used to initialize the final matrix with 0's because afterwards the values will increment.
  for (int i = 0; i < C->row; ++i)
  {
    for (int j = 0; j < C->col; ++j)
    {
      C->matValues[i][j] = 0;
    }
  }

  //These are 7 variables which used in strassen algorithm for storing multiple values
  int P, Q, R, S, T, U, V = 0;


  //Assigning values according to strassen algorithm
  P = (A->matValues[0][0] + A->matValues[1][1]) *
      (B->matValues[0][0] + B->matValues[1][1]);
  Q = (B->matValues[0][0] * (A->matValues[1][0] + A->matValues[1][1]));
  R = (A->matValues[0][0] * (B->matValues[0][1] - B->matValues[1][1]));
  S = (A->matValues[1][1] * (B->matValues[1][0] - B->matValues[0][0]));
  T = (B->matValues[1][1] * (A->matValues[0][0] + A->matValues[0][1]));
  U = (A->matValues[1][0] - A->matValues[0][0]) *
      (B->matValues[0][0] + B->matValues[0][1]);
  V = (A->matValues[0][1] - A->matValues[1][1]) *
      (B->matValues[1][0] + B->matValues[1][1]);

  //incrementing the result in the final matrix
  C->matValues[0][0] += P + S - T + V;
  C->matValues[0][1] += R + T;
  C->matValues[1][0] += S + Q;
  C->matValues[1][1] += P + R - Q + U;

  return C;   // returning the final matrix
}

//Just a simple matrix multiplication function to compare the final output
void multiply(struct Matrix *A, struct Matrix *B)
{
  // Mu1tip1ying first and second matrices and storing it in result
  struct Matrix *matrix_test = (struct Matrix *)malloc(sizeof(struct Matrix));
  matrix_test = Create_Matrix(matrix_test, 10);
  for (int i = 0; i < matrix_test->row; ++i)
  {
    for (int j = 0; j < matrix_test->col; ++j)
    {
      matrix_test->matValues[i][j] = 0;
    }
  }
  for (int i = 0; i < A->row; ++i)
  {
    for (int j = 0; j < B->col; ++j)
    {
      for (int k = 0; k < A->col; ++k)
      {
        matrix_test->matValues[i][j] += A->matValues[i][k] * B->matValues[k][j];
      }
    }
  }

  printf("\nTest matrix is \n");
  PrintMat(matrix_test);
  printf("\n");
}

//This Add function is accepting a C matric pointer named(A) which is the final matrix to store the result and B is the sub matrix which strassen function is returning after performing calculations
void Add(struct Matrix *A, struct Matrix *B, int var, int index)
{
  for (int i = index, k = 0; i <= index + 1; i++, k++)
  {
    for (int j = var, l = 0; j < var + 2; j++, l++)
    {
      A->matValues[i][j] += B->matValues[k][l];  //storing the values at specific indexes as index is being received in the parameters
    }
  }
}

struct Matrix strassensMatrixMultiplication(struct Matrix *one, struct Matrix *two, int rank, int nproc)
{
  struct Matrix *C;
  int maxrow = one->row > two->col ? one->row : two->col;
  int minrow = one->row > two->col ? two->col : one->col;
  // printf("Maxrow %d\n", maxrow);

  //creating a matric to store the final result
  C = Create_Matrix(C, maxrow);
  for (int i = 0; i < C->row; ++i)
  {
    for (int j = 0; j < C->col; ++j)
    {
      C->matValues[i][j] = 0;   //Assigning the 0 value's because values are going to increment
    }
  }
  int extra[C->row][C->col];

  //For parent thread
  if (rank == 0)
  {
    int noOfIterations = minrow / 2;
    noOfIterations /= (nproc - 1);
    int offset = 0;
    for (int i = 1; i < nproc; i++)
    {
      // if (i == nproc - 1)
      // {
      //   noOfIterations += (maxrow / 2) % (nproc - 1);
      // }
      MPI_Send(&offset, 1, MPI_INT, i, noOfIterations, MPI_COMM_WORLD);
      offset += (noOfIterations * 2);
    }
    MPI_Status status;
    for (int i = 1; i < nproc; i++)
    {
      for (int j = 0; j < C->row; j++)
        MPI_Recv(&extra[j][0], C->row, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      for (int i = 0; i < C->row; i++)
      {
        for (int j = 0; j < C->col; j++)
        {
          C->matValues[i][j] = extra[i][j];
        }
      }
    }
    // printf("Finalized matrix at rank 0\n");
    // PrintMat(C);
  }
  else
  {
    int index;    //For storing the index of row of matrix A which is going to be multiplied
    MPI_Status status;

    //creating the matrices which is going to be multipled 
    struct Matrix *sub_matrixA = (struct Matrix *)malloc(sizeof(struct Matrix));
    sub_matrixA = Create_Matrix(sub_matrixA, 2);
    struct Matrix *sub_matrixB = (struct Matrix *)malloc(sizeof(struct Matrix));
    sub_matrixB = Create_Matrix(sub_matrixB, 2);
    MPI_Recv(&index, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    int iterations = status.MPI_TAG;
    // printf("Rank %d will do %d iterations at offset %d\n", rank, iterations, index);
    // PrintMat(one);
    int col_index = 0;
    // printf("\n");
    // PrintMat(two);
    int sub_i, sub_j;
    int row_index = 0;
    int outer = 0;
    for (int i = 0; i < iterations; i++, index += 2)
    {
      outer = 0;  
      while (outer < minrow)    //For handing the columns of matrix B
      {
        while (col_index < minrow)  //For handling the multiplications of sub matrices of rows and columns one by one
        {
          //This loop is for sub matrices of rows of matrix A
          for (int i = index, sub_i = 0; i <= index + 1; i++, sub_i++)
          {
            for (int j = col_index, sub_j = 0; j < col_index + 2; j++, sub_j++)
            {
              sub_matrixA->matValues[sub_i][sub_j] = one->matValues[i][j];
            }
          }

          //This loop is for sub matrices of columns of matrix B
          for (int i = row_index, sub_i = 0; i < row_index + 2; i++, sub_i++)
          {
            for (int sub_j = 0, j = outer; j < outer + 2; j++, sub_j++)
            {
              sub_matrixB->matValues[sub_i][sub_j] = two->matValues[i][j];
            }
          }
          row_index += 2;   //Incremented by two because the size of sub matrix is 2*2
          col_index += 2;

          Add(C, Strassen(sub_matrixA, sub_matrixB), outer, index);   //This call is adding result of multiplication of sub matrices
          // printf("\n");
        }
        outer += 2;
        row_index = 0;
        col_index = 0;
      }
    }
    for (int j = 0; j < C->row; j++)
      MPI_Send(&(C->matValues[j][0]), C->row, MPI_INT, 0, 0, MPI_COMM_WORLD);

    // multiply(matrix_A, matrix_B);
    // printf("Process %d received number %d from process 0\n", rank, index);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&(C->matValues[0][0]), C->row * C->col, MPI_INT, 0, MPI_COMM_WORLD); // broadcast the resultant matrix from rank 0 to all processors
  // printf("C matrix at rank %d\n", rank);
  // PrintMat(C);
  // printf("%d here\n", rank);
  return *C;
}

void changeOrder()
{
  char *order;
  char *newStr = malloc(strlen(order) - 4 * sizeof(char)); // create a new char array
  int j = 0;
  for (int i = 0; i < strlen(order); i++) // run aloop to update the order
  {
    // if (i < I1.left || i > I1.right - 1) // copy the order without the innermost parenthisis
    // {
    newStr[j] = order[i];
    j++;
    //   }
    // }
    // newStr[I1.left] = (I1.index1 + '0'); // update the new order
    order = newStr;
  }
}

//===========================================================================
int main(int argc, char **argv)
{
  srand(time(NULL));         // set the seed from current time
	    double time_spent = 0.0;
    clock_t begin = clock();
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
    int maxsize = 0;
    for (i = 0; i < size; i++) // run a loop from 0 to number of matrixes
    {
      AllMat[i] = CreateMatrix(AllMat[i]); // setting random values to the matrixes
      AllMat[i] = Add_Padding(&AllMat[i]);
      maxsize = AllMat[i].row > maxsize ? AllMat[i].row : maxsize;
    }

    int *dim = getMatrixDimensions(AllMat, size);         // get the dimensions for optical order
    char *order = leastCostMultiplication(dim, size + 1); // get optical order in char array
    for (i = 0; i < size; i++)                            // run a loop from 0 to number of matrixes
    {
      AllMat[i] = Add_Padding_Size(&AllMat[i], maxsize);
    }
    int rank, nproc;
    int offset, *offsets;
    int rows, *row;
    MPI_Init(&argc, &argv);                // initalize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // get rank of the processor
    MPI_Comm_size(MPI_COMM_WORLD, &nproc); // get number of processors
    struct Matrix abc;

	      if (rank == 0)
    {
      clock_t end = clock();
      time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

      printf("The elapsed time is %f seconds\n", time_spent);
    }
    for (int i = 0; i < size - 1; i++) // run a loop for i=0 to number of multiplicans
    {
      struct indexes I1;
      I1 = getNextMatrixes(order); // get next matrix indexes to be multiplied
      // if(rank==0)
      // printf("%s\n",order);

      abc = strassensMatrixMultiplication(&AllMat[i], &AllMat[i + 1], rank, nproc);
      // printf("%d here Outside\n", rank);
      // order is updated
      MPI_Barrier(MPI_COMM_WORLD); // MPI barrier is called so that all processors are blocked until all processors  have reached here
    }
    // if (rank == 0) // print resulatant matrixes
    // PrintMat(&abc);

    MPI_Finalize(); // terminate MPI
  }
  return 0;
}
