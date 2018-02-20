@ vim: ft=arm
.globl irq_handler
irq_handler:
	// Correct lr_irq value (A2.6.1, P.55/1138, "Note" section)
	sub lr, lr, #4

	// Store Return State: push {lr_irq, spsr_irq} => sp_svc, then switch
    srsfd #0x13!
    cps #0x13

	// Save current process context (Push r0 - r12, lr)
	stmfd sp!, { r0 - r12, lr }

    mov r0, sp

    and r11, sp, #4
    sub sp, sp, r11

    bl irq_dispatch

    add sp, sp, r11
	// Restore current process context (Pop r0 - r12, lr)
	ldmfd sp!, { r0 - r12, lr }
	rfefd sp!
