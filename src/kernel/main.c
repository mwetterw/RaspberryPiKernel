#include "memory.h"
#include "hardware.h"
#include "timer.h"
#include "scheduler.h"
#include "pcb.h"
#include "bcm2835/uart.h"

void init ( );

void __attribute__ ( ( noreturn, naked ) ) kernel_main ( )
{
    kernel_memory_init ( );
    kernel_hardware_init ( );

    printu ( "Initializing Timer" );
    kernel_timer_init ( );

    printu ( "Initializing Scheduler" );
    kernel_scheduler_init ( );

    kernel_pcb_create ( init, 0 );

    printu ( "Bootup sequence complete! Yielding CPU to userspace..." );
    kernel_scheduler_yield_noreturn ( );
}
