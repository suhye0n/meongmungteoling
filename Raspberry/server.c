#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX 511
#define PORT 5000

int main(int argc, char argv[]) {
    int serv;
    int conn;

    int addlen, datalen;

    char buf[MAX + 1];
    int nbytes;
    int nport;

    pid_t pid;

    if (argc == 1) {
        nport = PORT;
    } else if (argc == 2) {
        nport = atoi(argv[1]);
    } else {
        printf("%s <port> \n", argv[0]);
        exit(0);
    }

    serv = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv = -1) {
        perror("소켓이 생성되지 않았음! \n");
        exit(0);
    }

    memset(&serv, 0, sizeof(serv_addr));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(nport);

    if (bind(serv, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
        perror("bind 에러 \n");
        exit(0);
    }

    if (listen(serv, 1) == -1) {
        perror("클라이언트 접속 에러!");
        exit(0);
    }
}