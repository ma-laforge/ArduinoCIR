/*arduino_timers.cpp
Defines low-level abstractions facilitating the control of Arduino
timers/counters.
*/

#include <arduino_timers.h>

namespace ArduinoHw {

namespace Timer16b {

	void Config::SetMode(Mode::ID mode, Presc::ID presc) {
		TCCRnA = 0; TCCRnB = 0; //Reset options
		_AHW_WRITE8_2B(TCCRnA, WGM10, mode);    //Write lower mode bits
		_AHW_WRITE8_2B(TCCRnB, WGM12, mode>>2); //Write upper mode bits
		_AHW_WRITE8_3B(TCCRnB, CS10, presc);    //Write prescaler bits
	}
	void Config::SetCOMnA(CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM1A0, oMode);
	}
	void Config::SetCOMnB(CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM1B0, oMode);
	}
#ifdef _AHW_TIMERS_HAS16B3OC
	void Config::SetCOMnC(CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM1C0, oMode);
	}
#endif
};

namespace Timer8b {

	void Config::SetMode(Mode::ID mode, Timer16b::Presc::ID presc) {
		TCCRnA = 0; TCCRnB = 0; //Reset options
		_AHW_WRITE8_2B(TCCRnA, WGM20, mode);    //Write lower mode bits
		_AHW_WRITE8_1B(TCCRnB, WGM22, mode>>2); //Write upper mode bits
		_AHW_WRITE8_3B(TCCRnB, CS20, presc);    //Write prescaler bits
	}
	void Config::SetCOMnA(Timer16b::CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM2A0, oMode);
	}
	void Config::SetCOMnB(Timer16b::CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM2B0, oMode);
	}
};

}; //namespace ArduinoHw

//Last Line
