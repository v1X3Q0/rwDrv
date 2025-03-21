#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

// krw non specific share
#include <k_share.h>
// linux specific share
#include <drv_share.h>

// seems like the symbols is sometimes _printk instead of printk
// need a macro to tell.
#ifdef LINUX_PRINTK_PREFIX
#define LINUX_PRINTK _printk
#else
#define LINUX_PRINTK printk
#endif

#define MAX_DEV 2

static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static void *trackedOff = 0;
static char message[KBUF_NET] = {0}; ///< Memory for the string that is passed from userspace

static const struct file_operations mychardev_fops = {
    .owner = THIS_MODULE,
    .open = mychardev_open,
    .release = mychardev_release,
    .unlocked_ioctl = mychardev_ioctl,
    .read = mychardev_read,
    .write = mychardev_write};

struct mychar_device_data
{
    struct cdev cdev;
};

// static int dev_major = 0;
// static struct class *mychardev_class = NULL;
// static struct mychar_device_data mychardev_data[MAX_DEV];

static int majorNumber;                     ///< Stores the device number -- determined automatically
static struct class *ebbcharClass = NULL;   ///< The device-driver class struct pointer
static struct device *ebbcharDevice = NULL; ///< The device-driver device struct pointer

#if defined(MYCHARCONST)
static int mychardev_uevent(const struct device *dev, struct kobj_uevent_env *env)
#else
static int mychardev_uevent(struct device *dev, struct kobj_uevent_env *env)
#endif
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init mychardev_init(void)
{
    LINUX_PRINTK(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
    majorNumber = register_chrdev(0, CHARNAME, &mychardev_fops);
    if (majorNumber < 0)
    {
        LINUX_PRINTK(KERN_ALERT "EBBChar failed to register a major number\n");
        return majorNumber;
    }
    LINUX_PRINTK(KERN_INFO "EBBChar: registered correctly with major number %d\n", majorNumber);

    // Register the device class
#if defined(MYCHARCONST)
    ebbcharClass = class_create(CLASSDRV);
#else
    ebbcharClass = class_create(THIS_MODULE, CLASSDRV);
#endif
    if (IS_ERR(ebbcharClass))
    { // Check for error and clean up if there is
        unregister_chrdev(majorNumber, CHARNAME);
        LINUX_PRINTK(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(ebbcharClass); // Correct way to return an error on a pointer
    }
    ebbcharClass->dev_uevent = mychardev_uevent;
    LINUX_PRINTK(KERN_INFO "EBBChar: device class registered correctly\n");

    // Register the device driver
    ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, CHARNAME);
    if (IS_ERR(ebbcharDevice))
    {                                // Clean up if there is an error
        class_destroy(ebbcharClass); // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, CHARNAME);
        LINUX_PRINTK(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(ebbcharDevice);
    }
    LINUX_PRINTK(KERN_INFO "EBBChar: device class created correctly\n"); // Made it! device was initialized
    return 0;
}

static void __exit mychardev_exit(void)
{
    // unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
    device_destroy(ebbcharClass, MKDEV(majorNumber, 0)); // remove the device
    class_unregister(ebbcharClass);                      // unregister the device class
    class_destroy(ebbcharClass);                         // remove the device class
    unregister_chrdev(majorNumber, CHARNAME);            // unregister the major number
    LINUX_PRINTK(KERN_INFO "EBBChar: Goodbye from the LKM!\n");
}

static int mychardev_open(struct inode *inode, struct file *file)
{
    LINUX_PRINTK("%s: Device open\n", __FILE__);
    return 0;
}

static int mychardev_release(struct inode *inode, struct file *file)
{
    LINUX_PRINTK("%s: Device close\n", __FILE__);
    return 0;
}

static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    LINUX_PRINTK("%s: Device ioctl\n", __FILE__);
    return 0;
}

static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    int error_count = -1;
    size_t szReadOut = count;
    LINUX_PRINTK("%s: Device read\n", __FILE__);

    if (count <= KBUF_SIZE)
    {
        if ((size_t)trackedOff == LEAK_PRINTK)
        {
            *(size_t *)message = (size_t)LINUX_PRINTK;
            szReadOut = sizeof(size_t);
        }
        else if ((size_t)trackedOff == LEAK_DEVREAD)
        {
            *(size_t *)message = (size_t)mychardev_read;
            szReadOut = sizeof(size_t);
        }
        else
        {
            memcpy(message, trackedOff, count);
        }
        // dumpMem(message, len);
        error_count = copy_to_user(buf, message, szReadOut);
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
    seek_struct *adjustStruct = (seek_struct *)message;
    LINUX_PRINTK("%s: Device write\n", __FILE__);

    if (count <= KBUF_NET)
    {
        ncopied += copy_from_user(message, buf, count);

        if (adjustStruct->opVals & SEEK_OP)
        {
            trackedOff = (void *)adjustStruct->offset;
            LINUX_PRINTK(KERN_INFO "%s: seeked to pointer 0x%lx\n", __func__, (size_t)trackedOff);
        }
        if (adjustStruct->opVals & WRITE_OP)
        {
            memcpy(trackedOff, &adjustStruct[1], adjustStruct->len);
            LINUX_PRINTK(KERN_INFO "%s: Received %zu characters from the user\n", __func__, count);
        }
    }

    return ncopied;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oleg Kutkov <elenbert@gmail.com>");

module_init(mychardev_init);
module_exit(mychardev_exit);
