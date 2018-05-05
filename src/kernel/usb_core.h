#ifndef _H_USB_CORE
#define _H_USB_CORE

#include "usb_std_device.h"
#include <stddef.h>

#define USB_MAX_INTF 4
#define USB_MAX_ENDP 4

void usb_init ( );

struct usb_request;
typedef void ( * usb_request_callback_t ) ( struct usb_request * req );

struct usb_device
{
    int used;
    struct usb_device * parent;

    struct usb_hub * hub;

    // Device Descriptor
    struct usb_dev_desc dev_desc;

    // Configuration Descriptor
    struct usb_conf_desc * conf_desc;
    struct usb_intf_desc * intf_desc [ USB_MAX_INTF ];
    struct usb_endp_desc * endp_desc [ USB_MAX_INTF ] [ USB_MAX_ENDP ];

    const struct usb_driver * driver;

    uint8_t addr;
};

enum usb_request_status
{
    USB_STATUS_SUCCESS,
    USB_STATUS_UNPROCESSED,
    USB_STATUS_PENDING,
    USB_STATUS_NOT_SUPPORTED,
    USB_STATUS_ERROR,
};

struct usb_request
{
    struct usb_device * dev;

    // Destination endpoint (null for Default Control Pipe)
    struct usb_endp_desc * endp;

    // Used for control requests
    struct usb_setup_req setup_req;

    void * data;
    size_t size;
    size_t xfer_size;

    enum usb_request_status status;

    usb_request_callback_t callback;

    void * priv;
};

struct usb_driver
{
    int ( * probe ) ( struct usb_device * );
    int ( * remove ) ( struct usb_device * );
};

int usb_register_driver ( const struct usb_driver * driver );

int usb_dev_is_root ( struct usb_device * dev );

struct usb_request * usb_alloc_request ( size_t data_size );
void usb_free_request ( struct usb_request * req );

int usb_submit_request ( struct usb_request * req );
void usb_request_done ( struct usb_request * req );

int usb_ctrl_req ( struct usb_device * dev,
        uint8_t recipient, uint8_t type, uint8_t dir,
        uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
        void * data, uint16_t wLength );

#endif
