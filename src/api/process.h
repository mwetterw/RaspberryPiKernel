#ifndef _H_API_PROCESS
#define _H_API_PROCESS

/*
 * Creates process with function f and arguments args.
 * Process is automatically ready to be executed.
 */
void api_process_create ( void * f, void * args );

#endif
