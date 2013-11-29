#include "scheduler.h"
#include "pcb.h"

static kernel_pcb_t * kernel_pcb_running;
static kernel_pcb_turnstile_t kernel_turnstile_round_robin;
static kernel_pcb_t kernel_pcb_idle;

static void kernel_scheduler_elect ( );

static void __attribute__ ( ( noreturn, naked ) ) kernel_idle_process ( );


void kernel_scheduler_init ( )
{
	kernel_pcb_idle.mpStack = 0;
	kernel_pcb_idle.mpSP = ( void * ) 0x3000;
	kernel_pcb_set_register ( &kernel_pcb_idle, pc, kernel_idle_process );
	kernel_pcb_inherit_cpsr ( &kernel_pcb_idle );
	kernel_pcb_enable_irq ( &kernel_pcb_idle );

	kernel_pcb_turnstile_init ( &kernel_turnstile_round_robin );
	kernel_pcb_running = 0;
}

void kernel_scheduler_yield_noreturn ( )
{
	kernel_scheduler_elect ( );
	__asm ( "mov sp, %0" : : "r" ( kernel_pcb_running -> mpSP ) );
	__asm ( "ldr r0, [sp, #+0x3c]" ); // Loads cpsr into r0
	__asm ( "msr cpsr_cxsf, r0" ); // Stores it into processor
	__asm ( "ldmfd sp!, { r0 - r12, lr }" );
	__asm ( "add sp, sp, #8" ); // Gets rid of cpsr and pc
	__asm ( "ldr pc, [sp, #-8]" ); // Manually gets back pc from the stack
	//__asm ( "rfefd sp!" );

	__builtin_unreachable ( );
}

void kernel_scheduler_handler ( )
{
	for ( ; ; );
}

void __attribute__((unused)) kernel_scheduler_elect ( )
{
	if ( kernel_turnstile_round_robin.mpFirst )
	{
		kernel_pcb_running = kernel_turnstile_round_robin.mpFirst;
		return;
	}

	kernel_pcb_running = &kernel_pcb_idle;
}

void kernel_idle_process ( )
{
	for ( ; ; );

	__builtin_unreachable ( );
}
