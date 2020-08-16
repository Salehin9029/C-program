#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h>

int main(int argc, char *argv[]){
    char message[400000], buffer[40000];
    char * token, *tmp;
    int server, portNumber; socklen_t len;
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
    sscanf(argv[2], "%d", &portNumber); servAdd.sin_port = htons((uint16_t)portNumber);

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
        fprintf(stderr, "Please type your command or type quit to terminate: \n");
        int n=read(0, message, sizeof message);
        //message[n]='\0';
        //printf("%c%c%c%c", message[0], message[1], message[2], message[3]);
        token = strtok(message,"\n");

        fprintf(stderr, "Type message is: %s, Token: %s\n", message, token);

        if(!strcmp("", message) || !strcmp("quit", token)){
            close(server);
            exit(0);
        }
        //+1 means add NULL to be end of string marker for message array, 
        //anything after NULL is not considered part of the string
        write(server, message, strlen(message)+1);
        
        memset(message, 0, sizeof message);
        int nnnn=0;

        //sleep(3);

        // if(!((nnnn=read(server, buffer, 400000)))){
        //     fprintf(stderr, "read() error\n");
        //     exit(3); 
        // }

        int count = 0;
        while(!((nnnn=read(server, buffer[count], 1)))){
            count ++;
        }

        printf("Server Response: %s, length: %lu\n", buffer,nnnn);

        if(message[0]=='N'){
            printf("File not found! Please check your file name!\n");
        }else{
            printf("File Downloaded\n");    
            int newFile = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, S_IWUSR|S_IRUSR);
            write(newFile, buffer, sizeof buffer);
        }

        /*
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
