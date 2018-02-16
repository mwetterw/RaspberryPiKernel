#ifndef _H_KERNEL_BCM2835
#define _H_KERNEL_BCM2835

#define PERI_BASE       0x20000000
#define SYSTIMER_BASE   ( PERI_BASE + 0x3000 )
#define PIC_BASE        ( PERI_BASE + 0xB200 )
#define GPIO_BASE       ( PERI_BASE + 0x200000 )
#define UART_BASE       ( PERI_BASE + 0x201000 )

#endif
