#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <asm/atomic.h>

#define MY_MAJOR       42
#define MY_MAX_MINORS	2  
#define BUF_LEN 512

#define WR_BUFFER_INDEX _IOW('a','a',int32_t*)
#define RD_BUFFER_INDEX _IOR('a','b',int32_t*)
#define WR_BUFFER _IOW('a','c',char*)
#define RD_BUFFER _IOR('a','d',char*)

static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

const struct file_operations mychardev_fops = {
    .owner      = THIS_MODULE,
    .open       = mychardev_open,
    .release    = mychardev_release,
    .unlocked_ioctl = mychardev_ioctl,
    .read       = mychardev_read,
    .write       = mychardev_write
};

struct mychar_device_data {
    struct cdev cdev;
    char buffer[BUF_LEN];
    int buffer_index;
    atomic_t atomic_variable;
};

/* dynamically assignment */
// static int dev_major = 0;
struct class *mychardev_class = NULL;
struct mychar_device_data mychardev_data[MY_MAX_MINORS];

static int mychardev_uevent(const struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

int __init mychardev_init(void)
{
    int err, i;
   
    /* static assignment */
    err = register_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS, "mychardev");
    if (err != 0) {
        /* report error */
        return err;
    }
    /* dynamically assignment */
    // dev_t dev;
    // err = alloc_chrdev_region(&dev, 0, MY_MAX_MINORS, "mychardev");
    // dev_major = MAJOR(dev);

    mychardev_class = class_create(THIS_MODULE, "mychardev");
    mychardev_class->dev_uevent = mychardev_uevent;

    for (i = 0; i < MY_MAX_MINORS; i++) {
        cdev_init(&mychardev_data[i].cdev, &mychardev_fops);
        mychardev_data[i].cdev.owner = THIS_MODULE;
        strncpy(mychardev_data[i].buffer, "default msg", BUF_LEN);
        mychardev_data[i].buffer_index = 0;
        atomic_set(&mychardev_data[i].atomic_variable, 0);

        /* static assignment */
        cdev_add(&mychardev_data[i].cdev, MKDEV(MY_MAJOR, i), 1);
        /* dynamically assignment */
        // cdev_add(&mychardev_data[i].cdev, MKDEV(dev_major, i), 1);
		
        /* static assignment */
        device_create(mychardev_class, NULL, MKDEV(MY_MAJOR, i), NULL, "mychardev-%d", i);
        /* dynamically assignment */
        // device_create(mychardev_class, NULL, MKDEV(dev_major, i), NULL, "mychardev-%d", i);
    }

    return 0;
}

void __exit mychardev_exit(void)
{
    int i;

    for (i = 0; i < MY_MAX_MINORS; i++) {
        /* static assignment */
        device_destroy(mychardev_class, MKDEV(MY_MAJOR, i));
        /* dynamically assignment */
        // device_destroy(mychardev_class, MKDEV(dev_major, i));
    }

    class_unregister(mychardev_class);
    class_destroy(mychardev_class);

    /* static assignment */
    unregister_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS);
    /* dynamically assignment */
    // unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

static int mychardev_open(struct inode *inode, struct file *file)
{
    int minor_num = MINOR(file->f_path.dentry->d_inode->i_rdev);
    /* 
    * atomic_cmpxchg(&atomic, a, b)
    * 若atomic的值與 a 相同
    * 則改寫為b
    * 若atomic的值與 a 不相同
    * 則不改寫為
    * 回傳值皆為原本的 atomic 值
    */
    if(!atomic_cmpxchg(&mychardev_data[minor_num].atomic_variable, 0, 1)){
        printk("MYCHARDEV: Device open successfully\n");
        return 0;
    }
    printk("MYCHARDEV: Device open failed\n");
    return -EBUSY; 
}

static int mychardev_release(struct inode *inode, struct file *file)
{
    int minor_num = MINOR(file->f_path.dentry->d_inode->i_rdev);
    printk("MYCHARDEV: Device close\n\n");
    atomic_set(&mychardev_data[minor_num].atomic_variable, 0);
    return 0;
}

static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    /*
    * arg 為 user space 的一段記憶體位址
    * copy_from_user 會從 user space 取出該記憶體位址的值
    * copy_to_user 會將值存入 user space 的記憶體位址的值
    */
    int minor_num = MINOR(file->f_path.dentry->d_inode->i_rdev);
    struct mychar_device_data *mychar_data = &mychardev_data[minor_num];
    printk("MYCHARDEV: Device ioctl\n");

    switch(cmd){
        case WR_BUFFER_INDEX:
            /* 透過 pointer 存取 struct 的成員，會存取到成員的值 */
            if(copy_from_user(&(mychar_data -> buffer_index), (int32_t*) arg, sizeof(mychar_data -> buffer_index)))
            {
                printk("Data Write : Err!\n");
            }
            break;
        case RD_BUFFER_INDEX:
            if(copy_to_user((int32_t*) arg, &(mychar_data -> buffer_index), sizeof(mychar_data -> buffer_index)))
            {
                printk("Data Read : Err!\n");
            }
            break;
        case WR_BUFFER:
            if(copy_from_user((mychar_data -> buffer) + (mychar_data -> buffer_index), (char*) arg, sizeof(char)))
            {
                printk("Data Read : Err!\n");
            }
            break;
        case RD_BUFFER:
            if(copy_to_user((char*) arg, &(mychar_data -> buffer[mychar_data -> buffer_index]), sizeof(char)))
            {
                printk("Data Read : Err!\n");
            }
            break;
        default:
            printk("Default\n");
            break;
    }
    return 0;
}

static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    size_t datalen = 1024;
    int minor_num = MINOR(file->f_path.dentry->d_inode->i_rdev);
    struct mychar_device_data *mychar_data = &mychardev_data[minor_num];

    printk("MYCHARDEV: Device read");
    // printk("Reading device: %d\n", minor_num);
    // printk("Copied %zd bytes from the device\n", count);
    // printk("mychar_data -> buffer: %s\n", mychar_data -> buffer);

    if (count > datalen) {
        count = datalen;
    }

    if (copy_to_user(buf, mychar_data -> buffer, count)) {
        return -EFAULT;
    }

    return count;
}

static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    size_t maxdatalen = BUF_LEN, ncopied;
    char databuf[BUF_LEN];
    int minor_num = MINOR(file->f_path.dentry->d_inode->i_rdev);
    struct mychar_device_data *mychar_data = &mychardev_data[minor_num];
    int i = 0;

    printk("MYCHARDEV: Device write");
    // printk("Writing device: %d\n", minor_num);

    if (count < maxdatalen) {
        maxdatalen = count;
    }

    ncopied = copy_from_user(databuf, buf, maxdatalen);

    if (ncopied == 0) {
        // printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        // printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    for(i = 0; i < BUF_LEN; i++){
        if(i < maxdatalen)
            mychar_data -> buffer[i] = databuf[i];
        else
            mychar_data -> buffer[i] = '\0';
    }

    // printk("mychar_data -> buffer: %s\n", mychar_data -> buffer);

    return count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tim Lee <ck1000356@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
