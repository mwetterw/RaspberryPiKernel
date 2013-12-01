#define _C_KERNEL_SCHEDULER
#include "scheduler.h"

kernel_pcb_t * kernel_pcb_running;
kernel_pcb_turnstile_t kernel_turnstile_round_robin;
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

	kernel_scheduler_set_next_deadline ( );

	__asm ( "mov sp, %0" : : "r" ( kernel_pcb_running -> mpSP ) );
	__asm ( "ldmfd sp!, { r0 - r12, lr }" );
	__asm ( "rfefd sp!" );

	__builtin_unreachable ( );
}

void __attribute__ ( ( noreturn, naked ) ) kernel_scheduler_handler ( )
{
	// Correct lr_irq value (A2.6.1, P.55/1138, "Note" section)
	__asm ( "sub lr, lr, #4" );

	// Store Return State: push {lr_irq, spsr_irq} => sp_svc
	kernel_arm_srsfd ( KERNEL_ARM_MODE_SVC );

	// Switch to SVC mode
	kernel_arm_set_mode ( KERNEL_ARM_MODE_SVC );

	// Save current process context (Push r0 - r12, lr)
	__asm ( "stmfd sp!, {r0 - r12, lr}" );

	// Store sp address in PCB
	__asm ( "mov %0, sp" : "=r" ( kernel_pcb_running -> mpSP ) );

	// Elect new process
	kernel_scheduler_elect ( );

	// Fix next IRQ deadline (reset timer)
	kernel_scheduler_set_next_deadline ( );

	// Load elected process's sp address into sp
	__asm ( "mov sp, %0" : : "r" ( kernel_pcb_running -> mpSP ) );

	// Restore elected process context (Pop r0 - r12, lr)
	__asm ( "ldmfd sp!, {r0 - r12, lr}" );

	// Return From Interrupt: pop {pc, cpsr}
	__asm ( "rfefd sp!" );

	__builtin_unreachable ( );
}

void kernel_scheduler_elect ( )
{
	if ( kernel_turnstile_round_robin.mpFirst )
	{
		kernel_pcb_running = kernel_turnstile_round_robin.mpFirst;
		kernel_pcb_turnstile_rotate ( &kernel_turnstile_round_robin );
		return;
	}

	kernel_pcb_running = &kernel_pcb_idle;
}

void kernel_idle_process ( )
{
	for ( ; ; );

	__builtin_unreachable ( );
}
