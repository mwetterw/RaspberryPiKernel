#include "led.h"
#include "../kernel/gpio.h"

void api_led_on ( )
{
	kernel_gpio_output_clear ( GPIO_LED );
}

void api_led_off ( )
{
	kernel_gpio_output_set ( GPIO_LED );
}
