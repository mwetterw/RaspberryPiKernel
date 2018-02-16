#ifndef _H_KERNEL_ARM
#define _H_KERNEL_ARM

#include <stdint.h>

#define kernel_arm_addr(addr) ( uint32_t * ) ( addr )
#define kernel_arm_addr32(addr) ( * ( uint32_t * ) ( addr ) )
#define kernel_arm_array32(addr, i) ( ( ( uint32_t * ) ( addr ) ) [ i ] )

#define KERNEL_ARM_MODE_USR 0x10
#define KERNEL_ARM_MODE_FIQ 0x11
#define KERNEL_ARM_MODE_IRQ 0x12
#define KERNEL_ARM_MODE_SVC 0x13
#define KERNEL_ARM_MODE_ABT 0x17
#define KERNEL_ARM_MODE_UND 0x18
#define KERNEL_ARM_MODE_SYS 0x1F

#define KERNEL_ARM_CPSR_IRQ_MASK 0x80

// BCM2835_SoC-Periphericals.pdf P112
#define KERNEL_ARM_IRQ_ENABLE_REGISTER1 0x2000B210

#define KERNEL_ARM_IRQ_SOURCE_TIMER 0x1
#define KERNEL_ARM_IRQ_SOURCE_MAILBOX 0x2
#define KERNEL_ARM_IRQ_SOURCE_DOORBELL0 0x4
#define KERNEL_ARM_IRQ_SOURCE_DOORBELL1 0x8
#define KERNEL_ARM_IRQ_SOURCE_GPU0_HALTED 0x10
#define KERNEL_ARM_IRQ_SOURCE_GPU1_HALTED 0x20
#define KERNEL_ARM_IRQ_SOURCE_ACCESS_ERROR1_HALTED 0x40
#define KERNEL_ARM_IRQ_SOURCE_ACCESS_ERROR0_HALTED 0x80

// Sets the desired IRQ source (one of the KERNEL_ARM_IRQ_SOURCE_*)
#define kernel_arm_enable_irq_source(source) \
	kernel_arm_addr32 ( KERNEL_ARM_IRQ_ENABLE_REGISTER1 ) = ( source )

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

#endif
