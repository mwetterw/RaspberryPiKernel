#include "watchdog.h"
#include "bcm2835.h"

struct watchdog_regs
{
    uint32_t reserved [ 7 ];
    uint32_t rstc;              // 0x1C
    uint32_t rsts;              // 0x20
    uint32_t wdog;              // 0x24
};

#define WATCHDOG_PWD 0x5a000000
#define WATCHDOG_TIMER_MASK 0xfffff

#define WATCHDOG_RSTC_CLRCFG 0xffffffcf

#define WATCHDOG_RSTC_FULL_RESET ( 1 << 5 )
#define WATCHDOG_RSTC_STOP 0x102

static volatile struct watchdog_regs * watchdog =
    ( volatile struct watchdog_regs * ) WATCHDOG_BASE;

void watchdog_start ( uint32_t timeout )
{
    // Set the watchdog timer
    watchdog -> wdog = ( WATCHDOG_PWD | ( timeout & WATCHDOG_TIMER_MASK ) );

    // Program the watchdog to do a full system reset upon timer expiration
    uint32_t cur = watchdog -> rstc & WATCHDOG_RSTC_CLRCFG;
    watchdog -> rstc = ( cur | WATCHDOG_PWD | WATCHDOG_RSTC_FULL_RESET );
}

void watchdog_stop ( )
{
    watchdog -> rstc = ( WATCHDOG_PWD | WATCHDOG_RSTC_STOP );
}
