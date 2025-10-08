#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
// data.txt 파일을 만들고, 그 안에 Let's go! 문자열을 작성하는 프로그램

void error_handling(char* message);

int main(void){
	int fd;
	char buf[] = "Let's go!\n"; //버퍼 배열 저장 
	fd = open("data.txt", O_CREAT |O_WRONLY|O_TRUNC,0644);
	if(fd==-1)
		error_handling("open() error!");
	printf("file descriptor: %d \n", fd);
	
	if (write(fd, buf, sizeof(buf))==-1)
		error_handling("write() error!");
	close(fd);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
