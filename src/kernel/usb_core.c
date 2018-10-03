#include "usb_hcdi.h"
#include "usb_hub.h"
#include "bcm2835/smsc9512.h"
#include "memory.h"
#include "semaphore.h"
#include "arm.h"
#include "bcm2835/uart.h"
#include "../libc/string.h"

#define USB_MAX_DEV 127
#define USB_MAX_DRIVERS 32
static struct usb_device usb_devs [ USB_MAX_DEV ];
static const struct usb_driver * usb_drivers [ USB_MAX_DRIVERS ];

struct usb_device * usb_root;



struct usb_device * usb_alloc_device ( struct usb_device * parent )
{
    uint32_t irqmask = irq_disable ( );
    for ( int i = 0 ; i < USB_MAX_DEV ; ++i )
    {
        if ( ! usb_devs [ i ].used )
        {
            struct usb_device * dev = & usb_devs [ i ];
            memset ( dev, 0, sizeof ( struct usb_device ) );
            dev -> used = 1;
            dev -> parent = parent;

            irq_restore ( irqmask );
            return dev;
        }
    }

    irq_restore ( irqmask );
    return 0;
}

static int
usb_bind_driver ( struct usb_device * dev, const struct usb_driver * driver )
{
    int status = driver -> probe ( dev );
    if ( status == USB_STATUS_SUCCESS )
    {
        dev -> driver = driver;
    }
    return status;
}

static void usb_unbind_driver ( struct usb_device * dev )
{
    if ( dev -> driver )
    {
        dev -> driver -> remove ( dev );
        dev -> driver = 0;
    }
}

void usb_free_device ( struct usb_device * dev )
{
    printuln ( "Free device" );

    // Unbind driver
    usb_unbind_driver ( dev );

    // De-allocate configuration descriptor
    if ( dev -> conf_desc )
    {
        memory_deallocate ( dev -> conf_desc );
    }

    // Release device
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

struct usb_request * usb_alloc_request ( size_t data_size )
{
    size_t size = sizeof ( struct usb_request ) + data_size;
    struct usb_request * req = memory_allocate ( size );

    if ( ! req )
    {
        return 0;
    }

    memset ( req, 0, size );
    req -> data = req + 1;
    req -> size = data_size;

    req -> status = USB_STATUS_UNPROCESSED;

    return req;
}

void usb_free_request ( struct usb_request * req )
{
    memory_deallocate ( req );
}

int usb_submit_request ( struct usb_request * req )
{
    hcd_submit_request ( req );

    return USB_STATUS_SUCCESS;
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
    req -> size = wLength;

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
            REQ_GET_DESC, USB_DESC_DEV << 8, 0,
            & ( dev -> dev_desc ), maxsize );
}

static int usb_set_device_addr ( struct usb_device * dev, uint8_t addr )
{
    int status = usb_ctrl_req ( dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_STD, REQ_DIR_OUT,
            REQ_SET_ADDR, addr, 0, 0, 0 );

    if ( status == USB_STATUS_SUCCESS )
    {
        dev -> addr = addr;
    }

    return status;
}

static int usb_get_conf_desc ( struct usb_device * dev, uint8_t idx,
        void * buf, uint16_t size )
{
    return usb_ctrl_req ( dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_STD, REQ_DIR_IN,
            REQ_GET_DESC, USB_DESC_CONF << 8 | idx, 0,
            buf, size );
}

static int usb_read_conf_desc ( struct usb_device * dev, uint8_t idx )
{
    struct usb_conf_desc conf;
    int status;

    struct usb_desc_hdr * hdr;
    struct usb_intf_desc * intf;
    struct usb_endp_desc * endp;

    int intf_idx;
    int endp_idx;

    /* First, fetch only the configuration desc without intf & endp.
     * That way we'll know the wTotalLength to allocate */
    status = usb_get_conf_desc ( dev, idx, &conf, sizeof ( conf ) );
    if ( status != USB_STATUS_SUCCESS )
    {
        return -1;
    }

    // Allocate the configuration descriptor
    dev -> conf_desc = memory_allocate ( conf.wTotalLength );
    if ( ! dev -> conf_desc )
    {
        printuln ( "Error when allocating memory for configuration desc" );
        return -1;
    }

    // Get the whole configuration desc with all intfs & endps
    status = usb_get_conf_desc ( dev, idx, dev -> conf_desc, conf.wTotalLength );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when getting whole configuration desc" );
    }

    // Set the interface and endpoint pointers
    intf_idx = -1;
    endp_idx = -1;
    for ( uint32_t i = 0 ;
            i < dev -> conf_desc -> wTotalLength ; i += hdr -> bLength )
    {
        hdr = ( struct usb_desc_hdr * ) ( ( uint8_t * ) ( dev -> conf_desc ) + i );

        if ( hdr -> bLength < sizeof ( struct usb_desc_hdr ) )
        {
            printuln ( "Invalid bLength in configuration descriptor header" );
            return -1;
        }

        switch ( hdr -> bDescriptorType )
        {
            case USB_DESC_INTF:
                intf = ( struct usb_intf_desc * ) hdr;

                // TODO: Handle alternate settings
                if ( intf -> bAlternateSetting != 0 )
                {
                    printuln ( "Skipping alternate settings intf..." );
                    break;
                }

                if ( ++intf_idx >= USB_MAX_INTF )
                {
                    printuln ( "Too many interfaces" );
                    return -1;
                }
                if ( intf_idx >= dev -> conf_desc -> bNumInterfaces )
                {
                    printuln ( "bNumInterfaces mismatch" );
                    return -1;
                }

                dev -> intf_desc [ intf_idx ] = intf;
                endp_idx = -1;
                break;

            case USB_DESC_ENDP:
                if ( intf_idx < 0 )
                {
                    printuln ( "Endpoint belonging to no Interface" );
                    return -1;
                }

                // TODO: Handle alternate settings
                if ( intf -> bAlternateSetting != 0 )
                {
                    printuln ( "Skipping endp of alternate settings intf..." );
                    break;
                }

                endp = ( struct usb_endp_desc * ) hdr;

                if ( ++endp_idx >= USB_MAX_ENDP )
                {
                    printuln ( "Too many endpoints" );
                    return -1;
                }
                if ( endp_idx >= intf -> bNumEndpoints )
                {
                    printuln ( "bNumEnpoints mismatch" );
                    return -1;
                }

                dev -> endp_desc [ intf_idx ] [ endp_idx ] = endp;
                break;

            default:
                break;
        }
    }

    return USB_STATUS_SUCCESS;
}

int usb_set_configuration ( struct usb_device * dev, uint8_t conf )
{
    return usb_ctrl_req ( dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_STD, REQ_DIR_OUT,
            REQ_SET_CONF, conf, 0, 0, 0 );
}

int usb_register_driver ( const struct usb_driver * driver_ )
{
    if ( driver_ -> probe == 0 )
    {
        printuln ( "The probe function must be implemented" );
        return -1;
    }

    uint32_t irqmask = irq_disable ( );
    int first_free = -1;

    for ( int i = 0 ; i < USB_MAX_DRIVERS ; ++i )
    {
        // Find the first free driver slot
        if ( first_free == -1 && usb_drivers [ i ] == 0 )
        {
            first_free = i;
        }

        // Find whether driver not already registered
        if ( usb_drivers [ i ] == driver_ )
        {
            irq_restore ( irqmask );
            return 0;
        }
    }

    if ( first_free < 0 )
    {
        irq_restore ( irqmask );
        return -1;
    }

    usb_drivers [ first_free ] = driver_;

    irq_restore ( irqmask );
    return 0;
}

void usb_unregister_driver ( const struct usb_driver * driver )
{
    uint32_t irqmask = irq_disable ( );

    // Lookup driver
    int driver_idx = -1;
    for ( int i = 0 ; i < USB_MAX_DRIVERS ; ++i )
    {
        if ( usb_drivers [ i ] == driver )
        {
            driver_idx = i;
            break;
        }
    }

    // Driver was not found
    if ( driver_idx == -1 )
    {
        return;
    }

    printuln ( "Unregistering driver" );

    // Unbind driver from devices
    for ( int i = 0 ; i < USB_MAX_DEV ; ++i )
    {
        if ( usb_devs [ i ].used && usb_devs [ i ].driver == driver )
        {
            usb_unbind_driver ( & usb_devs [ i ] );
        }
    }

    // Unregister driver
    usb_drivers [ driver_idx ] = 0;
    irq_restore ( irqmask );
}

int usb_find_driver_for_dev ( struct usb_device * dev )
{
    for ( int i = 0 ; i < USB_MAX_DRIVERS ; ++i )
    {
        const struct usb_driver * driver = usb_drivers [ i ];

        // Skip non allocated drivers
        if ( driver == 0 )
        {
            continue;
        }

        if ( usb_bind_driver ( dev, driver ) == USB_STATUS_SUCCESS )
        {
            return USB_STATUS_SUCCESS;
        }
    }

    return USB_STATUS_NOT_SUPPORTED;
}

int usb_enumerate_device ( struct usb_device * dev )
{
    int status;

    printuln ( "Starting device enumeration" );
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
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error on initial device descriptor reading" );
        return -1;
    }

    // Set device address
    status = usb_set_device_addr ( dev, usb_alloc_addr ( dev ) );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when setting device address" );
        return -1;
    }

    // Re-read the device descriptor only if bMaxPacketSize0 has increased
    if ( dev -> dev_desc.bMaxPacketSize0 > USB_LS_CTRL_DATALEN )
    {
        status = usb_read_device_desc ( dev, sizeof ( struct usb_dev_desc ) );
        if ( status != USB_STATUS_SUCCESS )
        {
            printuln ( "Error on full device descriptor reading" );
            return -1;
        }
    }

    // Read the first configuration descriptor
    status = usb_read_conf_desc ( dev, 0 );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when reading configuration descriptor" );
        return -1;
    }

    // Activate the first configuration
    status = usb_set_configuration ( dev,
            dev -> conf_desc -> bConfigurationValue );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when activating the first configuration" );
        return -1;
    }

    // Try to find a driver for our new attached device!
    status = usb_find_driver_for_dev ( dev );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "No driver found for newly attached device." );
        // This is not an error as drivers can be registered at any time
    }

    return 0;
}

void usb_init ( )
{
    // Register the hub driver
    if ( usb_register_driver ( & usb_hub_driver ) != 0 )
    {
        printuln ( "Unable to register vital USB Hub Driver" );
        return;
    }

    // Register SMSC LAN9512 driver
    if ( usb_register_driver ( & smsc9512_driver ) != 0 )
    {
        printuln ( "Unable to register SMSC LAN9512 USB Driver" );
    }

    // Request our Host Controller to start up
    if ( hcd_start ( ) != 0 )
    {
        printuln ( "USB Core failed to start the HCD" );
        goto err_usb_unregister_hub_driver;
    }

    // Create the root hub
    if ( ! ( usb_root = usb_alloc_device ( 0 ) ) )
    {
        printuln ( "USB Core failed to allocate the root hub" );
        goto err_hcd_stop;
    }

    // Enumerate the root hub
    if ( usb_enumerate_device ( usb_root ) != 0 )
    {
        printuln ( "USB Core failed to enumerate the root hub" );
        goto err_free_root_hub;
    }

    printuln ( "USB Core Initialization complete" );
    return;

err_free_root_hub:
    usb_free_device ( usb_root );
    usb_root = 0;
err_hcd_stop:
    hcd_stop ( );
err_usb_unregister_hub_driver:
    usb_unregister_driver ( & usb_hub_driver );
}
