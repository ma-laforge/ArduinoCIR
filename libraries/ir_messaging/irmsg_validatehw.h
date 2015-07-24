/*irmsg_validatehw.h
Validates weather the current hardware is supported by the ir_messaging library.

To add support for more hardware:
	-Add code to provide control of supported hardware timers.
	    (refer to arduino_timers/arduino_timers_ATmega2560.cpp as an example)
	-Add code to implement RxHw/TxHw interfaces for said timers.
	    (if not already implemented in ir_messaging/irhw.cpp)
*/

#ifndef IRMSG_VALIDATEHW_H
#define IRMSG_VALIDATEHW_H

#include <arduino_ext.h>

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) \
	|| defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	//Ok, these platforms are supported (in theory)
	//TODO: Test for __AVR_ATmega328__ & __AVR_ATmega1280__
#else
	#error "HW not supported. See ir_messaging/irmsg_validatehw.h for details."
#endif

#endif //#ifndef IRMSG_VALIDATEHW_H
//Last Line
