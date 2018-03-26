#include "../kernel/pcb.h"
#include "../kernel/arm.h"
#include "../kernel/scheduler.h"

void api_process_create ( void * f, void * args )
{
	uint32_t irqmask = irq_disable ( );
	pcb_create ( f, args );
	irq_restore ( irqmask );
}

void api_process_sleep ( uint32_t duration )
{
	uint32_t irqmask = irq_disable ( );
	pcb_sleep ( pcb_running, duration );
	irq_restore ( irqmask );
}
