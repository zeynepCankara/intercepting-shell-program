/**
 * A simple program to read M characters one by one.
 * @author Efe Acer
 * @version 1.0
 */

// Necessary imports
#include <unistd.h>
#include <stdlib.h>

// Definitions
#define STDIN_FD 0

// Global Variables
int M = 10;

// Main function
int main(int argc, char* argv[]) {
    if (argc > 1) {
        M = atoi(argv[1]); // get the value of M (will be used in experiments)
    }
    int bytesRead = 0;
    while (bytesRead < M) {
        char toRead;
        bytesRead += read(STDIN_FD, &toRead , 1);
    }
    return 0;
}
