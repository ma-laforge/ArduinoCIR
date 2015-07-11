/*ir_receiver.h
Declares interface for a receiver of IR controller signals.
*/

#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <arduino_ext.h>
#include <irhw_iface.h>
#include <ir_message.h>
#include <ir_packet.h>

namespace IRCtrl {

//Need to PopMsg/Reset after each received message for the reciever to start
//listening again.
//TODO:
//   -Instatiate in fast memory.
class Receiver {
private:
	//Min signal-free (idle) time for any supported packet to be considered
	//complete:
	//(Alias to control data type)
	static const usec_u16t SIGFREE_MIN = PktLimits::SIGFREE_MIN;

	enum RxState {
		RXSTATE_BUSY = 0, //Editing state.... Disable Rx part of ISR.
		RXSTATE_READY,
		RXSTATE_MSGCOMPLETE
	};

	RxHw &rxHw;
	RxState rxState;
	int ioPin; //Where to get HW readings from receiver.
	uint8_t rxSymMap[2]; //Adjusts for inverted rx signalling
	usec_u16t rxAccum; //Accumulator: Recieved symbol duration
	int rxLast; //Last symbols read/transmitted:
	int rxPos; //Index into rxBuf.symbols:

	bool DecodePkt(const PktInfo &prot, IRMsg *result) const;

public:
	void ProcessISR(); //ISR for message processing
	inline bool MsgAvailable() {return RXSTATE_MSGCOMPLETE==rxState;}

	void Reset(); //Reset state machine to read next message
	bool PopMsg(IRMsg *result); //Auto-resets state machine

	//Buffer for incomming IR signal:
	//(Place after often-used data; promotes the use of faster instructions
	// if present on processor?):
	RxBuffer rxBuf; //TODO: inBuf?
	//TODO: Do we need double-buffering?  Maybe use circular buffers?

	void Configure(); //Ensure gets called by Arduino setup()
	Receiver(RxHw &rxHw, bool invSig);
};

}; //namespace IRCtrl

#endif //#ifndef IR_RECEIVER_H
//Last Line
