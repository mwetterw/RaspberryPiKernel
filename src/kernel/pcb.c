#include "pcb.h"
#include "pcb_turnstile.h"
#include "memory.h"
#include "config.h"
#include "scheduler.h"

static void __attribute__ ( ( noreturn ) )
kernel_pcb_bigbang ( void * ( * f ) ( void * ), void * args );

kernel_pcb_t * kernel_pcb_create ( void * f, void * args )
{
    kernel_pcb_t * pcb = kernel_memory_allocate ( sizeof ( kernel_pcb_t ) );
    pcb -> mpStack = kernel_memory_allocate ( KERNEL_STACK_SIZE );
    pcb -> mpSP = ( pcb -> mpStack ) + KERNEL_STACK_SIZE - 16;
    kernel_pcb_inherit_cpsr ( pcb );
    kernel_pcb_enable_irq ( pcb );
    kernel_pcb_set_register ( pcb, pc, kernel_pcb_bigbang );
    kernel_pcb_set_register ( pcb, r0, f );
    kernel_pcb_set_register ( pcb, r1, args );

    kernel_pcb_add_turnstile ( pcb, &kernel_turnstile_round_robin );

    return pcb;
}

void kernel_pcb_bigbang ( void * ( * f ) ( void * ), void * args )
{
    f ( args );

    kernel_arm_disable_irq ( );
    kernel_pcb_remove_turnstile ( kernel_pcb_running, &kernel_turnstile_round_robin );
    kernel_memory_deallocate ( kernel_pcb_running -> mpStack );
    kernel_memory_deallocate ( kernel_pcb_running );

    kernel_scheduler_yield_noreturn ( );
}

void kernel_pcb_sleep ( kernel_pcb_t * pcb,
		uint32_t __attribute__ ( ( unused ) ) duration )
{
	kernel_pcb_remove_turnstile ( pcb, &kernel_turnstile_round_robin );
}
