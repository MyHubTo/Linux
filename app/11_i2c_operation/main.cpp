#include <string>
#include <iostream>
#include <filesystem>
#include <memory>
#include "i2c.h"
using namespace std;
using namespace std::filesystem;
#define MAX30100_DEV_ADDR   0x57    //定义MAX30100的设备地址
/**
 * \brief 测试程序
 */

int main(int argc, const char *argv[])
{
    unsigned char buf[16] = { 0 };
    int fd = 0;

    fd = i2c_init("/dev/i2c/0");    //初始化I2C设备
    if (fd < 0) {
        printf("i2c_init failed\n");
        return 0;
    }

    //i2c_read(fd, MAX30100_DEV_ADDR, 0x01, buf, 1);

    buf[0] = 0xF0;
    i2c_write(fd, MAX30100_DEV_ADDR, 0x01, buf, 1); //向0x01寄存器写入0xF0

    i2c_read(fd, MAX30100_DEV_ADDR, 0x01, buf, 1);  //从0x01寄存器读取数据

    return 0;
}
