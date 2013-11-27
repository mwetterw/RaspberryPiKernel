#include "kernel_hardware.h"
#include "kernel_gpio.h"

#define GPIO_LED GPIO16

static void kernel_hardware_led_init ( );

void kernel_hardware_init ( )
{
	kernel_hardware_led_init ( );
}



void kernel_hardware_led_on ( )
{
	kernel_gpio_clear ( GPIO_LED );
}

void kernel_hardware_led_off ( )
{
	kernel_gpio_set ( GPIO_LED );
}



void kernel_hardware_led_init ( )
{
	kernel_gpio_configure ( GPIO_LED, GPIO_FSEL_OUTPUT );
	kernel_gpio_set ( GPIO_LED );
}
