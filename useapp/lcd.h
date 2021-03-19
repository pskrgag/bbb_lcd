#ifndef __LCD__
#define __LCD__

#include "commands.h"

#include <stdint.h>

void lcd_init(void);
void lcd_print_message(const char *message);
void lcd_clear(void);
void lcd_putchar(char chr);

#endif /* __LCD__ */