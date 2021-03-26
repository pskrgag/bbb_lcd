#ifndef __LCD_PRIVATE__
#define __LCD_PRIVATE__

#include "lcd.h"

#define MAX_ROWS				2
#define MAX_COLUMNS				16

#define LCD_SET_DDRAM_POS(pos)			(0b10000000 | ((pos) & 0x7f))
#define LCD_NEW_LINE				LCD_SET_DDRAM_POS(0x40)
#define LCD_CLEAR_DISPLAY			0b00000001
#define LCD_CURSOR_HOME				0b00000010
#define LCD_CLEAR_RETURN_HOME			0b00000011
#define LCD_DISPLAY_OFF 			0b00001000
#define LCD_DISPLAY_ON(D, C, B)			(0b00001000 | (B) | ((C) << 1) | ((D) << 2))
#define LCD_FUNCTION_SET_8_BIT			0b0011
#define LCD_FUNCTION_SET_4_BIT			0b0010
#define LCD_SETUP_DISPLAY(DL, N, F)		(0b00100000 | ((DL) << 4) | ((N) << 3) | ((F) << 2))
#define LCD_ENTRY_MODE_SET(ID, S)		(0b00000100 | ((ID) << 1) | ((S) << 0))
#define LCD_CURSOR_OR_DISPLAY_SHIFT(SC, RL)	(0b00010000 | ((SC) << 3) | ((RL) << 2))

#define MATRIX_POS_TO_LCD(x, y)			((x) << 6 | y)

#define SAVE_REGISTERS_LOCK(lcd)			\
	int __rs__, __rw__;				\
	__rs__ = gpiod_get_value((lcd)->gpio_rs);	\
	__rw__ = gpiod_get_value((lcd)->gpio_rw);	\
	mutex_lock(&(lcd)->lock);

#define RESTORE_REGISTERS_UNLOCK(lcd)				\
	gpiod_set_value((lcd)->gpio_rs, __rs__);		\
	gpiod_set_value((lcd)->gpio_rw, __rw__);		\
	mutex_unlock(&(lcd)->lock);

#define SAVE_REGISTERS(lcd)				\
	int __rs__, __rw__;				\
	__rs__ = gpiod_get_value((lcd)->gpio_rs);	\
	__rw__ = gpiod_get_value((lcd)->gpio_rw);

#define RESTORE_REGISTERS(lcd)					\
	gpiod_set_value((lcd)->gpio_rs, __rs__);		\
	gpiod_set_value((lcd)->gpio_rw, __rw__);		\

typedef void (*lcd_cmd_handler_t)(struct lcd_data *, void *);

struct lcd_cmd {
	const char *cmd;
	const char *descr;
	lcd_cmd_handler_t handler;
};

#define DEFINE_CMD(_cmd, _descr, _handler)	\
{						\
	.cmd     = (_cmd),			\
	.descr   = (_descr),			\
	.handler = (_handler),			\
}

#endif /* __LCD_PRIVATE__ */
