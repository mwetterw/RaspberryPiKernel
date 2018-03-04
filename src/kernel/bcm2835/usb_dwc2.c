#include "usb_dwc2_regs.h"

#include "bcm2835.h"
#include "../usb_hcdi.h"
#include "power.h"
#include "pic.h"
#include "uart.h"

static struct dwc2_regs volatile * regs = ( struct dwc2_regs volatile * ) USB_HCD_BASE;

static void dwc2_reset ( );
static void dwc2_setup_fifos ( );
static void dwc2_setup_interrupts ( );
static void dwc2_parse_config ( );

static uint32_t CHANCOUNT;

#define NB_FIFOS 3

void hcd_start ( )
{
    // Ask the GPU to power the USB controller on
    power_device ( POWER_USB_HCD, POWER_ON );

    // Ask the core to reset and wait until complete
    dwc2_reset ( );

    // Fetch the specific factory configuration of the chip
    dwc2_parse_config ( );

    dwc2_setup_fifos ( );

    dwc2_setup_interrupts ( );
}

void dwc2_interrupt ( )
{
    printu ( "USB IRQ" );
}

struct fifos
{
    uint32_t * size;
    uint32_t max;
};

static void resize_fifos ( struct fifos fifos [ NB_FIFOS ], uint32_t dfifodepth )
{
    uint32_t remaining_fifos = NB_FIFOS;
    uint32_t remaining_space = dfifodepth;

    while ( remaining_space > 0 )
    {
        // Compute an equal share to give to every still-hungry FIFO
        uint32_t equal_share = remaining_space / remaining_fifos;

        /* If the last 2 FIFOs are racing to get the last rice grain,
         * just give it to the first and prevent infinite 1/2 = 0 */
        equal_share = ( equal_share == 0 ) ? 1 : equal_share;

        for ( int i = 0 ; i < NB_FIFOS ; ++i )
        {
            // Skip FIFO which has already eaten until full
            if ( fifos [ i ].size == 0 )
            {
                continue;
            }

            // FIFO is still hungry: try to give it an equal share
            * ( fifos [ i ].size ) += equal_share;

            uint32_t greediness = 0;

            // FIFO is full, and has even eaten too much!
            if ( * ( fifos [ i ].size ) > fifos [ i ].max )
            {
                // Find out in what extent FIFO has eaten too much
                greediness = * ( fifos [ i ].size ) - fifos [ i ].max;

                // Cap to max to give opportunity to other still-hungry FIFOs
                * ( fifos [ i ].size ) = fifos [ i ].max;

                // Mark FIFO as full and satisfied!
                fifos [ i ].size = 0;
                remaining_fifos--;
            }

            remaining_space -= equal_share - greediness;

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

        struct fifos fifos [ NB_FIFOS ] = {
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
    union gnptxf gnptxf;
    union hptxf hptxf;

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
    regs -> core.gahbcfg |= 1;

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
}
