/*arduino_timers_atmel.cpp
Defines low-level abstractions for controlling the Atmel timer registers.

//TODO: Instantiate objects in fast memory, if possible
*/

#include <arduino_timers.h>

namespace ArduinoHw {

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
Timer16b::Timer timer1(TCCR1A, TCCR1B, TCNT1, ICR1, OCR1A, 11, OCR1B, 12, OCR1C, 13, TIMSK1);
Timer8b::Timer timer2(TCCR2A, TCCR2B, OCR2A, 10, OCR2B, 9, TIMSK2);
Timer16b::Timer timer3(TCCR3A, TCCR3B, TCNT3, ICR3, OCR3A,  5, OCR3B,  2, OCR3C,  3, TIMSK3);
Timer16b::Timer timer4(TCCR4A, TCCR4B, TCNT4, ICR4, OCR4A,  6, OCR4B,  7, OCR4C,  8, TIMSK4);

#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
Timer16b::Timer timer1(TCCR1A, TCCR1B, TCNT1, ICR1, OCR1A, 9, OCR1B, 10, TIMSK1);
Timer8b::Timer timer2(TCCR2A, TCCR2B, OCR2A, 11, OCR2B, 3, TIMSK2);

#endif //__AVR_*

}; //namespace ArduinoHw
//Last Line
