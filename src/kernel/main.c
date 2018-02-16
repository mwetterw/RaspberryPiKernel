#include "memory.h"
#include "hardware.h"
#include "scheduler.h"
#include "pcb.h"
#include "bcm2835/uart.h"
#include "bcm2835/systimer.h"

void init ( );

void __attribute__ ( ( noreturn ) ) kernel_main ( uint32_t z, uint32_t mach, uint32_t atags )
{
    kernel_memory_init ( );
    kernel_hardware_init ( );

    printu ( "Initializing Timer" );
    systimer_init ( );

    printu ( "Initializing Scheduler" );
    kernel_scheduler_init ( );

    kernel_pcb_create ( init, 0 );

    printu ( "Bootup sequence complete! Yielding CPU to userspace..." );
    kernel_scheduler_yield_noreturn ( );
}
