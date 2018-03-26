#ifndef _H_USB_STD_HUB
#define _H_USB_STD_HUB

// USB 2.0 Section 11.23.1
// STT = Single-TT   MTT = Multi-TT
enum usb_hub_dev_proto
{
    USB_HUB_DEV_PROTO_LSFS,
    USB_HUB_DEV_PROTO_HS_STT,
    USB_HUB_DEV_PROTO_HS_MTT,
};

enum usb_hub_intf_proto
{
    USB_HUB_INTF_PROTO_NOTT_STT_ONLY,      // No-TT (LSFS) or STT-Only hubs (HS)
    USB_HUB_INTF_PROTO_HS_MTT_CAPABLE_STT, // STT Interface for MTT-capable hubs
    USB_HUB_INTF_PROTO_HS_MTT_CAPABLE_MTT, // MTT Interface for MTT-capable hubs
};

#define USB_HUB_STSCHG_ENDP_BINTERVAL 0xFF


// USB 2.0 Section 11.23.2.1
#define USB_HUB_DESC 0x29

struct usb_hub_desc
{
    uint8_t bLength;            // Number of bytes of this desc, incl. this byte
    uint8_t bDescriptorType;    // USB_HUB_DESC_HUB (0x29)
    uint8_t bNbrPorts;          // Number of downstream facing ports
    union
    {
        uint16_t raw;
        struct
        {
            uint16_t power          : 2; // Power switching mode
            uint16_t compound       : 1; // Compound device?
            uint16_t ovrcur_protec  : 2; // Overcurrent Protection Mode
            uint16_t tt_think_time  : 2;
            uint16_t port_indic     : 1; // Port Indicator Support?
            uint16_t reserved       : 8;
        };
    } wHubCharacteristics;
    uint8_t bPwrOn2PwrGood;     // Time (2ms intervals) from power-on until good
    uint8_t bHubContrCurrent;   // Max current req of the Hub Controller in mA

    uint8_t tail [ ];           // Usage of [ ] doesn't take any space in sizeof
        // -> DeviceRemovable   // Bitmap (8 to 256 bits), bit 0 reserved
        // -> PortPwrCtrlMask   // Bitmap (8 to 256 bits), all bits set to 1
} __attribute__ ( ( packed ) );

#define USB_HUB_MAX_PORTS 255

#define usb_hub_desc_tail_field_size(nbports) ( nbports / 8 + 1 )
#define usb_hub_desc_tail_size(nbports) \
    ( usb_hub_desc_tail_field_size ( nbports ) * 2 )
#define usb_hub_desc_bLength(nbports) ( sizeof ( struct usb_hub_desc ) + \
    usb_hub_desc_tail_size ( nbports ) )

#define USB_HUB_DESC_TAIL_MAXSIZE usb_hub_desc_tail_size ( USB_HUB_MAX_PORTS )
#define USB_HUB_DESC_MAX_BLENGTH usb_hub_desc_bLength ( USB_HUB_MAX_PORTS )

#define USB_HUB_PORT_PWR_CTRL_MASK 0xFF

enum usb_hub_desc_power_sw_mode
{
    USB_HUB_DESC_POWER_GANGED,
    USB_HUB_DESC_POWER_INDIV,
    USB_HUB_DESC_POWER_NOT_SUPPORTED,
};

enum usb_hub_desc_compound
{
    USB_HUB_DESC_COMPOUND_SIMPLE,
    USB_HUB_DESC_COMPOUND_COMPOUND,
};

enum usb_hub_desc_ovrcur_protec
{
    USB_HUB_DESC_OVRCUR_GLOBAL,
    USB_HUB_DESC_OVRCUR_INDIV,
    USB_HUB_DESC_OVRCUR_NOT_SUPPORTED1,
    USB_HUB_DESC_OVRCUR_NOT_SUPPORTED2,
};

enum usb_hub_desc_tt_think_time
{
    USB_HUB_DESC_TT_THINK_TIME_8FS_BIT,
    USB_HUB_DESC_TT_THINK_TIME_16FS_BIT,
    USB_HUB_DESC_TT_THINK_TIME_24FS_BIT,
    USB_HUB_DESC_TT_THINK_TIME_32FS_BIT,
};

enum usb_hub_port_indic_support
{
    USB_HUB_PORT_INDIC_NOT_SUPPORTED,
    USB_HUB_PORT_INDIC_SUPPORTED,
};

enum usb_hub_port_dev_removable
{
    USB_HUB_PORT_DEVICE_REMOVABLE,
    USB_HUB_PORT_DEVICE_NON_REMOVABLE,
};

// USB 2.0 Section 11.24.2
// Hub Class Request Codes
enum usb_hub_bRequest
{
    HUB_REQ_GET_STATUS,         // GetHub / GetPortStatus
    HUB_REQ_CLEAR_FEATURE,      // ClearHub / ClearPortFeature
    HUB_REQ_RESERVED1,
    HUB_REQ_SET_FEATURE,        // SetHub / SetPortFeature (reported in the sts)
    HUB_REQ_RESERVED2,
    HUB_REQ_RESERVED3,
    HUB_REQ_GET_DESC,           // GetHubDescriptor
    HUB_REQ_SET_DESC,           // SetHubDescriptor (optional)
    HUB_REQ_CLEAR_TT_BUFFER,    // Used for non-periodic endp only
    HUB_REQ_RESET_TT,           // Returns TT in a hub to a known state
    HUB_REQ_GET_TT_STATE,       // First stop TT, then request state (debug)
    HUB_REQ_STOP_TT,            // To restart, use reset TT

};

// Hub Class Feature Selectors
enum usb_hub_wValue_feature_hub
{
    HUB_FEATURE_C_HUB_LOCAL_POWER,
    HUB_FEATURE_C_HUB_OVER_CURRENT,
};

enum usb_hub_wValue_feature_port
{
    HUB_FEATURE_PORT_CONNECTION,
    HUB_FEATURE_PORT_ENABLE,
    HUB_FEATURE_PORT_SUSPEND,
    HUB_FEATURE_PORT_OVER_CURRENT,
    HUB_FEATURE_PORT_RESET,

    HUB_FEATURE_PORT_POWER          = 8,
    HUB_FEATURE_PORT_LOW_SPEED,

    HUB_FEATURE_C_PORT_CONNECTION   = 16,
    HUB_FEATURE_C_PORT_ENABLE,
    HUB_FEATURE_C_PORT_SUSPEND,
    HUB_FEATURE_C_PORT_OVER_CURRENT,
    HUB_FEATURE_C_PORT_RESET,

    HUB_FEATURE_PORT_TEST,
    HUB_FEATURE_PORT_INDICATOR,
};

// Get Hub Status
// USB 2.0 Section 11.24.2.6
struct usb_hub_status
{
    // Cannot be set or cleared using feature selector
    union
    {
        uint16_t raw;
        struct
        {
            uint16_t local_power    :  1; // Local Power Source
            uint16_t over_current   :  1; // (Only if no ovrcur/port)
            uint16_t reserved       : 14;
        };
    } wHubStatus;

    // Might be set or cleared using feature selector
    union
    {
        uint16_t raw;
        struct
        {
            uint16_t c_local_power  :  1; // Local Power changed in wHubStatus
            uint16_t c_over_current :  1; // Over-Current changed in wHubStatus
            uint16_t reserved       : 14;
        };
    } wHubChange;
};

// Get Port Status
// USB 2.0 Section 11.24.2.7
struct usb_hub_port_status
{
    union
    {
        uint16_t raw;
        struct
        {
            uint16_t connection     : 1; // Device is present (RO)
            uint16_t enable         : 1; // After reset (RO for enabling)
            uint16_t suspend        : 1;
            uint16_t over_current   : 1; // (RO)
            uint16_t reset          : 1; // (RO for deassert?)
            uint16_t reserved1      : 3;
            uint16_t power          : 1; // (only if indiv)
            uint16_t ls_dev         : 1; // (RO)
            uint16_t hs_dev         : 1; // (RO)
            uint16_t test_mode      : 1; // (RO for disabling)
            uint16_t indic_control  : 1;
            uint16_t reserved2      : 3;
        };
    } wPortStatus;

    union
    {
        uint16_t raw;
        struct
        {
            uint16_t c_connection   :  1; // Attach or deattach event
            uint16_t c_enable       :  1; // Set on port error condition
            uint16_t c_suspend      :  1; // Set when resume is complete
            uint16_t c_over_current :  1;
            uint16_t c_reset        :  1; // Set when reset is complete
            uint16_t reserved       : 11;
        };
    } wPortChange;
};

enum usb_hub_port_indic_sel
{
    HUB_PORT_INDIC_AUTO,
    HUB_PORT_INDIC_AMBER,
    HUB_PORT_INDIC_GREEN,
    HUB_PORT_INDIC_OFF,

};

#endif _H_USB_STD_HUB
