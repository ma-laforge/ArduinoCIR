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
	void Config::SetCOMnC(CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM1C0, oMode);
	}

};

namespace Timer8b {

	void Config::SetMode(Mode::ID mode, Timer16b::Presc::ID presc) {
		TCCRnA = 0; TCCRnB = 0; //Reset options
		_AHW_WRITE8_2B(TCCRnA, WGM10, mode);    //Write lower mode bits
		_AHW_WRITE8_1B(TCCRnB, WGM12, mode>>2); //Write upper mode bits
		_AHW_WRITE8_3B(TCCRnB, CS10, presc);    //Write prescaler bits
	}
	void Config::SetCOMnA(Timer16b::CmpOMode::ID oMode) {
		_AHW_WRITE8_2B(TCCRnA, COM1A0, oMode);
	}
};

}; //namespace ArduinoHw

//Last Line
