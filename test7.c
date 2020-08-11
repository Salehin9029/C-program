#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void child(char arr[]);

int main() {
    char cmd[255];
    char *charTokens;

    while(1) {
        memset(cmd, 0, sizeof(cmd));
        printf("\n******Basic Shell Program*******\n");
        read(fileno(stdin), cmd, 255);
        charTokens = strtok(cmd, "\n");

        if(!strcmp("", cmd) || !strcmp("exit", cmd)) {
            fprintf(stderr, "exited program\n\n");
            exit(0);
        }

        if(!fork()) {
            child(cmd);
        }
        else {
            int status;
            wait(&status);
        }
    }
}

void child(char arr[]) {
    execl("/bin/sh", "myShell", "-c", arr, NULL);
}