#ifndef _H_KERNEL_ARM
#define _H_KERNEL_ARM

#include <stdint.h>

#define r0 0
#define r1 1
#define r2 2
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7
#define r8 8
#define r9 9
#define r10 10
#define r11 11
#define r12 12
#define r13 13
#define r14 14
#define r15 15
#define cpsr 16

#define sp r13
#define lr r14
#define pc r15

#define KERNEL_ARM_MODE_USR 0x10
#define KERNEL_ARM_MODE_FIQ 0x11
#define KERNEL_ARM_MODE_IRQ 0x12
#define KERNEL_ARM_MODE_SVC 0x13
#define KERNEL_ARM_MODE_ABT 0x17
#define KERNEL_ARM_MODE_UND 0x18
#define KERNEL_ARM_MODE_SYS 0x1F

/*
 * Gets current processor mode
 * @see KERNEL_ARM_MODE_*
 */
uint32_t kernel_arm_get_mode ( );


/*
 * Sets processor mode
 */
void kernel_arm_set_mode_usr ( );
void kernel_arm_set_mode_fiq ( );
void kernel_arm_set_mode_irq ( );
void kernel_arm_set_mode_svc ( );
void kernel_arm_set_mode_abt ( );
void kernel_arm_set_mode_und ( );
void kernel_arm_set_mode_sys ( );


/*
 * Gets current CPSR (Status Register) value
 */
uint32_t kernel_arm_get_cpsr ( );


/*
 * Enables IRQ in CPSR
 */
void kernel_arm_enable_irq ( );


/*
 * Disables IRQ in CPSR
 */
void kernel_arm_disable_irq ( );

#endif
