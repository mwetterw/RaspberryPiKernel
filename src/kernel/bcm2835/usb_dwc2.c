#include "usb_dwc2_regs.h"

#include "bcm2835.h"
#include "../usb_hcdi.h"
#include "power.h"
#include "pic.h"
#include "uart.h"
#include "../../libc/math.h"
#include "../arm.h"

static struct dwc2_regs volatile * regs = ( struct dwc2_regs volatile * ) USB_HCD_BASE;

static uint32_t CHANCOUNT;
static int IS_BCM2708_INSTANCE;

static void __attribute__ ( ( unused ) ) dwc2_root_hub_reset_port ( )
{
    // Read the Host Port Control & Status Register
    union hprt hprt = regs -> host.hprt;

    // Zero the "Write 1 to Clear" bits so that we don't accidentally clear them
    hprt.raw &= ~DWC2_HPRT_WC_MASK;

    // Assert reset on the port
    hprt.prtrst = 1;
    regs -> host.hprt = hprt;

    // Wait the required delay
    cdelay ( 350000 );

    // De-assert reset on the port
    hprt.prtrst = 0;
    regs -> host.hprt = hprt;
}

static void __attribute__ ( ( unused ) ) dwc2_root_hub_power_on_port ( )
{
    // Read the Host Port Control & Status Register
    union hprt hprt = regs -> host.hprt;

    // Zero the "Write 1 to Clear" bits so that we don't accidentally clear them
    hprt.raw &= ~DWC2_HPRT_WC_MASK;

    // Power on port
    hprt.prtpwr = 1;

    // Commit
    regs -> host.hprt = hprt;
}

void dwc2_interrupt ( )
{
    printu ( "USB IRQ" );
    union gint gint = regs -> core.gintsts;

    // Handle host port interrupt
    if ( gint.prtint )
    {
        printu ( " Port IRQ" );

        union hprt hprt_r, hprt_w;
        hprt_r = hprt_w = regs -> host.hprt;
        hprt_w.raw &= DWC2_HPRT_WC_MASK;

        if ( hprt_r.prtconndet )
        {
            printu ( "  Port Connection Detect IRQ" );
            hprt_w.prtconndet = 1;
        }

        if ( hprt_r.prtenchng )
        {
            printu ( "  Port Enable Change IRQ" );
            hprt_w.prtenchng = 1;
        }

        if ( hprt_r.prtovrcurchng )
        {
            printu ( "  Port Overcurrent Change IRQ" );
            hprt_w.prtovrcurchng = 1;
        }

        regs -> host.hprt = hprt_w;
    }
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
    if ( ! ( regs -> core.ghwcfg2.dynfifosizing ) )
    {
        return;
    }

    // Fetch the total DFIFO size available to accommodate all 3 FIFOs
    uint32_t dfifodepth = regs -> core.ghwcfg3.dfifodepth;

    /* These power-on values represent the maximum size for each FIFO.
     * They must never be exceeded (but we can set less). */
    uint32_t max_grxfsiz =  regs -> core.grxf.siz;
    uint32_t max_gnptxfsiz = regs -> core.gnptxf.siz;
    uint32_t max_hptxfsiz = regs -> core.hptxf.siz;

    uint32_t grxfsiz = 0, gnptxfsiz = 0, hptxfsiz = 0;

    // FIFOs exceed DFIFO size. We need to resize them.
    if ( dfifodepth <
            ( uint32_t ) ( max_grxfsiz + max_gnptxfsiz + max_hptxfsiz ) )
    {
        printu ( "FIFOs don't fit into DFIFO. Resizing..." );

        fifos_t fifos = {
            { & grxfsiz, max_grxfsiz },
            { & gnptxfsiz, max_gnptxfsiz },
            { & hptxfsiz, max_hptxfsiz },
        };
        resize_fifos ( fifos, dfifodepth );
    }
    // Else, just use the maximum size for each of them because it will fit!
    else
    {
        grxfsiz = max_grxfsiz;
        gnptxfsiz = max_gnptxfsiz;
        hptxfsiz = max_hptxfsiz;
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

    printu_32h ( regs -> core.grxf.raw );
    printu_32h ( regs -> core.gnptxf.raw );
    printu_32h ( regs -> core.hptxf.raw );
}

static void dwc2_enable_dma ( )
{
    union gahbcfg gahbcfg = regs -> core.gahbcfg;

    if ( IS_BCM2708_INSTANCE )
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
    for ( uint32_t ch = 0 ; ch < CHANCOUNT ; ++ch )
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
    CHANCOUNT = ( regs -> core.ghwcfg2.numhstchnl ) + 1;
    IS_BCM2708_INSTANCE = ( regs -> core.guid == 0x2708A000 );
}

void hcd_start ( )
{
    printu ( "Starting up Synopsys Designware USB 2.0 OTG Controller" );
    // Ask the GPU to power the USB controller on
    power_device ( POWER_USB_HCD, POWER_ON );

    // Ask the core to reset and wait until complete
    dwc2_reset ( );

    // Fetch the specific factory configuration of the chip
    dwc2_parse_config ( );

    dwc2_setup_fifos ( );
    dwc2_enable_dma ( );
    dwc2_setup_interrupts ( );
}
