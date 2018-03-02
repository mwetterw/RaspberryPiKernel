#include "usb_dwc2_regs.h"
#include "bcm2835.h"
#include "../usb_hcdi.h"
#include "power.h"

static struct dwc2_regs volatile * regs = ( struct dwc2_regs volatile * ) USB_HCD_BASE;

static void dwc2_reset ( );

void hcd_start ( )
{
    // Ask the GPU to power the USB controller on
    power_device ( POWER_USB_HCD, POWER_ON );

    // Ask the core to reset and wait until complete
    dwc2_reset ( );

    // Force Host Mode
    regs -> core.gusbcfg = ( 1 << 29 );

    // Enable Host Port
    regs -> host.hprt = ( 1 << 12 );
}

static void dwc2_reset ( )
{
    regs -> core.grstctl = 1;
    while ( ( regs -> core.grstctl ) & 1 );
}
