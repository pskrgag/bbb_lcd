#include "lcd_threads.h"

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>

struct lcd_thread {
	struct task_struct *thread;
};

static struct lcd_thread current_thread;

int lcd_start_thread(struct lcd_data *lcd, int (*func)(void *))
__must_hold(&lcd->lock)
{
	current_thread.thread = kthread_run(func, lcd, "lcd-thread");
	if (IS_ERR(current_thread.thread))
		return PTR_ERR(current_thread.thread);
	
	return 0;
}

int lcd_stop_thread(void)
{
	if (!current_thread.thread)
		return 0;

	kthread_stop(current_thread.thread);
	current_thread.thread = NULL;

	return 1;
}
