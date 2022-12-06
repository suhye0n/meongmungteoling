#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZ 1024
#define PCF 120
#define DO 0
#define LED 2

void Print(int x)
{
    switch(x)
    {
        case 0:
            print("강아지의 체온이 너무 높아요! \n");
            break;
        case 1:
            printf("지금 강아지는 정상 체온이에요! \n");
            break;
        default:
            printf("print 값 에러 \n");
            break;
    }
}