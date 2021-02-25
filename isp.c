

// Copyright 2021 by the Zeynep Cankara. All rights reserved.
// Use of this source code is governed by a BSD-style license.
// The code below makes use of various system calls and pipes as an Inter Process Communication (IPC) mechanism.

// library imports
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

// definitions
#define MAX_ARGS 10
#define READ_END 0
#define WRITE_END 1
#define STDIN_FD 0
#define STDOUT_FD 1

// global variable(s)
int N = 10000;
int COMMAND_SIZE = 10000;

// custom defined methods
void clearShell();
void getcwdShell();
void initShell(int isNormalMode);
void runShell(int isNormalMode);
void execComposedNormal(char *cmd1[], char *cmd2[]);
void execComposedTapped(char *cmd1[], char *cmd2[]);
int execBuiltin(char *cmd1[]);
void runParser(char command[], int isNormalMode);
void readCommand(char *command);
//  **********************

void parseCommand(char command[], char *cmd1[]);
int parseComposedCommand(char command[], char *commands[]);
int parseUnknownCommand(char command[], char *cmd1[], char *cmd2[]);
void executeCommand(char *cmd1[]);

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
 * Takes a composed command made up of two commands, The normal
 * mode implementation relies on the use of single pipe
 * which handles the I/O direction in between the programs.
 * @param cmd1 Command referring to the first part of the composed command
 * @param cmd2 Command referring to the second part of the composed command
 */
void execComposedNormal(char *cmd1[], char *cmd2[])
{
    int fd[2]; // pipe
    if (pipe(fd) < 0)
    {
        fprintf(stderr, "\nPipe 1 failed.");
        exit(1);
    }

    pid_t pid1 = fork(); // fork child 1
    if (pid1 < 0)
    {
        fprintf(stderr, "\nERROR: fork() failed for child1!");
        exit(1);
    }
    else if (pid1 == 0)
    {                   // child 1
        dup2(fd[1], 1); /* this end of the pipe becomes the standard output */
        close(fd[0]);   /* this process don't need the other end */
        if (execvp(cmd1[0], cmd1) < 0)
        {
            fprintf(stderr, "\nExecution of the first command failed.");
            exit(1);
        }
    }
    else
    {
        pid_t pid2 = fork(); // fork child 2
        if (pid2 < 0)
        {
            fprintf(stderr, "\nERROR: fork() failed for child2!");
            exit(1);
        }
        else if (pid2 == 0)
        {                   // child 2
            dup2(fd[0], 0); /* this end of the pipe becomes the standard input */
            close(fd[1]);   /* this process doesn't need the other end */
            if (execvp(cmd2[0], cmd2) < 0)
            {
                fprintf(stderr, "\nExecution of the second command failed.");
                exit(1);
            }
        }
        else
        {
            // close the unused ends
            close(fd[0]);
            close(fd[1]);
            // wait for the child process to finish
            wait(NULL);
            wait(NULL);
        }
    }
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

void parseCommand(char command[], char *cmd1[])
{
    for (int i = 0; i < MAX_ARGS + 1; i++)
    {
        cmd1[i] = strsep(&command, " ");
        if (cmd1[i] == NULL)
        {
            break;
        }
        i -= (strlen(cmd1[i]) == 0);
    }
}

int parseComposedCommand(char command[], char *commands[])
{
    for (int i = 0; i < 2; i++)
    {
        commands[i] = strsep(&command, "|");
        if (commands[i] == NULL)
        {
            break;
        }
    }
    // return 1 (true) if the command is a composed one, 0 (false) otherwise
    return (commands[1] != NULL);
}

int parseUnknownCommand(char command[], char *cmd1[], char *cmd2[])
{
    char *commands[2];
    int isComposedCommand = parseComposedCommand(command, commands);
    if (isComposedCommand)
    {
        parseCommand(commands[0], cmd1);
        parseCommand(commands[1], cmd2);
    }
    else
    {
        parseCommand(command, cmd1);
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

void executeCommand(char *cmd1[])
{
    pid_t pid = fork(); // fork a child
    if (pid < 0)
    { // error case
        fprintf(stderr, "\nFork failed.");
        exit(1);
    }
    else if (pid == 0)
    { // child process
        if (execvp(cmd1[0], cmd1) < 0)
        {
            fprintf(stderr, "\nCommand execution failed.");
            exit(1);
        }
        exit(0); // successfully exit
    }
    else
    {               // parent process
        wait(NULL); // wait for the child to complete
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
    // some statistics
    int bytesTransferred = 0;
    int readCount = 0;
    int writeCount = 0;
    int fd1[2]; // pipe 1
    if (pipe(fd1) < 0)
    {
        fprintf(stderr, "\nPipe 1 failed.");
        exit(1);
    }
    int fd2[2]; // pipe 2
    if (pipe(fd2) < 0)
    {
        fprintf(stderr, "\nPipe 2 failed.");
        exit(1);
    }
    pid_t pid1 = fork(); // fork child 1
    if (pid1 < 0)
    {
        fprintf(stderr, "\nFork failed for child 1.");
        exit(1);
    }
    else if (pid1 == 0)
    {                         // child 1
        close(fd2[READ_END]); // close unused ends
        close(fd2[WRITE_END]);
        close(fd1[READ_END]);
        dup2(fd1[WRITE_END], STDOUT_FD);
        if (execvp(cmd1[0], cmd1) < 0)
        {
            fprintf(stderr, "\nExecution of the first command failed.");
            exit(1);
        }
    }
    else
    {
        pid_t pid2 = fork(); // fork child 2
        if (pid2 < 0)
        {
            fprintf(stderr, "\nFork failed for child 2.");
            exit(1);
        }
        else if (pid2 == 0)
        {                         // child 2
            close(fd1[READ_END]); // close unused ends
            close(fd1[WRITE_END]);
            close(fd2[WRITE_END]);
            dup2(fd2[READ_END], STDIN_FD);
            if (execvp(cmd2[0], cmd2) < 0)
            {
                fprintf(stderr, "\nExecution of the second command failed.");
                exit(1);
            }
        }
        else
        {                          // parent
            close(fd1[WRITE_END]); // close unused ends
            close(fd2[READ_END]);
            // Transfer the bytes written to pipe 1 by child 1 to pipe 2
            int bytesRead; // statistic
            char buffer[N];
            while ((bytesRead = read(fd1[READ_END], buffer, N)) > 0)
            {
                int bytesWritten = write(fd2[WRITE_END], buffer, bytesRead);
                bytesTransferred += bytesRead + bytesWritten;
                readCount++;
                writeCount++;
            }
            readCount++;
            close(fd1[READ_END]); // close unused ends
            close(fd2[WRITE_END]);
            wait(NULL);
            wait(NULL);
            printf("\ncharacter-count: %d\nread-call-count: %d\nwrite-call-count: %d\n",
                   bytesTransferred, readCount, writeCount);
        }
    }
}

void runParser(char command[], int isNormalMode)
{
    char *cmd1[MAX_ARGS + 1];
    char *cmd2[MAX_ARGS + 1];
    int isComposedCommand = parseUnknownCommand(command, cmd1, cmd2);
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
        executeCommand(cmd1);
    }
}
