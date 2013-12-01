#include "../kernel/pcb.h"
#include "../kernel/arm.h"

void api_process_create ( void * f, void * args )
{
	kernel_arm_disable_irq ( );
	kernel_pcb_create ( f, args );
	kernel_arm_enable_irq ( );
}
