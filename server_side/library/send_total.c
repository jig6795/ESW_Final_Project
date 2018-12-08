#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#include "../header/send_total.h"

#define BUF_SIZE 100//최대 99개의 문자 저장가능
#define MAX_CLNT 256//최대 256개의 Client 접속가능

extern pthread_mutex_t mutx;//뮤텍스 mutx 선언
extern int clnt_cnt;//현재 Client의 개수를 저장할 변수 선언
extern int clnt_socks[MAX_CLNT];//Client들의 소켓 정보를 저장하는 배열 선언

void* send_total()//Server가 입력한 데이터를 모든 Client들에게 전송하는 함수
{
	int s_idx;//모든 Client들에게 데이터를 전송시켜주기 위한  선언
	char send_BUF[BUF_SIZE];//Server가 입력한 데이터를 Client에게 보낼때 사용하는 배열 선언
	while(1)//무한루프 생성
	{
		memset(send_BUF,0,sizeof(send_BUF));//데이터를 보낼때 사용하는 배열의 버퍼 초기화
		fgets(send_BUF,BUF_SIZE,stdin);//키보드의 입력을 배열로 받아온다.
		if(!strcmp(send_BUF,"Q\n")||!strcmp(send_BUF,"q\n"))//만약 입력받은 문자열이 "Q"또는 "q"라면
		{
			exit(1);//프로그램 종료
		}
		pthread_mutex_lock(&mutx);//mutex LOCK
		for(s_idx = 0;s_idx<clnt_cnt;s_idx++)//모든 Client들에게 데이터를 전송하기 위한 반복문 생성
		{
			write(clnt_socks[s_idx],"server>> ",sizeof("server>> "));
			write(clnt_socks[s_idx],send_BUF,strlen(send_BUF));//각각의 Client들에게 데이터를 전송
		}
		pthread_mutex_unlock(&mutx);//mutex UNLOCK
		
	}
}