#ifndef _H_KERNEL_API_LED_MORSE
#define _H_KERNEL_API_LED_MORSE

/**
 * Dispays a string as a morse message using the led.
 * @param string The string to display
 *
 * ASSERT: string has to be a valid C string, i.e end with '\0'.
 * ASSERT: string only contains lower case ASCII letters and space.
 * Unknown letters/symbols will just be skipped.
 */
void api_led_morse ( const char * string );

#endif

