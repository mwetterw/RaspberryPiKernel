#include "gpio.h"
#include <stdint.h>

#define GPIO_GPFSEL0 kernel_arm_addr ( 0x20200000 )
#define GPIO_GPFSEL1 kernel_arm_addr ( 0x20200004 )
#define GPIO_GPFSEL2 kernel_arm_addr ( 0x20200008 )
#define GPIO_GPFSEL3 kernel_arm_addr ( 0x2020000c )
#define GPIO_GPFSEL4 kernel_arm_addr ( 0x20200010 )
#define GPIO_GPFSEL5 kernel_arm_addr ( 0x20200014 )

#define GPIO_BASE_ADDR GPIO_GPFSEL0

#define GPSET0 kernel_arm_addr ( 0x2020001C )
#define GPSET1 kernel_arm_addr ( 0x20200020 )
#define GPCLR0 kernel_arm_addr ( 0x20200028 )
#define GPCLR1 kernel_arm_addr ( 0x2020002C )

void kernel_gpio_configure ( unsigned char gpioPin, unsigned char fsel )
{
	uint32_t gpfselNumber = ( gpioPin / 10 );

	uint32_t currentConfig = kernel_arm_array32 ( GPIO_BASE_ADDR, gpfselNumber );

	uint32_t gpioPinRelativePos = gpioPin - 10 * gpfselNumber;

	// Resets the three FSEL bits of the chosen gpioPin
	currentConfig &= ~( 7 << ( gpioPinRelativePos * 3 ) );

	// Puts new FSEL values
	currentConfig |= fsel << ( gpioPinRelativePos * 3 );

	// Sends new configuration into GPIO
	kernel_arm_array32 ( GPIO_BASE_ADDR, gpfselNumber ) = currentConfig;
}

void kernel_gpio_output_set ( unsigned char gpioPin )
{
	uint32_t * gpset = ( gpioPin < 32 ? GPSET0 : GPSET1 );
	kernel_arm_addr32 ( gpset ) = ( 1 << ( gpioPin % 32 ) );
}

void kernel_gpio_output_clear ( unsigned char gpioPin )
{
	uint32_t * gpclr = ( gpioPin < 32 ? GPCLR0 : GPCLR1 );
	kernel_arm_addr32 ( gpclr ) = ( 1 << ( gpioPin % 32 ) );
}
