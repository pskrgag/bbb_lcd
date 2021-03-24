#ifndef __GPIO__
#define __GPIO__

static inline void gpio_pulse_e_pin(struct lcd_data *lcd)
__must_hold(&lcd->lock)
{
	gpiod_set_value_cansleep(lcd->gpio_e, 0x0);
	udelay(1);

	gpiod_set_value_cansleep(lcd->gpio_e, 0x1);
	udelay(1);

	gpiod_set_value_cansleep(lcd->gpio_e, 0x0);
	udelay(50);
}

static inline void gpio_set_4bit(struct lcd_data *lcd, uint8_t val)
__must_hold(&lcd->lock)
{
	gpiod_set_value_cansleep(lcd->gpio_dt1, (val >> 0) & 0x1);
	gpiod_set_value_cansleep(lcd->gpio_dt2, (val >> 1) & 0x1);
	gpiod_set_value_cansleep(lcd->gpio_dt3, (val >> 2) & 0x1);
	gpiod_set_value_cansleep(lcd->gpio_dt4, (val >> 3) & 0x1);

	gpio_pulse_e_pin(lcd);
}

static inline void gpio_set_8bit(struct lcd_data *lcd, uint8_t val)
__must_hold(&lcd->lock)
{
	gpio_set_4bit(lcd, val >> 4);
	gpio_set_4bit(lcd, val & 0xf);
}

static inline void gpio_set_to_output(struct lcd_data *lcd)
__must_hold(&lcd->lock)
{
	gpiod_direction_output(lcd->gpio_dt1, 0);
	gpiod_direction_output(lcd->gpio_dt2, 0);
	gpiod_direction_output(lcd->gpio_dt3, 0);
	gpiod_direction_output(lcd->gpio_dt4, 0);
}

static inline void gpio_set_to_input(struct lcd_data *lcd)
__must_hold(&lcd->lock)
{
	gpiod_direction_input(lcd->gpio_dt1);
	gpiod_direction_input(lcd->gpio_dt2);
	gpiod_direction_input(lcd->gpio_dt3);
	gpiod_direction_input(lcd->gpio_dt4);
}

#endif /* __GPIO__ */
