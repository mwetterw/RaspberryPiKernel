#include "uart_regs.h"
#include "uart.h"
#include "gpio.h"
#include "bcm2835.h"
#include "power.h"

#include <stdint.h>

#define UART_CLK 3000000

#define uart_reg(reg) ( ( uint32_t volatile * ) ( UART_BASE + reg ) )
#define uart_r32(reg) * uart_reg ( reg )
#define uart_w32(reg,data) * uart_reg ( reg ) = data

static void uart_set_baud_rate ( int brate )
{
    float baudiv = ( float ) UART_CLK / ( 16 * brate );
    int baudiv_int = baudiv;
    int baudiv_frac = ( ( baudiv - baudiv_int ) * ( FBRD_MASK + 1 ) + 0.5 );

    uart_w32 ( IBRD, baudiv_int & IBRD_MASK );
    uart_w32 ( FBRD, baudiv_frac & FBRD_MASK );
}

void uart_init ( )
{
    // Power on UART
    power_device ( POWER_UART0, POWER_ON );

    // Completely disable the UART
    uart_w32 ( CR, 0 );

    // Configure GPIO
    gpio_configure ( GPIO14, GPIO_FSEL_ALT0 );
    gpio_configure ( GPIO15, GPIO_FSEL_ALT0 );

    // Configure the UART
    uart_w32 ( ICR, INT_ALL ); // Clear all interrupts
    uart_set_baud_rate ( 115200 );
    uart_w32 ( LCRH, LCRH_WLEN_8BITS );

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

void printu_32h ( uint32_t val )
{
    uart_write_char ( '0' );
    uart_write_char ( 'x' );

    uint32_t cur;
    for ( int byte = 7 ; byte >= 0 ; --byte, val <<= 4 )
    {
        cur = ( val & ( 0xf << 28 ) ) >> 28;
        if ( cur <= 0x9 )
        {
            uart_write_char ( '0' + cur );
        }
        else if ( cur >= 0xa && cur <= 0xf )
        {
            uart_write_char ( 'a' + cur - 0xa );
        }
    }
    uart_write_char ( '\r' );
    uart_write_char ( '\n' );
}
