#include "hardware.h"
#include "bcm2835/gpio.h"
#include "bcm2835/uart.h"
#include "bcm2835/power.h"

static inline void kernel_hardware_led_init ( );

void kernel_hardware_init ( )
{
    power_init ( );
    kernel_hardware_led_init ( );
    uart_init ( );

    printu ( "Welcome!" );
    printu ( "Hardware initialization complete" );
}

void kernel_hardware_led_init ( )
{
    kernel_gpio_configure ( GPIO_LED, GPIO_FSEL_OUTPUT );
    kernel_gpio_output_set ( GPIO_LED );
}
