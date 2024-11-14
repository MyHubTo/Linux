#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define DEST_ADDR "240.10.10.10"
#define UDP_PORT 8099
#define BUFFSIZE 58010
using namespace std;
int main(int argc,char *argv[]){
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(8888);
    // inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr.s_addr);
    int num = 0;
    unsigned char buf[BUFFSIZE]={0};
    buf[0]=0xA5;
    buf[1]=0xA5;
    for(int i=2;i<BUFFSIZE;i++){
        buf[i]=0xAA;
    }
    buf[BUFFSIZE-1]=0x5A;
    buf[BUFFSIZE-2]=0x5A;
    while(1) {
        sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        unsigned char rbuf[BUFFSIZE]={0};
        int len = recvfrom(fd, rbuf, sizeof(rbuf), 0, NULL, NULL);
        printf("len:%d\n",len);
        if(len<0){
            printf("===>> error 1 %d\n",__LINE__);
            break;
        }
        for(int j=0;j<len;j++){
            printf("0x%x",rbuf[j]);
        }
        printf("\n");
       sleep(1);
    }
    close(fd);
    return 0;
}
