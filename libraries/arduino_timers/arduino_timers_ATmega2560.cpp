/*arduino_timers_ATmega2560.cpp
Defines low-level abstractions for controlling the ATmega2560 timers.

NOTE:
Will not compile unless target platform is detected.  This will avoid potential
symbol collisions with code for other platforms.
*/

#if defined(__AVR_ATmega2560__)

#include <arduino_timers_ATmega2560.h>

namespace ArduinoHw {
//Global objects used to access registers:
//TODO: Instantiate objects in fast memory, if possible
Timer8b::Timer timer0(TCCR0A, TCCR0B, OCR0A, 13, OCR0B, 4, TIMSK0);
Timer16b::Timer timer1(TCCR1A, TCCR1B, TCNT1, ICR1, OCR1A, 11, OCR1B, 12, OCR1C, 13, TIMSK1);
Timer8bAsync::Timer timer2(TCCR2A, TCCR2B, OCR2A, OCR2B, TIMSK2);
Timer16b::Timer timer3(TCCR3A, TCCR3B, TCNT3, ICR3, OCR3A,  5, OCR3B,  2, OCR3C,  3, TIMSK3);
Timer16b::Timer timer4(TCCR4A, TCCR4B, TCNT4, ICR4, OCR4A,  6, OCR4B,  7, OCR4C,  8, TIMSK4);
}; //namespace ArduinoHw

#endif //__AVR_ATmega2560__
//Last Line
