#include "usb_hub.h"
#include "usb_std_hub.h"

int usb_hub_probe ( struct usb_device * dev )
{
    ( void ) dev;
    return USB_REQ_STATUS_SUCCESS;
}

int usb_hub_remove ( struct usb_device * dev )
{
    ( void ) dev;
    return USB_REQ_STATUS_SUCCESS;
}

const struct usb_driver usb_hub_driver =
{
    .probe = usb_hub_probe,
    .remove = usb_hub_remove,
};
