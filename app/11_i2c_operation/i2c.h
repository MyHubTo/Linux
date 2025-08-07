#ifndef __I2C_H
#define __I2C_H
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
int i2c_init(const char* dev_path);

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
int i2c_write(int fd, unsigned short dev_addr, unsigned char reg_addr, unsigned char* data, unsigned short len);

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
int i2c_read(int fd, unsigned short dev_addr,  unsigned char reg_addr, unsigned char* data, unsigned short len);

#endif