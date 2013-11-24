#include "kernel_scheduler.h"
#include "kernel_pcb.h"

static kernel_pcb_t * __attribute__((unused)) kernel_pcb_running;
static kernel_pcb_t kernel_pcb_idle;
static kernel_pcb_turnstile_t kernel_turnstile_round_robin;

void kernel_scheduler_init ( )
{
	kernel_pcb_idle.mpSP = 0;
	kernel_pcb_idle.mpStack = 0;
	kernel_pcb_turnstile_init ( &kernel_turnstile_round_robin );
}
