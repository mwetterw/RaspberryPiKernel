#ifndef _H_BCM2835_DWC2_REGS
#define _H_BCM2835_DWC2_REGS

#include <stdint.h>

// Core Global CSRs
struct dwc2_regs_core
{
    uint32_t gotgctl;           // 0x00000 OTG Control and Status Register
    uint32_t gotgint;           // 0x00004 OTG Interrupt Register
    uint32_t gahbcfg;           // 0x00008 AHB Configuration Register
    uint32_t gusbcfg;           // 0x0000C USB Configuration Register
    uint32_t grstctl;           // 0x00010 Reset Control Register
    uint32_t gintsts;           // 0x00014 Interrupt Status Register
    uint32_t gintmsk;           // 0x00018 Interrupt Mask Register
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
    uint32_t ghwcfg2;           // 0x00048 User Hardware Config2 Register
    uint32_t ghwcfg3;           // 0x0004C User Hardware Config3 Register
    uint32_t ghwcfg4;           // 0x00050 User Hardware Config4 Register
    uint32_t glpmcfg;           // 0x00054 Core LPM Configuration Register
    uint32_t gpwrdn;            // 0x00058 Power Down Register
    uint32_t gdfifocfg;         // 0x0005C DFIFO Software Configuration Register
    uint32_t gadpctl;           // 0x00060 ADP Timer Control and Status Register

    unsigned char reserved1 [ 0x9C ];

    uint32_t hptxfsiz;          // 0x00100 Host Periodic Transmit FIFO Size Register
    uint32_t dieptxfn [ 15 ];   // 0x00104 -> 0x0013f Device IN Endpoint Transmit FIFO Size Register

    unsigned char reserved2 [ 0x2C0 ]; // 0x00140 -> 0x003FF
};

// Host Channel-Specific Registers
struct dwc2_regs_host_hc
{
    uint32_t hcchar;    // Host Channel Characteristics Register
    uint32_t hcsplt;    // Host Channel Split Control Register
    uint32_t hcint;     // Host Channel Interrupt Register
    uint32_t hcintmsk;  // Host Channel Interrupt Mask Register
    uint32_t hcsiz;     // Host Channel Transfer Size Register
    uint32_t hcdma;     // Host Channel DMA Address Register
    uint32_t reserved1; // Is this just after hcdmab?
    uint32_t hcdmab;    // Host Channel DMA Buffer Address Register
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

    uint32_t hprt;      // 0x00440 Host Port Control and Status Register

    unsigned char reserved3 [ 0xBC ];

    struct dwc2_regs_host_hc hc [ 16 ]; // 0x00500 -> 0x006FF Host Channel-Specific Registers

    unsigned char reserved4 [ 0x100 ];  // 0x00700 -> 0x007FF
};

// Device Mode CSRs
struct dwc2_regs_device
{
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
    struct dwc2_regs_core core;             // 0x00000 -> 0x003FF Core Global CSRs
    struct dwc2_regs_host host;             // 0x00400 -> 0x007FF Host Mode CSRs
    struct dwc2_regs_device device;         // 0x00800 -> 0x00DFF Device Mode CSRs
    struct dwc2_regs_pcg pcg;               // 0x00E00 -> 0x00FFF Power and Clock Gating CSRs

    unsigned char fifos [ 16 ] [ 0x1000 ];  // 0x01000 -> 0x10FFF Device Endpoints / Host Channels FIFOs
    unsigned char reserved [ 0xF000 ];      // 0x11000 -> 0x1FFFF
    unsigned char dfifodbg [ 0x20000 ];     // 0x20000 -> 0x3FFFF Direct Access to Data FIFO RAM for Debugging
};

#endif
