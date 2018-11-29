#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<signal.h>

#include<stdint.h>
#include<wiringPi.h>
#include<fcntl.h>
#include<termios.h>
#include<sys/types.h>

#define NAME_SIZE 20

char name[NAME_SIZE] = "[DEFAULT]";
int G_sock;

int main(int argc, char* argv[])
{
        int sock;
        struct sockaddr_in serv_addr;

        if(argc != 4)
        {
                printf("Usage : %s <IP>  <port> <name>\n\n", argv[0]);
                exit(1);
        }

        sprintf(name,"[%s]",argv[3]);
        sock = socket(PF_INET, SOCK_STREAM,0);

        memset(&serv_addr,0,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        serv_addr.sin_port = htons(atoi(argv[2]));

        if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))== -1)
        {
                printf("connect() error\n\n");
                exit(1);
        }

        close(sock);
        return 0;
}
