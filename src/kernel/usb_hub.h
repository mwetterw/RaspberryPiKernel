#ifndef _H_USB_HUB
#define _H_USB_HUB

#include "usb_core.h"

extern const struct usb_driver usb_hub_driver;

typedef void ( * usb_foreach_func_t ) ( struct usb_device * );

void usb_foreach ( struct usb_device * dev, usb_foreach_func_t );

#endif
