#include "usb_hub.h"
#include "usb_std_hub.h"

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
