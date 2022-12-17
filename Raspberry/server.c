#include <wiringPi.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define SLAVE_ADDR_01 0x48
#define GREEN 14
#define RED 15
#define LED 18

static const char* I2C_DEV = "/dev/i2c-1";

int GPIO_control_setup(void);              // GPIO setup 함수
void *thread_led(void *arg);               // LED 제어 함수     (클라이언트 -> 서버)
void turn_on_buzzer(int buzz_switch);      // BUZZER 제어 함수  (서버)
void *thread_temp(void *arg);              // 온도 함수         (서버 -> 클라이언트)
void error_handling(char *message);        // 에러 발생 유무 확인 함수

// 공유변수
int i2c_fd;
int serv_sock;
int clnt_sock;
pthread_mutex_t mutex;

// main
int main(void) {
    pthread_t p_thread1, p_thread2;  // thread 2개 생성
    pthread_mutex_init(&mutex, NULL);           // 뮤텍스 초기화

    if (GPIO_control_setup()==-1) {
        return -1;
    }

    // thread 생성
    pthread_create(&p_thread1, NULL, thread_led, NULL);
    pthread_create(&p_thread2, NULL, thread_temp, NULL);

    // thread 종료 대기
    pthread_join(p_thread1, NULL);
    pthread_join(p_thread2, NULL);

    // 소켓들 닫기
    close(clnt_sock);
    close(serv_sock);

    // 뮤텍스 제거
    pthread_mutex_destroy(&mutex);
    return 0;
}

// GPIO setup
int GPIO_control_setup(void)
{
    //if(wiringPiSetupGpio() < 0)
    if(wiringPiSetupGpio() < 0)
    {
        printf("wiringPiSetup() is failed\n");
        return 1;
    }
    
    if((i2c_fd = wiringPiI2CSetupInterface(I2C_DEV, SLAVE_ADDR_01)) < 0)
    {
        printf("wiringPiI2CSetup Failed: \n");
        return 1;
    }
    printf("I2C start....\n");
   
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
   
    serv_sock=socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1)
         error_handling("socket error");
    
    memset(&serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(8080);
    
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
    	error_handling("bind error");
    
    if(listen(serv_sock, 5)==-1)
    	error_handling("listen error");
    
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock==-1)
        error_handling("accept error");

    // 소켓 연결되면 실행
    printf("!!!! 소켓 연결됐당 !!!! \n");
}

void turn_on_sos(int sos_switch)
{
    pinMode(GREEN, OUTPUT);
    //pinMode(RED, OUTPUT);

    if(sos_switch == 1)
    {
        digitalWrite(GREEN, LOW);
        digitalWrite(RED, HIGH);
        printf("강아지의 체온에 이상이 있습니다\n");
    }
    else if(sos_switch == 0)
    {
        digitalWrite(RED,LOW);
        digitalWrite(GREEN,HIGH);
    }
}

void *thread_led(void *arg)
{
    int preVal = 0;
    int curVal = 0;
    int led_adcChannel = 2;
    int threshold = 239; 

    while(1)
    {
        pthread_mutex_lock(&mutex);
        wiringPiI2CWrite(i2c_fd, 0x40 | led_adcChannel);

        preVal = wiringPiI2CRead(i2c_fd);
        curVal = wiringPiI2CRead(i2c_fd);

        printf("%d \n", curVal);

        pinMode(LED, PWM_OUTPUT);
        
        if(curVal < threshold) pwmWrite(LED, 0);
        else pwmWrite(LED, 1024);

        pthread_mutex_unlock(&mutex);
        delay(900);
    }
    return 0;
}

void *thread_temp(void *arg)
{
    int preVal = 0;
    int curVal = 0;
    int temp_adcChannel = 1;

    int val = 0;
    float outVoltage = 0.0;

    int sos_switch = 0;

    while(1) 
    {
        pthread_mutex_lock(&mutex);
        wiringPiI2CWrite(i2c_fd, 0x41 | temp_adcChannel);
        preVal = wiringPiI2CRead(i2c_fd);
        curVal = wiringPiI2CRead(i2c_fd);
        curVal = (255-curVal)*1.9-17;

        char lowbyte = (curVal & 255);
        char highbyte = ((curVal >> 8) & 255);
        char sd = (highbyte << 8) + lowbyte;

        send(clnt_sock, (char*)&sd, sizeof(sd), 0);

        if(sd < 37 || sd > 39)
        {
            sos_switch = 1;
            turn_on_sos(sos_switch);
        }
        else
        {
            sos_switch = 0;
            turn_on_sos(sos_switch);
        }
        pthread_mutex_unlock(&mutex);
        delay(5000);
    }
}

void error_handling(char *message)
{
    fputs(message,stderr);
    fputc('\n', stderr);
    exit(1);
}
