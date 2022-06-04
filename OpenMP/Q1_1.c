#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

// Roll number 19I-2014 Hence 9 + 4 = 13
int noOfThreads = 13;

// Function which takes a number and returns if it is Prime or not
int isPrime(long long number)
{
    int isPrime = 1;
// Our main loop starts here
#pragma omp parallel for schedule(dynamic, 4) shared(isPrime) num_threads(noOfThreads)
    for (long divisor = 2; divisor <= number / 2; divisor++)
    {
        if (number % divisor == 0)
        {
            isPrime = 0;
        }
        // Break out of loop if not prime
        if (!isPrime)
        {
            divisor = number / 2;
        }
    }
    return isPrime;
}

int main(int argc, char *argv[])
{
    long long number = 0;
    int divisor = 2;
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    number = 999999999;
    // Run loop until found
    while (!isPrime(number) && number > 99999999)
    {
        number--;
    }

    end = clock();
    printf("The number %lld is a Prime Number!\n", number);
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("CPU Time:%.6f\n", cpu_time_used);
    return 0;
}