// Copyright 2021 by the Zeynep Cankara. All rights reserved.
// Use of this source code is governed by a BSD-style license.
// Program consumes M random alphanum letters incrementally.

// imports
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// definition
#define INPUT_FD 0

// global
// int M = 10;

// Comsumer program
int main(int argc, char *argv[])
{
    int M = 0;
    if (argc > 1)
    {
        // experiements
        M = atoi(argv[1]);
    }
    int bytesRead = 0;
    while (bytesRead < M)
    {
        char toRead;
        bytesRead += read(INPUT_FD, &toRead, 1);
    }
    return 0;
}
