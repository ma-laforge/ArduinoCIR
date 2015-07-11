/*irhw.h
Contains explicit declarations of the HW-specific components required for
IR signalling.
*/

#ifndef IRHW_H
#define IRHW_H

#include <irhw_iface.h>
#include <irmsg_validatehw.h>
#include <arduino_timers.h>

//Helper macros needed to simplify how tx/rx blocks are created
//TODO: Move macros to a higher-level file??
//******************************************************************************
//Create a high-level IR transmitter (TxHw/Transmitter) using two 16-bit timers:
//Suggestion: Use timer1 for modulator (highest priority/lowest jitter?)
#define __IRTX_CREATE_TMR16B(txSymbol, tmrMod, tmrCarrier) \
	/*-----Instantiate HW-specific components-----*/ \
	/*TODO: Create TxHw in fast memory?*/ \
	IRCtrl::Timer16b::TxHw (_irmsghw_##txSymbol)( \
		_AHW_GET_TIMER(tmrMod), _AHW_GET_TIMER(tmrCarrier) \
	); \
	/*-----Povide HW-agnostic objects-----*/ \
	IRCtrl::Transmitter (txSymbol)(_irmsghw_##txSymbol); \
	/*-----ISR probably gets defined in global namespace-----*/ \
	ISR(TIMER ## tmrMod ## _OVF_vect) {(txSymbol).ProcessISR();} \

//Create a high-level IR receiver (RxHw) using a 16-bit timer:
#define __IRRX_CREATE_TMR16B(rxSymbol, timer, ioPin, invSig) \
	/*-----Instantiate HW-specific components-----*/ \
	/*TODO: Create RxHw in fast memory?*/ \
	IRCtrl::Timer16b::RxHw (_irmsghw_##rxSymbol)( \
		_AHW_GET_TIMER(timer), (ioPin) \
	); \
	/*-----Povide HW-agnostic objects-----*/ \
	IRCtrl::Receiver (rxSymbol)(_irmsghw_##rxSymbol, (invSig)); \
	/*-----ISR probably gets defined in global namespace-----*/ \
	ISR(TIMER ## timer ## _COMPA_vect) {(rxSymbol).ProcessISR();} \

//Use following macros instead.  They allow define-d values for timer values.
#define IRTX_CREATE_TMR16B(txSymbol, tmrMod, tmrCarrier) \
	__IRTX_CREATE_TMR16B(txSymbol, tmrMod, tmrCarrier)
#define IRRX_CREATE_TMR16B(rxSymbol, timer, ioPin, invSig) \
	__IRRX_CREATE_TMR16B(rxSymbol, timer, ioPin, invSig)

//Low-level IR rx/tx controllers
//******************************************************************************
namespace IRCtrl {
namespace Timer16b {
	//Implementation of TxHw object using 2 of ATMEL's 16-bit timers:
	//NOTE:
	//   -Assume we have a 16MHz clock.  Should be ok given that this file
	//    is platform-specific, and the clock is known.
	//TODO: Assert that we have a 16MHz clock or make code more generic
	class TxHw: public ::IRCtrl::TxHw {
	public:
		//Assume: Clock: 16MHz.  If we div8, we need to mult. desired
		//time in us by two (shift by 1) to get clock count.
		static const ArduinoHw::Timer16b::Presc::ID CARRIER_PRESC =
			ArduinoHw::Timer16b::Presc::Div8;
		static const int USEC2CNT_SHIFT = 1; //Convert usec to counts

		ArduinoHw::Timer16b::Timer &tmrMod;
		ArduinoHw::Timer16b::Timer &tmrCarrier;
		ArduinoHw::Timer16b::Config cfgCarrier[2]; //Carrier config
		const int ioPin; //Cached

		inline int GetOutputPin() {return ioPin;}
		void SetOutput(int value);
		void Enable();
		void Disable();
		void SetModEventTime(usec_u16t nextEvent);
		void SetCarrier(int value_kHz);
		void Configure();

		TxHw(ArduinoHw::Timer16b::Timer &tmrMod,
			ArduinoHw::Timer16b::Timer &tmrCarrier);
	};

	//Implementation of HwMsgProc object using ATMEL's 16-bit timer:
	class RxHw: public ::IRCtrl::RxHw {
	public:
		ArduinoHw::Timer16b::Timer &timer; //Main timer

		void Configure();
		RxHw(ArduinoHw::Timer16b::Timer &timer, int ioPin);
	};
}; //namespace Timer16b
}; //namespace IRCtrl

#endif //#ifndef IRHW_H
//Last Line
