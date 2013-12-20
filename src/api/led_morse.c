#include "led_morse.h"
#include "_morse.h"
#include "led.h"
#include "process.h"


static void led_morse_write_letter ( morse_letter_t morseLetter );

void api_led_morse ( const char * string )
{
	_morse_write_str ( string, led_morse_write_letter );
}

static void led_morse_write_letter ( morse_letter_t morseLetter )
{
	unsigned char i;
	for ( i = morseLetter.signifbits ; i >= 1 ; --i )
	{
		api_led_on ( );

		if ( morseLetter.codebits & ( 1 << ( i - 1 ) ) )
		{
			api_process_sleep ( MORSE_DASH );
		}
		else
		{
			api_process_sleep ( MORSE_DOT );
		}

		api_led_off ( );

		if ( i > 1 )
		{
			api_process_sleep ( MORSE_INTRA_LETTER_DELAY );
		}
	}
}
