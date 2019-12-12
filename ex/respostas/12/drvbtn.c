#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define DRIVER_NAME	"drvbtn"

struct drvbtn_data {
	struct input_dev *input;
	unsigned int gpio;
	unsigned int key;
	unsigned int irq;
	struct work_struct work;
};

static void drvbtn_work_func(struct work_struct *work)
{
	struct drvbtn_data *data;
	int status;

	data = container_of(work, struct drvbtn_data, work);

	status = !gpio_get_value(data->gpio);

	input_report_key(data->input, data->key, status);
        input_sync(data->input);
}

static irqreturn_t drvbtn_isr(int irq, void *dev_id)
{
	struct drvbtn_data *data = dev_id;

	schedule_work(&data->work);

	return IRQ_HANDLED;
}

static int drvbtn_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *child;
	struct drvbtn_data *data;
	int ret = -1;

	if ((data = kzalloc(sizeof(*data), GFP_KERNEL)) == NULL) {
		pr_err("%s: Error allocating memory for driver data!\n", DRIVER_NAME);
		ret = -ENOMEM;
		goto ret_err_kzalloc;
	}

	platform_set_drvdata(pdev, data);

	if ((data->input = input_allocate_device()) == NULL) {
		pr_err("%s: Error allocating input device!\n", DRIVER_NAME);
		ret = -ENOMEM;
		goto ret_err_input_allocate_device;
	}

	data->input->name = "Embedded Labworks Button Driver";
	data->input->dev.parent = &pdev->dev;
	data->input->id.bustype = BUS_HOST;
	data->key = KEY_ENTER;
	input_set_capability(data->input, EV_KEY, data->key);

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

	if ((data->irq = gpio_to_irq(data->gpio)) < 0) {
		pr_err("%s: Unable to get irq number!\n", DRIVER_NAME);
		ret = data->irq;
		goto ret_err_gpio_to_irq;
	}

	ret = request_irq(data->irq, drvbtn_isr,
			  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			  DRIVER_NAME, data);
	if (ret < 0) {
		pr_err("%s: Error requesting irq!\n", DRIVER_NAME);
		goto ret_err_request_irq;
	}

        if ((ret = input_register_device(data->input))) {
		pr_err("%s: Error registering input device!\n", DRIVER_NAME);
		goto err_input_register_device;
	}

	INIT_WORK(&data->work, drvbtn_work_func);

	pr_info("%s: probe successful.\n", DRIVER_NAME);
	return 0;

err_input_register_device:
	free_irq(data->irq, data);
ret_err_request_irq:
ret_err_gpio_to_irq:
	gpio_free(data->gpio);
ret_err_gpio_request:
ret_err_of_get_gpio:
	input_free_device(data->input);
ret_err_input_allocate_device:
	kfree(data);
ret_err_kzalloc:
	return ret;
}

static int drvbtn_remove(struct platform_device *pdev)
{
	struct drvbtn_data *data = platform_get_drvdata(pdev);

	input_unregister_device(data->input);
	free_irq(data->irq, data);
	gpio_free(data->gpio);
	input_free_device(data->input);
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
MODULE_DESCRIPTION("Laboratory exercise 12");
MODULE_VERSION("1.0");
