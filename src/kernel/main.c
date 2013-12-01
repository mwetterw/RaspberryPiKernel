#include "memory.h"
#include "hardware.h"
#include "timer.h"
#include "scheduler.h"
#include "pcb.h"

void init ( );

void __attribute__ ( ( noreturn, naked ) ) kernel_main ( )
{
	kernel_memory_init ( );
	kernel_hardware_init ( );
	kernel_timer_init ( );
	kernel_scheduler_init ( );

	kernel_pcb_create ( init, 0 );

	kernel_scheduler_yield_noreturn ( );
}
