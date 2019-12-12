#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>

#define DRIVER_NAME	"drvled"

struct drvled_info
{
        struct led_classdev cdev;
        unsigned gpio;
};

static struct drvled_info *led_info;
static int led_count = 0;

static void drvled_change_state(struct led_classdev *led_cdev, enum led_brightness value)
{
        struct drvled_info *info = container_of(led_cdev, struct drvled_info, cdev);
        gpio_set_value(info->gpio, !!value);
}

static int drvled_probe(struct platform_device *pdev)
{
        struct device_node *np = pdev->dev.of_node, *child;
        int result = 0;
        struct drvled_info *info;

        for_each_available_child_of_node(np, child)
                ++led_count;

        if (!(led_info = kzalloc(sizeof(struct drvled_info) * led_count, GFP_KERNEL))) {
                pr_err("%s: kmalloc error\n", DRIVER_NAME);
                result = -ENOMEM;
                goto ret_err_kzalloc;
        }

        info = led_info;
        for_each_available_child_of_node(np, child) {
                info->gpio = of_get_gpio(child, 0);
                info->cdev.name = of_get_property(child, "label", NULL);
                info->cdev.default_trigger = of_get_property(child, "default-trigger", NULL);
                info->gpio = of_get_gpio(child, 0);
                info->cdev.brightness_set = drvled_change_state;

                if ((result = gpio_request_one(info->gpio, GPIOF_DIR_OUT, info->cdev.name))) {
                        pr_err("%s: gpio_request error\n", DRIVER_NAME);
                        goto ret_err_gpio_request;
                }

                if ((result = led_classdev_register(NULL, &info->cdev))) {
                        pr_err("%s: led_classdev_register error\n", DRIVER_NAME);
                        goto ret_err_led_classdev_register;
                }

                info++;
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
        //struct drvled_info *
        //for_each_available_child_of_node(np, child) {
                led_classdev_unregister(&led_info->cdev);
                gpio_free(led_info->gpio);
        //}

        kfree(led_info);
        pr_info("%s: exiting.\n", DRIVER_NAME);
        return 0;
}

static const struct of_device_id of_drvled_match[] =
{
        { .compatible = "labworks,drvled", },
        {                                  },
};

static struct platform_driver drvled_driver =
{
        .probe  = drvled_probe,
        .remove	= drvled_remove,
        .driver	= {
                .name   = "led",
                .owner  = THIS_MODULE,
                .of_match_table = of_drvled_match,
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
MODULE_DESCRIPTION("Laboratory exercise 10");
MODULE_VERSION("1.0");
