#include "../../api/process.h"

extern uint32_t smsc9512_write_reg ( uint16_t addr, uint32_t val );

void smsc9512_led_write ( uint8_t val )
{
    smsc9512_write_reg ( 0x24, 0x770 | ( ~val & 7 ) );
}

void smsc9512_led_chaser ( uint8_t max )
{
    uint8_t i, val;
    for ( i = 1, val = 1 ; i <= max ; val = ( 1 << ( i++ % 3 ) ) )
    {
        smsc9512_led_write ( val );
        api_process_sleep ( 250000 );
    }
}

void smsc9512_led_snake ( uint8_t max )
{
    uint8_t i, val;
    for ( i = 1, val = 1 ; i <= max ; val ^= ( 1 << ( i++ % 3 ) ) )
    {
        smsc9512_led_write ( val );
        api_process_sleep ( 250000 );
    }
}

void smsc9512_led_blink ( uint8_t max )
{
    uint8_t i;
    for ( i = 1 ; i <= max ; ++i )
    {
        smsc9512_led_write ( 7 );
        api_process_sleep ( 300000 );
        smsc9512_led_write ( 0 );
        api_process_sleep ( 1000000 );
    }
}

void smsc9512_led_binary_counter ( uint8_t max )
{
    uint8_t i;
    for ( i = 1 ; i <= max ; ++i )
    {
        smsc9512_led_write ( i );
        api_process_sleep ( 1000000 );
    }
}

void smsc9512_led_process ( )
{
    for ( ; ; )
    {
        smsc9512_led_chaser ( 30 );
        smsc9512_led_snake ( 36 );
        smsc9512_led_blink ( 15 );
        smsc9512_led_binary_counter ( 16 );
    }
}


