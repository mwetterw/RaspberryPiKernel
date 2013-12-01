source gdb/init.gdb
b kernel_main
b kernel_scheduler_handler
b kernel_scheduler_yield_noreturn
c
