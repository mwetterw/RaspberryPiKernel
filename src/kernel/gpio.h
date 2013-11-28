#ifndef _H_KERNEL_GPIO
#define _H_KERNEL_GPIO

#include <stdint.h>
#include "arm.h"

#define GPIO_FSEL_INPUT 0
#define GPIO_FSEL_OUTPUT 1
#define GPIO_FSEL_ALT0 4
#define GPIO_FSEL_ALT1 5
#define GPIO_FSEL_ALT2 6
#define GPIO_FSEL_ALT3 7
#define GPIO_FSEL_ALT4 3
#define GPIO_FSEL_ALT5 2

#define GPIO_GPFSEL0 kernel_arm_addr ( 0x20200000 )
#define GPIO_GPFSEL1 kernel_arm_addr ( 0x20200004 )
#define GPIO_GPFSEL2 kernel_arm_addr ( 0x20200008 )
#define GPIO_GPFSEL3 kernel_arm_addr ( 0x2020000c )
#define GPIO_GPFSEL4 kernel_arm_addr ( 0x20200010 )
#define GPIO_GPFSEL5 kernel_arm_addr ( 0x20200014 )

#define GPIO_BASE_ADDR GPIO_GPFSEL0

void kernel_gpio_configure ( unsigned char gpioPin, unsigned char fsel );

void kernel_gpio_set ( unsigned char gpioPin );
void kernel_gpio_clear ( unsigned char gpioPin );

#define GPIO0 0
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO6 6
#define GPIO7 7
#define GPIO8 8
#define GPIO9 9
#define GPIO10 10
#define GPIO11 11
#define GPIO12 12
#define GPIO13 13
#define GPIO14 14
#define GPIO15 15
#define GPIO16 16
#define GPIO17 17
#define GPIO18 18
#define GPIO19 19
#define GPIO20 20
#define GPIO21 21
#define GPIO22 22
#define GPIO23 23
#define GPIO24 24
#define GPIO25 25
#define GPIO26 26
#define GPIO27 27
#define GPIO28 28
#define GPIO29 29
#define GPIO30 30
#define GPIO31 31
#define GPIO32 32
#define GPIO33 33
#define GPIO34 34
#define GPIO35 35
#define GPIO36 36
#define GPIO37 37
#define GPIO38 38
#define GPIO39 39
#define GPIO40 40
#define GPIO41 41
#define GPIO42 42
#define GPIO43 43
#define GPIO44 44
#define GPIO45 45
#define GPIO46 46
#define GPIO47 47
#define GPIO48 48
#define GPIO49 49
#define GPIO50 50
#define GPIO51 51
#define GPIO52 52
#define GPIO53 53

#endif
