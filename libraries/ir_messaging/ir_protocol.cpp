/*ir_protocol.cpp
Implements classes and constants that describe IR signalling protocols.
*/

#include <ir_protocol.h>
#include <ir_packet.h>
#include <ir_debug.h>

namespace IRCtrl {
bool DecodePkt_Sony(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result);
bool DecodePkt_RC5(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result);

namespace IRProtocol {
	//Easy-to-use constants:
	const SymCount NEC_UNIT = INTDIV_RND(2250, 4); //Unit time: 562.5us
	const SymCount SONY_UNIT = INTDIV_RND(1200, 2); //Unit time: 600us
	const SymCount RC5_UNIT = INTDIV_RND(((uint32_t)8000), 9); //Unit time: ~889us
	const SymCount RC6_UNIT = INTDIV_RND(((uint32_t)4000), 9); //Unit time: ~444us
	const SymCount RCMM_UNIT = INTDIV_RND(1000, 36);  //Unit time: 27.77us

	typedef const SymCount SymCList[];

	//TODO: Store this in fast memory? Arduino has a keyword?
	//NOTE: Order *must* match enum IRProtocol::ID
	const PktInfo pktInfo[] = {
		PktInfo(), //0: Nothing
		PktInfo(true, IRProtocol::NEC, ENC_PULSE_POS1, 38,
			(SymCList) {16*NEC_UNIT, -8*NEC_UNIT, 0}, //Preamble
			(SymCList) {NEC_UNIT, 0},                 //Postamble
			(SymCList) {NEC_UNIT, -NEC_UNIT, 0},      //0
			(SymCList) {NEC_UNIT, -3*NEC_UNIT, 0},    //1
			(SymCList) {0}, (SymCList) {0}
		),
		PktInfo(true, IRProtocol::NEC_RPT, ENC_PULSE_POS1, 38,
			(SymCList) {16*NEC_UNIT, -4*NEC_UNIT, 0}, //Preamble
			(SymCList) {NEC_UNIT, 0},                 //Postamble
			(SymCList) {NEC_UNIT, -NEC_UNIT, 0},      //0
			(SymCList) {NEC_UNIT, -3*NEC_UNIT, 0},    //1
			(SymCList) {0}, (SymCList) {0}
		),
		//TODO: Hacked protocol.  Bit symbols are defined as starting
		//with a mark.  Using space instead so that the MatchSymbols
		//function works without dealing with last symbol
		//duration(space) < gap - instead of what we do now:
		//duration(mark) ~ meas_duration(mark)
		PktInfo(true, IRProtocol::SONY, ENC_PULSE_WIDTH1, 40,
			(SymCList) {4*SONY_UNIT, 0}, //Preamble
			(SymCList) {0},                          //Postamble
			(SymCList) {-SONY_UNIT, SONY_UNIT, 0},   //0
			(SymCList) {-SONY_UNIT, 2*SONY_UNIT, 0}, //1
			(SymCList) {0}, (SymCList) {0},
			DecodePkt_Sony
		),
		PktInfo(false, IRProtocol::RCMM, ENC_PULSE_POS2, 36,
			(SymCList) {15*RCMM_UNIT, -10*RCMM_UNIT, 0}, //Preamble
			(SymCList) {6*RCMM_UNIT, 0}, //Postamble
			(SymCList) {6*RCMM_UNIT, -10*RCMM_UNIT, 0},  //00
			(SymCList) {6*RCMM_UNIT, -16*RCMM_UNIT, 0},  //01
			(SymCList) {6*RCMM_UNIT, -22*RCMM_UNIT, 0},  //10
			(SymCList) {6*RCMM_UNIT, -28*RCMM_UNIT, 0}   //11
		),
		PktInfo(true, IRProtocol::RC5, ENC_MANCHESTER, 36,
			(SymCList) {0}, //Preamble
			(SymCList) {0}, //Postamble
			(SymCList) {-1*RC5_UNIT, 1*RC5_UNIT, 0},  //00
			(SymCList) {1*RC5_UNIT, -1*RC5_UNIT, 0},  //01
			(SymCList) {0}, (SymCList) {0},
			DecodePkt_RC5
		),
	};

	//TODO: Assert size with IRProtoclol::PROTOCOL_COUNT
	const int pktInfoCount = sizeof(pktInfo)/sizeof(PktInfo);
}; //namespace IRProtocol

//Message decoding
//******************************************************************************

const SymbolTol tolTbl_RC5[] = {
	SymbolTol(IRProtocol::RC5_UNIT), SymbolTol(2*IRProtocol::RC5_UNIT)
};
const SymbolTol tolTbl_RC6[] = {
	SymbolTol(IRProtocol::RC6_UNIT), SymbolTol(2*IRProtocol::RC6_UNIT),
	SymbolTol(3*IRProtocol::RC6_UNIT), SymbolTol(4*IRProtocol::RC6_UNIT)
};

const int tolTbl_RC5_len = sizeof(tolTbl_RC5)/sizeof(SymbolTol);
const int tolTbl_RC6_len = sizeof(tolTbl_RC6)/sizeof(SymbolTol);

/*NOTE:
	-Assumes result exists
	-Converts 01 sequences to 0, and 10 sequences to 0 when invertSym=false
*/
bool DecodeBitSeq_Manchester(uint32_t bitSeq, uint8_t seqLen, bool invertSym,
		IRMsg *result)
{
	uint32_t data = 0;
	uint8_t bitCount = 0;
	uint8_t dataWnd, curBit;

	bitSeq = bitSeq << (32-seqLen);

	while (seqLen > 0) {
		dataWnd = (bitSeq>>24) & (0x3<<6); //Hope this gets optimized well
		if ((0x1<<6) == dataWnd) {
			curBit = 0;
		} else if ((0x2<<6) == dataWnd) {
			curBit = 1;
		} else { //Not a valid message
			return false;
		}

		data = (data << 1) | curBit;
		++bitCount; seqLen -= 2; bitSeq = bitSeq << 2;
	}

	if (invertSym) {
		result->data = (~data) & ((1<<bitCount)-1);
	} else {
		result->data = data;
	}
	result->nbits = bitCount;
	return true;
}

//Decodes Sony signalling (in RxBuffer) to an IRMsg.
//validLen: How many elements of rxBuf.symbols[] are valid
bool DecodePkt_Sony(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result) {
	bool success;
	success = DecodePktDefault(info, rxBuf, validLen, result);

	if (success && (result)) {
		//Two supported pattern lengths:
		if (result->nbits != 12 && result->nbits != 24) return false;
	}

	return success;
}

//Decodes RC5 signalling (in RxBuffer) to an IRMsg.
//validLen: How many elements of rxBuf.symbols[] are valid
bool DecodePkt_RC5(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result) {
	int pos = 1; //Ignore first point... (saturated) time since last mark
	uint8_t seqLen;
	uint32_t bitSeq;
/*
	Background:
	-Read all bits as data bits (preamble is not special).
	-First character will be a 1... Must therefore pad with a leading 0.
*/
	//Try to match preamble:
//	if (!MatchSymbols(info.preamble, rxBuf.symbols, pos)) return false;

	//Convert to a bit sequence:
	seqLen = GetBitSeq(tolTbl_RC5, tolTbl_RC5_len, rxBuf.symbols, pos, bitSeq);

	//Ensure did not read beyond valid data:
	if (validLen < pos) return false;

	//Make sure message was read to the end:
	if (rxBuf.symbols[pos] > -PktLimits::SIGFREE_MIN) return false;
//	if (rxBuf.symbols[pos] != 0) return false; //Did not make it to end of signal

	//Pad with leading 0:
	++seqLen;

	//Add trailing 0, if needed:
	if (((uint8_t)seqLen) & 0x1) {
		++seqLen;
		bitSeq = bitSeq << 1;
	} //Unnecessary given decoding scheme

	if (seqLen > 32) return false; //Make sure data made it in register.

	if (!result) return false; //Simply will not match if nothing to write to.
	if (!DecodeBitSeq_Manchester(bitSeq, seqLen, true, result)) return false;
	result->protocol = IRProtocol::RC5;

	//Must match protocol (Avoid false matches):
//IRDebug::valAPu32 = 250+result->nbits;
	if (result->nbits != 14) return false;
//IRDebug::valAPu32 = result->data;
	if (result->data & 0x3000 != 0x3000) return false;

	return true;
}

/*
ABNORMAL PROTOCOL! NEEDS MORE THOUGHT!!

//Decodes RC6 signalling (in RxBuffer) to an IRMsg.
//validLen: How many elements of rxBuf.symbols[] are valid
bool DecodePkt_RC6(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result) {
	int pos = 1; //Ignore first point... (saturated) time since last mark
	uint8_t seqLen;
	uint32_t bitSeq;

	//Try to match preamble:
	if (!MatchSymbols(info.preamble, rxBuf.symbols, pos)) return false;

	//Convert to a bit sequence:
	seqLen = GetBitSeq(tolTbl_RC6, tolTbl_RC6_len, rxBuf.symbols, pos, bitSeq);

	//Ensure did not read beyond valid data:
	if (validLen >= pos) return false;

	if (rxBuf.symbols[pos] != 0) return false; //Did not make it to end of signal
	if (seqLen > 31) return false; //Will overflow in following operations.

	//Pad with leading 0:
	bitSeq = bitSeq << 1;
	++seqLen;

	if (!result) return false; //Simply will not match if nothing to write to.
	if (!DecodeBitSeq_Manchester(bitSeq, seqLen, true, result)) return false;
	result->protocol = IRProtocol::RC6;

	//Must match protocol (Avoid false matches):
	if (result->nbits != 14) return false;
	if (result->data & 0x3800 != 30000) return false;

	return true;
}
*/

}; //namespace IRCtrl

//Last Line
