#include "led.h"
#include "../kernel/hardware.h"
#include "../kernel/arm.h"


void api_led_on ( )
{
	kernel_arm_disable_irq ( );
	kernel_hardware_led_on ( );
	kernel_arm_enable_irq ( );
}

void api_led_off ( )
{
	kernel_arm_disable_irq ( );
	kernel_hardware_led_off ( );
	kernel_arm_enable_irq ( );
}
