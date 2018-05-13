#include "usb_hub.h"
#include "usb_std_hub.h"
#include "usb_core.h"

#include "arm.h"
#include "semaphore.h"
#include "../api/process.h"

#include "memory.h"
#include <string.h>

#include "../libc/math.h"

#include "bcm2835/uart.h"

static sem_t usb_hub_sem;

extern struct usb_device * usb_root;

#define USB_HUB_RST_TIMEOUT 800
#define USB_HUB_RST_DELAY 10

struct usb_hub
{
    struct usb_device * dev;
    struct usb_hub_desc * hub_desc;

    struct usb_request * status_changed_req;

    struct usb_hub_port * ports;


    uint8_t changed_size;
    uint8_t * changed;
};

struct usb_hub_port
{
    struct usb_hub_port_status status;
    struct usb_device * child;
};

static int usb_hub_allocate ( struct usb_device * dev )
{
    struct usb_hub * hub;

    hub = memory_allocate ( sizeof ( struct usb_hub ) );
    if ( ! hub )
    {
        return -1;
    }
    memset ( hub, 0, sizeof ( struct usb_hub ) );
    hub -> dev = dev;

    dev -> hub = hub;

    return 0;
}

static void usb_hub_free ( struct usb_hub * hub )
{
    if ( hub -> ports )
    {
        memory_deallocate ( hub -> ports );
    }

    if ( hub -> status_changed_req )
    {
        usb_free_request ( hub -> status_changed_req );
    }

    if ( hub -> hub_desc )
    {
        memory_deallocate ( hub -> hub_desc );
    }

    if ( hub -> changed )
    {
        memory_deallocate ( hub -> changed );
    }

    hub -> dev -> hub = 0;
    memory_deallocate ( hub );
}

static int usb_hub_get_hub_desc ( struct usb_hub * hub, void * buf, uint16_t size )
{
    return usb_ctrl_req ( hub -> dev,
            REQ_RECIPIENT_DEV, REQ_TYPE_CLASS, REQ_DIR_IN,
            HUB_REQ_GET_DESC, USB_HUB_DESC << 8, 0,
            buf, size );
}

static int usb_hub_read_hub_desc ( struct usb_hub * hub )
{
    struct usb_desc_hdr hdr;
    struct usb_hub_desc * hub_desc;
    int status;

    // Fetch the hub descriptor header to get its size
    status = usb_hub_get_hub_desc ( hub, &hdr, sizeof ( hdr ) );
    if ( status != USB_STATUS_SUCCESS )
    {
        return -1;
    }

    // Check reported bLength before memory allocation
    if ( hdr.bLength < USB_HUB_DESC_MIN_BLENGTH ||
            hdr.bLength > USB_HUB_DESC_MAX_BLENGTH )
    {
        printuln ( "The reported Hub desc bLength is out of allowed range" );
        return -1;
    }

    if ( ! ( hub_desc = memory_allocate ( hdr.bLength ) ) )
    {
        return -1;
    }
    hub -> hub_desc = hub_desc;

    // Fetch the full hub descriptor
    status = usb_hub_get_hub_desc ( hub, hub -> hub_desc, hdr.bLength );
    if ( status != USB_STATUS_SUCCESS )
    {
        memory_deallocate ( hub -> hub_desc );
        hub -> hub_desc = 0;
        return -1;
    }

    return 0;
}

static int
usb_hub_read_port_status ( struct usb_hub * hub, uint16_t port )
{
    return usb_ctrl_req ( hub -> dev,
        REQ_RECIPIENT_OTHER, REQ_TYPE_CLASS, REQ_DIR_IN,
        HUB_REQ_GET_STATUS, 0, port,
        & ( hub -> ports [ port ].status ), sizeof ( struct usb_hub_port_status ) );
}

static int
usb_hub_port_feature ( struct usb_hub * hub, uint8_t port, uint16_t feature, int set )
{
    enum usb_hub_bRequest req_code;
    req_code = ( set ) ? HUB_REQ_SET_FEATURE : HUB_REQ_CLEAR_FEATURE;

    return usb_ctrl_req ( hub -> dev,
        REQ_RECIPIENT_OTHER, REQ_TYPE_CLASS, REQ_DIR_OUT,
        req_code, feature, port,
        0, 0 );
}

static int usb_hub_port_reset ( struct usb_hub * hub, uint16_t port )
{
    int status;

    // Ask the port to reset
    status = usb_hub_port_feature ( hub, port, HUB_FEATURE_PORT_RESET, 1 );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when sending the request to reset the port" );
        return status;
    }

    // Wait until hub reports the reset is complete
    int delay = 0;
    for ( ; hub -> ports [ port ].status.reset ; delay += USB_HUB_RST_DELAY )
    {
        printuln ( "USB Hub Port Reset wait" );
        cdelay ( USB_HUB_RST_DELAY * 1000 * 7 );

        // Refresh the port status
        status = usb_hub_read_port_status ( hub, port );
        if ( status != USB_STATUS_SUCCESS )
        {
            printuln ( "Error when retrieving USB Hub Port Status during reset" );
            return status;
        }

        if ( delay >= USB_HUB_RST_TIMEOUT )
        {
            printuln ( "Hub port reset timed out" );
            return USB_STATUS_TIMEOUT;
        }
    }

    // Allow for the recovery interval
    // The USB spec requires 10ms at least. Here, we wait 3 times this interval.
    cdelay ( 3 * USB_HUB_RST_RECOVERY_INTERVAL * 1000 * 7 );
    return USB_STATUS_SUCCESS;
}

static void usb_hub_port_attach ( struct usb_hub * hub, uint16_t port )
{
    int status;
    struct usb_device * new_dev;

    printuln ( "New device plugged in. Resetting the port..." );
    status = usb_hub_port_reset ( hub, port );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when trying to reset the hub port" );
        return;
    }

    // Refresh the port status to get the attached device speed
    status = usb_hub_read_port_status ( hub, port );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error upon post-reset USB Hub Port Status retrieval" );
        usb_hub_port_feature ( hub, port, HUB_FEATURE_PORT_ENABLE, 0 );
        return;
    }

    if ( hub -> ports [ port ].status.ls_dev )
    {
        printuln ( "Newly attached device is Low-Speed" );
    }
    else if ( hub -> ports [ port ].status.hs_dev )
    {
        printuln ( "Newly attached device is High-Speed" );
    }
    else
    {
        printuln ( "Newly attached device is Full-Speed" );
    }

    // Allocate new device
    if ( ! ( new_dev = usb_alloc_device ( hub -> dev ) ) )
    {
        printuln ( "Error when trying to allocate device for new dev" );
        usb_hub_port_feature ( hub, port, HUB_FEATURE_PORT_ENABLE, 0 );
        return;
    }

    hub -> ports [ port ].child = new_dev;

    // Ask the USB Core to attach the device !
    if ( usb_attach_device ( new_dev ) != 0 )
    {
        printuln ( "Error during attachment of new device" );
        hub -> ports [ port ].child = 0;
        usb_free_device ( new_dev );
        usb_hub_port_feature ( hub, port, HUB_FEATURE_PORT_ENABLE, 0 );
    }
}

static void usb_hub_port_changed ( struct usb_hub * hub, uint16_t port )
{
    int status;

    printu ( "Processing Hub #");
    printu_32h ( hub -> dev -> addr );
    printu ( " Port #" );
    printu_32h ( port );
    printu ( " change");
    printuln ( 0 );

    status = usb_hub_read_port_status ( hub, port );
    if ( status != USB_STATUS_SUCCESS )
    {
        printuln ( "Error when retrieving USB Hub Port Status" );
        return;
    }

    // Connection changed
    if ( hub -> ports [ port ].status.c_connection )
    {
        printuln ( "Port connection changed" );
        // Acknowledge the event
        usb_hub_port_feature ( hub, port, HUB_FEATURE_C_PORT_CONNECTION, 0 );

        // A new USB device has been detected. Let's attach it!
        if ( hub -> ports [ port ].status.connection )
        {
            usb_hub_port_attach ( hub, port );
        }
    }

    // Reset changed
    if ( hub -> ports [ port ].status.c_reset )
    {
        printuln ( "Port reset changed" );
        // Acknowledge the event
        usb_hub_port_feature ( hub, port, HUB_FEATURE_C_PORT_RESET, 0 );
    }

    // Enable changed (spec says only when enable -> disable)
    if ( hub -> ports [ port ].status.c_enable )
    {
        printuln ( "Port enable changed" );
        // Acknowledge the event
        usb_hub_port_feature ( hub, port, HUB_FEATURE_C_PORT_ENABLE, 0 );
    }

    // Over-current changed
    if ( hub -> ports [ port ].status.c_over_current )
    {
        printuln ( "Port overcurrent changed" );
    }

    // Suspend changed
    if ( hub -> ports [ port ].status.c_suspend )
    {
        printuln ( "Port suspend changed" );
    }
}

static void usb_hub_hub_changed ( struct usb_hub * hub )
{
    ( void ) hub;
    printuln ( "Processing Hub Change..." );
}

static void usb_hub_status_changed_worker ( )
{
    void f ( struct usb_device * dev )
    {
        struct usb_hub * hub;
        uint8_t status_byte;
        uint16_t port;
        size_t s;

        int hub_changed = 0;

        // Skip non-hub device
        if ( ! ( hub = dev -> hub ) )
        {
            return;
        }

        // Process each status byte
        for ( s = 0 ; s < hub -> changed_size; ++s )
        {
            status_byte = ( hub -> changed ) [ s ];
            if ( status_byte )
            {
                hub_changed = 1;
            }

            // Process bits within the byte
            while ( status_byte )
            {
                // Get the port number and process the change
                port = 31 - __builtin_clz ( status_byte );
                status_byte ^= ( 1 << port );
                port += s * 8;

                if ( port )
                {
                    usb_hub_port_changed ( hub, port );
                }

                // Port "0" represents the whole hub
                else
                {
                    usb_hub_hub_changed ( hub );
                }
            }
        }

        // Re-submit USB Hub IRQ request
        if ( hub_changed )
        {
            usb_submit_request ( hub -> status_changed_req );
        }
    }

    for ( ; ; )
    {
        wait ( usb_hub_sem );
        usb_foreach ( usb_root, f );
    }
}

void usb_hub_status_changed_request_done ( struct usb_request * req )
{
    if ( req -> status != USB_STATUS_SUCCESS )
    {
        printuln ( "USB Hub Status Changed request failed" );
        return;
    }

    struct usb_hub * hub = req -> dev -> hub;

    size_t size = min ( req -> xfer_size, hub -> changed_size );
    memcpy ( hub -> changed, req -> data, size );

    if ( req -> xfer_size < hub -> changed_size )
    {
        memset ( hub -> changed + req -> xfer_size, 0,
                    hub -> changed_size - req -> xfer_size );
    }

    signal ( usb_hub_sem );
}

static int usb_hub_driver_init ( )
{
    sem_t sem = sem_create ( 0 );
    if ( sem < 0 )
    {
        return -1;
    }

    usb_hub_sem = sem;
    api_process_create ( usb_hub_status_changed_worker, 0 );

    return 0;
}

void usb_foreach ( struct usb_device * dev, usb_foreach_func_t f )
{
    if ( ! dev )
    {
        return;
    }

    f ( dev );

    // Leaf. Don't stack up and stop there
    if ( ! dev -> hub )
    {
        return;
    }

    // This is a hub (root or middle node). Let's stack up!
    for ( int i = 1 ; i <= dev -> hub -> hub_desc -> bNbrPorts ; ++i )
    {
        if ( dev -> hub -> ports [ i ].child )
        {
            usb_foreach ( dev -> hub -> ports [ i ].child, f );
        }
    }
}

int usb_hub_probe ( struct usb_device * dev )
{
    if ( ! usb_hub_sem )
    {
        if ( usb_hub_driver_init ( ) != 0 )
        {
            return -1;
        }
    }

    // Device Descriptor Check
    struct usb_dev_desc * dev_desc = & ( dev -> dev_desc );
    if ( dev_desc -> bDeviceClass != USB_CLASS_HUB ||
            dev_desc -> bDeviceSubClass != 0 ||
            dev_desc -> bDeviceProtocol > USB_HUB_DEV_PROTO_HS_MTT )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    // Configuration Descriptor Check
    struct usb_conf_desc * conf_desc = dev -> conf_desc;
    if ( conf_desc -> bNumInterfaces != 1 )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    // Interface Descriptor Check
    // TODO: Support bAlternateSetting for Multi-TT
    struct usb_intf_desc * intf_desc = dev -> intf_desc [ 0 ];
    if ( intf_desc -> bNumEndpoints != 1 ||
            intf_desc -> bInterfaceClass != USB_CLASS_HUB ||
            intf_desc -> bInterfaceSubClass != 0 ||
            intf_desc -> bInterfaceProtocol >
                USB_HUB_INTF_PROTO_HS_MTT_CAPABLE_MTT )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    // Endpoint Descriptor Check
    struct usb_endp_desc * endp_desc = dev -> endp_desc [ 0 ] [ 0 ];
    if ( endp_desc -> bEndpointAddress.dir != REQ_DIR_IN ||
            endp_desc -> bmAttributes.transfer != ENDP_XFER_INTERRUPT )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    // Allocate a hub
    if ( usb_hub_allocate ( dev ) != 0 )
    {
        return -1;
    }

    // Read Hub Descriptor
    printuln ( "Reading Hub Descriptor" );
    if ( usb_hub_read_hub_desc ( dev -> hub ) != 0 )
    {
        goto err_free_hub;
    }

    uint8_t nbports = dev -> hub -> hub_desc -> bNbrPorts;
    if ( nbports < 1 )
    {
        printuln ( "Hub reports having no downstream port" );
        goto err_free_hub;
    }

    // Allocate changed
    dev -> hub -> changed_size = usb_hub_desc_tail_field_size ( nbports );
    dev -> hub -> changed = memory_allocate ( dev -> hub -> changed_size );
    if ( ! dev -> hub -> changed )
    {
        goto err_free_hub;
    }
    memset ( dev -> hub -> changed, 0, dev -> hub -> changed_size );

    // Allocate Interrupt IN Status Changed request
    dev -> hub -> status_changed_req =
        usb_alloc_request ( usb_hub_desc_tail_field_size ( nbports ) );
    if ( ! dev -> hub -> status_changed_req )
    {
        goto err_free_hub;
    }

    // Allocate ports
    dev -> hub -> ports =
        memory_allocate ( ( nbports + 1 ) * sizeof ( struct usb_hub_port ) );
    if ( ! dev -> hub -> ports )
    {
        goto err_free_hub;
    }
    memset ( dev -> hub -> ports, 0, ( nbports + 1 ) * sizeof ( struct usb_hub_port ) );

    for ( uint8_t port = 1 ; port <= nbports ; ++port )
    {
        int status =
            usb_hub_port_feature ( dev -> hub, port, HUB_FEATURE_PORT_POWER, 1 );
        if ( status != USB_STATUS_SUCCESS )
        {
            printuln ( "Ignoring port power-on failure" );
        }
    }

    // Send USB Interrupt IN Request to USB Core
    struct usb_request * req = dev -> hub -> status_changed_req;
    req -> dev = dev;
    req -> endp = dev -> endp_desc [ 0 ] [ 0 ];
    req -> callback = usb_hub_status_changed_request_done;

    if ( usb_submit_request ( req ) != USB_STATUS_SUCCESS )
    {
        return USB_STATUS_ERROR;
    }

    return USB_STATUS_SUCCESS;

err_free_hub:
    usb_hub_free ( dev -> hub );
    return -1;
}

int usb_hub_remove ( struct usb_device * dev )
{
    ( void ) dev;
    return USB_STATUS_SUCCESS;
}

const struct usb_driver usb_hub_driver =
{
    .probe = usb_hub_probe,
    .remove = usb_hub_remove,
};
