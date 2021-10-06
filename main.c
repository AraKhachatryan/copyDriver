#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

static int copy_open(struct inode *inode, struct file *file);
static int copy_release(struct inode *inode, struct file *file);
static long copy_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static ssize_t copy_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t copy_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

static const struct file_operations copy_fops = {
    .owner      = THIS_MODULE,
    .open       = copy_open,
    .release    = copy_release,
    .unlocked_ioctl = copy_ioctl,
    .read       = copy_read,
    .write       = copy_write
};

struct mychar_device_data {
    struct cdev cdev;
};

static unsigned int dev_major = 0;
static unsigned int dev_count = 1;
static struct class *copy_class = NULL;
static struct mychar_device_data copy_data;

static int copy_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init copy_init(void)
{
    int err;
    dev_t dev;

    err = alloc_chrdev_region(&dev, 0, dev_count, "copy");

    copy_class = class_create(THIS_MODULE, "copy");
    copy_class->dev_uevent = copy_uevent;

    cdev_init(&copy_data.cdev, &copy_fops);
    copy_data.cdev.owner = THIS_MODULE;

    cdev_add(&copy_data.cdev, dev_major, 1);

    device_create(copy_class, NULL, dev_major, NULL, "copy");

    return 0;
}

static void __exit copy_exit(void)
{
    device_destroy(copy_class, dev_major);

    class_unregister(copy_class);
    class_destroy(copy_class);

    unregister_chrdev_region(dev_major, MINORMASK);
}

static int copy_open(struct inode *inode, struct file *file)
{
    printk("COPY: Device open\n");
    return 0;
}

static int copy_release(struct inode *inode, struct file *file)
{
    printk("COPY: Device close\n");
    return 0;
}

static long copy_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk("COPY: Device ioctl\n");
    return 0;
}

static ssize_t copy_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    uint8_t *data = "Hello from the kernel world!\n";
    size_t datalen = strlen(data);

    printk("Reading device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count > datalen) {
        count = datalen;
    }

    if (copy_to_user(buf, data, count)) {
        return -EFAULT;
    }

    return count;
}

static ssize_t copy_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    size_t maxdatalen = 30, ncopied;
    uint8_t databuf[maxdatalen];

    printk("Writing device: %d\n", MINOR(file->f_path.dentry->d_inode->i_rdev));

    if (count < maxdatalen) {
        maxdatalen = count;
    }

    ncopied = copy_from_user(databuf, buf, maxdatalen);

    if (ncopied == 0) {
        printk("Copied %zd bytes from the user\n", maxdatalen);
    } else {
        printk("Could't copy %zd bytes from the user\n", ncopied);
    }

    databuf[maxdatalen] = 0;

    printk("Data from the user: %s\n", databuf);

    return count;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ara Khachatryan <arakhachatryan91@gmail.com>");

module_init(copy_init);
module_exit(copy_exit);
