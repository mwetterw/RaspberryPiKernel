#ifndef _H_API__MORSE
#define _H_API__MORSE

#define MORSE_DOT 250000
#define MORSE_DASH ( MORSE_DOT * 3 )

#define MORSE_INTRA_LETTER_DELAY MORSE_DOT
#define MORSE_INTER_LETTER_DELAY MORSE_DASH
#define MORSE_INTER_WORD_DELAY ( MORSE_DOT * 7 )

struct morse_write_way_s;
typedef struct morse_write_way_s morse_write_way_t;

/**
 * Represents a morse letter.
 * - 'signifbits' is the number of symbols in this morse letter
 * - 'codebits' is the actual morse code for this letter (1 for dash, 0 for dot)
 *
 * Exemple: letter A is '. _'
 *	There are 2 symbols: 'signifbits' = 0b010
 *	The morse code is '. _': 'codebits' = 0b00001
 *	Resulting 'code': 0b00001010 = 0xa
 */
typedef union morse_letter_u
{
	struct
	{
		unsigned char signifbits : 3;
		unsigned char codebits : 5;
	};

	unsigned char code;
} morse_letter_t;


/**
 * Writes morse from a string.
 * @param string: The string to write
 * @param morse_write_way: The way a morse letter has to be written (dot & dash)
 * This param can be used to get various implementations (led, sound) of morse code.
 *
 * ASSERT: string has to be a valid C string, i.e end with '\0'.
 * ASSERT: string only contains lower case ASCII letters and space.
 * Unknown letters/symbols will just be skipped.
 */
void _morse_write_str
( const char * string, morse_write_way_t * morse_write_way );


/**
 * Represents the way a morse letter is written
 * @member dot: function pointer to the way a dot is written
 * @member dash: function pointer to the way a dash is written
 */
struct morse_write_way_s
{
	void ( * dot ) ( );
	void ( * dash ) ( );
};

#endif
