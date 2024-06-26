#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    printk(KERN_INFO "Hello World\n"); //内核空间函数 printk与printf用法相似；
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Hello World Exit!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Mrzhao <598184817@qq.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple example for linux module");
MODULE_ALIAS("a simplest module");