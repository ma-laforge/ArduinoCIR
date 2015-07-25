/*ir_receiver.cpp
Implements a receiver for IR controller signals.

*/

#include <ir_receiver.h>
#include <ir_debug.h>

namespace IRCtrl {

//Constructors & configuration
//******************************************************************************

Receiver::Receiver(RxHw &rxHw, bool invSig): rxHw(rxHw),
		ioPin(rxHw.ioPin), rxState(RXSTATE_BUSY) {
	if (invSig) {
		rxSymMap[0] = 1;
		rxSymMap[1] = 0;
	} else {
		rxSymMap[0] = 0;
		rxSymMap[1] = 1;
	}
	rxAccum = 0; //Reset does not reset this value
	Reset();
	pinMode(ioPin, INPUT);
}

void Receiver::Configure() {
	rxHw.Configure();
}

//Reset the Rx state machine:
void Receiver::Reset() {
	//WARNING: No HW control; Called by constructor
	rxState = RXSTATE_BUSY;
	rxLast = 0;
	rxPos = 0;
//	rxAccum = 0; //Keep accumulator value as count of dead time
	//NOTE: Might be wrong (ex: counting mark instead of space - but
	//      still useful to keep value)
	rxBuf.symbols[RxBuffer::PKTSYMB_MAX] = 0; //Not really necessary
	rxState = RXSTATE_READY;
}

//Read in a valid (supported) IRMsg if one is available
bool Receiver::PopMsg(IRMsg *result) {
	int i;

	if (!MsgAvailable()) return false;
	
	for (i=0; i<IRProtocol::pktInfoCount; ++i) {
		const PktInfo &pktInfo = IRProtocol::pktInfo[i];
		if (!pktInfo.rxDecode) continue;

		if (pktInfo.Decode(this->rxBuf, rxPos, result)) {
			Reset();
			return true;
		}
	}

	Reset(); //Message corrupt/unrecognized - prepare for next
	return false;
}

//TODO: Force in fast memory?
//Tag: LOWLEVELCODE
//TODO: make some functions using faster "LOWLEVELCODE"?
//TODO: Rx code appears amenable to a circular buffer implementation
void Receiver::ProcessISR() {
	const usec_s16t ACCUM_THRESH = 32000; //At least 2*RxHw::POLL_INTERVAL less than MAXINT16
	int symbol;
	bool done, newSymbol;

	if (RXSTATE_BUSY == rxState) return;

	//Read first (more accurate reading):
	symbol = rxSymMap[digitalRead(ioPin)&0x1];

	done = (rxAccum > SIGFREE_MIN && (0==symbol) && rxPos >= 1);
	newSymbol = (symbol != rxLast);

	//Handle a symbol change
	if (newSymbol || done) {
		//In case multiplies are expensive:
		if (rxLast > 0)
			rxBuf.symbols[rxPos] = rxAccum;
		else
			rxBuf.symbols[rxPos] = -rxAccum;

		if (!done) {
			//Prepare for next:
			rxAccum = 0;
			rxLast = symbol;
			++rxPos;
		} else {
			rxBuf.symbols[rxPos+1] = 0; //Clean break (easier to debug)
			rxState = RXSTATE_MSGCOMPLETE;
		}

		if (rxPos >= RxBuffer::PKTSYMB_MAX) {
			rxPos = RxBuffer::PKTSYMB_MAX-1; //Stall
		}
	}

	//Handle passage of time:
	rxAccum += RxHw::POLL_INTERVAL;
	if (rxAccum > ACCUM_THRESH) rxAccum = ACCUM_THRESH; //Saturate
}

}; //namespace IRCtrl

//Last Line
