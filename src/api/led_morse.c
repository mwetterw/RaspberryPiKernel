#include "led_morse.h"
#include "led.h"
#include "process.h"

#define MORSE_DOT 250000
#define MORSE_DASH ( MORSE_DOT * 3 )

#define MORSE_INTRA_LETTER_DELAY MORSE_DOT
#define MORSE_INTER_LETTER_DELAY MORSE_DASH
#define MORSE_INTER_WORD_DELAY ( MORSE_DOT * 7 )

#define morse_cast(value) \
	( ( morse_letter_t ) ( ( unsigned char ) value ) )

typedef union morse_letter_u
{
	struct
	{
		unsigned char signifbits : 3;
		unsigned char codebits : 5;
	};

	unsigned char code;
} morse_letter_t;

static inline void morse_write_char ( char letter );
static inline void morse_write_letter ( morse_letter_t morseLetter );

static const morse_letter_t MORSE_CODE [ 36 ] =
{
	morse_cast ( 0xa ),		// a
	morse_cast ( 0x44 ),	// b
	morse_cast ( 0xa4 ),	// c
	morse_cast ( 0x23 ),	// d
	morse_cast ( 0x1 ),		// e
	morse_cast ( 0x14 ),	// f
	morse_cast ( 0x33 ),   	// g
	morse_cast ( 0x4 ),		// h
	morse_cast ( 0x2 ),		// i
	morse_cast ( 0x3c ),   	// j
	morse_cast ( 0x2b ),   	// k
	morse_cast ( 0x24 ),   	// l
	morse_cast ( 0x1a ),   	// m
	morse_cast ( 0x12 ),	// n
	morse_cast ( 0x3b ),	// o
	morse_cast ( 0x34 ),   	// p
	morse_cast ( 0x6c ),   	// q
	morse_cast ( 0x13 ),   	// r
	morse_cast ( 0x3 ),		// s
	morse_cast ( 0x9 ),		// t
	morse_cast ( 0xb ),		// u
	morse_cast ( 0xc ),		// v
	morse_cast ( 0x1b ),   	// w
	morse_cast ( 0x4c ),   	// x
	morse_cast ( 0x5c ),   	// y
	morse_cast ( 0x64 ),   	// z

	morse_cast ( 0xfd ),   	// 0
	morse_cast ( 0x7d ),   	// 1
	morse_cast ( 0x3d ),   	// 2
	morse_cast ( 0x1d ),   	// 3
	morse_cast ( 0xd ),   	// 4
	morse_cast ( 0x5 ),   	// 5
	morse_cast ( 0x85 ),   	// 6
	morse_cast ( 0xc5 ),   	// 7
	morse_cast ( 0xe5 ),   	// 8
	morse_cast ( 0xf5 )   	// 9
};

void api_led_morse ( const char * string )
{
	if ( string == 0 )
	{
		return;
	}

	while ( * string != '\0' )
	{
		if ( * string != ' ' )
		{
			morse_write_char ( * string );
			if ( string [ 1 ] != '\0' && string [ 1 ] != ' ' )
			{
				api_process_sleep ( MORSE_INTER_LETTER_DELAY );
			}
		}
		else if ( string [ 1 ] != '\0' )
		{
			api_process_sleep ( MORSE_INTER_WORD_DELAY );
		}

		++string;
	}
}

static inline void morse_write_char ( char letter )
{
	morse_letter_t morseLetter;
	if ( letter >= 'a' && letter <= 'z' )
	{
		morseLetter = MORSE_CODE [ letter - 'a' ];
	}
	else if ( letter >= '0' && letter <= '9' )
	{
		morseLetter = MORSE_CODE [ 26 + ( letter - '0' ) ];
	}
	else
	{
		return;
	}

	morse_write_letter ( morseLetter );
}

static inline void morse_write_letter ( morse_letter_t morseLetter )
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
