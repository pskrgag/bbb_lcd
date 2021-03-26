#ifndef __LCD_THREADS__
#define __LCD_THREADS__

#include "lcd.h"

int lcd_start_thread(struct lcd_data *lcd, int (*func)(void *));
int lcd_stop_thread(void);

#endif /* __LCD_THREADS__ */
