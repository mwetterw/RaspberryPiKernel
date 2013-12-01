#include "../api/process.h"
#include "../api/led.h"

void led_on ( )
{
	for ( ; ; )
	{
		api_led_on ( );
	}
}

void led_off ( )
{
	for ( ; ; )
	{
		api_led_off ( );
	}
}

void init ( )
{
	api_process_create ( led_on, 0 );
	api_process_create ( led_off, 0 );
}
