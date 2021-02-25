// Copyright 2021 by the Zeynep Cankara. All rights reserved.
// Use of this source code is governed by a BSD-style license.
// Program produces M random alphanum letters incrementally.

// Necessary imports
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

// Definitions
#define STDOUT_FD 1
#define ALPHANUMERIC_SIZE 36

// Contant(s)
const char ALPHANUMERIC_CHARS[ALPHANUMERIC_SIZE] = {'a', 'b', 'c', 'd', 'e', 'f',
                                                    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
                                                    'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

// Global Variables
// int M = 1000;

// Main function
int main(int argc, char *argv[])
{
    int M = 0;
    if (argc > 1)
    {
        M = atoi(argv[1]); // get the value of M (will be used in experiments)
    }
    srand(time(NULL)); // set random seed
    int bytesWritten = 0;
    while (bytesWritten < M)
    {
        char randomChar = ALPHANUMERIC_CHARS[rand() % ALPHANUMERIC_SIZE];
        bytesWritten += write(STDOUT_FD, &randomChar, 1);
    }
    return 0;
}
