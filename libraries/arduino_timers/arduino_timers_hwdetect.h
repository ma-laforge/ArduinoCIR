/*arduino_timers_hwdetect.h
Declares tools to dected Arduino timer hardware.
*/

#ifndef ARDUINO_TIMERS_HWDETECT_H
#define ARDUINO_TIMERS_HWDETECT_H

#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	#define _AHW_TIMERS_HAS16B
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	#define _AHW_TIMERS_HAS16B
	#define _AHW_TIMERS_HAS16B3OC //16-bit timers have 3 output compares
#endif //__AVR_*

#endif //#ifndef ARDUINO_TIMERS_HWDETECT_H
//Last Line
