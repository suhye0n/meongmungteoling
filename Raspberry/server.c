#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h> //atoi를 사용하려면 있어야함
#include <string.h> // memset 등
#include <unistd.h> //sockaddr_in, read, write 등
#include <arpa/inet.h>  //htnol, htons, INADDR_ANY, sockaddr_in 등
#include <sys/socket.h>

#define SLAVE_ADDR_01 0x48
static const char* I2C_DEV = "/dev/i2c-1";

void error_handling(char * message);

int main(int argc, char* argv[]) {
    int i2c_fd;

    int preVal = 0;
    int curVal = 0;
    int adcChannel = 1;

    int val = 0;                // DAC 출력 값을 저장 
    float outVoltage = 0.0;     // DAC에 출력되는 값을 전압으로 변환한 값을 저장

    //wiringPi 셋업
    if(wiringPiSetupGpio() < 0)
    {
        printf("wiringPiSetup() is failed\n");
        return 1;
    }
    //i2c연결 시도
    if((i2c_fd = wiringPiI2CSetupInterface(I2C_DEV, SLAVE_ADDR_01)) < 0)
    {
        printf("wiringPiI2CSetup Failed: \n");
        return 1;
    }
    printf("I2C start....\n");

    int serv_sock;
    int clnt_sock;
   
    //sockaddr_in은 소켓 주소의 틀을 형셩해주는 구조체로 AF_INET일 경우 사용
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr; //accept함수에서 사용됨.
    socklen_t clnt_addr_size;
   
    //TCP연결지향형이고 ipv4 도메인을 위한 소켓을 생성
    serv_sock=socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1)
         error_handling("socket error");
    
    //주소를 초기화한 후 IP주소와 포트 지정
    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family=AF_INET;                //타입: ipv4
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY); //ip주소
    serv_addr.sin_port=htons(atoi(argv[1]));     //port
    
    //소켓과 서버 주소를 바인딩
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
    	error_handling("bind error");
    
    //연결 대기열 5개 생성 
    if(listen(serv_sock, 5)==-1)
    	error_handling("listen error");
    
    //클라이언트로부터 요청이 오면 연결 수락
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock==-1)
        error_handling("accept error");

    // 소켓 연결되면 실행
    wiringPiI2CWrite(i2c_fd, 0x00 | adcChannel);

    preVal = wiringPiI2CRead(i2c_fd);   // 이전 활성화 값
    curVal = wiringPiI2CRead(i2c_fd);   // 현재 ADC 값
    printf("!!!! 소켓 연결됐당 !!!!");
    
    printf("Previous value = %d, ", preVal);
    printf("Current value = %d, ", curVal);

    delay(500);
    
    //소켓들 닫기
    close(clnt_sock);
    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}
