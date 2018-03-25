#include "../kernel/pcb.h"
#include "../kernel/arm.h"
#include "../kernel/scheduler.h"

void api_process_create ( void * f, void * args )
{
	irq_disable ( );
	pcb_create ( f, args );
	irq_enable ( );
}

void api_process_sleep ( uint32_t duration )
{
	irq_disable ( );
	pcb_sleep ( pcb_running, duration );
	irq_enable ( );
}
