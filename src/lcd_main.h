#ifndef __LCD_MAIN__
#define __LCD_MAIN__

#include "lcd.h"

#define MAX_MSG			32		/* for time being */

#define COORDS_TO_X(coords)	(coords & 0x3)
#define COORDS_TO_Y(coords)	((coords >> 2) & 0xf)

void lcd_init(struct lcd_data *lcd);

int lcd_print_msg(struct lcd_data *lcd, const char *msg, bool user);
void lcd_set_ddram_addr(struct lcd_data *lcd, unsigned char x, unsigned char y);
unsigned char lcd_get_coords(struct lcd_data *lcd);

size_t lcd_print_cmds(char *buf);

int lcd_proccess_cmd(const char *cmd, struct lcd_data *lcd);

#endif /* __LCD_MAIN__ */
