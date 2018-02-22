#include "mbox.h"
#include "bcm2835.h"

static struct mbox_regs volatile * mbox =
    ( struct mbox_regs volatile * ) MBOX_BASE;

void mbox_write ( enum mbox_channel chan, uint32_t data )
{
    while ( mbox [ MBOX_ARM_VC ].status & MBOX_FULL );
    mbox [ MBOX_ARM_VC ].rw = ( data << CHAN_SHIFT ) | ( chan & CHAN_MASK );
}

uint32_t mbox_read ( enum mbox_channel chan )
{
    while ( mbox [ MBOX_VC_ARM ].status & MBOX_EMPTY );

    uint32_t data;

    do
    {
        data = mbox [ MBOX_VC_ARM ].rw;
    } while ( ( data & CHAN_MASK ) != chan );

    return data >> CHAN_SHIFT;
}
