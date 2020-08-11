#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int getRndmInt(int n)
{
    srand(time(NULL));
    return(rand()%n + 1);
}

void action()
{

}

void child(char *, int usrinpt);
int main(int argc, char *argv[])
{
    pid_t pid1, pid2, pid3;
    int usrinpt;
    FILE * myfl = fopen("SharedFile.bin", "w+");

    printf("This is 3 player game with refree\n");
    printf("Enter Score Here: >");

    scanf("%d", &usrinpt);
    if((pid1=fork()) == 0) child("TATA", usrinpt);
    if((pid2=fork()) == 0) child("TITI", usrinpt);
    if((pid3=fork()) == 0) child("TOTO", usrinpt);
    sleep(1);
    signal(SIGUSR1, action);

    while(1)
    {
        printf("\nRefree: TATA plays the game\n\n");
        kill(pid1, SIGUSR1);
        pause();

        printf("\nRefree: TITI plays the game\n\n");
        kill(pid2, SIGUSR1);
        pause();

        printf("\nRefree: TOTO plays the game\n\n");
        kill(pid3, SIGUSR1);
        pause();

    }

}

void child(char *s, int usrinpt)
{
    int points = 0;
    int dice, n = 20, player, threshold;
    signal(SIGUSR1, action);
    FILE * myfl = fopen("SharedFile.bin", "w+");

    if(!strcmp(s, "TATA"))
    {
        player = 0;
        fseek(myfl, 0, SEEK_SET);
        fread(&points, sizeof points, 1, myfl);
    }

    else if(!strcmp(s, "TITI"))
    {
        player = 1;
        fseek(myfl, sizeof points, SEEK_CUR);
        fread(&points, sizeof points, 1, myfl);
    }
    else
    {
        player = 2;
        fseek(myfl, 2*sizeof points, SEEK_CUR);
        fread(&points, sizeof points, 1, myfl);
    }

    while(1)
    {
        pause();
        printf("%S: Playingdice\n", s);
        dice = getRndmInt(n);
        printf("%S: got %d POINTS\n", s, dice);
        points+=dice;
        printf("%s: TOTAL POINTS: %d\n\n", s, points);
        sleep(2);
        if(points >= usrinpt)
        {
            printf("REFREE %s won with %d points\n", s, points);
            kill(0, SIGTERM);
        }
        fseek(myfl, player, SEEK_CUR);
        fwrite(&points, sizeof points, 1, myfl);

        kill(getppid(), SIGUSR1);
    }
}
