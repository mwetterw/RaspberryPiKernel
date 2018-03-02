#include "usb_dwc2_regs.h"

#include "bcm2835.h"
#include "../usb_hcdi.h"
#include "power.h"
#include "pic.h"
#include "uart.h"

static struct dwc2_regs volatile * regs = ( struct dwc2_regs volatile * ) USB_HCD_BASE;

static void dwc2_reset ( );
static void dwc2_setup_interrupts ( );
static void dwc2_parse_config ( );

static uint32_t CHANCOUNT;

void hcd_start ( )
{
    // Ask the GPU to power the USB controller on
    power_device ( POWER_USB_HCD, POWER_ON );

    // Ask the core to reset and wait until complete
    dwc2_reset ( );

    // Fetch the specific factory configuration of the chip
    dwc2_parse_config ( );

    // Force Host Mode
    regs -> core.gusbcfg = ( 1 << 29 );

    dwc2_setup_interrupts ( );
}

void dwc2_interrupt ( )
{
    printu ( "USB IRQ" );
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
    regs -> core.grstctl = 1;
    while ( ( regs -> core.grstctl ) & 1 );
}

static void dwc2_parse_config ( )
{
    CHANCOUNT = ( regs -> core.ghwcfg2.numhstchnl ) + 1;
}
