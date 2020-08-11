#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void childFunction(char *usrIn, int fileDesc) {

    printf("I'm a child waiting for my parent!\n");
    sleep(1);
    printf("User input: %s\n", usrIn);
    
    char * temp;
    char tempChar;

    size_t i=0, size, count=0;

    size = strlen(usrIn)-1;

    for(i; i <= size; i++) {
        tempChar = usrIn[i];
        usrIn[i] = usrIn[size];
        usrIn[size] = tempChar;
        size--;
    }

    write(fileDesc, usrIn, strlen(usrIn));
    
    printf("Inversed userinput : %s\n", usrIn);

}

int main(int argc, char* argv[]) {
    int fileDesc, status;
    pid_t pid;
    char usrIn[1024];

    fileDesc = open("codes.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR|S_IRUSR);

    printf("\n--ENCODING TEXT--\n\n");
    printf("Enter a sentence, example, Good morning sir, my name is Robin\n\n");

    read(fileno(stdin), usrIn, 1024);

    if(fileDesc == -1) {
        perror("Could not read input line.");
    }

    sleep(1);

    if((pid = fork()) > 0) {
        printf("\nCreated a child.\n");
        wait(&status);
    }
    else if(pid == 0) {
        sleep(1);
        childFunction(usrIn, fileDesc);
    }

}
