#ifndef _H_SEMAPHORE
#define _H_SEMAPHORE

#include <stdint.h>

typedef int sem_t;

void sem_init ( );

sem_t sem_create ( uint8_t count );
void sem_destroy ( sem_t sem );

void wait ( );

#endif
