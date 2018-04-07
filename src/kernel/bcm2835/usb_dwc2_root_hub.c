#include "usb_dwc2_regs.h"
#include "bcm2835.h"
#include "../usb_core.h"
#include "../usb_std_hub.h"

#include "../arm.h"
#include "uart.h"

#include "../../libc/string.h"
#include "../../libc/math.h"

#define DWC2_ROOT_HUB_NB_PORTS 1

static struct dwc2_regs volatile * regs = ( struct dwc2_regs volatile * ) USB_HCD_BASE;

struct usb_hub_port_status dwc2_root_hub_port_status;

// USB 2.0 Section 11.23.1
// Faked device descriptor for our root hub
static const struct usb_dev_desc dwc2_root_hub_dev_desc =
{
    .bLength = sizeof ( struct usb_dev_desc ),
    .bDescriptorType    = USB_DESC_DEV,
    .bcdUSB             = USB_BCD_2_0,
    .bDeviceClass       = USB_CLASS_HUB,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = USB_HUB_DEV_PROTO_HS_STT,
    .bMaxPacketSize0    = USB_HS_CTRL_DATALEN,
    .idVendor           = 0x0405,
    .idProduct          = 0,
    .bcdDevice          = 0,
    .iManufacturer      = 0,
    .iProduct           = 0,
    .iSerialNumber      = 0,
    .bNumConfigurations = 1,
};

// Faked configuration, interface & endpoint descriptors for the root hub
static struct
{
    struct usb_conf_desc conf;
    struct usb_intf_desc intf;
    struct usb_endp_desc endp;
} __attribute__ ( ( packed ) ) dwc2_root_hub_conf_desc =
{
    .conf.bLength                = sizeof ( struct usb_conf_desc ),
    .conf.bDescriptorType        = USB_DESC_CONF,
    .conf.wTotalLength           = sizeof ( dwc2_root_hub_conf_desc ),
    .conf.bNumInterfaces         = 1,
    .conf.bConfigurationValue    = 1,
    .conf.iConfiguration         = 0,
    .conf.bmAttributes.raw       = 0,
    .conf.bmAttributes.one       = 1,
    .conf.bmAttributes.selfpwr   = 1,
    .conf.bmAttributes.rmtwkp    = 0,
    .conf.bMaxPower              = 0,

    .intf.bLength                = sizeof ( struct usb_intf_desc ),
    .intf.bDescriptorType        = USB_DESC_INTF,
    .intf.bInterfaceNumber       = 0,
    .intf.bAlternateSetting      = 0,
    .intf.bNumEndpoints          = 1,
    .intf.bInterfaceClass        = USB_CLASS_HUB,
    .intf.bInterfaceSubClass     = 0,
    .intf.bInterfaceProtocol     = USB_HUB_INTF_PROTO_NOTT_STT_ONLY,
    .intf.iInterface             = 0,

    .endp.bLength                = sizeof ( struct usb_endp_desc ),
    .endp.bDescriptorType        = USB_DESC_ENDP,
    .endp.bEndpointAddress.raw   = 0,
    .endp.bEndpointAddress.endp  = 1,
    .endp.bEndpointAddress.dir   = REQ_DIR_IN,
    .endp.bmAttributes.raw       = 0,
    .endp.bmAttributes.transfer  = ENDP_XFER_INTERRUPT,
    .endp.bmAttributes.sync      = ENDP_SYNC_NOSYNC,
    .endp.bmAttributes.usage     = ENDP_USAGE_DATA,
    .endp.wMaxPacketSize.raw     = 0,
    .endp.wMaxPacketSize.size    = 1,
    .endp.bInterval              = USB_HUB_STSCHG_ENDP_BINTERVAL,
};

static const struct usb_hub_desc dwc2_root_hub_hub_desc =
{
    .bLength                = usb_hub_desc_bLength ( DWC2_ROOT_HUB_NB_PORTS ),
    .bDescriptorType        = USB_HUB_DESC,
    .bNbrPorts              = DWC2_ROOT_HUB_NB_PORTS,
    .wHubCharacteristics    =
    {   .raw                = 0,
        .power              = USB_HUB_DESC_POWER_GANGED,
        .compound           = USB_HUB_DESC_COMPOUND_SIMPLE,
        .ovrcur_protec      = USB_HUB_DESC_OVRCUR_GLOBAL,
        .tt_think_time      = 0,
        .port_indic         = USB_HUB_PORT_INDIC_NOT_SUPPORTED,
    },
    .bPwrOn2PwrGood         = 0,
    .bHubContrCurrent       = 0,
    .tail                   =
    {
        USB_HUB_PORT_DEVICE_NON_REMOVABLE << 1,
        USB_HUB_PORT_PWR_CTRL_MASK,
    },
};

void dwc2_root_hub_handle_port_interrupt ( )
{
    union hprt hprt = regs -> host.hprt;

    // Update our port status for the USB Hub driver
    dwc2_root_hub_port_status.connection        = hprt.prtconnsts;
    dwc2_root_hub_port_status.enable            = hprt.prtena;
    dwc2_root_hub_port_status.suspend           = hprt.prtsusp;
    dwc2_root_hub_port_status.over_current      = hprt.prtovrcuract;
    dwc2_root_hub_port_status.reset             = hprt.prtrst;
    dwc2_root_hub_port_status.power             = hprt.prtpwr;
    dwc2_root_hub_port_status.ls_dev            = hprt.prtspd == HPRT_PRTSPD_LS;
    dwc2_root_hub_port_status.hs_dev            = hprt.prtspd == HPRT_PRTSPD_HS;

    dwc2_root_hub_port_status.c_connection      = hprt.prtconndet;
    dwc2_root_hub_port_status.c_enable          = hprt.prtenchng;
    dwc2_root_hub_port_status.c_over_current    = hprt.prtovrcurchng;

    /* Write back the register to itself to acknowledge interrupts
     * This works because some bits are of type "WC" (Write 1 to Clear).
     * But don't clear prtena as this would disable the root hub port. */
    hprt.prtena = 0;
    regs -> host.hprt = hprt;
}

static void __attribute__ ( ( unused ) )
dwc2_root_hub_reset_port ( struct dwc2_regs * regs )
{
    // Read the Host Port Control & Status Register
    union hprt hprt = regs -> host.hprt;

    // Zero the "Write 1 to Clear" bits so that we don't accidentally clear them
    hprt.raw &= ~DWC2_HPRT_WC_MASK;

    // Assert reset on the port
    hprt.prtrst = 1;
    regs -> host.hprt = hprt;

    // Wait the required delay
    cdelay ( 350000 );

    // De-assert reset on the port
    hprt.prtrst = 0;
    regs -> host.hprt = hprt;
}

static void dwc2_root_hub_power_on_port ( )
{
    // Read the Host Port Control & Status Register
    union hprt hprt = regs -> host.hprt;

    // Zero the "Write 1 to Clear" bits so that we don't accidentally clear them
    hprt.raw &= ~DWC2_HPRT_WC_MASK;

    // Power on port
    hprt.prtpwr = 1;

    // Commit
    regs -> host.hprt = hprt;
}

static int dwc2_root_hub_std_request ( struct usb_request * req )
{
    size_t size;

    switch ( req -> setup_req.bRequest )
    {
        case REQ_GET_DESC:
            switch ( req -> setup_req.wValue >> 8 )
            {
                case USB_DESC_DEV:
                    size = min ( req -> size, dwc2_root_hub_dev_desc.bLength );
                    memcpy ( req -> data, &dwc2_root_hub_dev_desc, size );
                    return USB_STATUS_SUCCESS;
                case USB_DESC_CONF:
                    size = min ( req -> size, dwc2_root_hub_conf_desc.conf.wTotalLength );
                    memcpy ( req -> data, &dwc2_root_hub_conf_desc, size );
                    return USB_STATUS_SUCCESS;
                default:
                    return USB_STATUS_NOT_SUPPORTED;
            }

        case REQ_SET_ADDR:
        case REQ_SET_CONF:
            return USB_STATUS_SUCCESS;

        default:
            return USB_STATUS_NOT_SUPPORTED;
    }
}

static int dwc2_root_hub_set_port_feature ( uint16_t feature )
{
    switch ( feature )
    {
        case HUB_FEATURE_PORT_POWER:
            dwc2_root_hub_power_on_port ( );
            return USB_STATUS_SUCCESS;

        default:
            return USB_STATUS_NOT_SUPPORTED;
    }
}

static int dwc2_root_hub_class_request ( struct usb_request * req )
{
    size_t size;
    struct usb_setup_req * setup = & ( req -> setup_req );

    switch ( setup -> bRequest )
    {
        case HUB_REQ_GET_DESC:
            // Hubs only have one class specific descriptor
            if ( setup -> wValue >> 8 != USB_HUB_DESC )
            {
                return USB_STATUS_ERROR;
            }

            size = min ( req -> size, dwc2_root_hub_hub_desc.bLength );
            memcpy ( req -> data, &dwc2_root_hub_hub_desc, size );
            return USB_STATUS_SUCCESS;

        case HUB_REQ_SET_FEATURE:
            switch ( setup -> bmRequestType.recipient )
            {
                case REQ_RECIPIENT_OTHER:
                    return dwc2_root_hub_set_port_feature ( setup -> wValue );
                default:
                    return USB_STATUS_NOT_SUPPORTED;
            }

        default:
            return USB_STATUS_NOT_SUPPORTED;
    }
}

static int dwc2_root_hub_ctrl_req ( struct usb_request * req )
{
    switch ( req -> setup_req.bmRequestType.type )
    {
        case REQ_TYPE_STD:
            return dwc2_root_hub_std_request ( req );
        case REQ_TYPE_CLASS:
            return dwc2_root_hub_class_request ( req );
        default:
            return USB_STATUS_NOT_SUPPORTED;
    }
}

void dwc2_root_hub_request ( struct usb_request * req )
{
    printu ( "Processing Root Hub Request" );
    req -> status = dwc2_root_hub_ctrl_req ( req );
    usb_request_done ( req );
}
