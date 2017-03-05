#include "uart_regs.h"
#include "uart.h"
#include "../gpio.h"

#include <stdint.h>

#define UART_BASE 0x20201000
#define UART_CLK 3000000

static inline void uart_w32 ( int reg, int data )
{
    iowrite ( UART_BASE, reg, data );
}

static inline uint32_t uart_r32 ( int reg )
{
    return ioread ( UART_BASE, reg );
}

static void uart_set_baud_rate ( int brate )
{
    float baudiv = ( float ) UART_CLK / ( 16 * brate );
    int baudiv_int = baudiv;
    int baudiv_frac = ( ( baudiv - baudiv_int ) * FBRD_MASK + 0.5 );

    uart_w32 ( IBRD, baudiv_int & IBRD_MASK );
    uart_w32 ( FBRD, baudiv_frac & FBRD_MASK );
}

void uart_init ( )
{
    // Completely disable the UART
    uart_w32 ( CR, 0 );

    // Configure GPIO
    kernel_gpio_configure ( GPIO14, GPIO_FSEL_ALT0 );
    kernel_gpio_configure ( GPIO15, GPIO_FSEL_ALT0 );

    kernel_gpio_configure_pull_up_down ( GPIO14 | GPIO15, GPPUD_OFF );

    // Configure the UART
    uart_w32 ( ICR, INT_ALL ); // Clear all interrupts
    uart_set_baud_rate ( 115200 );
    uart_w32 ( LCRH, LCRH_WLEN_8BITS | LCRH_FEN );
    uart_w32 ( IMSC, INT_ALL );

    // Enable TX and enable the UART
    uart_w32 ( CR, CR_TXE | CR_UARTEN );
}

static void uart_write_char ( char c )
{
    while ( uart_r32 ( FR ) & FR_TXFF );
    uart_w32 ( DR, c );
}

void printu ( const char * str )
{
    for ( int i = 0 ; str [ i ] != '\0' ; ++i )
    {
        uart_write_char ( str [ i ] );
    }
    uart_write_char ( '\r' );
    uart_write_char ( '\n' );
}
