#include "lcd_main.h"
#include "lcd_private.h"
#include "gpio.h"
#include "lcd_threads.h"

#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/time.h>

static void lcd_clear_display(struct lcd_data *lcd, void *dt);
static void lcd_to_clock(struct lcd_data *lcd, void *dt);

static struct lcd_cmd lcd_cmds[] = {
	DEFINE_CMD("clear", "clears screen",		   lcd_clear_display),
	DEFINE_CMD("time",  "turns lcd into analog clock", lcd_to_clock),
};

#define CURRENT_IN			1
#define CURRENT_OUT			0

#define COORDS_SET_X(coords, x)		(coords |= (x))
#define COORDS_SET_Y(coords, y)		(coords |= (y << 2))

static unsigned char current_line;
static unsigned char current_pos;

static void lcd_set_to_write(struct lcd_data *lcd)
__must_hold(&lcd->lock)
{
	gpiod_set_value(lcd->gpio_rs, 1);
	gpiod_set_value(lcd->gpio_rw, 0);
}

static void lcd_set_to_command(struct lcd_data *lcd)
__must_hold(&lcd->lock)
{
	gpiod_set_value_cansleep(lcd->gpio_rs, 0);
	gpiod_set_value_cansleep(lcd->gpio_rw, 0);
}

static inline void lcd_new_line(struct lcd_data *lcd)
__must_hold(&lcd->lock)
{
	SAVE_REGISTERS(lcd);

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_NEW_LINE);
	mdelay(2);

	RESTORE_REGISTERS(lcd);
}

static inline void lcd_stop_thread_and_clear(struct lcd_data *lcd)
{
	if (lcd_stop_thread())
		lcd_clear_display(lcd, (void *) 1);
}

static int lcd_clock_thread(void *lcd)
{
	struct timespec time;
	char current_time[9];

	lcd_clear_display(lcd, (void *) 0);

	while (!kthread_should_stop()) {
		lcd_set_ddram_addr(lcd, 0, 0);

		getnstimeofday(&time);
		sprintf(current_time, "%.2lu:%.2lu:%.2lu",
				      (time.tv_sec / 3600) % (24),
				      (time.tv_sec / 60) % (60),
				      time.tv_sec % 60);

		lcd_print_msg(lcd, current_time, 0);
		ssleep(1);
	}

	return 0;
}

static void lcd_to_clock(struct lcd_data *lcd, void *dt)
{
	mutex_lock(&lcd->lock);
	
	lcd_start_thread(lcd, lcd_clock_thread);
	
	mutex_unlock(&lcd->lock);
}

static void lcd_clear_display(struct lcd_data *lcd, void *dt)
{
	bool user = (bool) dt;
	SAVE_REGISTERS_LOCK(lcd);

	if (user)
		lcd_stop_thread();

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_CLEAR_DISPLAY);
	mdelay(2);

	current_pos = 0;
	current_line = 0;

	RESTORE_REGISTERS_UNLOCK(lcd);
}

void lcd_set_ddram_addr(struct lcd_data *lcd, unsigned char x, unsigned char y)
{
	SAVE_REGISTERS_LOCK(lcd);

	current_pos  = y;
	current_line = x;

	lcd_set_to_command(lcd);
	gpio_set_8bit(lcd, LCD_SET_DDRAM_POS(MATRIX_POS_TO_LCD(x, y)));
	udelay(50);

	RESTORE_REGISTERS_UNLOCK(lcd);
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

int lcd_print_msg(struct lcd_data *lcd, const char *msg, bool user)
{
	size_t i;

	if (user)
		lcd_stop_thread_and_clear(lcd);

	mutex_lock(&lcd->lock);

	lcd_set_to_write(lcd);

	for (i = 0; i < strlen(msg); ++i) {
		gpio_set_8bit(lcd, msg[i]);

		if (++current_pos >= MAX_COLUMNS) {
			switch (current_line) {
			case 0:
				lcd_new_line(lcd);
				break;
			case 1:
				gpio_set_8bit(lcd, LCD_CURSOR_OR_DISPLAY_SHIFT(1, 1));
				udelay(60);
				break;
			default:
				WARN_ON(1);
			}

			current_pos = 0;
			current_line ^= 0x1;
		}
	}

	mutex_unlock(&lcd->lock);

	return 0;
}

unsigned char lcd_get_coords(struct lcd_data *lcd)
{
	unsigned char coords = 0;

	mutex_lock(&lcd->lock);

	COORDS_SET_X(coords, current_line);
	COORDS_SET_Y(coords, current_pos);

	mutex_unlock(&lcd->lock);

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
			lcd_cmds[i].handler(lcd, (void *) 1);
			return 0;
		}
	}

	return -1;
}
