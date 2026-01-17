#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8000  //? 포트가 필요해?
#define MAX_STORAGE 100  // 머간단하게.. 작은거니까 

// 자료구조 정의
typedef struct {
    char key[256]; //c string x
    char value[256];
    int flag; // flag : 1= 사용중, 0= 비어있음
} KeyValue;


//전역 변수 저장소 선언
KeyValue storage[MAX_STORAGE];

//파싱함수 선언
int parse_command(char *buffer, char *parts[]);


void process_list(char *response);
void process_put(char *key, char *value, char *response);
void process_get(char *key, char *response);
void process_delete(char *key, char *response);



int main() {
    //변수 선언
    int server_fd; // 서버 소켓 파일 디스크립터(소켓 고유 번호)
    int client_fd; // 클라이언트 소켓 파일 디스크립터
    int client_len;
    struct sockaddr_in server_addr; //서버 주소 정보 구조체
    struct sockaddr_in client_addr; //클라이언트 주소 정보 구조체
    socklen_t client_addr_len; // 클라이언트 주소 구조체 크기
    char buffer[1000];// 받을 버퍼 크기
    

    //저장소 모두 비어있음으로 flag를 초기화
    for (int i = 0; i < MAX_STORAGE; i++) {
        storage[i].flag = 0;
    }


    // 1. 소켓 생성
    server_fd = socket(AF_INET,SOCK_STREAM,0); //* socket(도메인, 타입, 프로토콜)
    // AF_INET : IPv4
    // SOCK_STREAM : TCP
    // 0 : 프로토콜 자동 선택

    if(server_fd == -1){
        perror("socket() 실패"); 
        exit(1);
    }
    printf("소켓 생성 완료 \n");

    
    // 2. 주소 설정
    memset(&server_addr,0,sizeof(server_addr)); //구조체 0으로 초기화
    //(채울 메모리 시작 주소, 채울 값, 채울 바이트 수)
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    // 서버주소에 프로토콜, 포트번호, IP 주소 할당



    // 3. bind : 소켓에 주소할당
    //  어느 포트를 사용할지
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));


    // 4. listen : 연결 대기
    listen(server_fd, 5); // 대기 큐에 최대 5개 연결 가능

    printf("서버 시작 (포트 : %d)\n",PORT);



    // 5. while 루프 : 계속 클라이언트 받기
    while(1){
        // accept 로 클라이언트가 연결을 수락
        // 클라이언트가 접속할 때까지 여기서 대기 : block

        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        printf("클라이언트 연결됨 \n");

        // recv 클라이언트로부터 데이터 받기
        memset(buffer,0,1000); //1000 = buffersize
        recv(client_fd,buffer,1000-1,0);
        printf("받은 명령 : %s \n",buffer);

        //? 명령처리?

        char response[4096];  // 응답 저장할 버퍼 (LIST는 길 수 있으니 크게)
        char temp[1000];      // buffer 복사본 (strtok가 원본을 바꾸니까)
        strcpy(temp, buffer); // buffer를 temp에 복사
        
        char *parts[10];      // 파싱된 결과 저장할 배열
        int count = parse_command(temp, parts);  // 명령 파싱
        
        // 명령어가 없으면
        if (count == 0) {
            strcpy(response, "Invalid command");
        }
        // LIST 명령
        else if (strcmp(parts[0], "LIST") == 0) {
            process_list(response);
        }
        // PUT 명령
        else if (strcmp(parts[0], "PUT") == 0) {
            if (count == 3) {  // PUT,key,value 형식 확인
                process_put(parts[1], parts[2], response);
            } else {
                strcpy(response, "Invalid PUT format");
            }
        }
        // GET 명령
        else if (strcmp(parts[0], "GET") == 0) {
            if (count == 2) {  // GET,key 형식 확인
                process_get(parts[1], response);
            } else {
                strcpy(response, "Invalid GET format");
            }
        }
        // DELETE 명령
        else if (strcmp(parts[0], "DELETE") == 0) {
            if (count == 2) {  // DELETE,key 형식 확인
                process_delete(parts[1], response);
            } else {
                strcpy(response, "Invalid DELETE format");
            }
        }
        // 알 수 없는 명령
        else {
            strcpy(response, "Unknown command");
        }


        

        // send : 클라이언트에게 응답 보내기
        send(client_fd, "OK",2,0);

        // close : 이 클라이언트와 연결 종료
        close(client_fd);
        printf("클라이언트 연결 종료 \n\n");

    }

    return 0;

}


// 6. 명령 파싱 함수 : 받은 데이터를 쪼개어 이해하기
int parse_command(char *buffer, char *parts[]){
    // ex) 입력 PUT,myname,jiwon  출력 : parts[0] = PUT..
    int count = 0;
    char *token = strtok(buffer,","); 
    //strtok : 문자열을 구분자로 나누는 함수

    while(token != NULL){
        parts[count] =token;
        count ++;
        token = strtok(NULL,",");
    }
    return count;
}



// 7. 함수 구현

void process_list(char *response) {
    int count = 0;
    response[0] = '\0';
    
    for (int i = 0; i < MAX_STORAGE; i++) {
        if (storage[i].flag == 1) {
            if (count > 0) {
                strcat(response, "\n");
            }
            strcat(response, storage[i].key);
            strcat(response, ",");
            strcat(response, storage[i].value);
            count++;
        }
    }
    
    if (count == 0) {
        strcpy(response, "Not exist!");
    }
}

void process_put(char *key, char *value, char *response) {
    for (int i = 0; i < MAX_STORAGE; i++) {
        if (storage[i].flag == 1 && strcmp(storage[i].key, key) == 0) {
            strcpy(storage[i].value, value);
            strcpy(response, "Success!");
            return;
        }
    }
    
    for (int i = 0; i < MAX_STORAGE; i++) {
        if (storage[i].flag == 0) {
            strcpy(storage[i].key, key);
            strcpy(storage[i].value, value);
            storage[i].flag = 1;
            strcpy(response, "Success!");
            return;
        }
    }
    
    strcpy(response, "Storage full!");
}

void process_get(char *key, char *response) {
    for (int i = 0; i < MAX_STORAGE; i++) {
        if (storage[i].flag == 1 && strcmp(storage[i].key, key) == 0) {
            strcpy(response, storage[i].value);
            return;
        }
    }
    strcpy(response, "Not exist!");
}

void process_delete(char *key, char *response) {
    for (int i = 0; i < MAX_STORAGE; i++) {
        if (storage[i].flag == 1 && strcmp(storage[i].key, key) == 0) {
            storage[i].flag = 0;
            strcpy(response, "Success!");
            return;
        }
    }
    strcpy(response, "Not exist!");
}