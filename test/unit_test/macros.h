#ifndef TEST_UNIT_TEST_MACROS_H
#define TEST_UNIT_TEST_MACROS_H
#include <stdio.h>

/** Print error message to stderr optionally preceding it with the custom message specified in _text.
 * @param[in] _text C string containing a custom message to be printed before the error message itself.
 * @return none
 */
#define LOG_ERROR(_text) (perror( _text))

#endif  // TEST_UNIT_TEST_MACROS_H
