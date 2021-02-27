

// Copyright 2021 by the Zeynep Cankara. All rights reserved.
// The code below makes use of various system calls and pipes as an Inter Process Communication (IPC) mechanism.

// library imports
#include <sys/wait.h> // children
#include <stdlib.h>
#include <sys/time.h> // gettimeofday()
#include <stdio.h>
#include <unistd.h>
#include <string.h> // prtinf

// definitions
#define IN_FD 0
#define OUT_FD 1
#define NOF_ARGS 20

// global variable(s)
int N = 5000;
int COMMAND_SIZE = 10000;

// custom defined methods
// shell related
void clearShell();
void getcwdShell();
void initShell(int isNormalMode);
void runShell(int isNormalMode);
void runParser(char *buffer, int isNormalMode);
void readCommand(char *command);
// command exec related
void execComposedNormal(char *cmd1[], char *cmd2[]);
void execComposedTapped(char *cmd1[], char *cmd2[]);
int execBuiltin(char *cmd1[]);
void runPipeSourceNormal(int pfd[], char *cmd1[]);
void runPipeDestNormal(int pfd[], char *cmd2[]);
int runPipeSourceTapped(int fd1[], int fd2[], char *cmd1[]);
int runPipeDestTapped(int fd1[], int fd2[], char *cmd2[]);
void runCommand(char *cmd1[]);
// parser related
void parseSingle(char *buffer, char *cmd1[]);
int parseComposedCommand(char *buffer, char *commandBuffer[]);
int parseUnknownCommand(char *buffer, char *cmd1[], char *cmd2[]);

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        N = atoi(argv[1]); // get the value of N (will be used in experiments)
    }
    int isNormalMode = (atoi(argv[2]) == 1);

    initShell(isNormalMode);
    runShell(isNormalMode);

    return 0;
}

/**
 * Clears the shell window.
 */
void clearShell()
{
    printf("\033[H\033[J");
}

/**
 * Prints information about the mode shell started.
 */
void initShell(int isNormalMode)
{
    clearShell();
    printf("$-Intercepting Shell Program.\n"
           "Execute system commands from the command shell.\nType \"exit\" "
           "to quit and \"help\" to read the manual.\n");
    if (isNormalMode)
    {
        printf("$-Shell running in the Normal mode. \n");
    }
    else
    {
        printf("$-Shell running in the Tapped mode. \n");
    }
}

/**
 * Execute getcwd command
 */
void getcwdShell()
{
    char buffer[1024];
    getcwd(buffer, sizeof(buffer));
    printf("\nguest@guest-VM:%s", buffer);
}

/**
 * Runs the program in the interactive mode, where the user enters the commands.
 * to the shell. The exexution continues untill user exits with 'exit' command.
 */
void runShell(int isNormalMode)
{
    char command[COMMAND_SIZE];
    while (1)
    {
        readCommand(command);
        runParser(command, isNormalMode);
    }
}

/**
 * Reads the comand from the input stream.
 * @param command The string obtained from the input stream.
 */
void readCommand(char *command)
{
    char buffer[COMMAND_SIZE];
    getcwdShell();
    printf("\nisp$: ");
    fgets(buffer, COMMAND_SIZE, stdin);
    buffer[strcspn(buffer, "\n\r")] = '\0';
    strcpy(command, buffer);
}

void parseSingle(char *buffer, char *cmd1[])
{
    int offset;
    for (int pos = 0; pos < NOF_ARGS + 1; pos++)
    {
        // seperate command to the arguments
        cmd1[pos] = strsep(&buffer, " ");
        if (!cmd1[pos])
        {
            break;
        }
        offset = (strlen(cmd1[pos]) == 0);
        pos -= offset;
    }
}

int parseComposedCommand(char *buffer, char *commandBuffer[])
{
    //int composed;
    for (int pos = 0; pos < 2; pos++)
    {
        commandBuffer[pos] = strsep(&buffer, "|");
        //composed = pos + 1;
        if (!commandBuffer[pos])
        {
            break;
        }
    }
    return (commandBuffer[1] != NULL);
}

int parseUnknownCommand(char *buffer, char *cmd1[], char *cmd2[])
{
    char *commandBuffer[2];
    int isComposedCommand = parseComposedCommand(buffer, commandBuffer);
    if (isComposedCommand)
    {
        parseSingle(commandBuffer[0], cmd1);
        parseSingle(commandBuffer[1], cmd2);
    }
    else
    {
        parseSingle(buffer, cmd1);
    }
    int isBuiltInCommand = execBuiltin(cmd1);
    if (isBuiltInCommand)
    {
        return -1;
    }
    return isComposedCommand;
}

int execBuiltin(char *cmd1[])
{
    int isBuiltInCmd = 0; // 0 if the command is a builtin command
    if (strcmp(cmd1[0], "exit") == 0)
    {
        printf("\nExiting the Intercepting Shell Program...\n");
        exit(0);
    }
    else if (strcmp(cmd1[0], "help") == 0)
    {
        printf("\nISP:\nAn intercepting shell program"
               "Supported builtin commands: \n > exit \n"
               "> cd\n> help\n> and UNIX commands (touch, cp, mkdir...)"
               "\n> two consecutive commands can executed when seperated by a pipe symbol (|)\n");
        return !isBuiltInCmd;
    }
    else if (strcmp(cmd1[0], "cd") == 0)
    {
        chdir(cmd1[1]);
        return !isBuiltInCmd;
    }
    return isBuiltInCmd;
}

void runCommand(char *cmd1[])
{
    int pid;

    switch (pid = fork())
    {

    case 0: // child process
        if (execvp(cmd1[0], cmd1) < 0)
        {
            fprintf(stderr, "\nCommand execution failed.");
            exit(1);
        }
        exit(0); // successfully exit

    default: // parent
        wait(NULL);
        break;

    case -1:
        perror("fork");
        exit(1);
    }
}

/**
 * Takes two argument vectors, which executed in order such that the output of the
 * first program fed as the input to the second program via using a pipe. The tapped
 * mode implementation relies on the use of two pipes which used to handle I/O direction
 * in between the programs. The output of the first program first directed to the main
 * program which later fed into the input of the second program via pipe.
 * @param cmd1 Command referring to the first part of the composed command
 * @param cmd2 Command referring to the second part of the composed command
 */
void execComposedTapped(char *cmd1[], char *cmd2[])
{
    int charCount = 0;
    int writeCount = 0;
    int readCount = 0;
    int fd1[2]; // pipe 1
    int fd2[2]; // pipe 2
    pipe(fd1);
    pipe(fd2);
    struct timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

    // run the command
    if (runPipeSourceTapped(fd1, fd2, cmd1) == 1)
    {
        if (runPipeDestTapped(fd1, fd2, cmd2) == 1)

        {                       // parent
            close(fd1[OUT_FD]); // close unused ends
            close(fd2[IN_FD]);
            // Transfer the bytes written to pipe 1 by child 1 to pipe 2
            int bytesRead;
            char buffer[N];
            while ((bytesRead = read(fd1[IN_FD], buffer, N)) > 0)
            {
                int bytesWritten = write(fd2[OUT_FD], buffer, bytesRead);
                charCount += (bytesRead + bytesWritten);
                readCount++;
                writeCount++;
            }
            readCount++;
            close(fd1[IN_FD]); // close unused ends
            close(fd2[OUT_FD]);
            wait(NULL);
            wait(NULL);
            printf("\ncharacter-count: %d\nread-call-count: %d\nwrite-call-count: %d\n",
                   charCount, readCount, writeCount);
            // timer stop
            gettimeofday(&t2, NULL);

            // calculate the elapsed time in millisec
            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
            printf("Elapsed time: %f ms.\n", elapsedTime);
        }
    }
}

int runPipeSourceTapped(int fd1[], int fd2[], char *cmd1[])
{
    int pid;

    switch (pid = fork())
    {

    case 0:                // child process
        close(fd2[IN_FD]); // close unused ends
        close(fd2[OUT_FD]);
        close(fd1[IN_FD]);
        dup2(fd1[OUT_FD], OUT_FD);
        execvp(cmd1[0], cmd1); // exec command
        perror(cmd1[0]);       // error

    default: // parent
        break;

    case -1:
        perror("fork");
        exit(1);
    }
    return 1;
}

int runPipeDestTapped(int fd1[], int fd2[], char *cmd2[])
{
    int pid;

    switch (pid = fork())
    {

    case 0:                // child process
        close(fd1[IN_FD]); // close unused ends
        close(fd1[OUT_FD]);
        close(fd2[OUT_FD]);
        dup2(fd2[IN_FD], IN_FD);
        execvp(cmd2[0], cmd2); // exec command
        perror(cmd2[0]);       // error

    default: // parent
        break;

    case -1:
        perror("fork");
        exit(1);
    }
    return 1;
}

void runParser(char *buffer, int isNormalMode)
{
    char *cmd1[NOF_ARGS + 1];
    char *cmd2[NOF_ARGS + 1];
    int isComposedCommand = parseUnknownCommand(buffer, cmd1, cmd2);
    if (isComposedCommand == 1)
    {
        if (isNormalMode == 1)
        {
            execComposedNormal(cmd1, cmd2);
        }
        else
        {
            // tapped
            execComposedTapped(cmd1, cmd2);
        }
    }
    else if (isComposedCommand == 0)
    {
        runCommand(cmd1);
    }
}

/**
 * Takes a composed command made up of two commands, The normal
 * mode implementation relies on the use of single pipe
 * which handles the I/O direction in between the programs.
 * @param cmd1 Command referring to the first part of the composed command
 * @param cmd2 Command referring to the second part of the composed command
 */
void execComposedNormal(char *cmd1[], char *cmd2[])
{
    int fd[2];
    pipe(fd);
    struct timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;    // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    printf("%f ms.\n", elapsedTime);
    // configure the pipes
    runPipeSourceNormal(fd, cmd1);
    runPipeDestNormal(fd, cmd2);
    // close the unused pipes
    close(fd[0]);
    close(fd[1]);
    //  wait execution of the children
    wait(NULL);
    wait(NULL);

    // stop timer
    gettimeofday(&t2, NULL);
    // ccompute the elapsed in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;    // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
    printf("Elapsed time: %f ms.\n", elapsedTime);
}

void runPipeSourceNormal(int pfd[], char *cmd1[])
{
    int pid;

    switch (pid = fork())
    {

    case 0:                        // child process
        dup2(pfd[OUT_FD], OUT_FD); // stdout
        close(pfd[IN_FD]);         // close unused end
        execvp(cmd1[0], cmd1);     // exec command
        perror(cmd1[0]);           // error

    default: // parent
        break;

    case -1:
        perror("fork");
        exit(1);
    }
}

void runPipeDestNormal(int pfd[], char *cmd2[])
{
    int pid;

    switch (pid = fork())
    {

    case 0:                      // child process
        dup2(pfd[IN_FD], IN_FD); // stdin
        close(pfd[OUT_FD]);      // close unused pipe
        execvp(cmd2[0], cmd2);   // exec command
        perror(cmd2[0]);         // error

    default: // parent
        break;

    case -1:
        perror("fork");
        exit(1);
    }
}