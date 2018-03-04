#ifndef _H_USB_STD_DEVICE
#define _H_USB_STD_DEVICE

enum usbdev_state
{
    USBDEV_STATE_ATTACHED,
    USBDEV_STATE_POWERED,
    USBDEV_STATE_DEFAULT,
    USBDEV_STATE_ADDRESS,
    USBDEV_STATE_CONFIGURED,
    USBDEV_STATE_SUSPENDED,
};

struct usbdev_setup_req
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

        struct endp
        {
            uint16_t endpoint   : 4;
            uint16_t reserved   : 3;
            uint16_t direction  : 1;
        };

        struct intf
        {
            uint16_t interface  : 8;
            uint16_t reserved   : 8;
        };
    };

    // Number of bytes to transfer if there is a data stage (second phase)
    uint16_t wLength;
};

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

struct usbdev_descriptor
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

#endif
