#include <stdint.h>

typedef struct kernel_pcb_s
{
	uint32_t * mpSP;
	uint32_t * mpStack;
	struct kernel_pcb_s * mpNext;
} kernel_pcb_t;
