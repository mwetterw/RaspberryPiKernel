#ifndef _H_KERNEL_TIMER
#define _H_KERNEL_TIMER

#include "arm.h"

// BCM2835 System Timer
// BCM2835_SoC-Periphericals.pdf, page 172

// System Timer Control / Status
#define KERNEL_TIMER_CS 0x20003000

// System Timer Counter (64 bits)
#define KERNEL_TIMER_CLO 0x20003004
#define KERNEL_TIMER_CHI 0x20003008

// System Timer Compare
#define KERNEL_TIMER_C0 0x2000300c
#define KERNEL_TIMER_C1 0x20003010
#define KERNEL_TIMER_C2 0x20003014
#define KERNEL_TIMER_C3 0x20003018

#define kernel_timer_init() \
	kernel_arm_enable_irq_source ( KERNEL_ARM_IRQ_SOURCE_MAILBOX )

#define kernel_timer_enable(channel) \
	kernel_arm_addr32 ( KERNEL_TIMER_CS ) |= ( 1 << ( channel ) )

#define kernel_timer_get_clock() \
	kernel_arm_addr32 ( KERNEL_TIMER_CLO )

#define kernel_timer_channel(channel) \
	kernel_arm_array32 ( KERNEL_TIMER_C0, channel )

#endif
