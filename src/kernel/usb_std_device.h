#ifndef _H_USB_STD_DEVICE
#define _H_USB_STD_DEVICE

enum usb_device_state
{
    USBDEV_STATE_ATTACHED,
    USBDEV_STATE_POWERED,
    USBDEV_STATE_DEFAULT,
    USBDEV_STATE_ADDRESS,
    USBDEV_STATE_CONFIGURED,
    USBDEV_STATE_SUSPENDED,
};

// Control Setup Request
struct usb_setup_req
{
    // Characteristics of the request
    union bmRequestType
    {
        uint8_t raw;
        struct
        {
            uint8_t recipient   : 5;
            uint8_t type        : 2;
            uint8_t direction   : 1;
        };
    };

    // Specific Request
    uint8_t bRequest;

    uint16_t wValue;

    union wIndex
    {
        uint16_t raw;

        // When wIndex used to specify endpoint
        struct endp
        {
            uint16_t endpoint   : 4;
            uint16_t reserved1  : 3;
            uint16_t direction  : 1;
            uint16_t reserved2  : 8;
        };

        // When wIndex used to specify interface
        struct intf
        {
            uint16_t interface  : 8;
            uint16_t reserved   : 8;
        };
    };

    // Number of bytes to transfer if there is a data stage (second phase)
    uint16_t wLength;
};

enum bmRequestType_direction
{
    REQUEST_DIRECTION_OUT,
    REQUEST_DIRECTION_IN,
};

enum bmRequestType_type
{
    REQUEST_TYPE_STANDARD,
    REQUEST_TYPE_CLASS,
    REQUEST_TYPE_VENDOR,
};

enum bmRequestType_recipient
{
    REQUEST_RECIPIENT_DEVICE,
    REQUEST_RECIPIENT_INTERFACE,
    REQUEST_RECIPIENT_ENDPOINT,
    REQUEST_RECIPIENT_OTHER,
};

// USB Standard Device Requests
enum bRequest
{
    REQUEST_GET_STATUS,
    REQUEST_CLEAR_FEATURE,
    REQUEST_RESERVED1,
    REQUEST_SET_FEATURE,
    REQUEST_RESERVED2,
    REQUEST_SET_ADDRESS,
    REQUEST_GET_DESCRIPTOR,
    REQUEST_SET_DESCRIPTOR,
    REQUEST_GET_CONFIGURATION,
    REQUEST_SET_CONFIGURATION,
    REQUEST_GET_INTERFACE,
    REQUEST_SET_INTERFACE,
    REQUEST_SYNCH_FRAME,
};

enum wValue_descriptor // (high-byte. low-byte = index)
{
    DESCRIPTOR_RESERVED,
    DESCRIPTOR_DEVICE,
    DESCRIPTOR_CONFIGURATION,
    DESCRIPTOR_STRING,
    DESCRIPTOR_INTERFACE,
    DESCRIPTOR_ENDPOINT,
    DESCRIPTOR_DEVICE_QUALIFIER,
    DESCRIPTOR_OTHER_SPEED_CONFIGURATION,
    DESCRIPTOR_INTERFACE_POWER,
};

enum wValue_feature
{
    FEATURE_ENDPOINT_HALT,          // For endpoint
    FEATURE_DEVICE_REMOTE_WAKEUP,   // For device
    FEATURE_TEST_MODE,              // For device, cannot be cleared
};

enum wIndex_test_selector // (high-byte. low byte = 0/intf/endp)
{
    TEST_SELECTOR_RESERVED,
    TEST_SELECTOR_TEST_J,
    TEST_SELECTOR_TEST_K,
    TEST_SELECTOR_SE0_NAK,
    TEST_SELECTOR_TEST_PACKET,
    TEST_SELECTOR_FORCE_ENABLE,
};

// USB Device Descriptor
struct usb_dev_descriptor
{
    uint8_t bLength;            // Size of this descriptor in bytes
    uint8_t bDescriptorType;    // DEVICE Descriptor Type
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

// USB Configuration Descriptor
struct usb_conf_descriptor
{
    uint8_t bLength;                // Size of this descriptor in bytes
    uint8_t bDescriptorType;        // CONFIGURATION Descriptor Type
    uint16_t wTotalLength;          // Total length of data returned for this conf
    uint8_t bNumInterfaces;         // Number of interfaces supported by this conf
    uint8_t bConfigurationValue;    // Value to use as arg to SetConf()
    uint8_t iConfiguration;         // Index of string descriptor for this conf
    uint8_t bmAttributes;           // Conf charac. (Self-powered/Remote WKP)
    uint8_t bMaxPower;              // Max power consumption (in 2mA units)
};

// USB Interface Descriptor
struct usb_intf_descriptor
{
    uint8_t bLength;                // Size of this descriptor in bytes
    uint8_t bDescriptorType;        // INTERFACE Descriptor Type
    uint8_t bInterfaceNumber;       // Number of this intf (index within conf)
    uint8_t bAlternateSetting;      // Value used to select this altn setting
    uint8_t bNumEndpoints;          // Number of endp in this intf (except EP0)
    uint8_t bInterfaceClass;        // Class code (assigned by USB-IF)
    uint8_t bInterfaceSubClass;     // Subclass code (assigned by USB-IF)
    uint8_t bInterfaceProtocol;     // Protocol Code (assigned by USB-IF)
    uint8_t iInterface;             // Index of string desc for this intf
};

// USB Endpoint Descriptor
struct usb_endp_descriptor
{
};

#endif
