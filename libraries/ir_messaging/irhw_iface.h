/*irhw_iface.h
High-level interface for the IR tx/rx abstractions.

NOTES:
-Must be implemented to control low-level timer hardware.
-TxHw/RxHw are implemented as virtual classes in case same platform has multiple
 (different) HW units that can perform the same function.
*/

#ifndef IRHW_IFACE_H
#define IRHW_IFACE_H

#include <arduino_ext.h>

namespace IRCtrl {

//Interface to abstract HW control of IR transmission.
class TxHw {
public:

	//Informative: allow user to query output pin.
	virtual int GetOutputPin() = 0;

	//Apply either a 1 or 0 to the output pin
	virtual void SetOutput(int value) = 0;

	//Enable HW transmission (using ISRs)
	virtual void Enable() = 0;

	//Disable HW transmission (and ISRs)
	virtual void Disable() = 0;

	//Sets modulator to trigger event a given time from last event:
	virtual void SetModEventTime(usec_u16t nextEvent) = 0;

	//Change carrier frequency, value in kHz.
	//TODO: Use more explicit type for frequency?
	//	     ex: typedef uint16_t ncycles_t; //Cycle count (timers)
	virtual void SetCarrier(int value_kHz) = 0;
	//NOTE: Only called for low output (no need to de-glitch for high output)

	//Apply register writes needed for initialization:
	virtual void Configure() = 0; //Will be called by Arduino setup()

	//TODO: Do we need to control duty cycle???
	//	     ex: typedef uint32_t dutycycle_t;
	//	         const int DUTY_FRACBITS = 16;
	//	         const dutycycle_t DUTY_UNITY = (1<<DUTY_FRACBITS)-1;};
};

//Interface to abstract HW control of IR receiver function.
//NOTE:
//   -Simply needs to provide an interrupt/handler that calls
//    Receiver::ProcessISR(), specify desired ioPin, & set it to read mode.
class RxHw {
public:
	//Base interval at which incomming IR signal is polled:
	static const usec_s16t POLL_INTERVAL = 50;

	const int ioPin;

	//Apply register writes needed for initialization:
	virtual void Configure() = 0;
		//Will be called by Arduino setup()

	inline RxHw(int ioPin): ioPin(ioPin) {;}
};

}; //namespace IRCtrl

#endif //#ifndef IRHW_IFACE_H
//Last Line
