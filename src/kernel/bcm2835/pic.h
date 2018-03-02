#ifndef _H_BCM2835_PIC
#define _H_BCM2835_PIC

#include <stdint.h>

#define IRQ_NUMBER  64

#define IRQ_TIMER0  0
#define IRQ_TIMER1  1
#define IRQ_TIMER2  2
#define IRQ_TIMER3  3
#define IRQ_UART    57

typedef void ( * interrupt_handler_t ) ( void );
extern interrupt_handler_t interrupt_handlers [ IRQ_NUMBER ];

void pic_enable_irq ( int irq );
void pic_disable_irq ( int irq );
void pic_disable_all_interrupts ( );

#endif
