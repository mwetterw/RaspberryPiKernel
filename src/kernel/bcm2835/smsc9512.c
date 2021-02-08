#include "../usb_core.h"
#include "../../api/process.h"
#include "uart.h"

#include "smsc9512_regs.h"

#define SMSC9512_PID 0xEC00

static struct usb_device * usb_dev;

extern void smsc9512_led_process ( );

uint32_t smsc9512_read_reg ( uint16_t addr )
{
    uint32_t value;

    usb_ctrl_req ( usb_dev,
        REQ_RECIPIENT_DEV, REQ_TYPE_VENDOR, REQ_DIR_IN,
        0xA1, 0, addr,
        &value, 4 );

    return value;
}

uint32_t smsc9512_write_reg ( uint16_t addr, uint32_t val )
{
    return usb_ctrl_req ( usb_dev,
        REQ_RECIPIENT_DEV, REQ_TYPE_VENDOR, REQ_DIR_OUT,
        0xA0, 0, addr,
        &val, 4 );
}

int smsc9512_probe ( struct usb_device * dev )
{
    // Device Descriptor Check
    struct usb_dev_desc * dev_desc = & ( dev -> dev_desc );
    if ( dev_desc -> bDeviceClass != USB_CLASS_VENDOR_SPECIFIC ||
            dev_desc -> bDeviceSubClass != 0 ||
            dev_desc -> bDeviceProtocol != 1 ||
            dev_desc -> idVendor != 0x0424 ||
            dev_desc -> idProduct != SMSC9512_PID )
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
    struct usb_intf_desc * intf_desc = dev -> intf_desc [ 0 ];
    if ( intf_desc -> bNumEndpoints != 3 ||
            intf_desc -> bInterfaceClass != USB_CLASS_VENDOR_SPECIFIC ||
            intf_desc -> bInterfaceSubClass != 0 ||
            intf_desc -> bInterfaceProtocol != USB_CLASS_VENDOR_SPECIFIC )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    struct usb_endp_desc * endp_desc;

    // Endpoint 1 (Bulk IN)
    endp_desc = dev -> endp_desc [ 0 ] [ 0 ];
    if ( endp_desc -> bEndpointAddress.dir != REQ_DIR_IN ||
            endp_desc -> bEndpointAddress.endp != 1 ||
            endp_desc -> bmAttributes.transfer != ENDP_XFER_BULK )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    // Endpoint 2 (Bulk OUT)
    endp_desc = dev -> endp_desc [ 0 ] [ 1 ];
    if ( endp_desc -> bEndpointAddress.dir != REQ_DIR_OUT ||
            endp_desc -> bEndpointAddress.endp != 2 ||
            endp_desc -> bmAttributes.transfer != ENDP_XFER_BULK )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    // Endpoint 3 (Interrupt IN)
    endp_desc = dev -> endp_desc [ 0 ] [ 2 ];
    if ( endp_desc -> bEndpointAddress.dir != REQ_DIR_IN ||
            endp_desc -> bEndpointAddress.endp != 3 ||
            endp_desc -> bmAttributes.transfer != ENDP_XFER_INTERRUPT )
    {
        return USB_STATUS_NOT_SUPPORTED;
    }

    usb_dev = dev;

    uint32_t id_rev = smsc9512_read_reg ( ID_REV );
    if ( id_rev >> 16 != SMSC9512_PID )
    {
        usb_dev = 0;
        return USB_STATUS_NOT_SUPPORTED;
    }

    printuln ( "SMSC LAN9512 driver bound" );
    printu ( "SMSC LAN9512 rev " ); printu_32h ( id_rev & 0xFFFF );
    printuln ( "" );

    api_process_create ( smsc9512_led_process, 0 );

    return USB_STATUS_SUCCESS;
}

int smsc9512_remove ( struct usb_device * dev )
{
    ( void ) dev;
    return USB_STATUS_SUCCESS;
}

const struct usb_driver smsc9512_driver =
{
    .probe = smsc9512_probe,
    .remove = smsc9512_remove,
};
