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
    // Creating short array as only 1s or 0s
    unsigned short *arr = (unsigned short *)calloc(number, sizeof(unsigned short));
    // sleep(1);
    arr[1] = 1;
    long upperLimit = sqrt(number);
    #pragma omp parallel for schedule(guided, 8) num_threads(noOfThreads)
    for (long divisor = 2; divisor <= upperLimit; divisor++)
    {
        if (arr[divisor] == 0)
        {
            for (long long counter = divisor + divisor; counter < number; counter += divisor)
            {
                arr[counter] = 1;
            }
        }
    }
    // Print the largest prime numbers
    for (int counter = 1; counter < number; counter++)
    {
        if (!arr[number - counter])
        {
            printf("Largest Prime Number: %lld\n", number - (counter));
            break;
        }
    }

    // Free our memory here
    free(arr);
    return 1;
}

int main(int argc, char *argv[])
{
    long long number = 0;
    int divisor = 2;
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    number = 999999999 + 1;
    isPrime(number);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nCPU Time: %.6f\n", cpu_time_used);
    return 0;
}