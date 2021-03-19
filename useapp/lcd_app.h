#ifndef __LCD_APP__
#define __LCD_APP__

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"

#define ERR				"Error: "
#define PROMPT				printf("~$ ");
#define MAX_CMD				1000
#define ERROR(fmt, ...)			printf(ERR fmt "\n", (__VA_ARGS__))
#define FGETS_STRCMP(str1, str2)	strcmp(str1, str2 "\n")

static void help(void)
{
	printf("Availbale commands: \n"
	       "\thelp        -- prints help\n"
	       "\texit        -- exits from this\n"
	       "\twrite <msg> -- writes message to LCD\n"
	       "\tclear       -- clears LCD display\n"
	       );
}

static void __exit(void)
{
	printf("Bye :(\n");
	exit(0);
}

static void __write(const char *msg)
{
#define __NO_MSG		0
#define __MSG_QUOTES		1

	int i, message_state = __NO_MSG;

	for (i = 0; msg[i] != '\n'; ++i) {
		if (msg[i] == ' ' && message_state == __NO_MSG)
			continue;

		if (msg[i] == '\"') {
			switch (message_state)
			{
			case __NO_MSG:
				message_state = __MSG_QUOTES;
				continue;
			case __MSG_QUOTES:
			default:
				return;
			}
		}

		lcd_putchar(msg[i]);
	}
}

#endif /* __LCD_APP__ */
