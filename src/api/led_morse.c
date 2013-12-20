#include "led_morse.h"
#include "_morse.h"
#include "led.h"
#include "process.h"

static void led_morse_write_dot ( );
static void led_morse_write_dash ( );

void api_led_morse ( const char * string )
{
	_morse_write_str ( string, led_morse_write_dot, led_morse_write_dash );
}

static void led_morse_write_dot ( )
{
	api_led_on ( );
	api_process_sleep ( MORSE_DOT );
	api_led_off ( );
}

static void led_morse_write_dash ( )
{
	api_led_on ( );
	api_process_sleep ( MORSE_DASH );
	api_led_off ( );
}

