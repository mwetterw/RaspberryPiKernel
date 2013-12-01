#ifndef _H_KERNEL_MEMORY
#define _H_KERNEL_MEMORY

#include <stdint.h>

/*
 * @infos: Initializes kernel heap.
 * The kernel has to call this function once,
 * if allocation system is needed.
 *
 * @return: void
 */
void kernel_memory_init ( );



/*
 * @infos: Requests allocation in the kernel heap
 *
 * @return:
 *  - pointer to allocated user memory
 *  - 0 if allocation was not possible
 */
void * kernel_memory_allocate ( uint32_t size );



/*
 * @infos: Requests de-allocation in the kernel heap
 *
 * @assert:
 * - Memory to be deallocated has been previously
 *		allocated by 'kernel_memory_allocate'.
 * - Address has to be inside the kernel heap space.
 * Use endless loops to punish if assertion is broken.
 *
 * @return: void
 */
void kernel_memory_deallocate ( void * address );

#endif
