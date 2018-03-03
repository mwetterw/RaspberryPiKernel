#ifndef _H_BCM2835_DWC2_REGS
#define _H_BCM2835_DWC2_REGS

#include <stdint.h>

#include "usb_dwc2_regs_inner.h"

#define MAX_CHAN 16
#define MAX_EP MAX_CHAN

// Core Global CSRs
struct dwc2_regs_core
{
    uint32_t gotgctl;           // 0x00000 OTG Control and Status Register
    uint32_t gotgint;           // 0x00004 OTG Interrupt Register
    uint32_t gahbcfg;           // 0x00008 AHB Configuration Register
    uint32_t gusbcfg;           // 0x0000C USB Configuration Register
    union grstctl grstctl;      // 0x00010 Reset Control Register
    union gint gintsts;         // 0x00014 Interrupt Status Register
    union gint gintmsk;         // 0x00018 Interrupt Mask Register
    uint32_t grxstsr;           // 0x0001C Receive Status Debug Read Register
    uint32_t grxstsp;           // 0x00020 Receive Status Stack Pointer (Read Pop) Register
    uint32_t grxfsiz;           // 0x00024 Receive FIFO Size Register
    uint32_t gnptxfsiz;         // 0x00028 Non-Periodic Transmit FIFO Size Register
    uint32_t gnptxsts;          // 0x0002C Non-Periodic Transmit FIFO Queue Status Register
    uint32_t gi2cctl;           // 0x00030 I²C Address Register
    uint32_t gpvndctl;          // 0x00034 PHY Vendor Control Register
    uint32_t ggpio;             // 0x00038 General Purpose Input/Output Register
    uint32_t guid;              // 0x0003C User ID Register
    uint32_t gsnpsid;           // 0x00040 Synopsys ID Register
    uint32_t ghwcfg1;           // 0x00044 User Hardware Config1 Register
    union ghwcfg2 ghwcfg2;      // 0x00048 User Hardware Config2 Register
    union ghwcfg3 ghwcfg3;      // 0x0004C User Hardware Config3 Register
    uint32_t ghwcfg4;           // 0x00050 User Hardware Config4 Register
    uint32_t glpmcfg;           // 0x00054 Core LPM Configuration Register
    uint32_t gpwrdn;            // 0x00058 Power Down Register
    uint32_t gdfifocfg;         // 0x0005C DFIFO Software Configuration Register
    uint32_t gadpctl;           // 0x00060 ADP Timer Control and Status Register

    unsigned char reserved1 [ 0x9C ];

    uint32_t hptxfsiz;                      // 0x00100 Host Periodic Transmit FIFO Size Register
    uint32_t dieptxfn [ MAX_EP - 1 ];  // 0x00104 -> 0x0013F Device IN Endpoint Transmit FIFO Size Register

    unsigned char reserved2 [ 0x2C0 ];      // 0x00140 -> 0x003FF
};

// Host Channel-Specific Registers
struct dwc2_regs_host_hc
{
    uint32_t hcchar;        // Host Channel Characteristics Register
    uint32_t hcsplt;        // Host Channel Split Control Register
    union hcint hcint;      // Host Channel Interrupt Register
    union hcint hcintmsk;   // Host Channel Interrupt Mask Register
    uint32_t hcsiz;         // Host Channel Transfer Size Register
    uint32_t hcdma;         // Host Channel DMA Address Register
    uint32_t reserved1;     // Is this just after hcdmab?
    uint32_t hcdmab;        // Host Channel DMA Buffer Address Register
};

// Host Mode CSRs
struct dwc2_regs_host
{
    uint32_t hcfg;      // 0x00400 Host Configuration Register
    uint32_t hfir;      // 0x00404 Host Frame Interval Register
    uint32_t hfnum;     // 0x00408 Host Frame Number / Frame Time Remaining Register
    uint32_t reserved1; // 0x0040C
    uint32_t hptxsts;   // 0x00410 Host Periodic Transmit FIFO / Queue Status Register
    uint32_t haint;     // 0x00414 Host All Channels Interrupt Register
    uint32_t haintmsk;  // 0x00418 Host All Channels Interrupt Mask Register
    uint32_t hflbaddr;  // 0x0041C Host Frame List Base Address Register

    unsigned char reserved2 [ 0x20 ];

    uint32_t hprt;  // 0x00440 Host Port Control and Status Register

    unsigned char reserved3 [ 0xBC ];

    struct dwc2_regs_host_hc hc [ MAX_CHAN ];  // 0x00500 -> 0x006FF Host Channel-Specific Registers

    unsigned char reserved4 [ 0x100 ];  // 0x00700 -> 0x007FF
};

// Device IN/OUT Endpoint-Specific Registers
struct dwc2_regs_device_dioep
{
    uint32_t dioepctl;  // Device Control IN/OUT Endpoint Control Register
    uint32_t reserved1;
    uint32_t dioepint;  // Device IN/OUT Endpoint Interrupt Register
    uint32_t reserved2;
    uint32_t dioeptsiz; // Device IN/OUT Endpoint Transfer Size Register
    uint32_t dioepdma;  // Device IN/OUT Endpoint DMA Address Register
    uint32_t dtxfsts;   // Device IN Endpoint Transmit FIFO Status Register
    uint32_t dioepdmab; // Device IN/OUT Endpoint DMA Buffer Address Register
};

enum dwc2_regs_device_dioep_e { DIOEP_IN, DIOEP_OUT };

// Device Mode CSRs
struct dwc2_regs_device
{
    uint32_t dcfg;          // 0x00800 Device Configuration Register
    uint32_t dctl;          // 0x00804 Device Control Register
    uint32_t dsts;          // 0x00808 Device Status Register
    uint32_t reserved1;     // 0x0080C
    uint32_t diepmsk;       // 0x00810 Device IN Endpoint Common Interrupt Mask Register
    uint32_t dioepmsk;      // 0x00814 Device OUT Endpoint Common Interrupt Mask Register
    uint32_t daint;         // 0x00818 Device All Endpoints Interrupt Register
    uint32_t daintmsk;      // 0x0081C Device All Endpoints Interrupt Mask Register
    uint32_t dtknqr1;       // 0x00820 Device IN Token Sequence Learning Queue Read Register 1
    uint32_t dtknqr2;       // 0x00824 Device IN Token Sequence Learning Queue Read Register 2
    uint32_t dvbusdis;      // 0x00828 Device VBUS Discharge Time Register
    uint32_t dvbuspulse;    // 0x0082C Device VBUS Pulsing Time Register
    union
    {
        uint32_t dtknqr3;       // 0x00830 Device IN Token Sequence Learning Queue Read Register 3
        uint32_t dthrctl;       // 0x00830 Device Threshold Control Register
    };
    union
    {
        uint32_t dtknqr4;       // 0x00834 Device IN Token Sequence Learning Queue Read Register 4
        uint32_t diepempmsk;    // 0x00834 Device IN Endpoint FIFO Empty Interrupt Mask Register
    };
    uint32_t deachint;                  // 0x00838 Device Each Endpoint Interrupt Register
    uint32_t deachintmsk;               // 0x0083C Device Each Endpoint Interrupt Mask
    uint32_t diepeachmsk [ MAX_EP ];    // 0x00840 -> 0x0087F Device Each IN Endpoint Interrupt Register
    uint32_t doepeachmsk [ MAX_EP ];    // 0x00880 -> 0x008BF Device Each OUT Endpoint Interrupt Register

    unsigned char reserved2 [ 0x40 ];   // 0x008C0 -> 0x008FF

    struct dwc2_regs_device_dioep dioep [ 2 ] [ MAX_EP ]; // 0x00900 -> 0x00CFF Device IN/OUT Endpoint Registers

    unsigned char reserved3 [ 0x100 ];  // 0x00D00 -> 0x00DFF
};

// Power and Clock Gating CSRs
struct dwc2_regs_pcg
{
    uint32_t pcgcctl; // 0x00E00
    unsigned char reserved [ 0x1FC ]; // 0x00E04 -> 0x00FFF
};

struct dwc2_regs
{
    // Control and Status Registers (CSRs)
    struct dwc2_regs_core core;         // 0x00000 -> 0x003FF Core Global CSRs
    struct dwc2_regs_host host;         // 0x00400 -> 0x007FF Host Mode CSRs
    struct dwc2_regs_device device;     // 0x00800 -> 0x00DFF Device Mode CSRs
    struct dwc2_regs_pcg pcg;           // 0x00E00 -> 0x00FFF Power and Clock Gating CSRs

    unsigned char fifos [ MAX_CHAN ] [ 0x1000 ]; // 0x01000 -> 0x10FFF Device Endpoints / Host Channels FIFOs
    unsigned char reserved [ 0xF000 ];  // 0x11000 -> 0x1FFFF
    unsigned char dfifodbg [ 0x20000 ]; // 0x20000 -> 0x3FFFF Direct Access to Data FIFO RAM for Debugging
};

#endif
