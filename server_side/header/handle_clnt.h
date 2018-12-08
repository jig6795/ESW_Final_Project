#ifndef HANDLE_CLNT_H
#define HANDLE_CLNT_H

void * handle_clnt(void * arg);//Client로 부터 입력받은 데이터를 처리하는 함수
int check_name(char* msg);//Client 닉네임의 길이를 반환시켜주는 함수
void read_data(char* data, char client_rpi_num);//온, 습도 데이터를 읽어 출력하는 함수
void send_msg(char * msg, int len);//입력받은 데이터를 모든 Client들에게 roof back 시켜주는 함수
void response_information(int socket);//모든 온,습도의 rpi의 정보를 client들에게 보내주는 함수

#endif