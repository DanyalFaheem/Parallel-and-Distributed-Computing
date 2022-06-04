
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>

int size = 1024;
float **A, **B, **C;

// Struct to pass as argument to our thread function
struct coordinate{
    int i,j;
} index;

// Function to create a new matrix of size x size and return it
float **Create_Matrix(int size)
{
    float **C = calloc(size, sizeof(float *));
    for (int counter = 0; counter < size; counter++)
    {
        C[counter] = calloc(size, sizeof(float));
        for (int count = 0; count < size; count++)
        {
            C[counter][count] = 0;
        }
    }
    return C;
}

//Our main thread function to multiply 2 matrices in parallel
void *SQUARE_MATRIX_MULTIPLY_THREAD(void *arg)
{
    // Get the passed argument here
    struct coordinate *arr = (struct coordinate *)arg;
    // Run the loop on the rows as specified in the argument
    for (int counter = arr->i; counter < arr->j; counter++)
    {
        for (int count = 0; count < size; count++)
        {
            for (int count2 = 0; count2 < size; count2++)
            {
                if (counter < size && count < size)
                {
                    C[counter][count] += A[counter][count2] * B[count2][count];
                }
            }
        }
    }
    pthread_exit(NULL);
}

//Simple function that takes a matrix and displays it in a matrix form
void Display_Matrix(float **M, int size)
{
    for (int counter = 0; counter < size; counter++)
    {
        printf("| ");
        for (int count = 0; count < size; count++)
        {
            printf("%.0f ", M[counter][count]);
        }
        printf("|\n");
    }
    printf("\n");
}

//A function to initialize values, currently just doing 1,2,3,4... per row
void randomizeMatrix(float **mat, int size)
{
    for (int counter = 0; counter < size; counter++)
    {
        for (int count = 0; count < size; count++)
        {
            mat[counter][count] = count + 1; 
        }
    }
}

// Function to validate results of 2 matrices passed by comparing each value
int validateResults(float **matC, float **matD, int size)
{
    for (int counter = 0; counter < size; counter++)
    {
        for (int count = 0; count < size; count++)
        {
            if (matC[counter][count] != matD[counter][count])
            {
                printf("Invalid: %d, %d, %.0f, %.0f\n", counter, count, matC[counter][count], matD[counter][count]);
                return 0;
            }
        }
    }
    return 1;
}

float **SQUARE_MATRIX_MULTIPLY(float **matA, float **matB, int size)
{
    float **D = Create_Matrix(size);
    for (int counter = 0; counter < size; counter++)
    {
        for (int count = 0; count < size; count++)
        {
            D[counter][count] = 0;
            for (int count2 = 0; count2 < size; count2++)
            {
                D[counter][count] += matA[counter][count2] * matB[count2][count];
            }
        }
    }
    return D;
}


int main(int argc, char *argv[])
{
    // Using a simple time library for own testing
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;
    A = Create_Matrix(size);
    B = Create_Matrix(size);
    C = Create_Matrix(size);
    randomizeMatrix(A, size);
    randomizeMatrix(B, size);

    // Getting the number of threads and validating
    int noOfThreads = atoi(argv[1]);
    while (noOfThreads % 2 != 0)
    {
        printf("Please enter number of threads divisible by 2!");
        scanf("%d", &noOfThreads);
    }

    //Using struct to pass our block sizes to the thread
    index.i = 0;
    index.j = size / noOfThreads;
    pthread_t tid[noOfThreads];
    start = clock();
    // Creating threads here 
    for (int counter = 0; counter < noOfThreads; counter++)
    {
        pthread_create(&tid[counter], NULL, SQUARE_MATRIX_MULTIPLY_THREAD, (void *)&index);
        sleep(2);
        index.i = index.j;
        index.j += (size / noOfThreads);
    }
    for (int counter = 0; counter < noOfThreads; counter++)
    {
        pthread_join(tid[counter], NULL);
    }

    float **D = SQUARE_MATRIX_MULTIPLY(A, B, size);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("%.6f\n", cpu_time_used);
    // Validation results
    printf("Results: %d\n", validateResults(C, D, size));
}