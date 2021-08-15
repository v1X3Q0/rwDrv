#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#include "ebbchar.h"

#define MAX_DEV 2

static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static opVals_t trackedState = 0;
static void* trackedOff = 0;
static char   message[KBUF_NET] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored

static const struct file_operations mychardev_fops = {
    .owner      = THIS_MODULE,
    .open       = mychardev_open,
    .release    = mychardev_release,
    .unlocked_ioctl = mychardev_ioctl,
    .read       = mychardev_read,
    .write       = mychardev_write
};

struct mychar_device_data {
    struct cdev cdev;
};

static int dev_major = 0;
static struct class *mychardev_class = NULL;
static struct mychar_device_data mychardev_data[MAX_DEV];

static int mychardev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init mychardev_init(void)
{
    int err, i;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "mychardev");

    dev_major = MAJOR(dev);

    mychardev_class = class_create(THIS_MODULE, "mychardev");
    mychardev_class->dev_uevent = mychardev_uevent;

    for (i = 0; i < MAX_DEV; i++) {
        cdev_init(&mychardev_data[i].cdev, &mychardev_fops);
        mychardev_data[i].cdev.owner = THIS_MODULE;

        cdev_add(&mychardev_data[i].cdev, MKDEV(dev_major, i), 1);

        device_create(mychardev_class, NULL, MKDEV(dev_major, i), NULL, "mychardev-%d", i);
    }

    return 0;
}

static void __exit mychardev_exit(void)
{
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        device_destroy(mychardev_class, MKDEV(dev_major, i));
    }

    class_unregister(mychardev_class);
    class_destroy(mychardev_class);

    unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
}

static int mychardev_open(struct inode *inode, struct file *file)
{
    printk("%s: Device open\n", __FILE__);
    return 0;
}

static int mychardev_release(struct inode *inode, struct file *file)
{
    printk("%s: Device close\n", __FILE__);
    return 0;
}

static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("%s: Device ioctl\n", __FILE__);
    return 0;
}

static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    int error_count = -1;
    size_t tempThing = 0;
    size_of_message = count;
    if (count <= sizeof(message))
    {
        if ((size_t)trackedOff == 0)
        {
            tempThing = (size_t)printk;
            memcpy(message, &tempThing, sizeof(tempThing));
        }
        else if ((size_t)trackedOff == 1)
        {
            tempThing = (size_t)mychardev_read;
            memcpy(message, &tempThing, sizeof(tempThing));
        }
        else
        {
            memcpy(message, trackedOff, count);
        }
        // dumpMem(message, len);
        error_count = copy_to_user(buf, message, count);
    }
    if (error_count != 0)
    {
        return -EFAULT;
    }

    return count;
}

static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    size_t ncopied = 0;
    opVals_t interpretState;
    void* newDest = (void*)((size_t)message + sizeof(size_t));
    
    if (count <= KBUF_NET)
    {
        ncopied += copy_from_user(message, buf, count);
        
        memcpy(&interpretState, message, sizeof(opVals_t));
        trackedState = interpretState;

        if (trackedState == SEEK_OP)
        {
            memcpy(&trackedOff, newDest, sizeof(void*));
            printk(KERN_INFO "%s: seeked to pointer %p\n", __func__, trackedOff);
        }
        else if (trackedState == WRITE_OP)
        {
            count -= sizeof(void*);
            memcpy(trackedOff, message, count);

            printk(KERN_INFO "%s: Received %zu characters from the user\n", __func__, count);
        }
    }

    return ncopied;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Kutkov <elenbert@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
