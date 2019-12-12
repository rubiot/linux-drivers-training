#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

#define DRIVER_NAME	"drvled"

#define LED_OFF		0
#define LED_ON		1

static unsigned int led_status = LED_OFF;

static dev_t drvled_dev;

static struct cdev drvled_cdev;

static void drvled_setled(unsigned int status)
{
	led_status = status;
}

static ssize_t drvled_read(struct file *file, char __user *buf,
					   size_t count, loff_t * ppos)
{
	const char *msg[] = { "OFF\n", "ON\n" };
	int size;

	/* check if EOF */
	if (*ppos > 0)
		return 0;

	size = strlen(msg[led_status]);
	if (size > count)
		size = count;

	if (copy_to_user(buf, msg[led_status], size))
		return -EFAULT;

	*ppos += size;

	return size;
}

static ssize_t drvled_write(struct file *file, const char __user *buf,
		                            size_t count, loff_t *ppos)
{
	char kbuf = 0;

	if (copy_from_user(&kbuf, buf, 1)) {
		return -EFAULT;
	}

	if (kbuf == '1') {
		drvled_setled(LED_ON);
		pr_info("LED ON!\n");
	} else if (kbuf == '0') {
		drvled_setled(LED_OFF);
		pr_info("LED OFF!\n");
	}

	return count;
}

static struct file_operations drvled_fops = {
	.owner = THIS_MODULE,
	.write = drvled_write,
	.read = drvled_read,
};

static int __init drvled_init(void)
{
	int result;

	if ((result = alloc_chrdev_region(&drvled_dev, 0, 1, DRIVER_NAME))) {
		pr_err("%s: Failed to allocate device number!\n", DRIVER_NAME);
		return result;
	}

	cdev_init(&drvled_cdev, &drvled_fops);

	if ((result = cdev_add(&drvled_cdev, drvled_dev, 1))) {
		pr_err("%s: Char driver registration failed!\n", DRIVER_NAME);
		unregister_chrdev_region(drvled_dev, 1);
		return result;
	}

        pr_info("%s: initialized.\n", DRIVER_NAME);
        return 0;
}

static void __exit drvled_exit(void)
{
	cdev_del(&drvled_cdev);
	unregister_chrdev_region(drvled_dev, 1);
        pr_info("%s: exiting.\n", DRIVER_NAME);
}

module_init(drvled_init);
module_exit(drvled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name here <name@e-mail.com>");
MODULE_DESCRIPTION("Laboratory exercise 5");
MODULE_VERSION("1.0");
