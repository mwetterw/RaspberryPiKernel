#ifndef _H_KERNEL_TIMER
#define _H_KERNEL_TIMER

#include <stdint.h>

struct systimer
{
    uint32_t cs;    // Control/Status
    uint32_t clo;   // Timer lower 32 bits
    uint32_t chi;   // Timer higher 32 bits
    uint32_t c0;    // Compare 0
    uint32_t c1;    // Compare 1
    uint32_t c2;    // Compare 2
    uint32_t c3;    // Compare 3
};

#define SYSTIMER_MATCH0 0x1
#define SYSTIMER_MATCH1 0x2
#define SYSTIMER_MATCH2 0x4
#define SYSTIMER_MATCH3 0x8

void systimer_init ( );
uint32_t systimer_get_clock ( );
void systimer_update ( uint32_t offset );

#endif
