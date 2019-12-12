#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/leds.h>
#include <linux/slab.h>

#define DRIVER_NAME	"drvled"

#define LED_OFF        0
#define LED_ON         1

#define GPIO2_DATA     0x020A0000
#define GPIO2_DIR      0x020A0004

struct drvled_info {
	struct led_classdev cdev;
	unsigned int gpio;
};

static void __iomem *baseaddr = NULL;

static struct drvled_info *led_info;

static void drvled_setled(unsigned int status)
{
	unsigned int val;

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

static void drvled_change_state(struct led_classdev *led_cdev,
				enum led_brightness value)
{
	if (value)
		drvled_setled(LED_ON);
	else
		drvled_setled(LED_OFF);
}

static int __init drvled_init(void)
{
	int result = 0;

	if (!(led_info = kzalloc(sizeof(struct drvled_info), GFP_KERNEL))) {
		pr_err("%s: kmalloc error\n", DRIVER_NAME);
		result = -ENOMEM;
		goto ret_err_kzalloc;
	}

	led_info->cdev.name = "ipe:green:usr1";
	led_info->gpio = 53;
	led_info->cdev.brightness_set = drvled_change_state;

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

	if ((result = led_classdev_register(NULL, &led_info->cdev))) {
		pr_err("%s: led_classdev_register error\n", DRIVER_NAME);
		goto ret_err_led_classdev_register;
	}

	drvled_setdirection();

        pr_info("%s: initialized.\n", DRIVER_NAME);
	goto ret_ok;

ret_err_led_classdev_register:
	iounmap(baseaddr);
err_ioremap:
	release_mem_region(GPIO2_DATA, 8);
ret_err_request_mem_region:
	kfree(led_info);
ret_err_kzalloc:
ret_ok:
        return result;
}

static void __exit drvled_exit(void)
{
	led_classdev_unregister(&led_info->cdev);
	iounmap(baseaddr);
	release_mem_region(GPIO2_DATA, 8);
	kfree(led_info);
        pr_info("%s: exiting.\n", DRIVER_NAME);
}

module_init(drvled_init);
module_exit(drvled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name here <name@e-mail.com>");
MODULE_DESCRIPTION("Laboratory exercise 7");
MODULE_VERSION("1.0");
