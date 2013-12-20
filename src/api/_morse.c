#include "_morse.h"
#include "process.h"

#define morse_cast(value) \
	( ( morse_letter_t ) ( ( unsigned char ) value ) )

static const morse_letter_t MORSE_CODE [ 36 ] =
{
	morse_cast ( 0xa ),		// a . _
	morse_cast ( 0x44 ),	// b _ . . .
	morse_cast ( 0xa4 ),	// c _ . _ .
	morse_cast ( 0x23 ),	// d _ . .
	morse_cast ( 0x1 ),		// e .
	morse_cast ( 0x14 ),	// f . . _ .
	morse_cast ( 0x33 ),   	// g _ _ .
	morse_cast ( 0x4 ),		// h . . . .
	morse_cast ( 0x2 ),		// i . .
	morse_cast ( 0x3c ),   	// j . _ _ _
	morse_cast ( 0x2b ),   	// k _ . _
	morse_cast ( 0x24 ),   	// l . _ . .
	morse_cast ( 0x1a ),   	// m _ _
	morse_cast ( 0x12 ),	// n _ .
	morse_cast ( 0x3b ),	// o _ _ _
	morse_cast ( 0x34 ),   	// p . _ _ .
	morse_cast ( 0x6c ),   	// q _ _ . _
	morse_cast ( 0x13 ),   	// r . _ .
	morse_cast ( 0x3 ),		// s . . .
	morse_cast ( 0x9 ),		// t _
	morse_cast ( 0xb ),		// u . . _
	morse_cast ( 0xc ),		// v . . . _
	morse_cast ( 0x1b ),   	// w . _ _
	morse_cast ( 0x4c ),   	// x _ . . _
	morse_cast ( 0x5c ),   	// y _ . _ _
	morse_cast ( 0x64 ),   	// z _ _ . .

	morse_cast ( 0xfd ),   	// 0 _ _ _ _ _
	morse_cast ( 0x7d ),   	// 1 . _ _ _ _
	morse_cast ( 0x3d ),   	// 2 . . _ _ _
	morse_cast ( 0x1d ),   	// 3 . . . _ _
	morse_cast ( 0xd ),   	// 4 . . . . _
	morse_cast ( 0x5 ),   	// 5 . . . . .
	morse_cast ( 0x85 ),   	// 6 _ . . . .
	morse_cast ( 0xc5 ),   	// 7 _ _ . . .
	morse_cast ( 0xe5 ),   	// 8 _ _ _ . .
	morse_cast ( 0xf5 )   	// 9 _ _ _ _ .
};

static inline void _morse_write_char ( char letter, void ( * f ) ( morse_letter_t morseLetter ) );

void _morse_write_str ( const char * string, void ( * f ) ( morse_letter_t morseLetter ) )
{
	if ( string == 0 )
	{
		return;
	}

	while ( * string != '\0' )
	{
		if ( * string != ' ' )
		{
			_morse_write_char ( * string, f );
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

static inline void _morse_write_char ( char letter, void ( * f ) ( morse_letter_t morseLetter ) )
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

	f ( morseLetter );
}
