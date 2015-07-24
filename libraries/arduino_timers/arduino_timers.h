/*arduino_timers.h
Declares low-level abstractions facilitating the control of Arduino
timers/counters.

It would be nice to provide a generic interface that could control the timers
from any platform in order to set the PWM period & duty-cycle at a high level.

This is probably not a trivial task.

REMARKS:
	-I don't like how constants are shared across different timers...
	 Some more thought would be needed here.
	-NOT FULLY CONFIRMED!!!
	-Arduino HW access is a little odd in a few respects:
		-Not obvious to which register a given bit constant applies
		 (ex: is WGM10 for TCCRnA or B?)
		-WGMnx not a contiguous bit field.
		-On ATmega2560, all TCCRnA/B values are the same for timers 1,3,4,5, but
		 multiple bit constants are defined (one for each timer).
	-Timer 0 is not supported.  Probably a bad idea to take control of it.
		Used to measure time & implement delays (millis(), delay(), ...).
	-Timer 1 flags are therefore used here for all Timer16b timers (is this valid?).
		(OCIE1A, TOIE1, COM1A0, COM1B0, COM1C0, WGM10, WGM12, CS10, ...)
	-Timer 2 flags are therefore used here for all Timer8b timers (is this valid?).
		(OCIE2A, COM2A0, WGM20, WGM22, CS20, ...)
*/

//TODO: Move Timer16b::Mode/Presc/CmpOMode/Config up one namespace???

#ifndef ARDUINO_TIMERS_H
#define ARDUINO_TIMERS_H

#include <arduino_timers_hwdetect.h>
#include <arduino_ext.h>
#include <arduino_hwext.h>

//Assumes implementation of timer abstractions will instantiate timers as
//ArduinoHw::timerX.  This is a workaround to the fact that timers of
//different classes cannot be accessed from a vector >>of concrete types<<.
#define _AHW_GET_TIMER(id) (ArduinoHw::timer ## id)

#ifndef F_CPU
//TODO: Find out if this is really used.
//It looks like there might be floating point versions of this #define.
//It would be preferable to have a const instead of a #define
	#define F_CPU 16000000L //Hz
#endif




namespace ArduinoHw {
	//Internal refclk:
	const hz_t refclk = F_CPU; //Hardcoded for now

//Abstraction of ATMEL's 16-bit timers:
namespace Timer16b {

	//Appears to work for Timer8b as well - but some settings are invalid
	namespace Mode {
		enum ID {
			Normal = 0,
			PhaseCorrectPWM_8bit,
			PhaseCorrectPWM_9bit,
			PhaseCorrectPWM_10bit,
			CTC_OCRnA,
			FastPWM_8bit,
			FastPWM_9bit,
			FastPWM_10bit,
			PhaseFreqCorrectPWM_ICR,
			PhaseFreqCorrectPWM_OCRnA,
			PhaseCorrectPWM_ICR,
			PhaseCorrectPWM_OCRnA,
			CTC_ICR,
			RESERVED,
			FastPWM_ICR,
			FastPWM_OCRnA,
		};
	};

	//Prescaler.  Only for certain timers:
	//Appears to work for Timer8b as well
	namespace Presc {
		enum ID {
			Select0 = 0, //Not sure what this is
			Div1, Div8, Div64, Div256, Div1024,
			SyncInv, Sync, //Not sure what these are
		};

		//Prescaler divider values.  Corresponds to ID:
		const uint16_t DIVIDERS[] = {0, 1, 8, 64, 256, 1024, 0, 0};
	};

	//Compare output modes:
	namespace CmpOMode {
		enum ID {
			//In non-PWM modes:
			NonPWM_Normal = 0,
			NonPWM_ToggleOnMatch,
			NonPWM_ClearOnMatch,
			NonPWM_SetOnMatch,

			//In fast-PWM modes:
			FastPWM_Normal = 0,
			FastPWM_ToggleOCnA_OnMatch,
			FastPWM_ClearOnMatch_SetOnBottom,
			FastPWM_SetOnMatch_ClearOnBottom,

			//In Phase/Phase-Frequency Corrected modes:
			PFCPWM_Normal = 0,
			PFCPWM_ToggleOCnA_OnMatch,
			PFCPWM_ClearOnMatch_SetOnBottom,
			PFCPWM_SetOnMatch_ClearOnBottom,
		};
	};

	//Timer16b Configuration object (abstracts away bitfields):
	//Multiple copies can be created, and used to apply onto "Timer"
	class Config {
	public:
		uint8_t TCCRnA, TCCRnB;

		//Select desired timer mode; Will ovrewrite COMnx values:
		void SetMode(Mode::ID mode, Presc::ID presc = Presc::Div1);

		//Select desired compare output mode:
		void SetCOMnA(CmpOMode::ID oMode);
		void SetCOMnB(CmpOMode::ID oMode);
#ifdef _AHW_TIMERS_HAS16B3OC
		void SetCOMnC(CmpOMode::ID oMode);
#endif

		inline Config(Mode::ID mode = Mode::Normal, Presc::ID presc = Presc::Div1)
			{SetMode(mode, presc);};
	};

	//Abstraction for Timer16b registers
	//NOTE: Not a direct memory map.  Not sure if code will suit all platforms.
	class Timer {
	public:
		//Access to registers:
		v_uint8_t &TCCRnA, &TCCRnB; //Main configuration
		v_uint16_t &TCNTn; //The timer register
		v_uint16_t &ICRn; //Input compare register (can reset/reverse timer)
		v_uint16_t &OCRnA; //Output compare register (can toggle output pin)
		v_uint16_t &OCRnB; //Output compare register (can toggle output pin)
#ifdef _AHW_TIMERS_HAS16B3OC
		v_uint16_t &OCRnC; //Output compare register (can toggle output pin)
#endif
		v_uint8_t &TIMSKn; //Interrupt register

		inline void SetConfig(const Config &cfg)
			{TCCRnA = cfg.TCCRnA; TCCRnB = cfg.TCCRnB;};
		//TODO: Make sure TCCRnx is written in correct order?
		//TODO: GetConfig()?

		//Clear all interupts:
		inline void ClearAllInt() {TIMSKn = 0;}

		//Set (1)/clear (0) OCnA interupt enable:
		inline void SetIEOCnA(int val) {_AHW_WRITE8_1B(TIMSKn, OCIE1A, val);}

		//Set (1)/clear (0) TOIEn interupt (TOVn; overflow) enable:
		inline void SetTOIEn(int val) {_AHW_WRITE8_1B(TIMSKn, TOIE1, val);}

		//TODO Clear individual interupts, set other interupts

		//Not really registers, but good place to store pin #:
		const uint8_t ioPinA, ioPinB;
#ifdef _AHW_TIMERS_HAS16B3OC
		const uint8_t ioPinC;

		inline Timer(v_uint8_t &TCCRnA, v_uint8_t &TCCRnB, v_uint16_t &TCNTn,
			v_uint16_t &ICRn, v_uint16_t &OCRnA, uint8_t ioPinA,
			v_uint16_t &OCRnB, uint8_t ioPinB, v_uint16_t &OCRnC, uint8_t ioPinC,
			v_uint8_t &TIMSKn): TCCRnA(TCCRnA), TCCRnB(TCCRnB),
			TCNTn(TCNTn), ICRn(ICRn),
			OCRnA(OCRnA), ioPinA(ioPinA), OCRnB(OCRnB), ioPinB(ioPinB),
			OCRnC(OCRnC), ioPinC(ioPinC), TIMSKn(TIMSKn) {;};
#else
		inline Timer(v_uint8_t &TCCRnA, v_uint8_t &TCCRnB, v_uint16_t &TCNTn,
			v_uint16_t &ICRn, v_uint16_t &OCRnA, uint8_t ioPinA,
			v_uint16_t &OCRnB, uint8_t ioPinB,
			v_uint8_t &TIMSKn): TCCRnA(TCCRnA), TCCRnB(TCCRnB),
			TCNTn(TCNTn), ICRn(ICRn),
			OCRnA(OCRnA), ioPinA(ioPinA), OCRnB(OCRnB), ioPinB(ioPinB),
			TIMSKn(TIMSKn) {;};
#endif
	};
};

//Abstraction of ATMEL's 8-bit asynchronous timers w/PWM (Timer 2):
namespace Timer8b {
	namespace Mode {
		enum ID {
			Normal = 0,
			PWM_PhaseCorrectPWM_8bit,
			CTC_OCR,
			FastPWM_8bit,
			Reserved1,
			PhaseCorrectPWM_OCR,
			Reserved2,
			FastPWM_OCR,
		};
	};

	//Configuration object (abstracts away bitfields):
	//Multiple copies can be created, and used to apply onto "Timer"
	class Config {
	public:
		uint8_t TCCRnA, TCCRnB;

		//Select desired timer mode; Will ovrewrite COMnx values:
		void SetMode(Mode::ID mode, Timer16b::Presc::ID presc = Timer16b::Presc::Div1);

		//Select desired compare output mode:
		void SetCOMnA(Timer16b::CmpOMode::ID oMode);
		void SetCOMnB(Timer16b::CmpOMode::ID oMode);

		inline Config(Mode::ID mode = Mode::Normal, Timer16b::Presc::ID presc = Timer16b::Presc::Div1)
			{SetMode(mode, presc);};
	};

	//Not fully implemented:
	class Timer {
	public:

		//Access to registers:
		v_uint8_t &TCCRnA, &TCCRnB; //Main configuration
		v_uint8_t &OCRnA; //Output compare register (can toggle output pin)
		v_uint8_t &OCRnB; //Output compare register (can toggle output pin)
		v_uint8_t &TIMSKn; //Interrupt register

		inline void SetConfig(const Config &cfg)
			{TCCRnA = cfg.TCCRnA; TCCRnB = cfg.TCCRnB;};
		//TODO: Make sure TCCRnx is written in correct order?

		inline void ClearAllInt() {TIMSKn = 0;}
		inline void SetIEOCnA(int val) {_AHW_WRITE8_1B(TIMSKn, OCIE2A, val);}

		//Not really registers, but good place to store pin #:
		const uint8_t ioPinA, ioPinB;

		inline Timer(v_uint8_t &TCCRnA, v_uint8_t &TCCRnB,
			v_uint8_t &OCRnA, uint8_t ioPinA, v_uint8_t &OCRnB, uint8_t ioPinB,
			v_uint8_t &TIMSKn): TCCRnA(TCCRnA), TCCRnB(TCCRnB),
			OCRnA(OCRnA), ioPinA(ioPinA), OCRnB(OCRnB), ioPinB(ioPinB),
			TIMSKn(TIMSKn) {;};
	};
};

}; //namespace ArduinoHw

//Include timer definitions for all supported platforms
//******************************************************************************
#include <arduino_timers_atmel.h>

#endif //#ifndef ARDUINO_TIMERS_H
//Last Line
