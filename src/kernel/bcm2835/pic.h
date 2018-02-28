#ifndef _H_BCM2835_PIC
#define _H_BCM2835_PIC

#include <stdint.h>
#include "bcm2835.h"

struct pic
{
    uint32_t basic_pending;
    uint32_t pending1;
    uint32_t pending2;
    uint32_t fiq_ctrl;
    uint32_t enable1;
    uint32_t enable2;
    uint32_t enable_basic;
    uint32_t disable1;
    uint32_t disable2;
    uint32_t disable_basic;
};

#define IRQ_TIMER0  0
#define IRQ_TIMER1  1
#define IRQ_TIMER2  2
#define IRQ_TIMER3  3
#define IRQ_UART    57

void pic_enable_irq ( int irq );
void pic_disable_irq ( int irq );
void pic_disable_all_interrupts ( );
int pic_irq_pending ( int irq );

#endif
