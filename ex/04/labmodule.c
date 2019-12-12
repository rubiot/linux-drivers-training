#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

#define MODULE_NAME "labmodule"

static char *name = "default";

static int __init labmodule_init(void)
{
	printk("%s: initialized with name [%s].\n", MODULE_NAME, name);
	return 0;
}

static void __exit labmodule_exit(void)
{
	printk("%s: exiting.\n", MODULE_NAME);
}

module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "User-defined name");

module_init(labmodule_init);
module_exit(labmodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rubio Terra<rubio.terra@tse.jus.br>");
MODULE_DESCRIPTION("Laboratory exercise 4.");
MODULE_VERSION("1.0");
