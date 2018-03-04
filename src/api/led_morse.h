#ifndef _H_API_LED_MORSE
#define _H_API_LED_MORSE

/**
 * Dispays a string as a morse message using the led.
 * @param string The string to display
 *
 * Cf _morse_write_str ASSERTS for accepted string values.
 */
void api_led_morse ( const char * string );

#endif

