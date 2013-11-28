#include "led.h"
#include "../kernel/hardware.h"


void api_led_on ( )
{
	kernel_hardware_led_on ( );
}

void api_led_off ( )
{
	kernel_hardware_led_off ( );
}
