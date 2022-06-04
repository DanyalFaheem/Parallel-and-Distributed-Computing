#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

int noOfThreads;

//Our main function which returns count of occurences
int countOccurence(char *text, char *pattern)
{
    int occurences = 0, flag = 1;
    for (int counter = 0; counter < strlen(text); counter++)
    {
        // If first letter matches then match all other letters too
        if (pattern[0] == text[counter])
        {
            flag = 1;
            for (int count = 1; count < strlen(pattern); count++)
            {
                if (pattern[count] != text[counter + count])
                {
                    flag = 0;
                    break;
                }
            }
            // Add count if match found
            if (flag)
            {
                occurences += 1;
            }
        }
    }
    return occurences;
}

int main(int argc, char *argv[])
{
    char text[500];
    strcpy(text, "AAABCDEFGAAGEREAANMT");
    char pattern[10];
    strcpy(pattern, "AA");
    clock_t start, end;
    double cpu_time_used;
    int occurences;
    start = clock();
    occurences = countOccurence(text, pattern);
    end = clock();
    printf("The number of occurrences in string %s of string %s are: %d\n", text, pattern, occurences);
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("CPU Time: %.6f\n", cpu_time_used);
    return 0;
}