#include "lcd_main.h"
#include "lcd_private.h"
#include "gpio.h"

#include <linux/string.h>

static struct lcd_cmd lcd_cmds[] = {
	DEFINE_CMD("clear", "clears screen", lcd_clear_display),
};

#define CURRENT_IN			1
#define CURRENT_OUT			0

#define COORDS_SET_X(coords, x)		(coords |= (x))
#define COORDS_SET_Y(coords, y)		(coords |= (y << 2))

static unsigned char current_line;
static unsigned char current_pos;

static void lcd_set_to_write(struct lcd_data *lcd)
{
	gpiod_set_value(lcd->gpio_rs, 1);
	gpiod_set_value(lcd->gpio_rw, 0);
}

static void lcd_set_to_command(struct lcd_data *lcd)
{
	gpiod_set_value_cansleep(lcd->gpio_rs, 0);
	gpiod_set_value_cansleep(lcd->gpio_rw, 0);
}

static inline void lcd_new_line(struct lcd_data *lcd)
{
	SAVE_REGISTERS(lcd);

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_NEW_LINE);
	mdelay(2);

	RESTORE_REGISTERS(lcd);
}

void lcd_clear_display(struct lcd_data *lcd)
{
	SAVE_REGISTERS(lcd);

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_CLEAR_DISPLAY);
	mdelay(2);

	RESTORE_REGISTERS(lcd);
}

void lcd_cursor_home(struct lcd_data *lcd)
{
	SAVE_REGISTERS(lcd);

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_CURSOR_HOME);
	mdelay(2);

	RESTORE_REGISTERS(lcd);
}

void lcd_set_ddram_addr(struct lcd_data *lcd, unsigned char x, unsigned char y)
{
	SAVE_REGISTERS(lcd);

	current_pos  = y;
	current_line = x;

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_SET_DDRAM_POS(MATRIX_POS_TO_LCD(x, y)));
	udelay(50);

	RESTORE_REGISTERS(lcd);
}

void lcd_init(struct lcd_data *lcd)
{
	lcd_set_to_command(lcd);

	gpio_set_4bit(lcd, LCD_FUNCTION_SET_8_BIT);
	mdelay(5);

	gpio_set_4bit(lcd, LCD_FUNCTION_SET_8_BIT);
	udelay(150);

	gpio_set_4bit(lcd, LCD_FUNCTION_SET_8_BIT);
	udelay(150);

	gpio_set_4bit(lcd, LCD_FUNCTION_SET_4_BIT);
	udelay(150);

	gpio_set_8bit(lcd, LCD_SETUP_DISPLAY(0, 1, 1));
	udelay(60);

	gpio_set_8bit(lcd, LCD_DISPLAY_OFF);
	udelay(60);

	gpio_set_8bit(lcd, LCD_CLEAR_DISPLAY);
	mdelay(4);

	gpio_set_8bit(lcd, LCD_ENTRY_MODE_SET(1, 0));
	udelay(60);

	gpio_set_8bit(lcd, LCD_DISPLAY_ON(1, 1, 0));
	udelay(60);

	lcd_set_to_write(lcd);
}

int lcd_print_msg(struct lcd_data *lcd, const char *msg)
{
	size_t i;

	lcd_set_to_write(lcd);

	for (i = 0; i < strlen(msg); ++i) {
		gpio_set_8bit(lcd, msg[i]);

		if (++current_pos >= MAX_COLUMNS) {
			lcd_new_line(lcd);
			current_pos = 0;
			current_line ^= 0x1;
		}
	}

	return 0;
}

unsigned char lcd_get_coords(struct lcd_data *lcd)
{
	unsigned char coords = 0;

	COORDS_SET_X(coords, current_line);
	COORDS_SET_Y(coords, current_pos);

	return coords;
}

size_t lcd_print_cmds(char *buf)
{
	int i;
	size_t ret = 0;

	ret += sprintf(buf, "Current available commands:\n");

	for (i = 0; i < ARRAY_SIZE(lcd_cmds); ++i)
		ret += sprintf(buf + ret, "\t%s\t%s\n", lcd_cmds[i].cmd, lcd_cmds[i].descr);

	return ret;
}

int lcd_proccess_cmd(const char *cmd, struct lcd_data *lcd)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lcd_cmds); ++i) {
		if (sysfs_streq(cmd, lcd_cmds[i].cmd)) {
			lcd_cmds[i].handler(lcd);
			return 0;
		}
	}

	return -1;
}
