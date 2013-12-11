#include "../kernel/pcb.h"
#include "../kernel/arm.h"
#include "../kernel/scheduler.h"

void api_process_create ( void * f, void * args )
{
	kernel_arm_disable_irq ( );
	kernel_pcb_create ( f, args );
	kernel_arm_enable_irq ( );
}

void api_process_sleep ( uint32_t duration )
{
	kernel_arm_disable_irq ( );
	kernel_pcb_sleep ( kernel_pcb_running, duration );
	kernel_arm_enable_irq ( );
}
