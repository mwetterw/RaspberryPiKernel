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

static void dwc2_channel_interrupt ( uint32_t chan );
static void dwc2_prepare_channel ( uint32_t chan );
static void dwc2_start_channel ( uint32_t chan );

static mailbox_t usb_requests_mbox;

// Keep track of the number of free channels
static sem_t dwc2_free_chan_sem;

// Free channels bitmap
static uint16_t dwc2_free_chans;

// Keep track of USB req for each channel
static struct usb_request * dwc2_chan_requests [ MAX_CHAN ];

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

static void dwc2_release_chan ( uint32_t chan )
{
    uint32_t irqmask = irq_disable ( );
    dwc2_free_chans ^= ( 1 << chan );
    signal ( dwc2_free_chan_sem );
    irq_restore ( irqmask );
}

static void dwc2_complete_request ( uint32_t chan, enum usb_request_status status )
{
    struct usb_request * req = dwc2_chan_requests [ chan ];

    dwc2_release_chan ( chan );
    req -> status = status;
    usb_request_done ( req );
}

void dwc2_interrupt ( )
{
    union gint gint = regs -> core.gintsts;

    // Handle host port interrupt
    if ( gint.prtint )
    {
        dwc2_root_hub_handle_port_interrupt ( );
    }

    // Channel Interrupt
    if ( gint.hchint )
    {
        uint32_t chans = regs -> host.haint;
        uint32_t chan;
        while ( chans )
        {
            chan = 31 - __builtin_clz ( chans );
            chans ^= ( 1 << chan );
            dwc2_channel_interrupt ( chan );
        }
    }
}

static void dwc2_channel_interrupt ( uint32_t chan )
{
    union hcint hcint = regs -> host.hc [ chan ].hcint;
    union hcchar hcchar = regs -> host.hc [ chan ].hcchar;
    struct usb_request * req = dwc2_chan_requests [ chan ];

    // Acknowledge interrupts for this channel
    regs -> host.hc [ chan ].hcint = hcint;

    printu ( "Interrupt on channel: " );
    printu_32h ( chan );

    if ( hcint.xfercompl )
    {
        printu ( " xfercompl" );
    }

    if ( hcint.chhltd )
    {
        printu ( " chhltd" );
    }

    if ( hcint.ahberr )
    {
        printu ( " ahberr" );
    }

    if ( hcint.stall )
    {
        printu ( " STALL" );
    }

    if ( hcint.nak )
    {
        printu ( " NAK" );
    }

    if ( hcint.ack )
    {
        printu ( " ACK" );
    }

    if ( hcint.nyet )
    {
        printu ( " NYET" );
    }

    if ( hcint.xacterr )
    {
        printu ( " xacterr" );
    }

    if ( hcint.bblerr )
    {
        printu ( " bblerr" );
    }

    if ( hcint.frmovrun )
    {
        printu ( " frmovrun" );
    }

    if ( hcint.datatglerr )
    {
        printu ( " datatglerr" );
    }

    printuln ( 0 );

    // Error
    if ( hcint.ahberr || hcint.stall || hcint.xacterr || hcint.bblerr ||
            hcint.frmovrun || hcint.datatglerr )
    {
        printuln ( "An error occurred" );
        dwc2_complete_request ( chan, USB_STATUS_ERROR );
        return;
    }

    switch ( hcchar.eptype )
    {
        case HCCHAR_EPTYPE_CTRL:
            if ( hcint.ack )
            {
                // Complete the request when status stage has completed
                if ( req -> ctrl_stage == USB_CTRL_STAGE_STATUS )
                {
                    dwc2_complete_request ( chan, USB_STATUS_SUCCESS );
                    return;
                }

                // Go on to the next stage
                req -> ctrl_stage++;

                // Skip DATA stage if there is no data to send/receive
                if ( req -> setup_req.wLength == 0 )
                {
                    req -> ctrl_stage++;
                }

                // Start the new transaction
                dwc2_prepare_channel ( chan );
                return;
            }
            break;

        case HCCHAR_EPTYPE_ISOC:
        case HCCHAR_EPTYPE_BLK:
        case HCCHAR_EPTYPE_IRQ:
            dwc2_complete_request ( chan, USB_STATUS_NOT_SUPPORTED );
            break;
    }
}

static void dwc2_prepare_channel ( uint32_t chan )
{
    union hcchar hcchar;
    uint32_t hcsplt;
    union hctsiz hctsiz;
    void * hcdma;

    struct usb_request * req;

    hcchar.raw = 0;
    hcsplt = 0;
    hctsiz.raw = 0;

    // Fetch the request associated to the channel
    req = dwc2_chan_requests [ chan ];

    // An endpoint has been specified (not EP0)
    if ( req -> endp )
    {
        hcchar.eptype = req -> endp -> bmAttributes.transfer;
        hcchar.epnum = req -> endp -> bEndpointAddress.endp;
        hcchar.mps = req -> endp -> wMaxPacketSize.size;
    }
    // No endpoint specified: Endpoint 0 (control)
    else
    {
        hcchar.eptype = HCCHAR_EPTYPE_CTRL;
        hcchar.epnum = 0;
        hcchar.mps = req -> dev -> dev_desc.bMaxPacketSize0;
    }

    switch ( hcchar.eptype )
    {
        case HCCHAR_EPTYPE_CTRL:
            switch ( req -> ctrl_stage )
            {
                /* USB 2.0 Section 8.5.3
                 * During the Setup stage, a SETUP transaction is used to
                 * transmit information to the control endpoint of a function.
                 * SETUP transactions are similar in format to an OUT but use a
                 * SETUP rather than an OUT PID. [...] A SETUP always uses a
                 * DATA0 PID for the data field of the SETUP transaction. The
                 * function receiving a SETUP must accept the SETUP data and
                 * respond with ACK; if the data is corrupted, discard the data
                 * and return no handshake. */
                // [SETUP DATA0 ACK]
                case USB_CTRL_STAGE_SETUP:
                    hcchar.epdir = HCCHAR_EPDIR_OUT;
                    hctsiz.pid = HCTSIZ_PID_MDATA_SETUP;
                    hctsiz.xfersize = sizeof ( req -> setup_req );
                    hcdma = &( req -> setup_req );
                    break;

                /* USB 2.0 Section 8.5.3
                 * "The Data Stage, if present, of a control transfer
                 * consists of one or more IN or OUT transactions and
                 * follows the same protocol rules as bulk transfers. All
                 * the transactions in the Data stage must be in the same
                 * direction (i.e., all INs or OUTs). The amount of data to
                 * be sent during the data stage and its direction are
                 * specified during the Setup stage. If the amount of data
                 * exceeds the prenegociated data packet size, the data is
                 * sent in multiple transactions (INs or OUTs) that carry
                 * the maximum packet size. Any remaining data is sent as
                 * residual in the last transaction." */
                // [IN DATA1 ACK] [IN DATA0 ACK]...
                case USB_CTRL_STAGE_DATA:
                    hcchar.epdir = req -> setup_req.bmRequestType.dir;
                    hctsiz.pid = HCTSIZ_PID_DATA1;
                    hctsiz.xfersize = req -> size;
                    hcdma = req -> data;
                    break;

                /* USB 2.0 Section 8.5.3
                 * "A Status stage is delineated by a change in direction of data
                 * flow from the previous stage and always uses a DATA1 PID." */
                // [OUT DATA1(ZLP) ACK]
                case USB_CTRL_STAGE_STATUS:
                    hcchar.epdir = ! ( req -> setup_req.bmRequestType.dir );
                    hctsiz.pid = HCTSIZ_PID_DATA1;
                    hctsiz.xfersize = 0;
                    hcdma = 0;
                    break;
            }
            break;

        default:
            dwc2_complete_request ( chan, USB_STATUS_NOT_SUPPORTED );
            return;
    }

    // Set device address
    hcchar.devaddr = req -> dev -> addr;

    // To be set only when communicating with a LS device through a FS-hub
    hcchar.lspddev = 0;

    hcchar.mcec = 1;

    hctsiz.dopng = 0;

    /* This field is programmed by the application with the expected number of
     * packets to be TX (OUT) or RX (IN). The host decrements this count on
     * every succesful transmission or reception of an OUT/IN packet. Once this
     * count reaches zero, the application is interrupted to indicate normal
     * completion. */
    // (x + y - 1)/y <=> ceil(x/y) for positive integers (WARNING overflow)
    hctsiz.pktcnt = ( hctsiz.xfersize + hcchar.mps - 1 ) / hcchar.mps;
    // We need to cap pktcnt to a minimum of 1, even for ZLPs
    if ( hctsiz.pktcnt == 0 )
    {
        hctsiz.pktcnt = 1;
    }

    // Program the channel
    regs -> host.hc [ chan ].hcsplt = hcsplt;
    regs -> host.hc [ chan ].hcchar = hcchar;
    regs -> host.hc [ chan ].hctsiz = hctsiz;
    regs -> host.hc [ chan ].hcdma = ( uintptr_t ) hcdma;

    // Transmit!
    dwc2_start_channel ( chan );
}

static void dwc2_start_channel ( uint32_t chan )
{
    union hcchar hcchar;
    hcchar = regs -> host.hc [ chan ].hcchar;

    // To be set only for periodic transfers
    hcchar.oddfrm = 0;

    // Start xfer!
    hcchar.chena = 1;
    regs -> host.hc [ chan ].hcchar = hcchar;
}

static void dwc2_real_request ( struct usb_request * req )
{
    uint32_t chan;

    chan = dwc2_get_free_chan ( );
    printu ( "Processing real USB Request" );
    printu ( " Free chan is " );
    printu_32h ( chan );
    printuln ( 0 );

    dwc2_chan_requests [ chan ] = req;
    dwc2_prepare_channel ( chan );
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
    union hcint hcint;

    // Host channel specific interrupts
    for ( int ch = 0 ; ch < hwcfg.chancount ; ++ch )
    {
        // Clear interrupts
        hcint = regs -> host.hc [ ch ].hcint;
        regs -> host.hc [ ch ].hcint = hcint;

        // Setup mask
        hcint.raw = 0;
        hcint.chhltd = 1;
        regs -> host.hc [ ch ].hcintmsk = hcint;
    }

    // Global host channels interrupts
    {
        uint32_t haint;

        // Clear interrupts
        haint = regs -> host.haint;
        regs -> host.haint = haint;

        // Enable interrupt for all channels
        regs -> host.haintmsk = ( 1 << hwcfg.chancount ) - 1;
    }

    // Core interrupts
    {
        union gint gint;

        // Clear interrupts
        gint = regs -> core.gintsts;
        regs -> core.gintsts = gint;

        // Setup mask
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

    // Mark all channels as free
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
    return dwc2_start_usb_consumer_thread ( );
}

void hcd_stop ( )
{
    printuln ( "HCD Stop" );
}

void hcd_submit_request ( struct usb_request * req )
{
    mailbox_send ( usb_requests_mbox, ( intptr_t ) req );
}
