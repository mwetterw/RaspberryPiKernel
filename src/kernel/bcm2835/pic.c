#include "pic.h"

static volatile struct pic * pic =
    ( volatile struct pic * ) PIC_BASE;

void pic_enable_irq ( int irq )
{
    ( & ( pic -> enable1 ) ) [ irq >> 5 ] = ( 1 << ( irq & 31 ) );
}
