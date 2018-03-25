#include "memory.h"
#include "hardware.h"
#include "semaphore.h"
#include "mailbox.h"
#include "scheduler.h"
#include "pcb.h"
#include "bcm2835/uart.h"

void init ( );

void kernel_main ( uint32_t z, uint32_t mach, uint32_t atags )
{
    ( void ) z; ( void ) mach; ( void ) atags;

    memory_init ( );
    hardware_init ( );

    sem_init ( );
    mailbox_init ( );

    printu ( "Initializing Scheduler" );
    scheduler_init ( );

    pcb_create ( init, 0 );

    printu ( "Bootup sequence complete! Yielding CPU to userspace..." );
    scheduler_reschedule ( 0 );
}
