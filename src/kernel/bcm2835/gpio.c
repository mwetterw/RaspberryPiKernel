#include "gpio.h"
#include "gpio_regs.h"
#include "bcm2835.h"
#include "../arm.h" // cdelay

#include <stdint.h>

#define gpio_reg(reg) ( ( uint32_t volatile * ) ( GPIO_BASE + reg ) )

#define gpio_r32(reg) ( * gpio_reg ( reg ) )
#define gpio_r32i(reg,offset) gpio_reg ( reg ) [ offset ]
#define gpio_w32(reg,data) ( * gpio_reg ( reg ) = data )
#define gpio_w32i(reg,offset,data) ( gpio_reg ( reg ) [ offset ] = data )

void gpio_configure ( unsigned char gpioPin, unsigned char fsel )
{
    uint32_t gpfselNumber = gpioPin / 10;
    uint32_t currentConfig = gpio_r32i ( GPFSEL0, gpfselNumber );
    uint32_t gpioPinRelativePos = gpioPin - 10 * gpfselNumber;

    // Resets the three FSEL bits of the chosen gpioPin
    currentConfig &= ~( 7 << ( gpioPinRelativePos * 3 ) );

    // Puts new FSEL values
    currentConfig |= fsel << ( gpioPinRelativePos * 3 );

    // Sends new configuration into GPIO
    gpio_w32i ( GPFSEL0, gpfselNumber, currentConfig );
}

void gpio_configure_pull_up_down ( unsigned char gpioPin, unsigned char state )
{
    // Set desired state
    gpio_w32 ( GPPUD, state );

    // Wait 150 cycles (control signal is setting up)
    cdelay ( 150 );

    // Clock the control signal in for the GPIO we want to impact
    gpio_w32i ( GPPUDCLK0, gpioPin >> 5, gpioPin );

    // Wait 150 cycles (required hold time for control signal)
    cdelay ( 150 );

    // Remove control signal
    gpio_w32 ( GPPUD, 0 );

    // Remove the clock
    gpio_w32i ( GPPUDCLK0, gpioPin >> 5, 0 );

}

void gpio_output_set ( unsigned char gpioPin )
{
    gpio_w32i ( GPSET0, gpioPin >> 5, ( 1 << ( gpioPin & 31 ) ) );
}

void gpio_output_clear ( unsigned char gpioPin )
{
    gpio_w32i ( GPCLR0, gpioPin >> 5, ( 1 << ( gpioPin & 31 ) ) );
}
