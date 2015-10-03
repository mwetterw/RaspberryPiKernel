#include "../api/process.h"
#include "../api/led_morse.h"

void morse ( )
{
    for ( ; ; )
    {
		api_led_morse ( "sos" );
        api_process_sleep ( 2000000 );
    }
}

void init ( )
{
	api_process_create ( morse, 0 );
}
