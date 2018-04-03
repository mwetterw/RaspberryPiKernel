#ifndef _H_USB_HCDI
#define _H_USB_HCDI

#include "usb_core.h"

int hcd_start ( );
void hcd_stop ( );

void hcd_submit_request ( struct usb_request * req );

#endif
