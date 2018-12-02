#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

char msg[BUF_SIZE];

extern char name[BUF_SIZE];

void* send_msg(void* arg)
{
  int sock = *((int*)arg);
  char name_msg[NAME_SIZE+BUF_SIZE];

  while(1)
  {
    fgets(msg,BUF_SIZE,stdin);
    if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n"))
    {
      close(sock);
      exit(1);
    }
    sprintf(name_msg,"%s > %s",name,msg);
    write(sock,name_msg,strlen(name_msg));
    memset(msg,0,sizeof(msg));
  }

  return NULL;
}

void* recv_msg(void* arg)
{
  int sock = *((int*)arg);
  char name_msg[NAME_SIZE+BUF_SIZE];
  int str_len;

  while(1)
  {
    str_len = read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);
    if(str_len == -1)
    {
      return (void*)-1;
    }
    name_msg[str_len]=0;
    fputs(name_msg,stdout);
  }
  return NULL;
}

/*
int main(void)
{
  while(1)
  {
    printf("1");
    return 0;
  }
}
*/
