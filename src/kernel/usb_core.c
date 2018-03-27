#include "usb_hcdi.h"
#include "memory.h"
#include "semaphore.h"
#include "arm.h"
#include "bcm2835/uart.h"

#define USB_MAX_DEV 32
static struct usb_device usb_devs [ USB_MAX_DEV ];

static struct usb_device * usb_root;



struct usb_device * usb_alloc_device ( struct usb_device * parent )
{
    uint32_t irqmask = irq_disable ( );
    for ( int i = 0 ; i < USB_MAX_DEV ; ++i )
    {
        if ( ! usb_devs [ i ].used )
        {
            struct usb_device * dev = & usb_devs [ i ];
            dev -> used = 1;
            dev -> parent = parent;
            dev -> addr = 0;

            irq_restore ( irqmask );
            return dev;
        }
    }

    irq_restore ( irqmask );
    return 0;
}

void usb_free_device ( struct usb_device * dev )
{
    dev -> used = 0;
}

static uint8_t usb_alloc_addr ( struct usb_device * dev )
{
    return dev - usb_devs + 1;
}

int usb_dev_is_root ( struct usb_device * dev )
{
    return dev -> parent == 0;
}

struct usb_request * usb_alloc_request ( int data_size )
{
    uint32_t irqmask = irq_disable ( );
    struct usb_request * req =
        memory_allocate ( sizeof ( struct usb_request ) + data_size );
    irq_restore ( irqmask );

    if ( ! req )
    {
        return 0;
    }

    req -> dev = 0;
    req -> setup_req = ( const struct usb_setup_req ) { 0 };
    req -> data = req + 1;
    req -> size = data_size;

    req -> status = USB_REQ_STATUS_UNPROCESSED;

    return req;
}

void usb_free_request ( struct usb_request * req )
{
    uint32_t irqmask = irq_disable ( );
    memory_deallocate ( req );
    irq_restore ( irqmask );
}

int usb_submit_request ( struct usb_request * req )
{
    hcd_submit_request ( req );

    return 0;
}

void usb_request_done ( struct usb_request * req )
{
    ( req -> callback ) ( req );
}

static void usb_ctrl_req_callback ( struct usb_request * req )
{
    signal ( req -> priv );
}

int usb_ctrl_req ( struct usb_device * dev,
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

    req -> callback = usb_ctrl_req_callback;
    req -> priv = ( void * ) ( long ) sem;

    usb_submit_request ( req );
    wait ( sem );

    int status = req -> status;

    sem_destroy ( sem );
    usb_free_request ( req );

    return status;
}

static int usb_read_device_desc ( struct usb_device * dev, uint16_t maxsize )
{
    return usb_ctrl_req ( dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_STD, REQ_DIR_IN,
            REQ_GET_DESC, DESC_DEV << 8, 0,
            & ( dev -> dev_desc ), maxsize );
}

static int usb_set_device_addr ( struct usb_device * dev, uint8_t addr )
{
    int status = usb_ctrl_req ( dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_STD, REQ_DIR_OUT,
            REQ_SET_ADDR, addr, 0, 0, 0 );

    if ( status == USB_REQ_STATUS_SUCCESS )
    {
        dev -> addr = addr;
    }

    return status;
}

int usb_attach_device ( struct usb_device * dev )
{
    int status;

    // USB 2.0 Section 5.5.3
    /* In order to determine the maximum packet size for the Default Control
     * Pipe, the USB System Software reads the device descriptor. The host will
     * read the first eight bytes of the device descriptor. The device always
     * responds with at least these initial bytes in a single packet. After the
     * host reads the initial part of the device descriptor, it is guaranteed
     * to have read this default pipe's wMaxPacketSize field (byte 7 of the
     * device descriptor). It will then allow the correct size for all
     * subsequent transactions. */
    dev -> dev_desc.bMaxPacketSize0 = USB_LS_CTRL_DATALEN;
    status = usb_read_device_desc ( dev, USB_LS_CTRL_DATALEN );
    if ( status != USB_REQ_STATUS_SUCCESS )
    {
        printu ( "Error on initial device descriptor reading" );
        return -1;
    }

    // Set device address
    status = usb_set_device_addr ( dev, usb_alloc_addr ( dev ) );
    if ( status != USB_REQ_STATUS_SUCCESS )
    {
        printu ( "Error when setting device address" );
        return -1;
    }

    // Re-read the device descriptor only if bMaxPacketSize0 has increased
    if ( dev -> dev_desc.bMaxPacketSize0 > USB_LS_CTRL_DATALEN )
    {
        status = usb_read_device_desc ( dev, sizeof ( struct usb_dev_desc ) );
        if ( status != USB_REQ_STATUS_SUCCESS )
        {
            printu ( "Error on full device descriptor reading" );
            return -1;
        }
    }

    return 0;
}

void usb_init ( )
{
    for ( int i = 0 ; i < USB_MAX_DEV ; ++i )
    {
        usb_devs [ i ].used = 0;
    }

    // Request our Host Controller to start up
    if ( hcd_start ( ) != 0 )
    {
        printu ( "USB Core failed to start the HCD" );
        return;
    }

    // Create the root hub
    struct usb_device * root_hub = usb_alloc_device ( 0 );

    // Attach the root hub
    if ( usb_attach_device ( root_hub ) != 0 )
    {
        printu ( "USB Core failed to attach the root hub" );
        usb_free_device ( root_hub );
        return;
    }

    usb_root = root_hub;

    printu ( "USB Core Initialization complete" );
}
