#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8000
#define BUFFER_SIZE 4096

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char command[1000];
    char response[BUFFER_SIZE];
    
    printf("=== Key-Value 저장소 클라이언트 ===\n");
    printf("명령어: PUT,key,value / GET,key / DELETE,key / LIST\n");
    printf("종료: quit\n\n");
    
    while (1) {
        printf("명령 입력> ");
        fgets(command, 1000, stdin);
        command[strcspn(command, "\n")] = '\0';
        
        if (strcmp(command, "quit") == 0) {
            printf("클라이언트 종료\n");
            break;
        }
        
        if (strlen(command) == 0) {
            continue;
        }
        
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("socket() 실패");
            continue;
        }
        
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("connect() 실패");
            close(sock);
            continue;
        }
        
        send(sock, command, strlen(command), 0);
        
        memset(response, 0, BUFFER_SIZE);
        int recv_len = recv(sock, response, BUFFER_SIZE - 1, 0);
        
        if (recv_len > 0) {
            response[recv_len] = '\0';
            printf("응답:\n%s\n\n", response);
        } else {
            printf("응답 받기 실패\n\n");
        }
        
        close(sock);
    }
    
    return 0;
}