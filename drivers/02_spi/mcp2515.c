
#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
dev_t dev_num;
struct cdev mcp2515_cdev;
struct class *mcp2515_class;
struct device *mcp2515_device;
struct spi_device *spi_dev;
void mcp2515_reset(void){
    int ret;
    char buff[]={0xc0};
    ret=spi_write(spi_dev,buff,sizeof(buff));
    if(ret<0){
        printk("spi_write error\n");
    }
}

char mcp2515_read_reg(char reg){
    char write_buff[]={0x03,reg}; 
    char read_buff;
    int ret;
    ret=spi_write_then_read(spi_dev,write_buff,sizeof(write_buff),&read_buff,sizeof(read_buff));
    if(ret<0){
        printk("spi_write_then_read error\n");
        return ret;
    }
    return read_buff;
}

void mcp2515_write_reg(char reg,char value){
    int ret;
    char write_buff[]={0x02,reg,value};
    ret=spi_write(spi_dev,write_buff,sizeof(write_buff));
    if(ret<0){
        printk("spi_write error\n");
    }
}

int mcp2515_open(struct inode *inode,struct file *file){
    return 0;
}
ssize_t mcp2515_read(struct file *file, char __user *buff, size_t size, loff_t *offset){
    char r_kbuf[13]={0};
    int ret;
    int i;
    for(i=0;i<sizeof(r_kbuf);i++){
        r_kbuf[i]=mcp2515_read_reg(0x61+i);
    }

    ret=copy_to_user(buff,r_kbuf,size);
    if(ret<0){
        printk("mcp2515_read error\n");
        return -1;
    }
    return 0;
}
ssize_t mcp2515_write(struct file *file, const char __user *buff, size_t size, loff_t *offset){
    char w_kbuf[5+8]={0};//根据mcp2515发送报文的要求最少13字节；
    int ret;
    int i;
    ret=copy_from_user(w_kbuf,buff,size);
    if(ret<0){
        printk("mcp2515_write error\n");
        return -1;
    }

    for(i=0;i<sizeof(w_kbuf);i++){
        mcp2515_write_reg(0x31+i,w_kbuf[i]);
    }
    //其它略
    return 0;
}

int mcp2515_release(struct inode *inode, struct file *file){
    return 0;
}

struct file_operations mcp2515_fops={
    .open =mcp2515_open,
    .read =mcp2515_read,
    .write=mcp2515_write,
    .release=mcp2515_release,
};

int mcp2515_probe(struct spi_device *spi){
    int ret;
    char value;
    printk("%s %d\n",__func__,__LINE__);
    spi_dev=spi;
    //probe到设备后，注册字符设备，用于open read write  操作
    ret=alloc_chrdev_region(&dev_num,0,1,"mcp2515"); 
    if(ret<0){
        printk("alloc_chrdev_region error\n");
        return -1;
    }
    cdev_init(&mcp2515_cdev,&mcp2515_fops);
    mcp2515_cdev.owner=THIS_MODULE;
    ret=cdev_add(&mcp2515_cdev,dev_num,1);
    if(ret<0){
        printk("cdev_add error\n");
        return -1;
    }
    //创建字符class 节点
    mcp2515_class=class_create(THIS_MODULE,"spi_to_can");
    if(IS_ERR(mcp2515_class)){
        printk("class_create error\n");
        return PTR_ERR(mcp2515_class);
    }
    //创建字符device 节点
    mcp2515_device=device_create(mcp2515_class,NULL,dev_num,NULL,"mcp2515");
    if(IS_ERR(mcp2515_device)){
        printk("mcp2515_device error\n");
        return PTR_ERR(mcp2515_device);
    }

    //配置mcp2515
    mcp2515_reset(); //复位进入配置模式
    value=mcp2515_read_reg(0x0e); //读取状态寄存器的值，判断是否进入配置模式
    printk("state reg:%x\n",value);
    return 0;
}
int mcp2515_remove(struct spi_device *spi){
    printk("%s %d\n",__func__,__LINE__);
    return 0;
}

const struct of_device_id mcp2515_of_match_table[]={
    {.compatible="my-mcp2515"},//使用设备树时，会优先使用此compatible进行匹配
    {}
};

const struct spi_device_id mcp2515_id_table[]={
    {"mcp2515"},
    {}
};

struct spi_driver spi_mcp2515 ={
    .probe =mcp2515_probe,
    .remove=mcp2515_remove,
    .driver={
        .name="mcp2515",
        .owner=THIS_MODULE,
        .of_match_table =mcp2515_of_match_table,
    },
    .id_table=mcp2515_id_table,
};

static int __init mcp2515_init(void)
{
    int ret;
    printk("%s %d\n",__func__,__LINE__);
    ret=spi_register_driver(&spi_mcp2515);      //注册spi设备驱动
    if(ret<0){
       printk("spi_register_driver error\n");
       return ret;
    }
    return ret;
}


static void __exit mcp2515_exit(void)
{
    printk("%s %d\n",__func__,__LINE__);
    //卸载字符驱动
    device_destroy(mcp2515_class,dev_num);
    class_destroy(mcp2515_class);
    cdev_del(&mcp2515_cdev);
    unregister_chrdev_region(dev_num,1);
    //注销spi设备驱动
    spi_unregister_driver(&spi_mcp2515);    
}

module_init(mcp2515_init);
module_exit(mcp2515_exit);

MODULE_AUTHOR("Mrzhao <598184817@qq.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple example spi for linux module");
MODULE_ALIAS("a simple module");