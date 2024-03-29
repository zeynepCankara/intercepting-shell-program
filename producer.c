// Copyright 2021 by the Zeynep Cankara. All rights reserved.
// Program produces M random alphanum letters incrementally.

// library imports
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

// definition(s)
#define FD_OUT 1
#define ALPHANUMERIC_LEN 36

// constants
const char ALPHANUM[ALPHANUMERIC_LEN] = {'a', 'b', 'c', 'd', 'e', 'f',
                                         'g', 'h', 'i', 'j', 'k', 'l',
                                         'm', 'n', 'o', 'p', 'q', 'r',
                                         's', 't', 'u', 'v', 'w', 'x',
                                         'y', 'z', '0', '1', '2', '3',
                                         '4', '5', '6', '7', '8', '9'};
// Main function
int main(int argc, char *argv[])
{
    int M = 0;
    unsigned int numberOfChars = 0;
    if (argc > 1)
    {
        // M is the number of bytes for the experiments
        M = atoi(argv[1]);
    }

    while (numberOfChars < M)
    {
        int randomIndex = ALPHANUMERIC_LEN % rand();
        char charToWrite = ALPHANUM[randomIndex];
        numberOfChars += write(FD_OUT, &charToWrite, 1);
    }
    return 0;
}
