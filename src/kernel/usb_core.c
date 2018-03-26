#include "usb_hcdi.h"
#include "memory.h"
#include "semaphore.h"
#include "bcm2835/uart.h"

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

int usb_dev_is_root ( struct usb_device * dev )
{
    return dev -> parent == 0;
}

struct usb_request * usb_alloc_request ( int data_size )
{
    struct usb_request * req =
        memory_allocate ( sizeof ( struct usb_request ) + data_size );

    if ( ! req )
    {
        return 0;
    }

    req -> dev = 0;
    req -> setup_req = ( const struct usb_setup_req ) { 0 };
    req -> data = req + 1;
    req -> size = data_size;

    return req;
}

void usb_free_request ( struct usb_request * req )
{
    memory_deallocate ( req );
}

int usb_submit_request ( struct usb_request * req )
{
    hcd_submit_request ( req );

    return 0;
}

void usb_ctrl_msg_callback ( struct usb_request * req )
{
    signal ( req -> priv );
}

int usb_ctrl_msg ( struct usb_device * dev,
        uint8_t recipient, uint8_t type, uint8_t dir,
        uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
        void * data, uint16_t wLength )
{
    struct usb_request * req = usb_alloc_request ( wLength );

    if ( ! req )
    {
        return -1;
    }

    sem_t sem = sem_create ( 0 );
    if ( sem < 0 )
    {
        usb_free_request ( req );
        return -1;
    }

    req -> setup_req.bmRequestType.recipient = recipient;
    req -> setup_req.bmRequestType.type = type;
    req -> setup_req.bmRequestType.dir = dir;

    req -> setup_req.bRequest = bRequest;
    req -> setup_req.wValue = wValue;
    req -> setup_req.wIndex.raw = wIndex;
    req -> setup_req.wLength = wLength;

    req -> data = data;
    req -> dev = dev;

    req -> callback = usb_ctrl_msg_callback;
    req -> priv = ( void * ) ( long ) sem;

    int res = usb_submit_request ( req );
    wait ( sem );

    sem_destroy ( sem );
    usb_free_request ( req );

    return res;
}

int usb_read_device_desc ( struct usb_device * dev, uint16_t maxsize )
{
    return usb_ctrl_msg ( dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_STD, REQ_DIR_IN,
            REQ_GET_DESC, DESC_DEV << 8, 0,
            & ( dev -> dev_desc ), maxsize );
}

int usb_attach_device ( struct usb_device * dev )
{
    dev -> dev_desc.bMaxPacketSize0 = 8;
    int ret = usb_read_device_desc ( dev, 8 );
    if ( ret )
    {
        printu ( "Error on initial device descriptor reading" );
        return -1;
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

    // Attach the root hub
    usb_attach_device ( usb_root );
}
