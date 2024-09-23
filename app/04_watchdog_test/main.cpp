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
#include <linux/watchdog.h>
using namespace std;
using namespace fmt;

int main() {
    int fd,ret;
    int time_out=10;//watch dog out time;
    string device="/dev/watchdog0";
    fd=open(device.c_str(),O_RDWR);
    if(fd<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "open, {} error!\n", device);
        return -1;
    }
    ret=ioctl(fd,WDIOC_SETTIMEOUT,&time_out);
    if(ret<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl,WDIOC_SETTIMEOUT {} error!\n", device);
        return -1;
    }

    while (true){
        ret=ioctl(fd,WDIOC_KEEPALIVE,NULL);
        if(ret<0){
            fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl,WDIOC_KEEPALIVE {} error!\n", device);
            return -1;
        }
        fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,
            "ioctl,WDIOC_KEEPALIVE {} ok!\n", device);
        sleep(1);
    }

    return 0;
}
