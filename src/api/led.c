#include "led.h"
#include "../kernel/bcm2835/gpio.h"

void api_led_on ( )
{
	gpio_output_clear ( GPIO_LED );
}

void api_led_off ( )
{
	gpio_output_set ( GPIO_LED );
}
