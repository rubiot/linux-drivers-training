#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x88d5d897, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x1a536f9b, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xaa748863, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x275ef902, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x462a406c, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0x67f07456, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0xff73f7a9, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xcd3a7819, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x403f9529, __VMLINUX_SYMBOL_STR(gpio_request_one) },
	{ 0xa12d929d, __VMLINUX_SYMBOL_STR(desc_to_gpio) },
	{ 0x649e4710, __VMLINUX_SYMBOL_STR(of_get_named_gpiod_flags) },
	{ 0xee01c77c, __VMLINUX_SYMBOL_STR(of_get_next_child) },
	{ 0xa54886e2, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0x7a12cfe4, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0xca633ec, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0x97255bdf, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x1cfb04fa, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x344b7739, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x5f419491, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0xf816c866, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x629d666d, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x1bf87895, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0xe744154e, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "CA7DF24CEC68464E884B9B4");
