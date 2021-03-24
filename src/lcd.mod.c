#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xc5a83b3a, "module_layout" },
	{ 0x6e68b940, "class_destroy" },
	{ 0xa191975f, "platform_driver_unregister" },
	{ 0xc5850110, "printk" },
	{ 0x2f777d07, "__platform_driver_register" },
	{ 0xe8553bdd, "__class_create" },
	{ 0xde8f960a, "_dev_err" },
	{ 0x8ca62a6c, "device_create_with_groups" },
	{ 0x3d938fd, "_dev_info" },
	{ 0x5a47296f, "gpiod_direction_output" },
	{ 0xd68404ca, "devm_gpiod_get" },
	{ 0x11c7e44a, "devm_kmalloc" },
	{ 0x8df5b544, "device_unregister" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");

