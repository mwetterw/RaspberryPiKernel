#ifndef _H_UART
#define _H_UART

#include <stdint.h>

void uart_init ( );
void printu ( const char * str );
void printuln ( const char * str );
void printu_32h ( uint32_t val );

#endif
