#ifndef _H_KERNEL_BCM2835
#define _H_KERNEL_BCM2835

#define PERI_BASE       0x20000000
#define SYSTIMER_BASE   ( PERI_BASE + 0x3000 )
#define PIC_BASE        ( PERI_BASE + 0xB200 )
#define MBOX_BASE       ( PERI_BASE + 0xB880 )
#define WATCHDOG_BASE   ( PERI_BASE + 0x100000 )
#define GPIO_BASE       ( PERI_BASE + 0x200000 )
#define UART_BASE       ( PERI_BASE + 0x201000 )
#define USB_HCD_BASE    ( PERI_BASE + 0x980000 )


#endif
