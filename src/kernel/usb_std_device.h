#ifndef _H_USB_STD_DEVICE
#define _H_USB_STD_DEVICE

#include <stdint.h>

// USB 2.0 Section 8.4.4
#define USB_LS_MAX_DATALEN 8
#define USB_FS_MAX_DATALEN 1023
#define USB_HS_MAX_DATALEN 1024

// USB 2.0 Section 5.5.3
#define USB_LS_CTRL_DATALEN USB_LS_MAX_DATALEN      // LS only allow 8
#define USB_FS_MAX_CTRL_DATALEN 64                  // FS allow 8,16,32,64
#define USB_HS_CTRL_DATALEN USB_FS_MAX_CTRL_DATALEN // HS only allow 64

#define USB_BCD_2_0 0x0200

enum usb_device_state
{
    USBDEV_STATE_ATTACHED,
    USBDEV_STATE_POWERED,
    USBDEV_STATE_DEFAULT,
    USBDEV_STATE_ADDRESS,
    USBDEV_STATE_CONFIGURED,
    USBDEV_STATE_SUSPENDED,
};

// D = usable in Device Descriptor, I = usable in Interface Descriptor
enum usb_class
{
    USB_CLASS_SEE_INTF_DESC,        // 0x00     D
    USB_CLASS_AUDIO,                // 0x01      I
    USB_CLASS_COM,                  // 0x02     DI
    USB_CLASS_HID,                  // 0x03      I
    USB_CLASS_RESERVED1,
    USB_CLASS_PHYSICAL,             // 0x05      I
    USB_CLASS_IMAGE,                // 0x06      I
    USB_CLASS_PRINTER,              // 0x07      I
    USB_CLASS_MASS_STORAGE,         // 0x08      I
    USB_CLASS_HUB,                  // 0x09     D
    USB_CLASS_CDC_DATA,             // 0x0A      I
    USB_CLASS_SMART_CARD,           // 0x0B      I
    USB_CLASS_RESERVED2,
    USB_CLASS_CONTENT_SECURITY,     // 0x0D      I
    USB_CLASS_VIDEO,                // 0x0E      I
    USB_CLASS_PERSONAL_HEALTHCARE,  // 0x0F      I
    USB_CLASS_AUDIO_VIDEO,          // 0x10      I
    USB_CLASS_BILLBOARD,            // 0x11     D
    USB_CLASS_USB_TYPE_C_BRIDGE,    // 0x12      I

    USB_CLASS_DIAGNOSTIC            =  0xDC, // DI
    USB_CLASS_WIRELESS              =  0xE0, //  I
    USB_CLASS_MISC                  =  0xEF, // DI
    USB_CLASS_APP_SPECIFIC          =  0xFE, //  I
    USB_CLASS_VENDOR_SPECIFIC       =  0xFF, // DI
};

// Control Setup Request (USB 2.0 Section 9.3)
struct usb_setup_req
{
    // Characteristics of the request
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t recipient   : 5;
            uint8_t type        : 2;
            uint8_t dir         : 1;
        };
    } bmRequestType;

    // Specific Request
    uint8_t bRequest;

    uint16_t wValue;

    union
    {
        uint16_t raw;

        // When wIndex used to specify endpoint
        struct
        {
            uint16_t endp       : 4;
            uint16_t reserved1  : 3;
            uint16_t dir        : 1;
            uint16_t reserved2  : 8;
        } endp;

        // When wIndex used to specify interface
        struct
        {
            uint16_t intf       : 8;
            uint16_t reserved   : 8;
        } intf;
    } wIndex;

    // Number of bytes to transfer if there is a data stage (second phase)
    uint16_t wLength;
};

enum bmRequestType_direction
{
    REQ_DIR_OUT,
    REQ_DIR_IN,
};

enum bmRequestType_type
{
    REQ_TYPE_STD,
    REQ_TYPE_CLASS,
    REQ_TYPE_VENDOR,
};

enum bmRequestType_recipient
{
    REQ_RECIPIENT_DEV,
    REQ_RECIPIENT_INTF,
    REQ_RECIPIENT_ENDP,
    REQ_RECIPIENT_OTHER,
};

// USB Standard Device Requests (USB 2.0 Section 9.4)
enum bRequest
{
    REQ_GET_STATUS,
    REQ_CLEAR_FEATURE,
    REQ_RESERVED1,
    REQ_SET_FEATURE,
    REQ_RESERVED2,
    REQ_SET_ADDR,
    REQ_GET_DESC,
    REQ_SET_DESC,
    REQ_GET_CONF,
    REQ_SET_CONF,
    REQ_GET_INTF,   // Returns selected alternate setting
    REQ_SET_INTF,   // Selects an alternate setting
    REQ_SYNCH_FRAME,
};

enum wValue_descriptor // (high-byte. low-byte = index)
{
    DESC_RESERVED,
    DESC_DEV,
    DESC_CONF,
    DESC_STRING,
    DESC_INTF,
    DESC_ENDP,
    DESC_DEV_QUALIFIER,
    DESC_OTHER_SPD_CONF,
    DESC_INTF_POWER,
};

enum wValue_feature
{
    FEATURE_ENDP_HALT,      // For endpoint
    FEATURE_DEV_RMT_WKP,    // For device
    FEATURE_TEST_MODE,      // For device, cannot be cleared
};

enum wIndex_test_selector // (high-byte. low byte = 0/intf/endp)
{
    TESTSEL_RESERVED,
    TESTSEL_TEST_J,
    TESTSEL_TEST_K,
    TESTSEL_SE0_NAK,
    TESTSEL_TEST_PACKET,
    TESTSEL_FORCE_ENABLE,
};

// USB Device Descriptor
struct usb_dev_desc
{
    uint8_t bLength;            // Size of this descriptor in bytes
    uint8_t bDescriptorType;    // DESC_DEV
    uint16_t bcdUSB;            // USB Release Number, Binary-Coded Decimal
    uint8_t bDeviceClass;       // Class code
    uint8_t bDeviceSubClass;    // Subclass code
    uint8_t bDeviceProtocol;    // Protocol code
    uint8_t bMaxPacketSize0;    // Max packet size for EP 0 (8,16,32,64)
    uint16_t idVendor;          // Vendor ID
    uint16_t idProduct;         // Product ID
    uint16_t bcdDevice;         // Device release number in Binay-Coded Decimal
    uint8_t iManufacturer;      // Index of string desc describing manufacturer
    uint8_t iProduct;           // Index of string desc describing product
    uint8_t iSerialNumber;      // Index of string desc describing serial number
    uint8_t bNumConfigurations; // Number of possible configurations
};

// USB Device Qualifier Descriptor
// Return info about how dev would operate at other than current speed
// First retrieve dev_qualifier desc before other speed conf desc
struct usb_dev_qualifier_desc
{
    uint8_t bLength;            // Size of descriptor
    uint8_t bDescriptorType;    // DESC_DEV_QUALIFIER
    uint16_t bcdUSB;            // USB spec. version number
    uint8_t bDeviceClass;       // Class code
    uint8_t bDeviceSubClass;    // SubClass code
    uint8_t bDeviceProtocol;    // Protocol Code
    uint8_t bMaxPacketSize0;    // Maximum packet size for other speed's EP0
    uint8_t bNumConfigurations; // Number of other-speed configurations
    uint8_t reserved;
};

// USB Configuration Descriptor
struct usb_conf_desc
{
    uint8_t bLength;                // Size of this descriptor in bytes
    uint8_t bDescriptorType;        // DESC_CONF
    uint16_t wTotalLength;          // Total length returned for this conf
    uint8_t bNumInterfaces;         // Number of intfs supported by this conf
    uint8_t bConfigurationValue;    // Value to use as arg to SetConf()
    uint8_t iConfiguration;         // Index of string descriptor for this conf
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t reserved    : 5;
            uint8_t rmtwkp      : 1; // Whether this conf supports remote wakeup
            uint8_t selfpwr     : 1; // Whether this conf makes dev self-powered
            uint8_t one         : 1; // Must always be set to one
        };
    } bmAttributes;                 // Conf charac. (Self-powered/Remote WKP)
    uint8_t bMaxPower;              // Max power consumption (in 2mA units)
};

// USB Other Speed Configuration Descriptor
/* Use same layout as struct usb_conf_descriptor
 * But bDescriptorType is set to DESC_OTHER_SPD_CONF */

// USB Interface Descriptor
struct usb_intf_desc
{
    uint8_t bLength;                // Size of this descriptor in bytes
    uint8_t bDescriptorType;        // DESC_INTF
    uint8_t bInterfaceNumber;       // Number of this intf (index within conf)
    uint8_t bAlternateSetting;      // Value used to select this altn setting
    uint8_t bNumEndpoints;          // Number of endp in this intf (except EP0)
    uint8_t bInterfaceClass;        // Class code (assigned by USB-IF)
    uint8_t bInterfaceSubClass;     // Subclass code (assigned by USB-IF)
    uint8_t bInterfaceProtocol;     // Protocol Code (assigned by USB-IF)
    uint8_t iInterface;             // Index of string desc for this intf
};

// USB Endpoint Descriptor
struct usb_endp_desc
{
    uint8_t bLength;            // Size of this descriptor
    uint8_t bDescriptorType;    // DESC_ENDP

    union                       // Address of the endp described by this desc
    {
        uint8_t raw;
        struct
        {
            uint8_t endp        : 4; // Endpoint Number
            uint8_t reserved    : 3;
            uint8_t dir         : 1; // Direction (ignored for ctrl endp)
        };
    } bEndpointAddress;

    union                       // Endpoint attributes
    {
        uint8_t raw;
        struct
        {
            uint8_t transfer    : 2; // Transfer Type
            uint8_t sync        : 2; // Synchronisation Type (0 if non-isoc)
            uint8_t usage       : 2; // Usage Type (0 if non-isoc)
            uint8_t reserved    : 2;
        };
    } bmAttributes;

    union                       // Max packet size this endp is capable of TX/RX
    {
        uint16_t raw;
        struct
        {
            uint16_t size           : 11; // Maximum Packet Size in bytes
            uint16_t addi_transac   :  2; // Additional Transac. opportunities
            uint16_t reserved       :  3;
        };
    } wMaxPacketSize;

    uint8_t bInterval;          // Interval for polling endp in F or uF
};

enum usb_endp_desc_bmAttributes_transfer
{
    ENDP_XFER_CONTROL,
    ENDP_XFER_ISOCHRONOUS,
    ENDP_XFER_BULK,
    ENDP_XFER_INTERRUPT,
};

enum usb_endp_desc_bmAttributes_sync
{
    ENDP_SYNC_NOSYNC,
    ENDP_SYNC_ASYNC,
    ENDP_SYNC_ADAPTIVE,
    ENDP_SYNC_SYNC,
};

enum usb_endp_desc_bmAttributes_usage
{
    ENDP_USAGE_DATA,
    ENDP_USAGE_FEEDBK,
    ENDP_USAGE_IMPL_FEEDBK_DATA, // Implicit Feedback Data
};

#endif
