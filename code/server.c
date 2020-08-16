#define _XOPEN_SOURCE 700 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <ftw.h>
#include <fcntl.h>
#include <libgen.h>

typedef enum {
   F, T
}
bool;

char * fOrDirName;
bool found = F;

int processEntry(char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    //fpath contains fileName
    //printf("Path: - %s, FileName: - %s\n",fpath, basename(fpath));
    if(!strcmp(basename(fpath),fOrDirName) && typeflag == FTW_F){
        printf("Target-File Found: %s\n", fpath);
        found = T;
    }
    return(0);
}

void child(int sd){
    char message[400000], buffer[400000];
    char* token, *token2, *word = "Found", *word2 = "Not Found";
    while(1){

        if(!read(sd, message, 400000) && !strcmp("",message)){
            close(sd);
            fprintf(stderr,"Bye, client dead, wait for a new client\n");
            exit(0); 
        }

        fprintf(stderr, "Before strtok on server, message: %s, Token: %s\n", message, token);

        //get for part of the string seperated using space/\n
        token = strtok(message," \n");
        //we can even loop through every token in the string if string is long
        //get the second part, we can have token345667, so on so forth
        //message has already been passed in, so this time the first argument is NULL
        token2 = strtok(NULL, " \n");
        fOrDirName = token2;

        if(!strcmp(token, "get")){
            fprintf(stderr, "Clent send get command: %s, Token: %s, Token2: %s\n", message, token, token2);
            
            if(token2==NULL){
                write(sd, word2, strlen(word2) +1);
                sleep(1);
            }else{
                if (nftw(".", processEntry, 25, 0) == -1) {
                    perror("Call to nftw failed\n");
                    exit(0); 
                }

                if(found==F){
                    write(sd, word2, strlen(word2));
                    printf("write for fail\n");
                    found = F;
                }else{
                    write(sd, token2, strlen(token2)+1);
                    int tempFd = open(token2, O_RDONLY);
                    int count = 0;
                    while(read(tempFd, &(buffer[count]), 1)){
                        count++;
                    }
                    printf("%s\n", buffer);

                    int bytes_writte = write(sd, buffer, count+1);
                    printf("write for success ByteWritten:s %d\n", bytes_writte);
                    found = F;
                }
                memset(buffer, 0, sizeof buffer);

            }

        } else if(!strcmp(token, "put")){
            fprintf(stderr, "Clent send get command: %s, Token: %s, Token2: %s\n", message, token, token2);
            char* fileName = token2;
            if(!read(sd, buffer, sizeof buffer)){
                fprintf(stderr, "read() error\n");
                exit(5); 
            }

            int newFile = open(fileName, O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR|S_IRUSR);
            write(newFile, buffer, sizeof buffer);
            memset(buffer, 0, sizeof buffer);

            fprintf(stderr, "%s\n", buffer);
        }
        else{
            char* wtf = "Please type meaning command like put/get Filename\n";
            write(sd, wtf, sizeof(wtf)+1);
        }

        /*
        pseudo code
        if(get filename){
            if(找到file){
                write(fileContent);
            }else{
                write(not found)
            }
        }

        if(put filename){
            read(filecontent);
            fd = open(建个新文件, "filename")
        }
        
        
        */

    }
}

int main(int argc, char *argv[]){
    //Server 
    int sd, client, portNumber;
    socklen_t len;
    struct sockaddr_in servAdd;

    if(argc != 2){
        printf("Call model: %s <Port #>\n", argv[0]);
        exit(0);
    }

    if ((sd=socket(AF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1); 
    }

    servAdd.sin_family = AF_INET; servAdd.sin_addr.s_addr = htonl(INADDR_ANY); 
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber); 
    bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd)); 
    listen(sd, 5);
 
    printf("=============Server is Running=============\n");

    while(1){
        client=accept(sd,(struct sockaddr*)NULL,NULL);
        printf("==============Got a client From Server Side=============\n");
        if(!fork()){
            child(client);
        }else{
            waitpid(-1, NULL, WNOHANG);
        }
        close(client);
    } 
}


