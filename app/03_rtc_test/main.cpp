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
#include <linux/rtc.h>
using namespace std;
using namespace fmt;

int main() {
    const string device="/dev/rtc";
    int fd,ret;
    rtc_time time;
    fd=open(device.c_str(),O_RDWR);
    if(fd<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "open, {} error!\n", device);
        return -1;
    }
    //read RTC time
    ret=ioctl(fd,RTC_RD_TIME,&time);
    if(ret<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl, RTC_RD_TIME {} error!\n", device);
        return -1;
    }
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,"{}-{}-{} {}:{}:{}\n"
        ,time.tm_year+1900,time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec);

    time.tm_mon=6;
    //set RTC time
    ret=ioctl(fd,RTC_SET_TIME,&time);
    if(ret<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl, RTC_RD_TIME {} error!\n", device);
        return -1;
    }

    //read RTC time agin
    ret=ioctl(fd,RTC_RD_TIME,&time);
    if(ret<0){
        fmt::print(fg(fmt::color::red) | fmt::emphasis::bold,
            "ioctl, RTC_RD_TIME {} error!\n", device);
        return -1;
    }
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold,"{}-{}-{} {}:{}:{}\n"
        ,time.tm_year+1900,time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec);
    return 0;
}
