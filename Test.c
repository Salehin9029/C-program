#include <stdio.h>          //Reverse the contents of a text file line-by-line
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void flip(FILE *fp_1, FILE *fp_2, int col, int row, long int size)
{
    int i;
    char pic[col];

    fseek(fp_2, size, SEEK_SET);

    for (i = 0; i < row; i++)
    {
        fread(pic, 1, col, fp_1);
        fseek(fp_2, -col, SEEK_CUR);
        fwrite(pic, 1, col, fp_2);
        fseek(fp_2, -col, SEEK_CUR);
    }
}

int main(int argc, char *argv[])
{
    FILE *fp_1, *fp_2;
    long int size;
    char temp[100];
    int col, row;

    if((fp_1 = fopen(argv[1], "r")) != NULL)
    {
        if((fp_2 = fopen(argv[2], "w")) != NULL)
        {
            fseek(fp_1, 0, SEEK_END);
            size = ftell(fp_1);
            rewind(fp_1);

            fgets(temp, 100, fp_1);
            fputs(temp, fp_2);

            fscanf(fp_1, "%d %d\n", &col, &row);
            fprintf(fp_2, "%d %d\n", col, row);

            fgets(temp, 100, fp_1);
            fputs(temp, fp_2);

            flip(fp_1, fp_2, col, row, size);
        }
        else
        {
            fprintf(stderr, "Can not open file %s", argv[2]);
        }
    }
    else
    {
        fprintf(stderr, "ERROR %s", argv[1]);
    }

    fclose(fp_1);
    fclose(fp_2);

    return 0;
}
