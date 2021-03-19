#ifndef __GPIO__
#define __GPIO__

#include <stdint.h>

#define BASE_PATH		"/sys/class/gpio_dev/"

#define LCD_RS			"gpio2.6"
#define LCD_RW			"gpio2.7"
#define LCD_ENABLE		"gpio2.8"

#define LCD_DATA4		"gpio2.9"
#define LCD_DATA5		"gpio2.10"
#define LCD_DATA6		"gpio2.11"
#define LCD_DATA7		"gpio2.12"

#define HIGH			1
#define LOW			0

void gpio_send_4bit(uint8_t val);
void gpio_send_8bit(uint8_t val);
int gpio_set_value(const char *name, uint8_t value);
int gpio_get_value(const char *name);
void gpio_set_to_input();
void gpio_set_to_command();

#endif /* __GPIO__ */
