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

    unsigned char reserved2 [ 0x2c0 ];
};

// Host Mode CSRs
struct dwc2_regs_host
{
};

// Device Mode CSRs
struct dwc2_regs_device
{
};

// Power and Clock Gating CSRs
struct dwc2_regs_pcg
{
    uint32_t pcgcctl; // 0x00e00
    unsigned char reserved [ 0x1fc ]; // 0x00e04 -> 0x00fff
};

struct dwc2_regs
{
    // Control and Status Registers (CSRs)
    struct dwc2_regs_core core;             // 0x00000 -> 0x003ff Core Global CSRs
    struct dwc2_regs_host host;             // 0x00400 -> 0x007ff Host Mode CSRs
    struct dwc2_regs_device device;         // 0x00800 -> 0x00dff Device Mode CSRs
    struct dwc2_regs_pcg pcg;               // 0x00e00 -> 0x00fff Power and Clock Gating CSRs

    unsigned char fifos [ 16 ] [ 0x1000 ];  // 0x01000 -> 0x10fff Device Endpoints / Host Channels FIFOs
    unsigned char reserved [ 0xf000 ];      // 0x11000 -> 0x1ffff
    unsigned char dfifodbg [ 0x20000 ];     // 0x20000 -> 0x3ffff Direct Access to Data FIFO RAM for Debugging
};

#endif
