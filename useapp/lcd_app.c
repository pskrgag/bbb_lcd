#include <string.h>
#include <errno.h>

#include "lcd_app.h"

void __attribute__((noreturn)) console(void)
{
	char command[MAX_CMD] = {0};

	while (1) {
		PROMPT;

		fgets(command, MAX_CMD, stdin);

		if (command[0] == '\n')
			continue;
		else if (!FGETS_STRCMP(command, "help"))
			help();
		else if (!FGETS_STRCMP(command, "exit"))
			__exit();
		else if (!strncmp(command, "write ", 6))
			__write(command + 6);
		else if (!FGETS_STRCMP(command, "clear"))
			lcd_clear();
		else {
			command[strcspn(command, "\n")] = 0x00;
			ERROR("Command %s not found", command);
		}

		memset(command, 0, MAX_CMD);
	}
}

int main(void)
{
	lcd_init();

	printf("Welcome to LCD console!\n"
	       "If you have any questions about it just type \"help\"\n");

	console();
}