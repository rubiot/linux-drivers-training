#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

#define MODULE_NAME  "labmodule"

static char *name = "John";

static int __init labmodule_init(void)
{
	printk("%s: initialized. Hello %s!\n", MODULE_NAME, name);
	return 0;
}

static void __exit labmodule_exit(void)
{
	printk("%s: exiting.\n", MODULE_NAME);
}

module_init(labmodule_init);
module_exit(labmodule_exit);

module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "Your name here!");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name here <name@e-mail.com>");
MODULE_DESCRIPTION("Laboratory exercise 4.");
MODULE_VERSION("1.0");
