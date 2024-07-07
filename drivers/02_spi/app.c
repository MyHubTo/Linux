#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(){
    int fd;
    char w_buf[13]={0x66,0x08,0x22,0x33,0x88,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    char r_buf[13]={0};

    fd=open("/dev/mcp2515",O_RDWR); 
    if(fd<0){
        printf("open /dev/mcp2515 error\n");
        return -1;
    }

    write(fd,w_buf,sizeof(w_buf));
    read(fd,r_buf,sizeof(r_buf));

    for(int i=0;i<13;i++){
        printf("r_buf[%d] is %x\n",i,r_buf[i]);
    }
    close(fd);
    return 0;
}