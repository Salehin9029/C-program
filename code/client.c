#define _XOPEN_SOURCE 700 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <ftw.h> 
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

int main(int argc, char *argv[]){

    char message[40000], returnedFileNameOrFailMessage[40000];
    char * token, tmp[255];
    int server, portNumber; 
    socklen_t len;
    struct sockaddr_in servAdd;

    if(argc != 3){
        printf("Call model:%s <IP> <Port#>\n",argv[0]);
        exit(0);
    }

    if((server = socket(AF_INET, SOCK_STREAM, 0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1); 
    }

    servAdd.sin_family = AF_INET;
    sscanf(argv[2], "%d", &portNumber); 
    servAdd.sin_port = htons((uint16_t)portNumber);

    if(inet_pton(AF_INET,argv[1],&servAdd.sin_addr)<0){ 
        fprintf(stderr, " inet_pton() has failed\n"); 
        exit(2);
    }

    if(connect(server, (struct sockaddr *)&servAdd, sizeof(servAdd))<0){
        fprintf(stderr, "connect() has failed, exiting\n");
        exit(3);
    }

    while(1){ 
        memset(message, 0, sizeof message);
        //fprintf(stderr, "Server’s messgae: %s\n",message);
        int n;
        fprintf(stderr, "Please type your command or type quit to terminate: \n");
        
        while((n=read(0, message, 254))<=1){
            //check for ctrl-D
            if(!strcmp("", message)){ 
                close(server);
                exit(0);
            }
            printf("Please Type the comand like get or put filename!\n");
        }

        int i=0;
        while(message[i]){
            tmp[i] = message[i];
            i++;
        }

        token = strtok(message,"\n");
        fprintf(stderr, "Type message is: %s, Token: %s\n", message, token);

        if(!strcmp("quit", token)){
            close(server);
            exit(0);
        }
        
        //sprintf(tmp, "%c%c%c", message[0], message[1], message[2]);
        char * tmpToken = strtok(tmp, " \t\n"), *tmpToken2 = strtok(NULL, " \t\n\0");

        //2 hrs sacrificed on this shitty point, 
        //need to cast "put" into char* before strcmp two char points
        //otherwise segmentation fault 11
        if(!strcmp((char *)"put", tmpToken)){
            
            if(tmpToken2!= NULL){
                fOrDirName = tmpToken2;
                printf("tmpToken2: %s \n", tmpToken2);
                if (nftw(".", processEntry, 25, 0) == -1) {
                    perror("Call to nftw failed\n");
                    exit(0); 
                }
                if(found == T){
                    int tempFd = open(tmpToken2,O_RDONLY);
                    char buffer[40000];
                    int count = 0;
                    while(read(tempFd, &(buffer[count]), 1)){
                        count++;
                    }
                    printf("%s\n",buffer);
                    write(server, message, strlen(message)+1);
                    sleep(1);
                    write(server, buffer, count+1);
                    found = F;
                }else{
                    printf("File to put onto the server is not found, try again!\n");
                }
            }else{
                printf("Please Give a FileName to continue.\n");
            }

        }else if (!strcmp((char *)"get", tmpToken)){
            //+1 means add NULL to be end of string marker for message array, 
            //anything after NULL is not considered part of the string
            write(server, message, strlen(message)+1);
            
            memset(message, 0, sizeof message);
            
            if(!(read(server, returnedFileNameOrFailMessage, 40000))){
                fprintf(stderr, "read() error\n");
                exit(3); 
            }

            fprintf(stderr, "Server Response: %s\n", returnedFileNameOrFailMessage);
            
            if(returnedFileNameOrFailMessage[0]=='N'){
                printf("File not found! Please check your file name!\n");
            }else{
                printf("File Downloaded\n");
                
                memset(message, 0, sizeof message);
                if(!(read(server, message, 40000))){
                    fprintf(stderr, "read() error\n");
                    exit(3); 
                }
                printf("returnedFileName: %s\n", returnedFileNameOrFailMessage);
                int newFile = open(returnedFileNameOrFailMessage, O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR|S_IRUSR);
                write(newFile, message, sizeof message);
            }
        }else{
            write(server, message, strlen(message)+1);
            if(!(read(server, returnedFileNameOrFailMessage, 40000)) 
                    || returnedFileNameOrFailMessage[0]=='P'){
                printf("%s\n", returnedFileNameOrFailMessage);
                sleep(1);
            }
        }

        /*
        pseudo code:

        call read无论是put还是get
        
        get的话:
            if(message = not found)//就是没找到
            else 
                d = open(建个新文件, "filename")


        put的话应该是不会失败:

        if(文件在本地没找到){
            print 没找到
        }
        else就肯定成功

        */

    }
}
