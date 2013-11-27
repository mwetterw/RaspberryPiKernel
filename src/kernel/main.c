#include "memory.h"
#include "scheduler.h"
#include "hardware.h"

void __attribute__ ( ( noreturn, naked ) ) kernel_main ( )
{
	kernel_memory_init ( );
	kernel_hardware_init ( );
	kernel_scheduler_init ( );

	for ( ; ; );

	__builtin_unreachable ( );
}
