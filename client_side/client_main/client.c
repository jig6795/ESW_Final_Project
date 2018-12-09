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

#include "dht22.h"
#include "send_recv_msg.h"

#define NAME_SIZE 20

char name[NAME_SIZE] = "[DEFAULT]";
//int G_sock;

pthread_t snd_thread, rcv_thread, temp_thread;
void* thread_return;

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

        if(strncmp(argv[3], "RPI", 3) == 0)
        {
          pthread_create(&snd_thread,NULL,send_msg,(void*)&sock);
          pthread_create(&rcv_thread,NULL,recv_msg,(void*)&sock);
          pthread_create(&temp_thread,NULL,send_temp,(void*)&sock);

          pthread_join(snd_thread, &thread_return);
          pthread_join(rcv_thread, &thread_return);
          pthread_join(temp_thread, &thread_return);
        }
        else
        {
          pthread_create(&snd_thread,NULL,send_msg,(void*)&sock);
          pthread_create(&rcv_thread,NULL,recv_msg,(void*)&sock);

          pthread_join(snd_thread, &thread_return);
          pthread_join(rcv_thread, &thread_return);
        }

        close(sock);
        return 0;
}
