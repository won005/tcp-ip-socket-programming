#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

	// 얘네가 전부 뭘 뜻하는걸까?
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[] = "Hello World"; // 이거 버퍼로 배열로서 저장하는건가

		if (argc != 2)
	{
		printf("usage : %s <port>\n", argv[0]); // 포트..? argv의 첫번째 요소를 꺼내는거라고?
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); //이 소켓 함수가 뭘 return하는 거엿더라
	if (serv_sock == -1)
	{
		error_handling("socket() error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1){
		error_handling("bind() error"); //바인드 함수가 뭐였지ㅠㅠ
	}
	if(listen(serv_sock, 5) ==-1){
		error_handling("listen() error");
	} 
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if(clnt_sock == -1){
		error_handling("accept() error");
	}

	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	return 0;

}

void error_handling(char *message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}



//	gcc hello_server.c -o helloServer
//	./helloServer

//출력
//	./helloServer <port>
