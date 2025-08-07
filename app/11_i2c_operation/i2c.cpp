#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "i2c.h"


/**
 * \brief I2C读数据
 *
 * \param[in] fd：I2C设备文件描述符
 * \param[in] dev_addr：I2C设备地址
 * \param[in] reg_addr：寄存器地址
 * \param[in] data：指向希望写入的数据地址
 * \param[in] len：希望写入的字节个数
 *
 * \retval 成功返回0，失败返回-1
 */
int i2c_init(const char* dev_path)
{
    int fd = 0;

    //打开IIC总线设备节点
    fd = open(dev_path, O_RDWR);

    return fd;
}

/**
 * \brief I2C读数据
 *
 * \param[in] fd：I2C设备文件描述符
 * \param[in] dev_addr：I2C设备地址
 * \param[in] reg_addr：寄存器地址
 * \param[in] data：指向希望写入的数据地址
 * \param[in] len：希望写入的字节个数
 *
 * \retval 成功返回0，失败返回-1
 *
 * \note 该函数适用于8位从机地址，且寄存器地址只有1个字节的情况
 */
int i2c_write(int fd, unsigned short dev_addr, unsigned char reg_addr, unsigned char* data, unsigned short len)
{
    int ret = -1;
    unsigned char buff[20] = { 0 };

    buff[0] = reg_addr;
    memcpy(&buff[1], data, len);

    //写数据是1个msg
    __u16 total_len = len + 1;
    
    struct i2c_msg msg = {
        .addr = dev_addr,
        .flags = 0,
        .len = total_len,
        .buf = buff,
    };

    struct i2c_rdwr_ioctl_data rdwr_msg = {
        .msgs = &msg,
        .nmsgs = 1,
    };

    ret = ioctl(fd, I2C_RDWR, &rdwr_msg);

    return ret;
}

/**
 * \brief I2C读数据
 *
 * \param[in] fd：I2C设备文件描述符
 * \param[in] dev_addr：I2C设备地址
 * \param[in] reg_addr：寄存器地址
 * \param[out] data：存放读取到的数据
 * \param[in] len：希望读取的字节个数
 *
 * \retval 成功返回0，失败返回-1
 *
 * \note 该函数适用于8位从机地址，且寄存器地址只有1个字节的情况
 */
int i2c_read(int fd, unsigned short dev_addr,  unsigned char reg_addr, unsigned char* data, unsigned short len)
{
    int ret = -1;

    //读数据有2个msg
    struct i2c_msg msg[2] = {
        {
            .addr = dev_addr,   //设备地址
            .flags = 0,         //标志，为0表示写数据
            .len = 1,           //要写的数据的长度
            .buf = &reg_addr,   //要写的数据的地址
        },
        {
            .addr = dev_addr,   //设备地址
            .flags = I2C_M_RD,  //标志，I2C_M_RD表示主机向主机读数据
            .len = len,         //要读取的数据的长度
            .buf = data,        //读取的数据存放的地址
        },
    };

    struct i2c_rdwr_ioctl_data rdwr_msg = {
        .msgs = msg,
        .nmsgs = 2,
    };

    ret = ioctl(fd, I2C_RDWR, &rdwr_msg);

    return ret;
}