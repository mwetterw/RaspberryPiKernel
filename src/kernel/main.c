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

    sem_init ( );
    mailbox_init ( );

    scheduler_init ( );

    hardware_init ( );

    printuln ( "Bootup sequence complete! Yielding CPU to userspace..." );
    pcb_create ( init, 0 );
    scheduler_reschedule ( 0 );
}

void crash ( )
{
    printuln ( "CRASH" );
    for ( ; ; );
}
