// +---------------------------------------------------+
// | Utilities                  |
// | Benigno Gobbo                                     |
// | © 2024 Istituto Nazionale di Fisica Nucleare      |
// +---------------------------------------------------+
#ifndef UTILITIES_H
#define UTILITIES_H

#include <avr/io.h>
#include <avr/wdt.h>
#include <ctype.h>

// +--------------------+
// | Software AVR reset |
// +--------------------+
void resetAVR(void) {
  wdt_enable( WDTO_30MS );
  while ( true ) {
  };
}

// Reset Function
// void( *hardReset )(void) = 0; //

#endif // UTILITIES_H
