
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>
#include <math.h>

int size = 1024;
float **A, **B, **C;

struct coordinate
{
    int i, j, k;
} index;

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
void *SQUARE_MATRIX_MULTIPLY_THREAD_CYCLIC(void *arg)
{
    struct coordinate *arr = (struct coordinate *)arg;
    int i = arr->i, j = arr->j, k = arr->k;
    for (int counter = i; counter < j; counter++)
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
        if (counter + 1 < size && counter + 1 == j)
        {
            counter += k - (j - i);
            i += k;
            j += k;
        }
    }
    pthread_exit(NULL);
}

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
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;
    A = Create_Matrix(size);
    B = Create_Matrix(size);
    C = Create_Matrix(size);
    randomizeMatrix(A, size);
    randomizeMatrix(B, size);
    int noOfThreads = atoi(argv[1]);
    while (noOfThreads % 2 != 0)
    {
        printf("Please enter number of threads divisible by 2!");
        scanf("%d", &noOfThreads);
    }

    double change = (double)size / (noOfThreads * 4);
    change = ceil(change);
    index.i = 0;
    index.j = change;
    index.k = change * noOfThreads; 
    pthread_t tid[noOfThreads];
    start = clock();
    for (int counter = 0; counter < noOfThreads; counter++)
    {

        pthread_create(&tid[counter], NULL, SQUARE_MATRIX_MULTIPLY_THREAD_CYCLIC, (void *)&index);
        sleep(1);
        index.i = index.j;
        change = (double)size / (noOfThreads * 4);
        change = ceil(change);
        index.j += change;
    }
    for (int counter = 0; counter < noOfThreads; counter++)
    {
        pthread_join(tid[counter], NULL);
    }
    float **D = SQUARE_MATRIX_MULTIPLY(A, B, size);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%.6f\n", cpu_time_used);

    printf("Results: %d\n", validateResults(C, D, size));

}