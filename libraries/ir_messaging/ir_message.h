/*ir_message.h
High-level IR controller signalling classes and constants
*/

#ifndef IR_MESSAGE_H
#define IR_MESSAGE_H

#include <arduino_ext.h>
#include <ir_signalling.h>
#include <ir_protocol.h>

namespace IRCtrl {
//TODO: use more compact data type for delays??
//typedef unsigned int IRDelay; //Short delays for IR signalling

//High-level description of infrared packet:
class IRMsg {
public:
	uint8_t protocol; //Identifies the IR protocol; really type IRProtocol::ID
	uint32_t data;
	uint8_t nbits;    //Number of active bits in data
	//Panasonic needs address - investigate

	bool operator==(const IRMsg &ref) const;
	inline bool operator!=(const IRMsg &ref) const {return !(ref==*this);}
	void BuildRaw(PktRaw &dest) const;

	IRMsg();
	inline IRMsg(const IRMsg &src) {*this = src;}
	inline IRMsg(IRProtocol::ID protocol, uint32_t data, int nbits):
		protocol(protocol), data(data), nbits(nbits) {}
};

//-----Useful Messages-----
const IRMsg IRMSG_NA = IRMsg(IRProtocol::NONE, 0, 0); //Not applicable
const IRMsg IRMSG_NONE = IRMsg(IRProtocol::NONE, 0, 0);

inline IRMsg::IRMsg() {*this = IRMSG_NA;}

class IRSequenceElem {
public:
	//TODO: idleMin_ms: 11 bits; repeat: 5 bits; include repeat forever?
	IRMsg msg;
	msec_t idleMin_ms; //Min idle time before message can be sent.
	int repeat; //Number of times to repeat (0: send msg only once)

	inline IRSequenceElem():
		msg(IRMSG_NA), idleMin_ms(0), repeat(0) {}
	inline IRSequenceElem(const IRMsg &msg, msec_t idleMin_ms, int repeat):
		msg(msg), idleMin_ms(idleMin_ms), repeat(repeat) {}
};

//Wrapper object that points to sequence data:
class IRSequence {
public:
	const IRSequenceElem *elem;
	int length;

	inline IRSequence(const IRSequenceElem *elem = NULL, int length = 0):
		elem(elem), length(length) {}
};

}; //namespace IRCtrl

#endif //#ifndef IR_MESSAGE_H
//Last Line
