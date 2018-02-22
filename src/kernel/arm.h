#ifndef _H_KERNEL_ARM
#define _H_KERNEL_ARM

#include <stdint.h>

#define KERNEL_ARM_MODE_USR 0x10
#define KERNEL_ARM_MODE_FIQ 0x11
#define KERNEL_ARM_MODE_IRQ 0x12
#define KERNEL_ARM_MODE_SVC 0x13
#define KERNEL_ARM_MODE_ABT 0x17
#define KERNEL_ARM_MODE_UND 0x18
#define KERNEL_ARM_MODE_SYS 0x1F

#define KERNEL_ARM_CPSR_IRQ_MASK 0x80

// Wait a number of cycles
void cdelay ( int cycles );

// Get the processor mode
uint32_t kernel_arm_get_mode ( );

// Set processor mode
#define _kernel_arm_set_mode(mode) __asm ( "cps #" #mode )
#define kernel_arm_set_mode(mode) _kernel_arm_set_mode(mode)

// Launch srsfd #mode!
#define _kernel_arm_srsfd(mode) __asm ( "srsfd #" #mode "!" )
#define kernel_arm_srsfd(mode) _kernel_arm_srsfd(mode)

// Get current CPSR (Status Register) value
uint32_t kernel_arm_get_cpsr ( );

// Enable IRQ in CPSR
#define kernel_arm_enable_irq() __asm ( "cpsie i" )

// Disable IRQ in CPSR
#define kernel_arm_disable_irq() __asm ( "cpsid i" )

extern void dmb ( );

#endif
