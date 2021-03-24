#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "lcd.h"
#include "lcd_main.h"

static int gpio_probe(struct platform_device *);
static int gpio_remove(struct platform_device *);

static ssize_t lcdtext_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count);

static ssize_t lcdcoord_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t lcdcoord_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count);

static ssize_t lcdcmd_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t lcdcmd_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count);

static DEVICE_ATTR_WO(lcdtext);
static DEVICE_ATTR_RW(lcdcoord);
static DEVICE_ATTR_RW(lcdcmd);

static struct attribute *attr_list[] = {
	&dev_attr_lcdtext.attr,
	&dev_attr_lcdcoord.attr,
	&dev_attr_lcdcmd.attr,
	NULL,
};

const static struct attribute_group attr_group = {
	.attrs = attr_list,
};

const static struct attribute_group *attr_groups[] = {
	&attr_group,
	NULL,
};

static struct of_device_id gpio_ids[] = {
	OF_DEVICE_ID("pskr,lcd", NULL),
	END_OF_LIST
};

static struct gpiodrv_data driver_data;
static struct platform_driver gpio_driver = {
	.probe  = gpio_probe,
	.remove = gpio_remove,
	.driver = {
		.name = "lcd",
		.of_match_table = (const struct of_device_id *)
				  of_match_ptr(&gpio_ids),
	},
};

static int gpio_probe(struct platform_device *dev)
{
	struct lcd_data *lcd_data;

	lcd_data = devm_kzalloc(&dev->dev, sizeof(*lcd_data), GFP_KERNEL);
	if (!lcd_data) {
		dev_err(&dev->dev, "No memory lol\n");
		return -1;
	}

	GET_GPIOD_WITH_CHECK(lcd_data, dev, rw);
	GET_GPIOD_WITH_CHECK(lcd_data, dev, rs);
	GET_GPIOD_WITH_CHECK(lcd_data, dev, e);

	GET_GPIOD_WITH_CHECK(lcd_data, dev, dt1);
	GET_GPIOD_WITH_CHECK(lcd_data, dev, dt2);
	GET_GPIOD_WITH_CHECK(lcd_data, dev, dt3);
	GET_GPIOD_WITH_CHECK(lcd_data, dev, dt4);

	dev_info(&dev->dev, "All stuff is accupied\n");

	driver_data.lcd_dev = device_create_with_groups(driver_data.class, &dev->dev, 0, lcd_data,
					   (const struct attribute_group **) &attr_groups,
					   "lcd16x2");
	if (IS_ERR(driver_data.lcd_dev)) {
		dev_err(&dev->dev, "Failed to create groups\n");
		return -2;
	}

	lcd_init(lcd_data);
	lcd_print_msg(lcd_data, "Hello");

	return 0;
}

static int gpio_remove(struct platform_device *dev)
{
	device_unregister(driver_data.lcd_dev);
	return 0;
}

static ssize_t lcdtext_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	struct lcd_data *lcd = dev_get_drvdata(dev);
	char *user_msg;
	size_t str_len;

	user_msg = kmemdup_nul(buf, count, GFP_KERNEL);
	if (!user_msg)
		return -ENOMEM;

	str_len = strlen(user_msg);

	user_msg[str_len - 1] == '\n'? user_msg[str_len - 1] = 0: (void) 0;

	lcd_print_msg(lcd, user_msg);

	kfree(user_msg);
	return count;
}

static ssize_t lcdcoord_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct lcd_data *lcd = dev_get_drvdata(dev);
	unsigned coords = lcd_get_coords(lcd);

	return sprintf(buf, "%hhu:%hhu\n", COORDS_TO_X(coords), COORDS_TO_Y(coords));
}

static ssize_t lcdcoord_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	char *tmp_buf = (char *) buf;			/* no const today :) */
	struct lcd_data *lcd = dev_get_drvdata(dev);
	unsigned char x, y;
	int res;

	res = kstrtos8(strsep(&tmp_buf, ":"), 0, &x);
	if (res || x > 1)
		return -EINVAL;

	res = kstrtos8(strsep(&tmp_buf, ":"), 0, &y);
	if (res || y > 15)
		return -EINVAL;

	lcd_set_ddram_addr(lcd, x, y);

	return count;
}

static inline ssize_t lcdcmd_show(struct device *dev, struct device_attribute *attr,
				  char *buf)
{
	return lcd_print_cmds(buf);
}

static ssize_t lcdcmd_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct lcd_data *lcd = dev_get_drvdata(dev);
	int ret;

	ret = lcd_proccess_cmd(buf, lcd);

	return ret? -EINVAL: count;
}

static int gpio_init(void)
{
	driver_data.class = class_create(THIS_MODULE, gpio_driver.driver.name);
	if (IS_ERR(driver_data.class)) {
		pr_err("Failed to create class\n");
		return -2;
	}

	platform_driver_register(&gpio_driver);

	return 0;
}

static void gpio_exit(void)
{
	platform_driver_unregister(&gpio_driver);
	class_destroy(driver_data.class);
}

MODULE_AUTHOR("Pavel Skripkin <paskripkin@gmail.com>");
MODULE_LICENSE("GPL");

module_init(gpio_init);
module_exit(gpio_exit);
