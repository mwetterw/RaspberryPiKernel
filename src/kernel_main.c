#include "kernel_memory.h"
void __attribute__((noreturn, naked)) kernel_main ( )
{
	kernel_memory_init ( );

	for ( ; ; );

	__builtin_unreachable ( );
}
