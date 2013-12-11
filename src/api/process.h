#ifndef _H_API_PROCESS
#define _H_API_PROCESS

#include <stdint.h>

/*
 * Creates process with function f and arguments args.
 * Process is automatically ready to be executed.
 */
void api_process_create ( void * f, void * args );

/*
 * Gives the CPU to other processes while this one sleeps.
 * @params:
 * - duration: sleep during that many microseconds
 */
void api_process_sleep ( uint32_t duration );

#endif
