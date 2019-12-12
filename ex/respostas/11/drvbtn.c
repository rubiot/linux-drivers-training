#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>

#define DRIVER_NAME	"drvbtn"

struct drvbtn_data {
	dev_t devt;
	struct cdev cdev;
	int gpio;
	unsigned int event;
	unsigned long counter;
	struct task_struct *task;
	wait_queue_head_t wait;
};

static int drvbtn_open(struct inode *i, struct file *file)
{
	struct drvbtn_data *data;

	data = container_of(i->i_cdev, struct drvbtn_data, cdev);
	file->private_data = data;

	data->counter = 0;

	return 0;
}

static ssize_t drvbtn_read(struct file *file, char __user * buf,
			   size_t count, loff_t * ppos)
{
	struct drvbtn_data *data = file->private_data;
	char msg[256];
	int ret = 0;

	if (!wait_event_interruptible(data->wait, data->event == 1)) {

		data->event = 0;

		sprintf(msg, "Button pressed %ld times!\n", ++data->counter);

		if ((ret = copy_to_user(buf, msg, strlen(msg))))
			ret = -EFAULT;
		else
			ret = strlen(msg);
	}

	return ret;
}

static struct file_operations drvbtn_fops = {
	.owner = THIS_MODULE,
	.read = drvbtn_read,
	.open = drvbtn_open,
};

static int drvbtn_thread(void *param)
{
	struct drvbtn_data *data = (struct drvbtn_data *)param;

	while (!kthread_should_stop()) {

		msleep(100);

		if (!gpio_get_value(data->gpio)) {
			data->event = 1;
			wake_up_interruptible(&data->wait);
		}
	}

	return (0);
}

static int drvbtn_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	struct drvbtn_data *data;
	int ret = -1;

	if ((data = kzalloc(sizeof(*data), GFP_KERNEL)) == NULL) {
		ret = -ENOMEM;
		goto ret_err;
	}

	platform_set_drvdata(pdev, data);

	child = of_get_next_child(np, NULL);

	data->gpio = of_get_gpio(child, 0);
	if (!gpio_is_valid(data->gpio)) {
		pr_err("%s: Invalid GPIO!\n", DRIVER_NAME);
		goto ret_err_of_get_gpio;
	}

	if ((ret = gpio_request_one(data->gpio, GPIOF_DIR_IN, DRIVER_NAME))) {
		pr_err("%s: GPIO request error\n", DRIVER_NAME);
		goto ret_err_gpio_request;
	}

	if ((ret = alloc_chrdev_region(&data->devt, 0, 1, DRIVER_NAME))) {
		pr_err("%s: Failed to allocate device number!\n", DRIVER_NAME);
		goto ret_err_alloc_chrdev_region;
	}

	cdev_init(&data->cdev, &drvbtn_fops);

	if ((ret = cdev_add(&data->cdev, data->devt, 1))) {
		pr_err("%s: Char driver registration failed!\n", DRIVER_NAME);
		goto ret_err_cdev_add;
	}

	data->task = kthread_run(drvbtn_thread, data, DRIVER_NAME);
	if (IS_ERR(data->task)) {
		pr_err("%s: Error creating thread!\n", DRIVER_NAME);
		ret = PTR_ERR(data->task);
		goto ret_err_kthread_run;
	}

	init_waitqueue_head(&data->wait);

	pr_info("%s: probe successful.\n", DRIVER_NAME);
	return 0;

ret_err_kthread_run:
	cdev_del(&data->cdev);
ret_err_cdev_add:
	unregister_chrdev_region(data->devt, 1);
ret_err_alloc_chrdev_region:
	gpio_free(data->gpio);
ret_err_of_get_gpio:
ret_err_gpio_request:
	kfree(data);
ret_err:
	return ret;
}

static int drvbtn_remove(struct platform_device *pdev)
{
	struct drvbtn_data *data = platform_get_drvdata(pdev);

	kthread_stop(data->task);
	cdev_del(&data->cdev);
	unregister_chrdev_region(data->devt, 1);
	gpio_free(data->gpio);
	kfree(data);

	return 0;
}

static const struct of_device_id of_drvbtn_match[] = {
	{ .compatible = "labworks,drvbtn", },
	{ },
};

static struct platform_driver drvbtn_driver = {
	.probe = drvbtn_probe,
	.remove = drvbtn_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner  = THIS_MODULE,
		.of_match_table = of_drvbtn_match,
	},
};

module_platform_driver(drvbtn_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name here <name@e-mail.com>");
MODULE_DESCRIPTION("Laboratory exercise 11");
MODULE_VERSION("1.0");
