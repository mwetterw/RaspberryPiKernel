#include "pic.h"
#include "bcm2835.h"

static volatile struct pic * pic =
    ( volatile struct pic * ) PIC_BASE;

void pic_enable_irq ( int irq )
{
    pic -> enable1 = ( 1 << irq );
}
