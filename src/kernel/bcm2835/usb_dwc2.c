#include "usb_dwc2_regs.h"

#include "bcm2835.h"
#include "power.h"
#include "pic.h"
#include "uart.h"

#include "../usb_hcdi.h"
#include "../mailbox.h"
#include "../semaphore.h"
#include "../arm.h"

#include "../../api/process.h"
#include "../../libc/math.h"

static struct dwc2_regs volatile * regs = ( struct dwc2_regs volatile * ) USB_HCD_BASE;

extern void dwc2_root_hub_request ( struct usb_request * req );
extern void dwc2_root_hub_handle_port_interrupt ( );

static mailbox_t usb_requests_mbox;

// Keep track of the number of free channels
static sem_t dwc2_free_chan_sem;

// Free channels bitmap
static uint16_t dwc2_free_chans;

// This struct holds various Read-Only register values
static struct hwcfg
{
    union gsnpsid gsnpsid;
    uint32_t guid;

    uint32_t ghwcfg1;
    union ghwcfg2 ghwcfg2;
    union ghwcfg3 ghwcfg3;
    uint32_t ghwcfg4;

    /* Following registers are not Read-Only, but their power-on values have to
     * be memorized as they indicate the maximum individual FIFOs sizes. */
    union grxf grxf;
    union txf gnptxf;
    union txf hptxf;

    int chancount;
} hwcfg;

void dwc2_interrupt ( )
{
    union gint gint = regs -> core.gintsts;

    // Handle host port interrupt
    if ( gint.prtint )
    {
        dwc2_root_hub_handle_port_interrupt ( );
    }
}

static uint32_t dwc2_get_free_chan ( )
{
    uint32_t chan;
    uint32_t irqmask;

    irqmask = irq_disable ( );
    wait ( dwc2_free_chan_sem );
    chan = 31 - __builtin_clz ( dwc2_free_chans );
    dwc2_free_chans ^= ( 1 << chan );
    irq_restore ( irqmask );

    return chan;
}

static void __attribute__ ( ( unused ) ) dwc2_release_chan ( uint32_t chan )
{
    uint32_t irqmask = irq_disable ( );
    dwc2_free_chans ^= ( 1 << chan );
    signal ( dwc2_free_chan_sem );
    irq_restore ( irqmask );
}

static void dwc2_real_request ( struct usb_request * req )
{
    uint32_t chan;


    chan = dwc2_get_free_chan ( );
    printu ( "Processing real USB Request" );
    printu ( " Free chan is " );
    printu_32h ( chan );
    printuln ( 0 );

    req -> status = USB_STATUS_NOT_SUPPORTED;
    usb_request_done ( req );
}

#define NB_FIFOS 3
struct fifos
{
    uint32_t * size;
    uint32_t max;
};
typedef struct fifos fifos_t [ NB_FIFOS ];

static void resize_fifos ( fifos_t fifos, uint32_t remaining_space )
{
    uint32_t remaining_fifos = NB_FIFOS;

    while ( remaining_space > 0 )
    {
        // Compute an equal share to give to every still-hungry FIFO
        uint32_t equal_share = max ( remaining_space / remaining_fifos, 1 );

        for ( int i = 0 ; i < NB_FIFOS ; ++i )
        {
            uint32_t until_full = fifos [ i ].max - * ( fifos [ i ].size );

            // Skip FIFO which has already eaten until full
            if ( until_full == 0 )
            {
                continue;
            }

            // FIFO is still hungry: feed it
            uint32_t eat = min ( equal_share, until_full );
            * ( fifos [ i ].size ) += eat;

            // FIFO is satisfied and optimum
            if ( * ( fifos [ i ].size ) == fifos [ i ].max )
            {
                remaining_fifos--;
            }

            remaining_space -= eat;

            // No food left! Good job! =) Wasting is not good...
            if ( remaining_space == 0 )
            {
                break;
            }
        }
    }
}

static void dwc2_setup_fifos ( )
{
    // We can only resize the FIFOs if dynamic resizing is available
    if ( ! hwcfg.ghwcfg2.dynfifosizing )
    {
        return;
    }

    // Fetch the total DFIFO size available to accommodate all 3 FIFOs
    uint32_t dfifodepth = hwcfg.ghwcfg3.dfifodepth;

    uint32_t grxfsiz = 0, gnptxfsiz = 0, hptxfsiz = 0;

    // FIFOs exceed DFIFO size. We need to resize them.
    if ( dfifodepth < ( uint32_t )
            ( hwcfg.grxf.siz + hwcfg.gnptxf.siz + hwcfg.hptxf.siz ) )
    {
        printuln ( "FIFOs don't fit into DFIFO. Resizing..." );

        /* These power-on values (in hwcfg struct) represent the maximum size
         * for each FIFO. They must never be exceeded (but we can set less). */
        fifos_t fifos = {
            { & grxfsiz, hwcfg.grxf.siz },
            { & gnptxfsiz, hwcfg.gnptxf.siz },
            { & hptxfsiz, hwcfg.hptxf.siz },
        };
        resize_fifos ( fifos, dfifodepth );
    }
    // Else, just use the maximum size for each of them because it will fit!
    else
    {
        grxfsiz = hwcfg.grxf.siz;
        gnptxfsiz = hwcfg.gnptxf.siz;
        hptxfsiz = hwcfg.hptxf.siz;
    }

    union grxf grxf;
    union txf gnptxf, hptxf;

    grxf.raw = 0; // Necessary because of the reserved bits
    grxf.siz = grxfsiz;
    gnptxf.siz = gnptxfsiz;
    hptxf.siz = hptxfsiz;

    /* Sizes have been determined. Now compute and set up the FIFO pointers. :)
     * Note that the RX FIFO's pointer can't be set as it is always zero.
     * -------- * <--- Pointer always at 0x0 (cannot be set or changed)
     *   grxf   *
     * -------  * <--- Pointer at grxfsiz
     * gnptxf   *
     * -------- * <--- Pointer at (grxfsiz + gnptxfsiz)
     *  hptxf   *
     * * * * * * */
    gnptxf.addr = grxf.siz;
    hptxf.addr = ( grxf.siz + gnptxf.siz );

    // Commit!
    regs -> core.grxf = grxf;
    regs -> core.gnptxf = gnptxf;
    regs -> core.hptxf = hptxf;
}

static void dwc2_enable_dma ( )
{
    union gahbcfg gahbcfg = regs -> core.gahbcfg;

    // Special case for the Broadcom 2708 instance of the core
    if ( hwcfg.guid == BCM2708_GUID )
    {
        gahbcfg.hbstlen |= BCM2708_AXI_WAIT;
    }

    // Enable DMA
    gahbcfg.dmaen = 1;

    regs -> core.gahbcfg = gahbcfg;
}

static void dwc2_setup_interrupts ( )
{
    // Clear host channel specific interrupts
    for ( int ch = 0 ; ch < hwcfg.chancount ; ++ch )
    {
        regs -> host.hc [ ch ].hcintmsk.raw = 0;
        regs -> host.hc [ ch ].hcint.raw = ~0;
    }

    // Clear global host channels interrupts
    regs -> host.haintmsk = 0;
    regs -> host.haint = ( ~0 );

    // Clear core interrupts
    regs -> core.gintmsk.raw = 0;
    regs -> core.gintsts.raw = ~0;


    // Enable core interrupts
    {
        union gint gint;
        gint.raw = 0;
        gint.hchint = 1;
        gint.prtint = 1;
        regs -> core.gintmsk = gint;
    }

    // Enable global interrupt generation
    {
        union gahbcfg gahbcfg = regs -> core.gahbcfg;
        gahbcfg.glblintrmsk = 1;
        regs -> core.gahbcfg = gahbcfg;
    }

    // Route the top level interrupt to CPU
    interrupt_handlers [ IRQ_USB_HCD ] = dwc2_interrupt;
    pic_enable_irq ( IRQ_USB_HCD );
}

static void dwc2_reset ( )
{
    union grstctl rst;
    rst.raw = 0;

    // Perform the reset
    rst.csftrst = 1;
    regs -> core.grstctl = rst;

    // Wait until reset is complete and AHB is idle
    do
    {
        rst = regs -> core.grstctl;
    }
    while ( ( rst.csftrst ) || ! ( rst.ahbidle ) );

    /* Just after reset, the controller temporarily transitions to device mode.
     * We need to wait until it comes back to host mode.
     * This is important as some SPRAM registers will be auto-updated. */
    while ( regs -> core.gintsts.curmod != HOST_MODE );
}

static void dwc2_parse_config ( )
{
    hwcfg.gsnpsid = regs -> core.gsnpsid;
    hwcfg.guid = regs -> core.guid;

    hwcfg.ghwcfg1 = regs -> core.ghwcfg1;
    hwcfg.ghwcfg2 = regs -> core.ghwcfg2;
    hwcfg.ghwcfg3 = regs -> core.ghwcfg3;
    hwcfg.ghwcfg4 = regs -> core.ghwcfg4;

    /* These power-on values represent the maximum size for each FIFO.
     * Will be useful to resize the FIFOs */
    hwcfg.grxf = regs -> core.grxf;
    hwcfg.gnptxf = regs -> core.gnptxf;
    hwcfg.hptxf = regs -> core.hptxf;

    hwcfg.chancount = ( hwcfg.ghwcfg2.numhstchnl ) + 1;
}

static int dwc2_is_host_capable ( )
{
    return hwcfg.ghwcfg2.otgmode != OTGMODE_DEV_SRP &&
            hwcfg.ghwcfg2.otgmode != OTGMODE_DEV_NO_SRP &&
            hwcfg.ghwcfg2.otgmode < OTGMODE_RESERVED;
}

static int dwc2_probe ( )
{
    if ( hwcfg.gsnpsid.product != DWC2_PRODUCT_ID )
    {
        printuln ( "This is not a Synopsys DWC2 USB 2.0 OTG Controller!" );
        return 0;
    }

    if ( ! dwc2_is_host_capable ( ) )
    {
        printuln ( "This release of the DWC2 is not host-capable." );
        return 0;
    }

    if ( hwcfg.gsnpsid.version != VERSION_2_80A )
    {
        printuln ( "Warning: This release of the DWC2 is untested." );
    }

    if ( hwcfg.guid != BCM2708_GUID )
    {
        printuln ( "Warning: This instance (non-BCM2708) of the DWC2 is untested." );
    }

    return 1;
}

static void dwc2_usb_consumer_thread ( )
{
    for ( ; ; )
    {
        struct usb_request * req =
            ( void * ) ( long ) mailbox_recv ( usb_requests_mbox );

        if ( usb_dev_is_root ( req -> dev ) )
        {
            dwc2_root_hub_request ( req );
        }
        else
        {
            dwc2_real_request ( req );
        }
    }
}

static int dwc2_start_usb_consumer_thread ( )
{
    if ( ( usb_requests_mbox = mailbox_create ( 512 ) ) < 0 )
    {
        return -1;
    }

    if ( ( dwc2_free_chan_sem = sem_create ( hwcfg.chancount ) ) < 0 )
    {
        mailbox_destroy ( usb_requests_mbox );
        return -1;
    }

    dwc2_free_chans = ( 1 << hwcfg.chancount ) - 1;

    api_process_create ( dwc2_usb_consumer_thread, 0 );

    return 0;
}

int hcd_start ( )
{
    printuln ( "Starting up Synopsys DesignWare USB 2.0 OTG Controller" );

    // Ask the GPU to power the USB controller on
    power_device ( POWER_USB_HCD, POWER_ON );

    // Ask the core to reset and wait until complete
    dwc2_reset ( );

    // Fetch the specific factory configuration of the chip
    dwc2_parse_config ( );

    // Check whether we are able to drive this device
    if ( ! dwc2_probe ( ) )
    {
        return -1;
    }

    dwc2_setup_fifos ( );
    dwc2_enable_dma ( );
    dwc2_setup_interrupts ( );

    // Start the USB consumer thread
    int ret = dwc2_start_usb_consumer_thread ( );
    if ( ret != 0 )
    {
        power_device ( POWER_USB_HCD, POWER_OFF );
    }

    return ret;
}

void hcd_stop ( )
{
    printuln ( "HCD Stop" );
}

void hcd_submit_request ( struct usb_request * req )
{
    mailbox_send ( usb_requests_mbox, ( intptr_t ) req );
}
