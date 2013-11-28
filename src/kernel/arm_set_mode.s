.globl kernel_arm_set_mode_usr
kernel_arm_set_mode_usr:
    cps #0x10
    bx lr

.globl kernel_arm_set_mode_fiq
kernel_arm_set_mode_fiq:
    cps #0x11
    bx lr

.globl kernel_arm_set_mode_irq
kernel_arm_set_mode_irq:
    cps #0x12
    bx lr

.globl kernel_arm_set_mode_svc
kernel_arm_set_mode_svc:
    cps #0x13
    bx lr

.globl kernel_arm_set_mode_abt
kernel_arm_set_mode_abt:
    cps #0x17
    bx lr

.globl kernel_arm_set_mode_und
kernel_arm_set_mode_und:
    cps #0x18
    bx lr

.globl kernel_arm_set_mode_sys
kernel_arm_set_mode_sys:
    cps #0x1F
    bx lr
