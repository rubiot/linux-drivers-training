#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <asm/io.h>

#define DRIVER_NAME	"drvled"

#define LED_OFF        0
#define LED_ON         1

#define GPIO2_DATA     0x020A0000
#define GPIO2_DIR      0x020A0004

static unsigned int led_status = LED_OFF;

static dev_t drvled_dev;

static struct cdev drvled_cdev;

static void __iomem *baseaddr = NULL;

static void drvled_setled(unsigned int status)
{
	unsigned int val;

	led_status = status;

	val = readl(baseaddr);

	if (status == LED_ON)
		val |= 0x200000;
	else if (status == LED_OFF)
		val &= ~0x200000;

	writel(val, baseaddr);
}

static void drvled_setdirection(void)
{
	unsigned int val;

	val = readl(baseaddr + 4);
	val |= 0x200000;
	writel(val, baseaddr + 4);
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
	int result = 0;

	if (request_mem_region(GPIO2_DATA, 8, DRIVER_NAME) == NULL) {
		pr_err("%s: Error requesting memory-mapped I/O!\n", DRIVER_NAME);
		result = -EBUSY;
		goto ret_err_request_mem_region;
	}

	if ((baseaddr = ioremap(GPIO2_DATA, 8)) == NULL) {
		pr_err("%s: Error mapping I/O!\n", DRIVER_NAME);
		result = -ENOMEM;
		goto err_ioremap;
	}

	if ((result = alloc_chrdev_region(&drvled_dev, 0, 1, DRIVER_NAME))) {
		pr_err("%s: Failed to allocate device number!\n", DRIVER_NAME);
		goto ret_err_alloc_chrdev_region;
	}

	cdev_init(&drvled_cdev, &drvled_fops);

	if ((result = cdev_add(&drvled_cdev, drvled_dev, 1))) {
		pr_err("%s: Char driver registration failed!\n", DRIVER_NAME);
		goto ret_err_cdev_add;
	}

	drvled_setdirection();

        pr_info("%s: initialized.\n", DRIVER_NAME);
	goto ret_ok;

ret_err_cdev_add:
	unregister_chrdev_region(drvled_dev, 1);
ret_err_alloc_chrdev_region:
	iounmap(baseaddr);
err_ioremap:
	release_mem_region(GPIO2_DATA, 8);
ret_err_request_mem_region:
ret_ok:
        return result;
}

static void __exit drvled_exit(void)
{
	cdev_del(&drvled_cdev);
	unregister_chrdev_region(drvled_dev, 1);
	iounmap(baseaddr);
	release_mem_region(GPIO2_DATA, 8);
        pr_info("%s: exiting.\n", DRIVER_NAME);
}

module_init(drvled_init);
module_exit(drvled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name here <name@e-mail.com>");
MODULE_DESCRIPTION("Laboratory exercise 6");
MODULE_VERSION("1.0");
