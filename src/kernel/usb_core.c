#include "usb_hcdi.h"

void usb_init ( )
{
    // Request our Host Controller to start up
    hcd_start ( );
}
