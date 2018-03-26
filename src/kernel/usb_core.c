#include "usb_hcdi.h"

void usb_init ( )
{
    // Request our Host Controller to start up
    hcd_start ( );
}

int usb_dev_is_root ( struct usb_device * dev )
{
    return dev -> parent == 0;
}
