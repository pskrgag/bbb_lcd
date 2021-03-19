#include "gpio.h"

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VALUE_SIZE		2

int gpio_set_value(const char *name, uint8_t value)
{
	char full_path[sizeof(BASE_PATH) + 5 + 9] = {0x00};
	int fd, res, ret = 0;

	assert(value < 2);

	sprintf(full_path, BASE_PATH "%s/value", name);

	fd = open(full_path, O_WRONLY);
	if (fd < 0)
		return -1;

	switch (value)
	{
	case LOW:
		res = write(fd, "0", VALUE_SIZE);
		break;
	case HIGH:
		res = write(fd, "1", VALUE_SIZE);
		break;
	}

	if (res != VALUE_SIZE)
		ret = -1;

	usleep(30);
	return ret;
}

/*
 * LCD Start reading data only when E pin
 * is pulsed
 */
static int pulse_e_pin(void)
{
	gpio_set_value(LCD_ENABLE, 0x00);
	usleep(1);
	gpio_set_value(LCD_ENABLE, 0x01);
	usleep(1);
	gpio_set_value(LCD_ENABLE, 0x00);
	usleep(40);

	return 0;
}

/* Send command for 4 data pins
 * 	@val - 4 lower bits are valuable
 * 
 * 	1 bit -> DB4
 * 	2 bit -> DB5
 * 	3 bit -> DB6
 * 	4 bit -> DB7
 */
void gpio_send_4bit(uint8_t val)
{
	gpio_set_value(LCD_DATA4, val & 0x1);
	gpio_set_value(LCD_DATA5, (val >> 1) & 0x1);
	gpio_set_value(LCD_DATA6, (val >> 2) & 0x1);
	gpio_set_value(LCD_DATA7, (val >> 3) & 0x1);

	pulse_e_pin();
}

void gpio_send_8bit(uint8_t val)
{
	gpio_send_4bit((val >> 4) & 0xf);
	gpio_send_4bit(val & 0xf);
}

int gpio_get_value(const char *name)
{
	char full_path[sizeof(BASE_PATH) + 5 + 9] = {0x00};
	int fd, res = -1;
	char val[2] = {0x00};

	sprintf(full_path, BASE_PATH "%s/value", name);

	fd = open(full_path, O_RDONLY);
	if (fd < 0)
		return -1;

	res = read(fd, val, 2);
	if (res != 2)
		goto out;

	res = atoi(val);

out:
	close(fd);
	return res;
}

void gpio_set_to_input(void)
{
	gpio_set_value(LCD_RS, 1);
	gpio_set_value(LCD_RW, 0);
}

void gpio_set_to_command(void)
{
	gpio_set_value(LCD_RS, 0);
	gpio_set_value(LCD_RW, 0);
}
