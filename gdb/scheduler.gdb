source gdb/init.gdb
b kernel_main
b irq_handler
b irq_dispatch
b scheduler_handler
b scheduler_reschedule
b scheduler_yield
b kernel_pcb_bigbang
c
