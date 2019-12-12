#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/drvled.h>

#define DRIVER_NAME	"drvled"

struct drvled_info {
	struct led_classdev cdev;
	unsigned int gpio;
};

static struct drvled_info *led_info;

static void drvled_change_state(struct led_classdev *led_cdev,
				enum led_brightness value)
{
	if (value)
		gpio_set_value(led_info->gpio, 1);
	else
		gpio_set_value(led_info->gpio, 0);
}

static int drvled_probe(struct platform_device *pdev)
{
        struct drvled_platform_data *pdata = pdev->dev.platform_data;
        int result = 0;

        if (!(led_info = kzalloc(sizeof(struct drvled_info), GFP_KERNEL))) {
                pr_err("%s: kmalloc error\n", DRIVER_NAME);
                result = -ENOMEM;
                goto ret_err_kzalloc;
        }

        led_info->cdev.name = pdata->name;
        led_info->gpio = pdata->gpio;
        led_info->cdev.brightness_set = drvled_change_state;

        if ((result = gpio_request_one(led_info->gpio, GPIOF_DIR_OUT,
                                        led_info->cdev.name))) {
                pr_err("%s: gpio_request error\n", DRIVER_NAME);
                goto ret_err_gpio_request;
        }

        if ((result = led_classdev_register(NULL, &led_info->cdev))) {
                pr_err("%s: led_classdev_register error\n", DRIVER_NAME);
                goto ret_err_led_classdev_register;
        }

        pr_info("%s: initialized.\n", DRIVER_NAME);
        goto ret_ok;

ret_err_led_classdev_register:
        gpio_free(led_info->gpio);
ret_err_gpio_request:
        kfree(led_info);
ret_err_kzalloc:
ret_ok:
        return result;
}

static int drvled_remove(struct platform_device *pdev)
{
        led_classdev_unregister(&led_info->cdev);
        gpio_free(led_info->gpio);
        kfree(led_info);
        pr_info("%s: exiting.\n", DRIVER_NAME);
        return 0;
}

static struct platform_driver drvled_driver = {
        .probe = drvled_probe,
        .remove = drvled_remove,
        .driver = {
                .name = "drvled",
                .owner = THIS_MODULE,
        },
};

static int __init drvled_init(void)
{
        return platform_driver_register(&drvled_driver);
}

static void __exit drvled_exit(void)
{
        platform_driver_unregister(&drvled_driver);
}

module_init(drvled_init);
module_exit(drvled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name here <name@e-mail.com>");
MODULE_DESCRIPTION("Laboratory exercise 9");
MODULE_VERSION("1.0");
