#ifndef _H_BCM2835_MBOX
#define _H_BCM2835_MBOX

#include <stdint.h>

#define MBOX_VC_ARM 0 // VC -> ARM (Read Only)
#define MBOX_ARM_VC 1 // ARM -> VC (Write Only)

#define MBOX_FULL   ( 1 << 31 )
#define MBOX_EMPTY  ( 1 << 30 )

#define CHAN_SHIFT 4
#define CHAN_MASK 0xf

enum mbox_channel
{
    MBOX_CHAN_PM,               // Power Management
    MBOX_CHAN_FRAMEBUFFER,
    MBOX_CHAN_VIRT_UART,
    MBOX_CHAN_VCHIQ,
    MBOX_CHAN_LEDS,
    MBOX_CHAN_BUTTONS,
    MBOX_CHAN_TOUCH_SCREEN,
    MBOX_CHAN_RESERVED,
    MBOX_CHAN_PROPERTY_WRITE,   // Request ARM -> VC
    MBOX_CHAN_PROPERTY_READ,    // Request VC -> ARM (rare)
};

struct mbox_regs
{
    uint32_t rw;        // Read/Write
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t poll;      // Non-pop read
    uint32_t sender;    // Sender read (LS 2 bits only)
    uint32_t status;    // Status read
    uint32_t config;    // Config Read/Write
};

struct mbox_property_buf_hdr
{
    uint32_t size;
    uint32_t code;
};

struct mbox_property_tag_hdr
{
    uint32_t id;
    uint32_t value_buf_size;
    uint32_t code;
};

void mbox_write ( enum mbox_channel chan, uint32_t data );
uint32_t mbox_read ( enum mbox_channel chan );

#endif
