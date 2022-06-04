
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

//Our main function to multiply 2 matrices serially
float **SQUARE_MATRIX_MULTIPLY(float **A, float **B, int size)
{
    float **C = Create_Matrix(size);
    for (int counter = 0; counter < size; counter++)
    {
        for (int count = 0; count < size; count++)
        {
            C[counter][count] = 0;
            for (int count2 = 0; count2 < size; count2++)
            {
                C[counter][count] += A[counter][count2] * B[count2][count];
            }
        }
    }
    return C;
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


int main()
{
    // Using a simple time library for own testing
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;
    int size = 1024;
    float **mat = Create_Matrix(size);
    randomizeMatrix(mat, size);
    start = clock();
    float **result = SQUARE_MATRIX_MULTIPLY(mat, mat, size);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    // printf("%.6f\n", cpu_time_used);
}