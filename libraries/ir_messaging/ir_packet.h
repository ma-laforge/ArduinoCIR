/*ir_packet.h
Declares base classes and constants that store/manipulate IR message packets.

Includes high-level understanding of protocol (ex: classes of encoding schemes).
*/

#ifndef IR_PACKET_H
#define IR_PACKET_H

#include <stdint.h>
#include <arduino_ext.h>
#include <ir_signalling.h>
#include <ir_message.h>

namespace IRCtrl {

//Generic protocol identifiers:
namespace IRProtocol {
	enum Encoding {
	//Tools:
		ENC_LSB_BITS_PER_SYMB = 0,
		ENC_MASK_BITS_PER_SYMB = 0x3<<ENC_LSB_BITS_PER_SYMB,
		ENC_LSB_TYPE = 2,
		ENC_MASK_TYPE = 0x3<<ENC_LSB_TYPE,

	//# of bits per symbol:
		ENC_1BIT_PER_SYMB  = 1<<ENC_LSB_BITS_PER_SYMB,
		ENC_2BITS_PER_SYMB = 2<<ENC_LSB_BITS_PER_SYMB,

	//Encoding types:
		//Pulse-position encoding (vary # of spaces):
		ENC_PULSE_POS = 0x1<<ENC_LSB_TYPE,
		//Pulse-width encoding (vary # of marks):
		ENC_PULSE_WIDTH = 0x2<<ENC_LSB_TYPE,
		//Manchester (is this the right name for a type?):
		ENC_MANCHESTER = 0x3<<ENC_LSB_TYPE,
		//TODO: Maybe replace ENC_MANCHESTER with ENC_OTHER (needs special processing)?

	//Aliases:
		ENC_PULSE_POS1   = ENC_PULSE_POS|ENC_1BIT_PER_SYMB,
		ENC_PULSE_WIDTH1 = ENC_PULSE_WIDTH|ENC_1BIT_PER_SYMB,
		ENC_PULSE_POS2   = ENC_PULSE_POS|ENC_2BITS_PER_SYMB,
	};
}; //namespace IRProtocol

bool DecodePktDefault(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result);
void BuildRawDefault(const PktInfo &pktInfo, uint32_t data, uint8_t nbits,
		PktRaw &dest);

//Describes an IR controller packet:
/*TODO:
	-Store preamble/postamble/bits as pointers to static data?!
	 (Speed issues vs storage issues?)
	-Remove rxDecode, and use a list of PktInfo in ir_receiver instead?
	-Add "int bitLength": Number of bits in a valid signal?
	-Add "usec_t signalFree": Required trailing signal-free time for
	 valid signal?
*/
class PktInfo {
public:
	//Aliases for # of required SymCount elements:
	static const int PREAMB_MAX = PktLimits::PREAMB_MAX;
	static const int POSTAMB_MAX = PktLimits::POSTAMB_MAX;
	static const int BIT_MAX = PktLimits::BIT_MAX;

	//Number of different bit combinations represented:
	static const int BITS_NUMELEM = 4; //00, 01, 10, 10

	IRProtocol::ID protocol; //Identifies this protocol
	IRProtocol::Encoding encoding;
	int carrier; //kHz

	bool rxDecode; //Should the receiver decode this protocol?

	typedef bool (*DecodeFnPtr)(const PktInfo &pktInfo,
		const RxBuffer &rxBuf, int validLen, IRMsg *result);
	typedef void (*BuildRawFnPtr)(const PktInfo &pktInfo,
		uint32_t data, uint8_t nbits, PktRaw &dest);

	//Assumption: Always a valid pointer
	DecodeFnPtr decodeFn;
	BuildRawFnPtr buildRawFn;

	inline bool Decode(const RxBuffer &rxBuf, int validLen, IRMsg *result) const
		{return decodeFn(*this, rxBuf, validLen, result);}
	inline void BuildRaw(uint32_t data, uint8_t nbits, PktRaw &dest) const
		{buildRawFn(*this, data, nbits, dest);}

	//Packet elements; to reconstruct symbols:
	//NOTE: Add 1 for trailing NULL
	SymCount preamble[PREAMB_MAX+1];
	SymCount postamble[POSTAMB_MAX+1];
	SymCount bits[BITS_NUMELEM][BIT_MAX+1]; //Represents: 00, 01, 10, 11

	//TODO: Overwrite with validating function?:
	virtual bool IsValid(uint32_t data, int nbits) {return true;}

	PktInfo();
	//WARNING! Ensure all SymCount * values provided are NULL terminated!!
	PktInfo(bool rxDecode,
		IRProtocol::ID protocol, IRProtocol::Encoding encoding, int carrier,
		const SymCount *preamble, const SymCount *postamble,
		const SymCount *_00, const SymCount *_01,
		const SymCount *_10, const SymCount *_11,
		DecodeFnPtr decodeFn = DecodePktDefault,
		BuildRawFnPtr buildRawFn = BuildRawDefault
	);
};

}; //namespace IRCtrl

#endif //#ifndef IR_PACKET_H
//Last Line
