#include "pic.h"
#include "bcm2835.h"
#include "../scheduler.h"

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

static volatile struct pic * pic =
    ( volatile struct pic * ) PIC_BASE;

// Transform absolute IRQ number to 0 or 1 (register offset)
#define irq2reg ( irq >> 5 )

// Transform absolute IRQ number to a relative one (irq % 32)
#define irq2rel ( irq & 31 )

// Tranform relative IRQ number to an absolute one
#define irq2abs(reg, irq_rel) ( irq_rel + ( reg << 5 ) )

// Private cache of currently enabled interrupts
static uint32_t irq_mask [ 2 ];

// List of all interrupt handlers
interrupt_handler_t interrupt_handlers [ IRQ_NUMBER ];

void pic_enable_irq ( int irq )
{
    if ( ! ( irq_mask [ irq2reg ]  & ( 1 << irq2rel ) ) )
    {
        irq_mask [ irq2reg ] |= ( 1 << irq2rel );
        ( & ( pic -> enable1 ) ) [ irq2reg ] = ( 1 << irq2rel );
    }
}

void pic_disable_irq ( int irq )
{
    if ( irq_mask [ irq2reg ] & ( 1 << irq2rel ) )
    {
        irq_mask [ irq2reg ] &= ~( 1 << irq2rel );
        ( & ( pic -> disable1 ) ) [ irq2reg ] = ( 1 << irq2rel );
    }
}

void pic_disable_all_interrupts ( )
{
    irq_mask [ 0 ] = 0;
    irq_mask [ 1 ] = 0;

    pic -> disable_basic = 0xff;
    pic -> disable1 = ~0;
    pic -> disable2 = ~0;
    pic -> fiq_ctrl = 0;
}

void * irq_dispatch ( void * oldSP )
{
    void * newSP = oldSP;

    for ( uint32_t reg = 0 ; reg < 2 ; ++reg )
    {
        uint32_t mask = irq_mask [ reg ];
        while ( mask )
        {
            // Fetch the position of the first set bit
            // This leverages our ARM CPU "clz" instruction
            // This is our IRQ relative number
            uint32_t irq_rel = 31 - __builtin_clz ( mask );

            // Check if interrupt is pending
            if ( ( & ( pic -> pending1 ) ) [ reg ] & ( 1 << irq_rel ) )
            {
                // Convert to absolute IRQ number
                uint32_t irq = irq2abs ( reg, irq_rel );

                // Special case for scheduler -- FIXME :(
                if ( irq == IRQ_TIMER1 )
                {
                    newSP = scheduler_handler ( oldSP );
                }

                // Call specific handler!
                else if ( interrupt_handlers [ irq ] )
                {
                    ( * interrupt_handlers [ irq ] ) ( );
                }
            }

            mask &= ~( 1 << irq_rel );
        }
    }

    return newSP;
}
