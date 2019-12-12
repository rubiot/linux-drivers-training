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
	{ 0x403f9529, __VMLINUX_SYMBOL_STR(gpio_request_one) },
	{ 0xaaa0e9f, __VMLINUX_SYMBOL_STR(of_get_property) },
	{ 0xa12d929d, __VMLINUX_SYMBOL_STR(desc_to_gpio) },
	{ 0x649e4710, __VMLINUX_SYMBOL_STR(of_get_named_gpiod_flags) },
	{ 0x892c3970, __VMLINUX_SYMBOL_STR(led_classdev_register) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xb8057706, __VMLINUX_SYMBOL_STR(of_get_next_available_child) },
	{ 0x687934e9, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0xf816c866, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xd5c2bb79, __VMLINUX_SYMBOL_STR(led_classdev_unregister) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "1E75162F961D515D8EB2848");
