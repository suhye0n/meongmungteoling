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
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in conn_addr;

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

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(nport);

    if (bind(serv, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
        perror("bind 에러 \n");
        exit(0);
    }

    if (listen(serv, 1) == -1) {
        perror("클라이언트 접속 에러!");
        exit(0);
    }

    addrlen = sizeof(conn);
    conn = accept(serv, (struct sockaddr *)&conn_addr, &addrlen);
    if (conn == -1) {
        perror("accept 에러! \n");
        exit(0);
    }

    if ((pid = fork()) == -1) {
        close(conn);
        perror("fork 에러");
    } else if (pid == 0) {
        while(1) {
            fgets(buf, sizeof(buf), stdin);
            nbytes = strlen(buf);
            write(conn, buf, MAX);
            if (stdncmp(buf, "exit", 4) == 0) {
                puts("프로그램 종료 \n");
                exit(0);
            }
        }
    } else if (pid > 0) {
        while(1) {
            if ((nbytes = read(conn, buf, MAX)) < 0) {
                perror("read 에러");
                exit(0);
            }

            print("%s \n", buf);

            if (strncmp(buf, "exit", 4) == 0) {
                exit(0);
            }
        }
    }

    close(conn);
    close(serv);
    return 0;
}