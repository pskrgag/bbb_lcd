#ifndef __LCD__
#define __LCD__

#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#define OF_DEVICE_ID(str, _data)	{ .compatible = (str), .data = (void *) (_data) }
#define END_OF_LIST			{ }

#define GET_GPIOD_WITH_CHECK(lcd_data, dev, func)					\
	(lcd_data)->gpio_##func = devm_gpiod_get(&(dev)->dev, #func , 0);		\
	if (IS_ERR((lcd_data)->gpio_##func)) {						\
		dev_err(&dev->dev, "Failed to get %s", #func "-gpio");			\
		return -1;								\
	}										\
	gpiod_direction_output((lcd_data)->gpio_##func, 1);

struct gpiodrv_data {
	struct class *class;
	struct device *lcd_dev;
};

struct lcd_data {
	struct mutex lock;
	struct gpio_desc *gpio_rs;
	struct gpio_desc *gpio_rw;
	struct gpio_desc *gpio_e;

	struct gpio_desc *gpio_dt1;
	struct gpio_desc *gpio_dt2;
	struct gpio_desc *gpio_dt3;
	struct gpio_desc *gpio_dt4;
};

#endif /* __LCD__ */
