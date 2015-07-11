/*ir_transmitter.h

*/

#ifndef IR_TRANSMITTER_H
#define IR_TRANSMITTER_H

#include <irhw_iface.h>
#include <ir_message.h>
#include <ir_packet.h>

namespace IRCtrl {

class Transmitter {
	enum TxState {
		TXSTATE_BUSY = 0, //Editing state.... Disable Rx part of ISR.
		TXSTATE_TRANSMITTING,
		TXSTATE_IDLE
	};

public:
//TODO: private:
	TxHw &txHw; //Controls the hardware
	volatile TxState txState; //Casting volatile is not sufficient for TxIdle?!?
	int txPos; //Index into tx buffers

	void SendTxPacket(); //Starts transmitting data written to txBuf

public:
	inline int GetOutputPin() {return txHw.GetOutputPin();}
	void ResetTx(); //Reset Tx State machine
	void ProcessISR(); //ISR for signal transmission
	inline bool TxIdle() {return TXSTATE_IDLE==txState;}
	void Send(const IRMsg &msg);

	//Buffer for outgoing IR signal:
	//(Place after often-used data; promotes the use of faster instructions
	// if present on processor?):
	PktRaw txBuf; //TODO: outBuf

	void Configure(); //Ensure gets called by Arduino setup()

	Transmitter(TxHw &txHw);
};

}; //namespace IRCtrl

#endif //#ifndef IR_TRANSMITTER_H
//Last Line
