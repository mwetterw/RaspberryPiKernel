#ifndef _H_KERNEL_SCHEDULER
#define _H_KERNEL_SCHEDULER

void kernel_scheduler_init ( );

void __attribute__ ( ( noreturn, naked ) ) kernel_scheduler_yield_noreturn ( );

#endif
