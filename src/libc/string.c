#include "string.h"

void * memcpy ( void * dest, const void * src, size_t n )
{
    unsigned char * dest_ = dest;
    const unsigned char * src_ = src;

    for ( size_t i = 0 ; i < n ; ++i )
    {
        dest_ [ i ] = src_ [ i ];
    }

    return dest;
}
