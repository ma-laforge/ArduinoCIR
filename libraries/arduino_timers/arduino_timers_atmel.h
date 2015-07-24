/*arduino_timers_ATmega2560.h
Declares low-level abstractions for controlling the ATmega2560 timers.

NOTE:
Will not include unless target platform is detected.  This will avoid potential
symbol collisions with code for other platforms.
*/

/*Correspondence between ATMEL OCnx IO pins, and Arduino ATmega2560 header pins
IO Pin list (Pxy/z: ATMEL ID/Arduino IO pin #):
Timer #     OCnA       OCnB       OCnC
Timer 0    PB7/13     PG5/04
Timer 1    PB5/11     PB6/12     PB7/13
Timer 2    PB4/10     PH6/09
Timer 3    PE3/05     PE4/02     PE5/03
Timer 4    PH3/06     PH4/07     PH5/08
Timer 5    PL3/46     PL4/45     PL5/44
*/

#if defined(__AVR_ATmega2560__)

#ifndef ARDUINO_TIMERS_ATMEGA2560_H
#define ARDUINO_TIMERS_ATMEGA2560_H

#include <arduino_timers.h>

namespace ArduinoHw {
//Global objects used to access registers:
//TODO: Add other timers
extern Timer8b::Timer timer0;
extern Timer16b::Timer timer1;
extern Timer8bAsync::Timer timer2;
extern Timer16b::Timer timer3;
extern Timer16b::Timer timer4;
//NOTE: Timers are not arrayed because they are of different types
}; //namespace ArduinoHw

#endif //#ifndef ARDUINO_TIMERS_ATMEGA2560_H
#endif //__AVR_ATmega2560__
//Last Line
