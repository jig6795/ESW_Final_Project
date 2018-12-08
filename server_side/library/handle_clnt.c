#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#include "../header/handle_clnt.h"

#define BUF_SIZE 100//최대 99개의 문자 저장가능
#define MAX_CLNT 256//최대 256개의 Client 접속가능

extern pthread_mutex_t mutx;//뮤텍스 mutx 선언
extern int rpi_total_cnt;
extern int clnt_cnt;//현재 Client의 개수를 저장할 변수 선언
extern int clnt_socks[MAX_CLNT];//Client들의 소켓 정보를 저장하는 배열 선언

extern struct rpi_total_data{
	char timestamp[50];
	char temp[50];
	char humi[50];
}RPI[10];

void * handle_clnt(void * arg)//Client로 부터 입력받은 데이터를 처리하는 함수
{
	int clnt_sock=*((int*)arg);//인자로 받아온 소켓값을 sock에 저장
	int str_len=0, i;//입력받은 데이터의 길이를 저장하는 변수 및 roof back message 반복문에서 이용할 변수 선언
	char msg[BUF_SIZE];//입력받은 데이터를 저장하는 배열 선언
	char check_info[50];
	char user[25];//Client의 닉네임을 저장하는 변수 선언
	char initial = 0;//초기에 닉네임을 받아 배열에 저장하기 위한 변수 선언

	char client_rpi_num;
	char check_rpi = 0;
	while(1)//무한 루프 생성
	{	
		memset(msg,0,sizeof(msg));//입력받을 데이터를 저장하는 배열 초기화
		str_len=read(clnt_sock, msg, sizeof(msg));//read()를 이용해 데이터를 읽고 해당 데이터의 Size를 반환한다

		if(str_len==0)//만약 입력받은 데이터가 없다면
		{	
			break;//무한루프 탈출
		}

		if(strncmp(msg,"TeMp",4)==0)//만약 온도 데이터라면
		{
			if(!check_rpi)
			{
				pthread_mutex_lock(&mutx);
				client_rpi_num = rpi_total_cnt++;
				pthread_mutex_unlock(&mutx);
				check_rpi = 1;
			}
			read_data(msg,client_rpi_num);
		}
		else//채팅 데이터라면
		{	
			int cnt = check_name(msg);//해당 채팅데이터의 아이디 사이즈를 반환받음
			if(initial == 0)//해당 아이디를 찾기 위한 조건문
			{
				memcpy(user,msg,cnt+2);//받은 아이디를 user 버퍼에 저장 +2 는 괄호 2개
				initial = 1;//다음에 호출되지 않기 위하여 Flag set
			}

			printf("%s\n",msg);
			memset(check_info,0,sizeof(check_info));
			strncpy(check_info,msg,sizeof(check_info)-1);
			memmove(check_info,check_info+cnt+5,strlen(check_info));		
			if(!strncmp(check_info,"ReQuEsT",7))
			{
				response_information(clnt_sock);
			}
			else
			{
				send_msg(msg,str_len);//채팅 데이터만 loop back
			}
		}
	}	
	pthread_mutex_lock(&mutx);//metux LOCK
	for(i=0; i<clnt_cnt; i++)   //disconnecte된 Client를 제거하기 위한 반복문
	{
		if(clnt_sock==clnt_socks[i])//해당 Client socket을 찾았다면
		{
			while(i++<clnt_cnt-1)//Client개수 -1만큼 반복
				clnt_socks[i]=clnt_socks[i+1];//해당 인덱스에 다음 인덱스 데이터를 대입시킨다
			break;//for문 탈출
		}
	}
	clnt_cnt--;//총 Client 개수를 -1씩 감소
	pthread_mutex_unlock(&mutx);//mutex UNLOCK
	if(initial==0)//만약 아무런 채팅 입력이 없었다면
	{
		printf("\n\ndisconnected : N/A\n\n");//N/A 출력
	}
	else
	{
		printf("\n\ndisconnected : %s\n\n",user);//입력한 데이터가 있다면 유저 닉네임 출력
	}
	close(clnt_sock);//Client 파일을 닫는다
	return NULL;
}

int check_name(char* msg)//Client 닉네임의 길이를 반환시켜주는 함수
{
	int count = 1;//이름 길이를 세는 count 함수 선언
	while(msg[count]!=']')//닫힌 괄호가 아닐때까지 반복
	{
		count++;//카운트 1씩 증가
	}
	return count-1;//마지막 1증가로 인해 1감소
}

void read_data(char* data, char client_rpi_num)//온, 습도 데이터를 읽어 출력하는 함수
{
	int itg_data = 0;//온,습도를 정수로 받기 위한 변수 선언
	int temp = 0;
	int hud = 0;//온,습도를 정수로 저장하는 변수 선언

	memmove(data,data+4,strlen(data));//'TeMp'단어 제거 data+4부터 데이터 길이까지 앞으로 이동

	itg_data = atoi(data);//입력받는 온, 습도 데이터를 정수로 변환
	hud = itg_data/1000;//1000으로 나누어 습도를 저장
	itg_data %= 1000;//온도를 받아오기위하여 1000으로 나눈 나머지를 자신에게 대입
	temp = itg_data;//1000으로 나눈 나머지값을 온도를 대입받는다

	time_t tim;//현재 시간을 구하기 위한 tim 변수 
	struct tm *t;//시, 분, 초 각각 출력하기 위한 tm 구조체 선언
	tim = time(NULL);//현재 시간을 초단위로 얻는다.
	t = localtime(&tim);//연, 월, 일, 시, 분, 초로 분리하여 tm구조체에 저장한다.

	pthread_mutex_lock(&mutx);

	memset(RPI[client_rpi_num].temp,0,sizeof(RPI[client_rpi_num].temp));
	memset(RPI[client_rpi_num].humi,0,sizeof(RPI[client_rpi_num].humi));
    memset(RPI[client_rpi_num].timestamp,0,sizeof(RPI[client_rpi_num].timestamp));
	sprintf(RPI[client_rpi_num].timestamp,"%04d-%02d-%02d %02d:%02d:%02d",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,t->tm_hour, t->tm_min, t->tm_sec);
	sprintf(RPI[client_rpi_num].temp,"%d.%d",temp/10,temp%10);
	sprintf(RPI[client_rpi_num].humi,"%d.%d",hud/10,hud%10);
	printf("RPI %d\n%s  %s\n",client_rpi_num+1,RPI[client_rpi_num].temp,RPI[client_rpi_num].humi);//현재의 시간과 온, 습도 화면에 출력

	pthread_mutex_unlock(&mutx);
}

void send_msg(char * msg, int len)//입력받은 데이터를 모든 Client들에게 roof back 시켜주는 함수
{
	int idx;//연결되어 있는 모든 Client들에게 데이터를 전송하기 위한 반복문에 쓰일 인덱스
	char loop_Back_data[BUF_SIZE];//입력받은 데이터를 모든 Client들에게 roof back 시켜줄때 쓰이는 배열 
	
	pthread_mutex_lock(&mutx);//mutex LOCK
	sprintf(loop_Back_data,"total>> %s\n",msg);//"total>>"문자열과 입력받은 데이터를 하나의 배열에 합친다
	for(idx=0; idx<clnt_cnt; idx++)//모든 Client들에게 데이터를 전송시키기 위해 반복한다
	{
		write(clnt_socks[idx], loop_Back_data,strlen(loop_Back_data));//해당 Client에 데이터를 전송
	}	
	pthread_mutex_unlock(&mutx);//mutex UNLOCK
}

void response_information(int socket)//모든 온,습도의 rpi의 정보를 client들에게 보내주는 함수
{
	char send_info[BUF_SIZE] = {0};
	for(int i = 0 ; i<rpi_total_cnt;i++)
	{
		memset(send_info,0,sizeof(send_info));
		sprintf(send_info,"RPI %d : timestamp: %s  temperature %s 'C    humidity %s %%\n",i+1,RPI[i].timestamp,RPI[i].temp,RPI[i].humi);
        printf("%s\n",send_info);
		write(socket,send_info,sizeof(send_info));
	}
}