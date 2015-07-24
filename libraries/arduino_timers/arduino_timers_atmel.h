/*arduino_timers_atmel.h
Declares low-level abstractions for controlling the Atmel timer registers.

//NOTE: Timers are not arrayed because they are of different types
//TODO: Test for __AVR_ATmega328__ & __AVR_ATmega1280__
*/

#ifndef ARDUINO_TIMERS_ATMEL_H
#define ARDUINO_TIMERS_ATMEL_H

//Declare global objects used to access timer registers:
namespace ArduinoHw {

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
/*Correspondence between ATMEL OCnX IO pins, and Arduino Mega/2560 header pins
IO Pin list (Pxy/z: ATMEL ID/Arduino IO pin #):
Timer #     OCnA       OCnB       OCnC
Timer 1    PB5/11     PB6/12     PB7/13
Timer 2    PB4/10     PH6/09
Timer 3    PE3/05     PE4/02     PE5/03
Timer 4    PH3/06     PH4/07     PH5/08
Timer 5    PL3/46     PL4/45     PL5/44
*/
extern Timer16b::Timer timer1;
extern Timer8b::Timer timer2;
extern Timer16b::Timer timer3;
extern Timer16b::Timer timer4;
#define _AHW_TIMERS_LAST 4

#elif defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
/*Correspondence between ATmega328 OCnX IO pins & Arduino header pins
IO Pin list (Pxy/z: ATMEL ID/Arduino IO pin #):
Timer #     OCnA       OCnB
Timer 1    PB1/09     PB2/10
Timer 2    PB3/11     PD3/03
*/
extern Timer16b::Timer timer1;
extern Timer8b::Timer timer2;
#define _AHW_TIMERS_LAST 2

#endif //__AVR_*

}; //namespace ArduinoHw

#endif //#ifndef ARDUINO_TIMERS_ATMEL_H
//Last Line
