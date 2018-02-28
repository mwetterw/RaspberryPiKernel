#include "pic.h"

static volatile struct pic * pic =
    ( volatile struct pic * ) PIC_BASE;

void pic_enable_irq ( int irq )
{
    ( & ( pic -> enable1 ) ) [ irq >> 5 ] = ( 1 << ( irq & 31 ) );
}

void pic_disable_irq ( int irq )
{
    ( & ( pic -> disable1 ) ) [ irq >> 5 ] = ( 1 << ( irq & 31 ) );
}

void pic_disable_all_interrupts ( )
{
    pic -> disable_basic = 0xff;
    pic -> disable1 = ~0;
    pic -> disable2 = ~0;
    pic -> fiq_ctrl = 0;
}

int pic_irq_pending ( int irq )
{
    return ( ( & ( pic -> pending1 ) ) [ irq >> 5 ] ) & ( 1 << ( irq & 31 ) );
}
