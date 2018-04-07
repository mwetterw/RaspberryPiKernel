#ifndef _H_BCM2835_WATCHDOG
#define _H_BCM2835_WATCHDOG

#include <stdint.h>

/*
 * Program the watchdog to trigger a full system reset
 * @params:
 * - timeout: When to trigger the reset (in 1/65536 second increments).
 *   A 0 timeout won't have any effect.
 */
void watchdog_start ( uint32_t timeout );


/*
 * Stop the previously started watchdog to prevent it firing
 */
void watchdog_stop ( );

#endif
