// Copyright 2021 by the Zeynep Cankara. All rights reserved.
// Use of this source code is governed by a BSD-style license.
// Program consumes M random alphanum letters incrementally.

// imports
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// definition
#define FD_IN 0

// Comsumer program
int main(int argc, char *argv[])
{
    int M = 0;
    if (argc > 1)
    {
        // M is the number of bytes for the experiments
        M = atoi(argv[1]);
    }
    unsigned int numberOfChars = 0;
    while (numberOfChars < M)
    {
        char toRead;
        numberOfChars += read(FD_IN, &toRead, 1);
    }
    return 0;
}
