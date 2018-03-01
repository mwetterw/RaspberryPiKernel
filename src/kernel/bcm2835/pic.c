#include "pic.h"
#include "../scheduler.h"

static volatile struct pic * pic =
    ( volatile struct pic * ) PIC_BASE;

// Transform absolute IRQ number to 0 or 1 (register offset)
#define irq_reg ( irq >> 5 )

// Transform absolute IRQ number to a relative one (irq % 32)
#define irq_rel ( irq & 31 )

// This is our internal private cache of currently enabled interrupts
static uint64_t irq_mask;

void pic_enable_irq ( int irq )
{
    if ( ! ( irq_mask  & ( 1 << irq ) ) )
    {
        irq_mask |= ( 1 << irq );
        ( & ( pic -> enable1 ) ) [ irq_reg ] = ( 1 << irq_rel );
    }
}

void pic_disable_irq ( int irq )
{
    if ( irq_mask & ( 1 << irq ) )
    {
        irq_mask &= ~( 1 << irq );
        ( & ( pic -> disable1 ) ) [ irq_reg ] = ( 1 << irq_rel );
    }
}

void pic_disable_all_interrupts ( )
{
    irq_mask = 0;

    pic -> disable_basic = 0xff;
    pic -> disable1 = ~0;
    pic -> disable2 = ~0;
    pic -> fiq_ctrl = 0;
}

int pic_irq_pending ( int irq )
{
    return ( ( & ( pic -> pending1 ) ) [ irq_reg ] ) & ( 1 << irq_rel );
}

void * irq_dispatch ( void * oldSP )
{
    if ( pic_irq_pending ( IRQ_TIMER1 ) )
    {
        return scheduler_handler ( oldSP );
    }

    return oldSP;
}
