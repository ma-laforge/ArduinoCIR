/*irhw.cpp
Implements IR TxHw/RxHw abstractions.
*/

#include <irhw.h>
#include <ir_debug.h>

namespace IRCtrl {

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

//		IRDebug::valAPu32 = (uint32_t)&ArduinoHw::timer1.TIMSKn;
//		IRDebug::valAPu32 = (uint32_t)&ArduinoHw::timer3.TIMSKn;
//		IRDebug::valAPu32 = (uint32_t)&tmrMod.TIMSKn;

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
		const usec_u16t SETUPTIME = 100;

		//NOTE:
		//Expect modulator counter to count up by SETUPTIME, trigger an
		//overflow interrupt, and start execute externally (user)-defined ISR.
		//Thus, there is a SETUPTIME delay before transmissionin;  SETUPTIME must
		//be large enough to perform task in interrupt handler.

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
		const uint32_t USEC_PER_SEC = 1000000;
		const uint64_t POLL_INTERVAL = RxHw::POLL_INTERVAL;
		//NOTE: Use uint64_t to avoid overflow.
		const uint16_t nCycles =
			(ArduinoHw::refclk * POLL_INTERVAL) / USEC_PER_SEC;
		Config cfg;

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
