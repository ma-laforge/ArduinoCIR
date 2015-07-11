/*ir_transmitter.cpp

*/

#include <ir_transmitter.h>
#include <ir_packet.h>

namespace IRCtrl {

//Constructors & configuration
//******************************************************************************
Transmitter::Transmitter(TxHw &txHw): txHw(txHw), txState(TXSTATE_BUSY) {
	ResetTx();
}

void Transmitter::Configure() {
	txHw.Configure();
}

//Resets the Tx state machine:
void Transmitter::ResetTx() {
	//WARNING: No HW control; Called by constructor
	txState = TXSTATE_BUSY;
	txPos = 0;
//	txHw.SetOutput(0);
	txState = TXSTATE_IDLE;
}

//******************************************************************************
//Start transmitting data stored in txBuf
void Transmitter::SendTxPacket() {
	SymCount symbolLen;

	if (txState != TXSTATE_IDLE) return;
	ResetTx(); //Just in case

	//NOTE: Must first ResetTx, then write to txBuf, then execute.
	//TODO: Move sequence to this function
	txBuf.symbols[PktRaw::PKTSYMB_MAX] = 0; //Faster here - safer in ISR
	txHw.SetCarrier(txBuf.carrier);

	symbolLen = txBuf.symbols[0];

	txState = TXSTATE_TRANSMITTING;
	txHw.Enable();
}

void Transmitter::Send(const IRMsg &msg) {
	ResetTx();
	msg.BuildRaw(txBuf);
	SendTxPacket();

	//TODO: Check TxIdle on ProcessEvent instead of "blocking thread"
	//      with while loop
	while (!TxIdle()) ;
}

//******************************************************************************
//TODO: Force in fast memory?
//Tag: LOWLEVELCODE
//TODO: make some functions using faster "LOWLEVELCODE"?
void Transmitter::ProcessISR() {
	SymCount symbolLen, nextLen;

	//Should not happen:
	if (TXSTATE_TRANSMITTING != txState) return;

	symbolLen = txBuf.symbols[txPos];
	nextLen = txBuf.symbols[txPos+1];

	if (0 == symbolLen) {
		//TODO: wait a minimum amount of time?
		txHw.Disable();
		ResetTx();
		return;
	}

	txHw.SetOutput(symbolLen > 1);
	txHw.SetModEventTime(abs(symbolLen));
	++txPos; //Move to next symbol duration
}

}; //namespace IRCtrl

//Last Line
