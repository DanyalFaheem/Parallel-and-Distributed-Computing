#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>
#include <math.h>

//Roll number 19I-2014 Hence 9 + 4 = 13
int noOfThreads = 13;


// Function which takes a number and returns the array of prime numbers uptil then
int isPrime(long long number)
{
    //Allocating our memory here
    long long *arr = (long long *)calloc(number, sizeof(long long));
    arr[0] = 1;
    int isPrime = 1;
    long upperLimit = sqrt(number);
    for (long long counter = 2; counter < number; counter++)
    {
        arr[counter] = counter;
    }
    #pragma omp parallel for schedule(guided, 4) num_threads(noOfThreads)
    for (long divisor = 2; divisor <= upperLimit; divisor++)
    {
        if (arr[divisor] != 0)
        {
            long counter = divisor * divisor;
            for (; counter < number; counter += divisor)
            {
                arr[counter] = 0;
            }
        }
    }
    // Calculate number of prime numbers here
    int count = 0, index = 0;
    for (long long counter = 2; counter < number; counter++)
    {
        if (arr[counter] != 0) {
            count++;
        }
    }
    // Create arrays of prime numbers here
    long long *L = (long long *)calloc(count, sizeof(long long));
    for (long long counter = 2; counter < number; counter++)
    {
        if (arr[counter] != 0) {
            L[index] = arr[counter];
            index++;
        }
    }
    // Free our memory
    free(arr);
        //printf("Prime numbers: ");
    // for (int counter = count - 10; counter <= count; counter++) {
    //     printf("%lld  ", L[counter]);
    // }
    free(L);
    return 1;
}

int main(int argc, char *argv[])
{
    long long number = 0;
    int divisor = 2;
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    number = 99999999 + 1;
    isPrime(number);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nCPU Time:%.6f\n", cpu_time_used);
    return 0;
}