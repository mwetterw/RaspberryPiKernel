#include "usb_hub.h"
#include "usb_std_hub.h"
#include "usb_core.h"

#include "memory.h"
#include <string.h>

#include "bcm2835/uart.h"

struct usb_hub
{
    struct usb_device * dev;
    struct usb_hub_desc * hub_desc;

    struct usb_hub_port * ports;
};

struct usb_hub_port
{
    struct usb_hub_port_status status;
    struct usb_device * child;
};

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
        printu ( "The reported Hub desc bLength is out of allowed range" );
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

int usb_hub_probe ( struct usb_device * dev )
{
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
    dev -> hub = memory_allocate ( sizeof ( struct usb_hub ) );
    if ( ! dev -> hub )
    {
        return -1;
    }
    memset ( dev -> hub, 0, sizeof ( struct usb_hub ) );
    dev -> hub -> dev = dev;

    // Read Hub Descriptor
    if ( usb_hub_read_hub_desc ( dev -> hub ) != 0 )
    {
        memory_deallocate ( dev -> hub );
        dev -> hub = 0;
        return -1;
    }

    return USB_STATUS_SUCCESS;
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
