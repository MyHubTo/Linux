#include <stdio.h>
#include <iostream>
#include <string>
#include <fmt/core.h>
#include <fmt/color.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
using namespace std;
using namespace fmt;
const string device="/dev/hello";
typedef struct{
    int tag;
    unsigned int size;
    char msg[64];
}parcel_t;

#define SET_VALUE        _IOW('F', 0x11, unsigned long)
#define GET_VALUE        _IOR('F', 0x12, unsigned long)
#define ATTR_DEVICE     "/sys/class/hello/hello/hello_debug"
int main() {
    int fd=open(device.c_str(),O_RDONLY);
    if(fd<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "open, {} error!\n", device);
        return -1;
    }
    parcel_t msg={0};
    msg.tag=0x000000FF;
    strcpy(msg.msg,"this is a application message\n");
    msg.size=sizeof(msg.msg);
    if(ioctl(fd,SET_VALUE,&msg)){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl, {} error!\n", msg.msg);
        return -1;
    }
    parcel_t msg_received={0};
    if(ioctl(fd,GET_VALUE,&msg_received)){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl, {} error!\n", msg_received.msg);
        return -1;
    }
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,
            "ioctl,get message tag:{} size:{} msg:{}",msg_received.tag,msg_received.size, msg_received.msg);

    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,"show attribute test\n");

    int f=open(ATTR_DEVICE,O_RDWR);
    if(f<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "open, {} error!\n", ATTR_DEVICE);
        return -1;
    }
    char value[64]={0};
    int ret= read(f,value,sizeof(value));
    if(ret<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "read, {} error!\n", ATTR_DEVICE);
        return -1;
    }
    string pay="this a application message to store attribute test\n";
    ret=write(f,pay.c_str(),64);
    if(ret<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "write, {} error!\n", ATTR_DEVICE);
        return -1;
    }
    return 0;
}
