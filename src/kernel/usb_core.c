#include "usb_hcdi.h"

#define USB_MAX_DEV 32

static struct usb_device usb_devs [ USB_MAX_DEV ];

static struct usb_device * usb_root;

struct usb_device * usb_alloc_device ( struct usb_device * parent )
{
    for ( int i = 0 ; i < USB_MAX_DEV ; ++i )
    {
        if ( ! usb_devs [ i ].used )
        {
            struct usb_device * dev = & usb_devs [ i ];
            dev -> used = 1;
            dev -> parent = parent;

            return dev;
        }
    }

    return 0;
}

void usb_init ( )
{
    // Request our Host Controller to start up
    hcd_start ( );

    // Create the root hub
    struct usb_device * root_hub = usb_alloc_device ( 0 );
    usb_root = root_hub;
}

int usb_dev_is_root ( struct usb_device * dev )
{
    return dev -> parent == 0;
}
