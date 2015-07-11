/*ir_signalling.h
Declares low-level classes and constants, and functions used to transmit/recieve
physical IR signals.
*/

#ifndef IR_SIGNALLING_H
#define IR_SIGNALLING_H

#include <stdint.h>
#include <arduino_ext.h>

namespace IRCtrl {

//Signal storage
//******************************************************************************

//Keep count of symbols by storing *span* of a given symbol, in microsec.
//NOTE:
//   -Low-level symbol (high/low); not a data symbol - which is made of multiple
//    low-level symbols
typedef int16_t SymCount;

//Size limits of suported packets
namespace PktLimits {
	//Min signal-free (idle) time for any supported packet to be considered
	//complete:
	//TODO: Deprecate version in ir_receiver.h
	static const usec_s16t SIGFREE_MIN = 10000;

	//Maximum supported number of message bits:
	static const int MSGBITS_MAX = 32;

	//Current estimate on max # of required SymCount elements:
	static const int PREAMB_MAX = 5;
	static const int POSTAMB_MAX = 5;
	static const int BIT_MAX = 2;

	//# SymCount required to represent IR controller packet in raw form:
	static const int PKTSYMB_MAX =
		PREAMB_MAX + POSTAMB_MAX + MSGBITS_MAX*BIT_MAX;
}

//Copy null-terminated array of SymCount (no checks):
inline int CopyNTA(SymCount dest[], const SymCount src[]) {
	int count = 0;
	do {
		*(dest++) = *(src++);
		++count;
		//TODO: Assert smaller than MAX?
	} while (*src != 0);

	return count-1;
}

//Store packet in raw form (used mostly for transmission):
class PktRaw {
public:
	//# SymCount required to represent IR controller packet in raw form:
	static const int PKTSYMB_MAX = PktLimits::PKTSYMB_MAX;

	int carrier; //kHz
	SymCount symbols[PKTSYMB_MAX + 1];

	PktRaw() {symbols[0] = 0;}
};

//Stores incomming IR signal as durations of 1 and 0 patterns.
//Use arrays of 16-bit signed values, in microseconds:
//   Positive: Signal detected (1), store duration.
//   Negative: No signal detected (0), store duration.
//NOTE:
//   -Format selected because different protocols work with completely
//    different timebases.
//   -Can store durations up to -32.768ms...32.767ms: Do not expect consecutive
//    1s or 0s to exceed this duration in any IR protocol.
class RxBuffer {
public:
	//Estimate # of required delay values:
	static const int SFREE_MAX = 1; //Signal-free time measurement

	//Max # of duration values requred to store IR controller packet in raw form:
	//Add one for garbage @ end of read.
	static const int PKTSYMB_MAX = PktLimits::PKTSYMB_MAX + SFREE_MAX + 1;

	//TODO: Use circular buffer (power of 2)?  Make longer?:
	usec_s16t symbols[PKTSYMB_MAX+1]; //Add one: Null-terminated
	RxBuffer() {symbols[0] = 0;}
};

//Signal decoding tools
//******************************************************************************

//Symbol tolerance values during signal reception:
class SymbolTol {
public:
	//Relative tolerance, in 16ths:
	static const uint8_t REL_TOL = 3; //18.75%
	static const int RELTOL_SHIFT = 4; //To cnvert back to 

	SymCount nominal; //Nominal value
	SymCount absTol;  //Absolute tolerance

	inline SymbolTol(SymCount nominal): nominal(nominal),
		absTol((nominal*REL_TOL)>>RELTOL_SHIFT) {};
};

//Match a pattern's symbols with what is in the message.
//Automatically advance the position counter.
//Not convinced this will actually inline!
//Tag: LOWLEVELCODE
//NOTE: Valid for PDE, PWE, & PPE encoding only.  Manchester not supported.
inline bool MatchSymbols(const SymCount pat[], const usec_s16t msg[], int &pos) {
	int _pos = pos;

	while (*pat != 0) {
		//Signs must match:
		if ((*pat<0)^(msg[_pos]<0)) return false;
		//Symbol durations must be similar:
		if (abs(*pat-msg[_pos])
				> ((abs(*pat)*SymbolTol::REL_TOL)>>SymbolTol::RELTOL_SHIFT))
			return false;
		++pat; ++_pos;
	}

	pos = _pos; //Move position forward on success
	return true;
}

//Converts usec_s16t array into a 32-bit bit sequence of symbols.
uint8_t GetBitSeq(const SymbolTol tolTbl[], uint8_t tblLen,
		const usec_s16t msg[], int &pos, uint32_t &result);

}; //namespace IRCtrl

#endif //#ifndef IR_SIGNALLING_H
//Last Line
