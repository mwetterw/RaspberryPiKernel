#include "power.h"
#include "mbox.h"

static uint32_t power_mask;

static uint32_t power_get_mask ( )
{
    return mbox_read ( MBOX_CHAN_PM );
}

static void power_set_mask ( uint32_t mask )
{
    mbox_write ( MBOX_CHAN_PM, mask );
}

void power_init ( )
{
    power_mask = 0;
    power_set_mask ( 0 );
}

int power_device ( enum pm_device dev, enum pm_state state )
{
    int bit = ( 1 << dev );
    int cur_state = ( power_mask & bit );

    // No need to do anything, state is already as wanted
    if ( cur_state == state )
    {
        return 0;
    }

    uint32_t requested_mask = power_mask ^ bit;
    power_set_mask ( requested_mask );
    power_mask = power_get_mask ( );

    if ( requested_mask != power_mask )
    {
        return -1;
    }

    return 0;
}
