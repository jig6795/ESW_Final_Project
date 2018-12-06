#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE 100//최대 99개의 문자 저장가능
#define MAX_CLNT 256//최대 256개의 Client 접속가능

int check_name(char* msg);//Client 닉네임의 길이를 반환시켜주는 함수
void * snd_total();//Server가 입력한 데이터를 모든 Client들에게 전송하는 함수
void * handle_clnt(void * arg);//Client로 부터 입력받은 데이터를 처리하는 함수
void send_msg(char * msg, int len);//입력받은 데이터를 모든 Client들에게 roof back 시켜주는 함수
void read_data(char* data,char client_rpi_numt);//온, 습도 데이터를 읽어 파일에 출력하는 함수
void response_information(int socket);
int clnt_cnt=0;//현재 Client의 개수를 저장할 변수 선언
int clnt_socks[MAX_CLNT];//Client들의 소켓 정보를 저장하는 배열 선언
pthread_mutex_t mutx;//뮤텍스 mutx 선언

struct rpi_total_data{
	char timestamp[50];
	char temp[50];
	char humi[50];
}RPI[10];
int rpi_total_cnt = 0;

int main(int argc, char *argv[])
{

	int serv_sock, clnt_sock;//소켓 정보를 저장할 변수 선언
	struct sockaddr_in serv_adr, clnt_adr;//소켓에 대한 주소 구조체 변수 선언
	int clnt_adr_sz;//Client의 주소 구조체 크기를 저장하는 변수 선언
	pthread_t t_id,send_id;//쓰레드 변수 선언

	if(argc!=2) {//프로그램 실행시 인수가 받아지지 않았다면
		printf("Usage : %s <port>\n", argv[0]);//해당 실행 명령에 화면에 출력
		exit(1);//프로그램 종료
	}
	pthread_mutex_init(&mutx, NULL);//mutx 초기화
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//Server 소켓 생성

	memset(&serv_adr, 0, sizeof(serv_adr));//Server 통신 설정 변수 초기화
	serv_adr.sin_family=AF_INET;//IPv4인터넷 프로토콜 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);//서버의 IP주소 자동으로 찾아서 대입 
	serv_adr.sin_port=htons(atoi(argv[1]));//인수로 받았던 정수로 변환후 데이터 포트로 설정
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)//IP주소와 PORT 할당중 문제가 생겼다면
	{
		printf("bind() error\n\n");//에러문구 출력
		exit(1);//프로그램 종료
	}

	if(listen(serv_sock, 5)==-1)//연결요청 가능상태로 변경중 문제가 생겼다면 
	{
		printf("listen() error\n\n");//에러문구 출력
		exit(1);//프로그램 종료
	}

	pthread_create(&send_id,NULL,snd_total,NULL);//snd_total()에 대한 쓰레드 생성
	pthread_detach(send_id);

	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);//accept()를 하기 위해 Client 소켓의 사이즈를 얻는다
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);//Client의 연결요청에 대한 수락을 한다.
		if(clnt_sock==-1)//연결에 대한 요청이 수락되지 않았다면
		{
			printf("accept() error\n\n");//경고문구 출력
			exit(1);//프로그램 종료
		}

		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));//해당 네트워크 주소 문자열로 화면에 출력

		pthread_mutex_lock(&mutx);//mutex LOCK
		clnt_socks[clnt_cnt++]=clnt_sock;//Client에 대한 소켓정보를 순서대로 누적하여 저장
		pthread_mutex_unlock(&mutx);//mutex UNLOCK
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);//handle_clnt()에 대한 쓰레드 생성
		pthread_detach(t_id);//쓰레드 종료후 각각의 쓰레드 반환
	}
	close(serv_sock);
	return 0;
}

void* snd_total()//Server가 입력한 데이터를 모든 Client들에게 전송하는 함수
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

int check_name(char* msg)//Client 닉네임의 길이를 반환시켜주는 함수
{
	int count = 1;//이름 길이를 세는 count 함수 선언
	while(msg[count]!=']')//닫힌 괄호가 아닐때까지 반복
	{
		count++;//카운트 1씩 증가
	}
	return count-1;//마지막 1증가로 인해 1감소
}

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

void response_information(int socket)
{
	char send_info[BUF_SIZE] = {0};
	for(int i = 0 ; i<rpi_total_cnt;i++)
	{
		memset(send_info,0,sizeof(send_info));
		sprintf(send_info,"RPI %d : timestamp: %s  temperature %s 'C    humidity %s %%\n",i+1,RPI[i].timestamp,RPI[i].temp,RPI[i].humi);
		write(socket,send_info,sizeof(send_info));
	}
}