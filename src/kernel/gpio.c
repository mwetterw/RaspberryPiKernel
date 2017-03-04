#include "gpio.h"
#include "gpio_regs.h"

#include <stdint.h>

#define GPIO_BASE 0x20200000

static inline void gpio_w32 ( int reg, int data )
{
    iowrite ( GPIO_BASE, reg, data );
}

static inline void gpio_w32i ( int reg, int offset, int data )
{
    iowritei ( GPIO_BASE, reg, offset, data );
}

void kernel_gpio_configure ( unsigned char gpioPin, unsigned char fsel )
{
    uint32_t gpfselNumber = ( gpioPin / 10 );
    uint32_t currentConfig = ioreadi ( GPFSEL0, gpfselNumber );
    uint32_t gpioPinRelativePos = gpioPin - 10 * gpfselNumber;

    // Resets the three FSEL bits of the chosen gpioPin
    currentConfig &= ~( 7 << ( gpioPinRelativePos * 3 ) );

    // Puts new FSEL values
    currentConfig |= fsel << ( gpioPinRelativePos * 3 );

    // Sends new configuration into GPIO
    gpio_w32i ( GPFSEL0, gpfselNumber, currentConfig );
}

void kernel_gpio_configure_pull_up_down ( unsigned char gpioPins, unsigned char state )
{
    // Set desired state
    gpio_w32 ( GPPUD, state );

    // Wait 150 cycles (control signal is setting up)
    cdelay ( 150 );

    // Clock the control signal in for the GPIOS we want to impact
    gpio_w32 ( GPPUDCLK0, gpioPins );

    // Wait 150 cycles (required hold time for control signal)
    cdelay ( 150 );

    // Remove control signal
    gpio_w32 ( GPPUD, 0 );

    // Remove the clock
    gpio_w32 ( GPPUDCLK0, 0 );

}

void kernel_gpio_output_set ( unsigned char gpioPin )
{
    uint32_t gpset = ( gpioPin < 32 ? GPSET0 : GPSET1 );
    gpio_w32 ( gpset, ( 1 << ( gpioPin % 32 ) ) );
}

void kernel_gpio_output_clear ( unsigned char gpioPin )
{
    uint32_t gpclr = ( gpioPin < 32 ? GPCLR0 : GPCLR1 );
    gpio_w32 ( gpclr, ( 1 << ( gpioPin % 32 ) ) );
}
