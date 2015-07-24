/*irhw.cpp
Implements IR TxHw/RxHw abstractions.
*/

#include <irhw.h>
#include <ir_debug.h>

namespace IRCtrl {

		const usec_u16t SETUPTIME = 100;
		//NOTE:
		//Expect modulator counter to count up by SETUPTIME, trigger an
		//overflow interrupt, then execute externally (user)-defined ISR.
		//Thus, there is a SETUPTIME delay before transmission.  SETUPTIME must
		//be large enough to not re-trigger before ISR can re-configure timer.


//******************************************************************************
namespace Timer8bSysClk {

	TxHw::TxHw(ArduinoHw::Timer8b::Timer &tmrCarrier, TxHwFetchSymbolHdlr fetchSymbol):
			tmrCarrier(tmrCarrier), ioPin(tmrCarrier.ioPinB), nextEvent(SETUPTIME),
			fetchSymbol(fetchSymbol), done(true) {
		using namespace ArduinoHw::Timer8b;

		//Configuration to transmit a 0:
		cfgCarrier[0].SetMode(Mode::PhaseCorrectPWM_OCR, ArduinoHw::Timer16b::Presc::Div1);
		//Transmit bit on normal data path (will be 0):
		cfgCarrier[0].SetCOMnB(ArduinoHw::Timer16b::CmpOMode::PFCPWM_Normal); //Bypass WfrmGenerator

		//Configuration to transmit a 1:
		cfgCarrier[1].SetMode(Mode::PhaseCorrectPWM_OCR, ArduinoHw::Timer16b::Presc::Div1);
		//Pulse output at the carrier rate (will be set by PWM control)
		cfgCarrier[1].SetCOMnB(ArduinoHw::Timer16b::CmpOMode::PFCPWM_ClearOnMatch_SetOnBottom);
	}

	void TxHw::Configure() {
		digitalWrite(ioPin, LOW);
		pinMode(ioPin, OUTPUT);
		SetCarrier(38); //Put in a default carrier
		SetOutput(0);
	}

	void TxHw::SetOutput(int value) {
		tmrCarrier.SetConfig(cfgCarrier[value&0x1]);
	}

	//Disables the modulator output once transmission is complete.
	//NOTE: Turns the output off
	void TxHw::Disable() {
		SetOutput(0);
		done = true;
	}

	//Enables the modulator output (start transmission):
	void TxHw::Enable() {
		done = false;
		nextEvent = SETUPTIME; //Mimic timing of interrupt-based solution
		while (!done) {
			wait_usec(nextEvent);
			(*fetchSymbol)();
		}
	}

	void TxHw::SetCarrier(int value_kHz) {
		//# of ref clock cycles for a period:
		const uint8_t nCycles = (ArduinoHw::refclk / 2000) / value_kHz;

		tmrCarrier.OCRnA = nCycles;

		//Hard-coded duty cycle for now:
		tmrCarrier.OCRnB = nCycles/3;
	}

}; //namespace Timer8bSysClk


//******************************************************************************
namespace Timer16b {
	TxHw::TxHw(ArduinoHw::Timer16b::Timer &tmrMod,
			ArduinoHw::Timer16b::Timer &tmrCarrier):
			tmrMod(tmrMod), tmrCarrier(tmrCarrier), ioPin(tmrCarrier.ioPinA) {
		using namespace ArduinoHw::Timer16b;

		//Configuration to transmit a 0:
		cfgCarrier[0].SetMode(Mode::PhaseCorrectPWM_ICR, Presc::Div1);
		//Transmit bit on normal data path (will be 0):
		cfgCarrier[0].SetCOMnA(CmpOMode::PFCPWM_Normal); //Bypass WfrmGenerator

		//Configuration to transmit a 1:
		cfgCarrier[1].SetMode(Mode::PhaseCorrectPWM_ICR, Presc::Div1);
		//Pulse output at the carrier rate (will be set by PWM control)
		cfgCarrier[1].SetCOMnA(CmpOMode::PFCPWM_ClearOnMatch_SetOnBottom);
	}

	void TxHw::Configure() {
		using namespace ArduinoHw::Timer16b;
		ArduinoHw::Timer16b::Config cfg;

		cfg.SetMode(Mode::FastPWM_ICR, CARRIER_PRESC);
		digitalWrite(ioPin, LOW);
		pinMode(ioPin, OUTPUT);
		//NOTE:
		//   -pinMode appears to re-write timer config bits!!!!!
		//    Must do before writing those registers.

		tmrMod.SetConfig(cfg);
		SetCarrier(38); //Put in a default carrier
		SetOutput(0);
	}

	void TxHw::SetOutput(int value) {
		tmrCarrier.SetConfig(cfgCarrier[value&0x1]);
	}

	//Disables the modulator output once transmission is complete.
	//NOTE: Turns the output off
	void TxHw::Disable() {
		tmrMod.SetTOIEn(0); //Disable TOIEn overflow interrupt
		SetOutput(0);
	}

	//Enables the modulator output (get ready for transmission):
	void TxHw::Enable() {
		tmrMod.TCNTn = 0; //Put in known state
		SetModEventTime(SETUPTIME);
		tmrMod.SetTOIEn(1); //Enable TOIEn overflow interrupt
		//User-defined interrupt handler has to handle actual transmission
	}

	//Sets modulator to trigger event a given time from last event:
	void TxHw::SetModEventTime(usec_u16t nextEvent) {
		tmrMod.ICRn = nextEvent<<USEC2CNT_SHIFT;
	}

	void TxHw::SetCarrier(int value_kHz) {
		//# of ref clock cycles for a period:
		const uint8_t nCycles = (ArduinoHw::refclk / 2000) / value_kHz;

		tmrCarrier.ICRn = nCycles;

		//Hard-coded duty cycle for now:
		tmrCarrier.OCRnA = nCycles/3;
	}

//******************************************************************************
	RxHw::RxHw(ArduinoHw::Timer16b::Timer &timer, int ioPin):
			::IRCtrl::RxHw(ioPin), timer(timer) {
	}

	void RxHw::Configure() {
		using namespace ArduinoHw::Timer16b;
		const usec_u32t USEC_PER_SEC = 1000000; //1sec in us
		const usec_s64t POLL_INTERVAL = RxHw::POLL_INTERVAL;
		//NOTE: Use uint64_t to avoid overflow.
		const uint16_t nCycles =
			(ArduinoHw::refclk * POLL_INTERVAL) / USEC_PER_SEC;
		ArduinoHw::Timer16b::Config cfg;

		pinMode(ioPin, INPUT);

		cli(); //Disable interrupts?

		//Configure timer to trigger interrupts @POLL_INTERVAL:
		cfg.SetMode(Mode::CTC_OCRnA, Presc::Div1);
		timer.SetConfig(cfg);
		timer.OCRnA = nCycles;
		timer.TCNTn = 0; //Reset timer
		timer.SetIEOCnA(1); //Enable OCnA interrupt

		sei(); //Enable interrupts?
	}
}; //namespace Timer16b
}; //namespace IRCtrl

//Last Line
