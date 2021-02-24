/**
 * A simple program to print M random alphanumeric characters to screen one by one.
 * @author Efe Acer
 * @version 1.0
 */

// Necessary imports
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Definitions
#define STDOUT_FD 1
#define ALPHANUMERIC_SIZE 36

// Contant(s)
const char ALPHANUMERIC_CHARS[ALPHANUMERIC_SIZE] = { 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
    'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

// Global Variables
int M = 1000;

// Main function
int main(int argc, char* argv[]) {
    if (argc > 1) {
        M = atoi(argv[1]); // get the value of M (will be used in experiments)
    }
    srand(time(NULL)); // set random seed
    int bytesWritten = 0;
    while (bytesWritten < M) {
        char randomChar = ALPHANUMERIC_CHARS[rand() % ALPHANUMERIC_SIZE];
        bytesWritten += write(STDOUT_FD, &randomChar, 1);
    }
    return 0;
}
