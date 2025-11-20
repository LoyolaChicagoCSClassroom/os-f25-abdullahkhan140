#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

int main() {
    char *line = NULL;
    size_t bufsize = 0;

    while (1) {
        printf("$ ");
        fflush(stdout);

        // Read input line
        if (getline(&line, &bufsize, stdin) == -1) {
            printf("\n");
            break; // EOF or error
        }

        // Remove newline
        if (line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';

        // Skip empty lines
        if (strlen(line) == 0) continue;

        // Exit command
        if (strcmp(line, "exit") == 0) break;

        // Split line into arguments using strtok
        char *argz[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < MAX_ARGS-1) {
            argz[argc++] = token;
            token = strtok(NULL, " ");
        }
        argz[argc] = NULL; // execv/execvp requires NULL-terminated argv

        // Fork a child process
        pid_t pid = fork();
        if (pid == 0) {
            // Child: execute command using execvp (searches PATH)
            if (execvp(argz[0], argz) == -1) {
                perror("shittyshell");
                exit(1);
            }
        } else if (pid > 0) {
            // Parent: wait for child to finish
            wait(NULL);
        } else {
            perror("fork");
        }
    }

    free(line);
    return 0;
}

