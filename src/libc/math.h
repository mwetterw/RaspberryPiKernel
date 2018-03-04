#ifndef _H_LIB_MATH
#define _H_LIB_MATH

static inline uint32_t min ( uint32_t a, uint32_t b )
{
    return ( a < b ) ? a : b;
}

static inline uint32_t max ( uint32_t a, uint32_t b )
{
    return ( a > b ) ? a : b;
}

#endif
