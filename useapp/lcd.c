#include "gpio.h"
#include "commands.h"

#include <string.h>
#include <unistd.h>

static uint8_t current_pos;
static uint8_t current_line;

static void lcd_new_line()
{
	gpio_set_value(LCD_RS, 0);
	gpio_set_value(LCD_RW, 0);

	gpio_send_8bit(LCD_NEW_LINE);

	current_pos = 0;
	current_line ^= 1;
}

void lcd_putchar(char chr)
{
	gpio_send_8bit(chr);

	if (++current_pos > 15) {
		lcd_new_line();
		gpio_set_value(LCD_RS, 1);
	}
}

void lcd_init(void)
{
	gpio_set_value(LCD_RS, 0);
	gpio_set_value(LCD_RW, 0);

	gpio_send_4bit(0b0011);
	usleep(41000);

	gpio_send_4bit(0b0011);
	usleep(150);

	gpio_send_4bit(0b0011);
	usleep(150);

	gpio_send_4bit(0b0010);
	usleep(150);

	gpio_send_4bit(0b0010);
	gpio_send_4bit(0b1100);
	usleep(60);

	gpio_send_4bit(0x00);
	gpio_send_4bit(0b1000);
	usleep(60);

	gpio_send_4bit(0x00);
	gpio_send_4bit(0b0001);
	usleep(31000);

	gpio_send_4bit(0x00);
	gpio_send_4bit(0b0110);
	usleep(60);

	gpio_send_4bit(0x00);
	gpio_send_4bit(0b1100);
	usleep(60);

	gpio_set_to_input();
}

void lcd_print_message(const char *message)
{
	const size_t message_len = strlen(message);
	size_t i;

	for (i = 0; i < message_len; ++i) {
		lcd_putchar(message[i]);
	}
}

static int lcd_write_command(uint8_t cmd)
{
	int rs, rw;

	rs = gpio_get_value(LCD_RS);
	rw = gpio_get_value(LCD_RW);

	if (rs < 0 || rw < 0)
		return -1;

	gpio_set_to_command();

	gpio_send_8bit(cmd);

	gpio_set_value(LCD_RS, rs);
	gpio_set_value(LCD_RW, rw);

	return 0;
}

void lcd_clear(void)
{
	lcd_write_command(LCD_CLEAR_DISPLAY);
	lcd_write_command(LCD_CLEAR_RETURN_HOME);
	usleep(2000);

	current_pos = 0;
	current_line = 0;
}

