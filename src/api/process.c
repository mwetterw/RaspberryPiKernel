#include "../kernel/pcb.h"
#include "../kernel/arm.h"
#include "../kernel/scheduler.h"

void api_process_create ( void * f, void * args )
{
	arm_disable_irq ( );
	pcb_create ( f, args );
	arm_enable_irq ( );
}

void api_process_sleep ( uint32_t duration )
{
	arm_disable_irq ( );
	pcb_sleep ( pcb_running, duration );
	arm_enable_irq ( );
}
