#ifndef _H_USB_CORE
#define _H_USB_CORE

#include "usb_std_device.h"

#define USB_MAX_INTF 4
#define USB_MAX_ENDP 4

void usb_init ( );

struct usb_request;
typedef void ( * usb_request_callback_t ) ( struct usb_request * req );

struct usb_device
{
    int used;
    struct usb_device * parent;

    // Device Descriptor
    struct usb_dev_desc dev_desc;

    // Configuration Descriptor
    struct usb_conf_desc * conf_desc;
    struct usb_intf_desc * intf_desc [ USB_MAX_INTF ];
    struct usb_endp_desc * endp_desc [ USB_MAX_INTF ] [ USB_MAX_ENDP ];

    uint8_t addr;
};

enum usb_request_status
{
    USB_REQ_STATUS_UNPROCESSED,
    USB_REQ_STATUS_PENDING,
    USB_REQ_STATUS_NOT_SUPPORTED,
    USB_REQ_STATUS_ERROR,
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

    usb_request_callback_t callback;

    void * priv;
};

struct usb_driver
{
    int used;

    int ( * probe ) ( struct usb_device * );
    int ( * remove ) ( struct usb_device * );
};

int usb_register_driver ( const struct usb_driver * driver );

int usb_dev_is_root ( struct usb_device * dev );

struct usb_request * usb_alloc_request ( int data_size );
int usb_submit_request ( struct usb_request * req );

void usb_request_done ( struct usb_request * req );

#endif
