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
using namespace std;
#define BUFFSIZE 2048
int main(int argc,char *argv[]){

    int fd = socket(PF_INET, SOCK_DGRAM, 0);

    if(fd == -1){
        perror("udpServer.cpp socket() errors");
        exit(-1);
    }
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);
    int ret = bind(fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if(ret == -1){
        perror("udpServer.cpp bind() errors");
        exit(-1);
    }
    while(1){
        unsigned char buf[BUFFSIZE]={0};
        char ipBuf[16];
        struct sockaddr_in clientAddr;
        socklen_t size = sizeof(clientAddr);

        int len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *) &clientAddr, &size);
        cout << "client IP = " << inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, ipBuf, sizeof(ipBuf))
                        << "client PORT = " << ntohs(clientAddr.sin_port) << endl;
        sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr));
    }

    close(fd);
    return 0;
}
