#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define NEWCHRDEV_CNT       1               // 设备号个数
#define NEWCHRDEV_NAME      "hello"         // 名字
#define CLASS_NAME          "hello"         // 创建/sys/class/hello类

//ioctl
#define SET_VALUE        _IOW('F', 0x11, unsigned long)
#define GET_VALUE        _IOR('F', 0x12, unsigned long)

typedef struct{
    int tag;
    unsigned int size;
    char msg[64];
}parcel_t;

/**
 *
<include/linux/cdev.h>
struct cdev {
    struct kobject kobj;                  //内嵌的内核对象.
    struct module *owner;                 //该字符设备所在的内核模块的对象指针.
    const struct file_operations *ops;    //该结构描述了字符设备所能实现的方法，是极为关键的一个结构体.
    struct list_head list;                //用来将已经向内核注册的所有字符设备形成链表.
    dev_t dev;                            //字符设备的设备号，由主设备号和次设备号构成.
    unsigned int count;                   //隶属于同一主设备号的次设备号的个数.
};
*/

//cdev_device设备结构体
struct cdev_device{
    dev_t   devid;              // 设备号
    struct  cdev cdev;          // cdev
    struct  class *class;       // 类
    struct  device *device;     // 设备
    int     major;              // 主设备号
    int     minor;              // 次设备号
};

struct cdev_device dev_device;  // 设备demo

/**
 * @brief           : 打开设备
 * @param - inode   : 传递给驱动的inode(自动传递)
 * @param - filp    : 设备文件，file结构体有个叫做private_data的成员变量
 *                    一般在open的时候将private_data指向设备结构体。
 * @return          : 0 成功;其他 失败
 */
static int hello_open (struct inode *inode, struct file *filep)
{
    printk("%s %d\n",__func__,__LINE__);
    return 0;
}

/**
 * @brief           : 关闭/释放设备
 * @param filep     : 要关闭的设备文件(文件描述符)
 * @return int      : 0 成功;其他 失败
 */
static int hello_release (struct inode *inode, struct file *filep)
{
    printk("%s %d\n",__func__,__LINE__);
    return 0;
}

static long hello_ioctl(struct file *filp,unsigned int cmd,unsigned long arg){
    parcel_t value={0};
    void __user *argp=(void __user *)arg;
    if(NULL==argp){
        printk("invalid argument.\n");
        return -EINVAL;
    }
    //!函数内的变量声明必须放在代码的开始处，不允许在for循环、if语句或任何其他代码块中声明变量
    //!在内核中还需要将printk的打印放在变量定义之后;
    printk("%s %d\n",__func__,__LINE__);
    switch (cmd)
    {
    case SET_VALUE:
        printk("%s set value\n",__func__);
        if(copy_from_user(&value,(parcel_t *)argp,sizeof(parcel_t))){
            printk("%s copy_from_user error\n",__func__);
            return -EFAULT;
        }
        printk("%s %d %d %s\n",__func__,value.tag,value.size,value.msg);
        break;
    case GET_VALUE:
        printk("%s get value\n",__func__);
        value.tag=0x0000FFFF;
        strcpy(value.msg,"this is a ioctl default message!\n");
        value.size=sizeof(value.msg);
        if(copy_to_user((void *)arg,&value,sizeof(parcel_t))){
            printk("%s copy_to_user error\n",__func__);
            return -EFAULT;
        }
        break;
    default:
        break;
    }
    return 0;
}
// /**
//  * @brief cat命令时,将会调用该函数
//  *
//  */
// ssize_t (*show)(struct device *dev, struct device_attribute *attr,
// 			char *buf);
// /**
//  * @brief echo 此属性节点时会调用此命令
//  *
//  */
// ssize_t (*store)(struct device *dev, struct device_attribute *attr,
// 			 const char *buf, size_t count);

static ssize_t show_sys_attr(struct device *dev, struct device_attribute *attr,char *buf){
    char msg[64]={0};
    int ret=0;
    printk("%s\n",__func__);
    ret =sprintf(msg,"%s\n","call show_sys_attr and show this message\n");
    printk("%s\n",msg);
    return ret;
}

static ssize_t store_sys_attr(struct device *dev, struct device_attribute *attr, const char *buf, size_t count){
    char receive_msg[64]={0};
    int ret=0;
    printk("%s\n",__func__);
    ret=sprintf(receive_msg,"received:%s size:%ld\n",buf,count);
    printk("%s\n",receive_msg);
    return ret;
}
//! 此处的宏中name会被展开为dev_attr_hello_debug
static DEVICE_ATTR(hello_debug, S_IWUSR | S_IRUSR, show_sys_attr, store_sys_attr);
/**
 * @brief 设备操作方法集
 *
 */
static struct file_operations dev_device_ops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
    .unlocked_ioctl=hello_ioctl,
};

// /**
//  * @brief       用于已知起始的设备号的情况;
//  *
//  * @param from  要分配的设备编号范围的初始值，明确了主设备号和起始次设备号;
//  * @param count 设备个数，即次设备号个数;
//  * @param name  相关联的设备名称;(可在/proc/devices目录下查看到),也即本组设备的驱动名称;
//  * @return int
//  */
// int register_chrdev_region(dev_t from,unsigned count,const char *name);

// /**
//  * @brief           用于未知设备号的情况;
//  *
//  * @param dev       获得一个分配到的设备号;
//  * @param baseminor 次设备号起始编号;
//  * @param count     次设备号的个数;
//  * @param name      相关联的设备名称;(同上)
//  * @return int
//  */
// int alloc_chrdev_region(dev_t *dev,unsigned baseminor,unsigned count,const char *name);

// /**
//  * @brief 从dev_t 中获得主设备号;
//  */
// MAJOR(dev_t dev)

// /**
//  * @brief 从dev_t 中获得次设备号;
//  */
// MINOR(dev_t dev)

// /**
//  * @brief 通过主设备号与次设备号生成dev_t;
//  *
//  * @param major 主设备号
//  * @param minor 次设备号
//  */
// MKDEV(int major,int minor);

/**
 * @brief       : 驱动入口函数
 * @param       : 无
 * @return      : 0 成功;其他 失败
 */
static int hello_init(void)
{
    int result = 0;
    printk("%s %d\n",__func__,__LINE__);
    /* 1、创建设备号 */
    if (dev_device.major) {/* 定义了设备号 */
        dev_device.devid = MKDEV(dev_device.major, 0);
        /* 据定义设备号申请注册 */
        result = register_chrdev_region(dev_device.devid, NEWCHRDEV_CNT, NEWCHRDEV_NAME);
        if(result < 0){
            printk("register_chrdev fail \n");
            goto out_err_1;
        }
    } else {/* 没有定义设备号,自动分配*/
        result = alloc_chrdev_region(&dev_device.devid, 0, NEWCHRDEV_CNT, NEWCHRDEV_NAME);  /* 申请设备号 */
        if(result < 0){
            printk("alloc_chrdev_region fail \n");  //自动分配设备号错误
            goto out_err_1;
        }
        dev_device.major = MAJOR(dev_device.devid); //MAJOR宏获取分配的主设备号
        dev_device.minor = MINOR(dev_device.devid); //MINOR宏获取分配的次设备号
    }
    printk("dev_device major=%d,minor=%d\r\n",dev_device.major, dev_device.minor);

    /* 2、初始化cdev */
    dev_device.cdev.owner = THIS_MODULE;
    cdev_init(&dev_device.cdev, &dev_device_ops);
    /* 3、添加一个cdev */
    cdev_add(&dev_device.cdev, dev_device.devid, NEWCHRDEV_CNT);

    /* 4、创建类 */
    dev_device.class = class_create(THIS_MODULE, CLASS_NAME); //生成sys/class/设备类
    if (IS_ERR(dev_device.class)) {
        printk(KERN_ERR "class_create() failed\n");
        result = PTR_ERR(dev_device.class);
        goto out_err_2;
    }

    /* 5、创建设备 */
    dev_device.device = device_create(dev_device.class, NULL, dev_device.devid, NULL, NEWCHRDEV_NAME);
    if (IS_ERR(dev_device.device)) {
        printk(KERN_ERR "device_create() failed\n");
        result = PTR_ERR(dev_device.device);
        goto out_err_3;
    }

    /*6、创建调试的属性节点*/
    if(sysfs_create_file(&dev_device.device->kobj,&dev_attr_hello_debug.attr)){
        printk(KERN_ERR "sysfs_create_file() failed\n");
        goto out_err_3;
    }
    return result;

//释放已申请的资源返回
out_err_3:
    device_destroy(dev_device.class, dev_device.devid);         //删除device
out_err_2:
    class_destroy(dev_device.class);                            //删除class
    unregister_chrdev_region(dev_device.devid, NEWCHRDEV_CNT);  //注销设备号
    cdev_del(&dev_device.cdev);                                 //删除cdev
out_err_1:
    return  result;
}
/**
 * @brief       : 驱动出口函数
 * @param       : 无
 * @return      : 无
 */
static void hello_exit(void)
{
    printk("%s %d\n",__func__,__LINE__);
    /* 注销字符设备驱动 */
    sysfs_remove_file(&dev_device.device->kobj,&dev_attr_hello_debug.attr);
    device_destroy(dev_device.class, dev_device.devid);         //删除device
    class_destroy(dev_device.class);                            //删除class
    unregister_chrdev_region(dev_device.devid, NEWCHRDEV_CNT);  //注销设备号
    cdev_del(&dev_device.cdev);                                 //删除cdev
    return;
}

MODULE_LICENSE("GPL"); //遵循GPL协议
MODULE_AUTHOR("ZhaoCheng");
MODULE_DESCRIPTION("Just for Demon");

module_init(hello_init);
module_exit(hello_exit);