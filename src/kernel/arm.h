#ifndef _H_ARM
#define _H_ARM

#include <stdint.h>

#define ARM_MODE_USR 0x10
#define ARM_MODE_FIQ 0x11
#define ARM_MODE_IRQ 0x12
#define ARM_MODE_SVC 0x13
#define ARM_MODE_ABT 0x17
#define ARM_MODE_UND 0x18
#define ARM_MODE_SYS 0x1F

#define ARM_CPSR_IRQ_MASK 0x80

// Wait a number of cycles
void cdelay ( int cycles );

// Get the processor mode
extern uint32_t arm_get_mode ( );

// Set processor mode
#define _arm_set_mode(mode) __asm ( "cps #" #mode )
#define arm_set_mode(mode) _arm_set_mode(mode)

// Launch srsfd #mode!
#define _arm_srsfd(mode) __asm ( "srsfd #" #mode "!" )
#define arm_srsfd(mode) _arm_srsfd(mode)

// Get current CPSR (Status Register) value
extern uint32_t arm_get_cpsr ( );

// Enable IRQ in CPSR
#define arm_enable_irq() __asm ( "cpsie i" )

// Disable IRQ in CPSR
#define arm_disable_irq() __asm ( "cpsid i" )

extern void dmb ( );

extern void pause ( );
extern void __attribute__ (( noreturn )) halt ( );

#endif
