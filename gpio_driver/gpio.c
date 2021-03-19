#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/kthread.h>

#define MAX_LABEL			20
#define OF_DEVICE_ID(str, _data)	{ .compatible = (str), .data = (void *) (_data) }
#define END_OF_LIST			{ }

static int gpio_probe(struct platform_device *);
static int gpio_remove(struct platform_device *);

struct gpiodev_data {
	char label[MAX_LABEL];
	struct gpio_desc *gpio_desc;
};

struct gpiodrv_data {
	struct class *class;
	uint8_t total_dev;
	struct device **dev_arr;
};

static ssize_t direction_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t direction_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);
static ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t value_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);
static ssize_t label_show(struct device *dev, struct device_attribute *attr, char *buf);

static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);

static struct attribute *attr_list[] = {
	&dev_attr_direction.attr,
	&dev_attr_value.attr,
	&dev_attr_label.attr,
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
	OF_DEVICE_ID("pskr,gpio-test", NULL),
	OF_DEVICE_ID("pskr,gpio-test", NULL),
	END_OF_LIST
};

static struct gpiodrv_data driver_data;
static struct platform_driver gpio_driver = {
	.probe  = gpio_probe,
	.remove = gpio_remove,
	.driver = {
		.name = "gpio_driver",
		.of_match_table = (const struct of_device_id *)
				  of_match_ptr(&gpio_ids),
	},
};

static int gpio_probe(struct platform_device *dev)
{
	struct device_node *parent = dev->dev.of_node, *child = NULL;
	struct gpiodev_data *dev_data;
	int res;
	int i = 0;
	const char *label;

	pr_info("Probe lol");

	driver_data.total_dev = of_get_child_count(parent);
	if (!driver_data.total_dev) {
		dev_err(&dev->dev, "No childs");
		return -EINVAL;
	}

	driver_data.dev_arr = devm_kzalloc(&dev->dev, sizeof(*driver_data.dev_arr) *
					   driver_data.total_dev, GFP_KERNEL);
	if (!driver_data.dev_arr) {
		dev_err(&dev->dev, "No memory");
		return -ENOMEM;
	}

	for_each_available_child_of_node(parent, child) {
		dev_data = devm_kzalloc(&dev->dev, sizeof(*dev_data), GFP_KERNEL);
		if (!dev_data) {
			dev_err(&dev->dev, "No memory");

			return -EINVAL;
		}

		res = of_property_read_string(child, "label", &label);
		if (res < 0) {
			dev_err(&dev->dev, "No prorerty");
			snprintf(dev_data->label, sizeof(dev_data->label), "Unknown");
		} else {
			strncpy(dev_data->label, label, MAX_LABEL);
		}

		dev_data->gpio_desc = devm_fwnode_get_index_gpiod_from_child(&dev->dev,
						"lcd", 0, &child->fwnode, 0, dev_data->label);

		if (IS_ERR(dev_data->gpio_desc)) {
			dev_err(&dev->dev, "Failed to get gpiod\n");
			return PTR_ERR(dev_data->gpio_desc);
		}

		res = gpiod_direction_output(dev_data->gpio_desc, 1);
		if (res) {
			dev_err(&dev->dev, "Failed to set gpio to output\n");
			return PTR_ERR(dev_data->gpio_desc);
		}

		driver_data.dev_arr[i] = device_create_with_groups(driver_data.class, &dev->dev, 0, dev_data,
						    attr_groups, label);
		if (IS_ERR(driver_data.dev_arr[i])) {
			dev_err(&dev->dev, "Failed to create dev");
			return PTR_ERR(driver_data.dev_arr[i]);
		}

		++i;
	}

	return 0;
}

static int gpio_remove(struct platform_device *dev)
{
	int i;

	for (i = 0; i < driver_data.total_dev; ++i)
		device_unregister(driver_data.dev_arr[i]);
	
	return 0;
}

static ssize_t direction_show(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct gpiodev_data *data = dev_get_drvdata(dev);
	int res;

	res = gpiod_get_direction(data->gpio_desc);
	if (res < 0)
		return sprintf(buf, "Smth is broken");

	return sprintf(buf, "%s\n", res ? "In": "Out");
}

static ssize_t direction_store(struct device *dev, struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct gpiodev_data *data = dev_get_drvdata(dev);
	int res;

	if (sysfs_streq(buf, "In"))
		res = gpiod_direction_input(data->gpio_desc);
	else if (sysfs_streq(buf, "Out"))
		res = gpiod_direction_output(data->gpio_desc, 0);
	else
		res = -EINVAL;

	return res;
}

static ssize_t value_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct gpiodev_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", gpiod_get_value(data->gpio_desc));
}

static ssize_t value_store(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct gpiodev_data *data = dev_get_drvdata(dev);
	long res, value;

	res = kstrtol(buf, 0, &value);
	if (res < 0)
		return -EINVAL;

	gpiod_set_value(data->gpio_desc, value);

	return count;
}

static inline ssize_t label_show(struct device *dev, struct device_attribute *attr,
				 char *buf)
{
	struct gpiodev_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%s\n", data->label);
}

static int gpio_init(void)
{
	driver_data.class = class_create(THIS_MODULE, "gpio_dev");
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

MODULE_AUTHOR("Pavel Skripkin");
MODULE_LICENSE("GPL");

module_init(gpio_init);
module_exit(gpio_exit);
