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
	{ 0x5a47296f, "gpiod_direction_output" },
	{ 0x97255bdf, "strlen" },
	{ 0xd68404ca, "devm_gpiod_get" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0x85df9b6c, "strsep" },
	{ 0x8ca62a6c, "device_create_with_groups" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x2f777d07, "__platform_driver_register" },
	{ 0xc5850110, "printk" },
	{ 0x3560e651, "kmemdup_nul" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0xde8f960a, "_dev_err" },
	{ 0x3d938fd, "_dev_info" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x7bb82ada, "gpiod_get_value" },
	{ 0x37a0cba, "kfree" },
	{ 0x6e68b940, "class_destroy" },
	{ 0xaa2fcb25, "gpiod_set_value_cansleep" },
	{ 0x8df5b544, "device_unregister" },
	{ 0x222e7ce2, "sysfs_streq" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x7429e20c, "kstrtos8" },
	{ 0xa191975f, "platform_driver_unregister" },
	{ 0x2624265, "gpiod_set_value" },
	{ 0x11c7e44a, "devm_kmalloc" },
	{ 0xe8553bdd, "__class_create" },
};

MODULE_INFO(depends, "");

