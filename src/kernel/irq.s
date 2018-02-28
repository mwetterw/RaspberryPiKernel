@ vim: ft=arm
.globl irq_handler
irq_handler:
	// Correct lr_irq value due to the ARM pipeline design
	sub lr, lr, #4

    // Push lr_irq and spsr_irq to sp_svc
    srsfd #0x13!

    // Switch to SVC mode, store context
    cps #0x13
	stmfd sp!, { r0 - r12, lr }

    // Retrieve sp_svc to r0. Then switch back to IRQ mode.
    // r0 isn't banked: we gain access to sp_svc from IRQ mode.
    mov r0, sp
    cps #0x12

    // r0 contain the old stack

    bl irq_dispatch

    // r0 Contain the new stack to load

    // Switch back to SVC mode
    cps #0x13

	// Restore current process context (Pop r0 - r12, lr)
    mov sp, r0
	ldmfd sp!, { r0 - r12, lr }
	rfefd sp!
