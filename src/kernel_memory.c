#include "kernel_memory.h"
#include "kernel_memory_private.h"
#include "kernel_config.h"


kernel_heap_part_t * kernel_private_allocate_memory ( uint32_t size, kernel_heap_part_t * pPrevious );

static char kernel_memory_heap [ KERNEL_HEAP_SIZE ];

static const void *
	KERNEL_HEAP_ADDR_MIN = kernel_memory_heap + 2 * sizeof ( kernel_heap_part_t );

static const void *
	KERNEL_HEAP_ADDR_MAX =
		( void * ) kernel_memory_heap +
		KERNEL_HEAP_SIZE -
		sizeof ( kernel_heap_part_t );



void kernel_memory_init ( )
{
	kernel_heap_part_t * pHead = ( kernel_heap_part_t * ) kernel_memory_heap;

	kernel_heap_part_t * pFoot = ( kernel_heap_part_t * )(
		kernel_memory_heap +
		KERNEL_HEAP_SIZE -
		sizeof ( kernel_heap_part_t )
	);

	pHead -> mpNext = pFoot;
	pHead -> mpPrevious = 0;
	pHead -> mSize = 0;

	pFoot -> mpNext = 0;
	pFoot -> mpPrevious = pHead;
	pHead -> mSize = 0;
}

void * kernel_memory_allocate ( uint32_t size )
{
	// Overflow check
	if ( size >= ( ~0 - sizeof ( kernel_heap_part_t ) ) )
	{
		return 0;
	}

	kernel_heap_part_t * current = ( kernel_heap_part_t * ) kernel_memory_heap;
	while ( current -> mpNext )
	{
		// If there is space between current and next one,
		// we choose this place! O(n)
		if
		(
			(
				( ( uint32_t ) current -> mpNext ) -
				( ( uint32_t ) ( ( char * ) ( current + 1 ) ) + current -> mSize )
			)
			>=
			(
				sizeof ( kernel_heap_part_t ) + size
			)
		)
		{
			kernel_heap_part_t * new = kernel_private_allocate_memory ( size, current );
			return new + 1;
		}

		current = current->mpNext;
	}

	// We didn't find any space :'(
	return 0;
}

void kernel_memory_deallocate ( void * address )
{
	// Boudaries check for address
	if ( address < KERNEL_HEAP_ADDR_MIN || address >= KERNEL_HEAP_ADDR_MAX )
	{
		for ( ; ; );
	}

	// We get the kernel memory header pointer
	kernel_heap_part_t * heap_part_head = &( ( ( kernel_heap_part_t * ) address ) [ -1 ] );

	// Some other checks for address validity
	if ( heap_part_head -> mpPrevious -> mpNext != heap_part_head ||
		heap_part_head -> mpNext -> mpPrevious != heap_part_head )
	{
		for ( ; ; );
	}


    // User gave us valid address. We can start deallocate!
	heap_part_head -> mpPrevious -> mpNext = heap_part_head -> mpNext;
	heap_part_head -> mpNext -> mpPrevious = heap_part_head -> mpPrevious;
}

// ASSERT
// pPrevious HAS TO be valid. There is no check
kernel_heap_part_t * kernel_private_allocate_memory ( uint32_t size, kernel_heap_part_t * pPrevious )
{
	kernel_heap_part_t * new = ( kernel_heap_part_t * ) (
		( ( char * ) pPrevious ) +
		sizeof ( kernel_heap_part_t ) +
		pPrevious -> mSize
	);

	new -> mpPrevious = pPrevious;
	new -> mpNext = pPrevious -> mpNext;
	new -> mSize = size;

	pPrevious -> mpNext -> mpPrevious = new;
	pPrevious -> mpNext = new;

	return new;
}
