#ifndef _H_USB_CORE
#define _H_USB_CORE

#include "usb_std_device.h"

void usb_init ( );

struct usb_device
{
    int used;
    struct usb_device * parent;
};

int usb_dev_is_root ( struct usb_device * dev );


enum usb_request_status
{
    USB_REQ_STATUS_PENDING,
    USB_REQ_STATUS_SUCCESS,
};

struct usb_request
{
    struct usb_device * dev;

    // Used for control requests
    struct usb_setup_req setup_req;

    void * data;
    int size;

    enum usb_request_status status;
};

#endif
