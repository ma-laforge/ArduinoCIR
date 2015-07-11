/*ir_packet.cpp
Defines classes and constants that store/manipulate IR message packets.
*/

#include <ir_packet.h>
#include <ir_message.h>
#include <ir_debug.h>

namespace IRCtrl {

//Constructors & configuration
//******************************************************************************
PktInfo::PktInfo(): decodeFn(DecodePktDefault) {
	rxDecode = false;
	protocol = IRProtocol::NONE;
	encoding = IRProtocol::ENC_PULSE_POS1;
	carrier = 38;
	preamble[0] = 0;
	postamble[0] = 0;
	for (int i=0; i<BITS_NUMELEM; ++i) bits[i][0] = 0;
}

PktInfo::PktInfo(bool rxDecode,
		IRProtocol::ID protocol, IRProtocol::Encoding encoding, int carrier,
		const SymCount *preamble, const SymCount *postamble,
		const SymCount *_00, const SymCount *_01,
		const SymCount *_10, const SymCount *_11,
		DecodeFnPtr decodeFn, BuildRawFnPtr buildRawFn):
		decodeFn(decodeFn), buildRawFn(buildRawFn) {
	int i;
	this->rxDecode = rxDecode;
	this->protocol = protocol;
	this->encoding = encoding;
	this->carrier = carrier;

	//TODO: Validate inputs (safe copy)?
	CopyNTA(this->preamble, preamble);
	CopyNTA(this->postamble, postamble);

	CopyNTA(this->bits[0], _00);
	CopyNTA(this->bits[1], _01);
	CopyNTA(this->bits[2], _10);
	CopyNTA(this->bits[3], _11);
}

//Message decoding
//******************************************************************************

//Default IRMsg decoder for an RxBuffer structure.
//NOTE: Valid for PDE, PWE, & PPE encoding only.  Manchester not supported.
//TODO: Check that rxPos does not exceed protocol max to speed up algorithm?
//validLen: How many elements of rxBuf.symbols[] are valid
bool DecodePktDefault(const PktInfo &info, const RxBuffer &rxBuf, int validLen,
		IRMsg *result) {
	const uint8_t bitsPerSymb =
		(info.encoding & IRProtocol::ENC_MASK_BITS_PER_SYMB)
		>>IRProtocol::ENC_LSB_BITS_PER_SYMB;
	const uint8_t nBitPatterns = bitsPerSymb<<1;
	//NOTE: could access a live version of rxPos instead of validLen
//	volatile int &rxPos(rxBuf->rxPos); //TODO: Exit routine if rxPos is moved earlier?
	int pos = 1; //Ignore first point... (saturated) time since last mark
	int bit = 0;
	uint8_t bitCount = 0;
	uint32_t data = 0;
	bool success, bitsMatched;

	//Try to match preamble:
	success = MatchSymbols(info.preamble, rxBuf.symbols, pos);

	//Try to match data-bits:
	bitsMatched = success;
	while (bitsMatched) {
		for (bit = 0; bit < nBitPatterns; ++bit) {
			bitsMatched = MatchSymbols(info.bits[bit], rxBuf.symbols, pos);
			if (bitsMatched) {
				data = (data<<bitsPerSymb)|bit;
				bitCount += bitsPerSymb;
				break;
			}
		}

		//Protect from runaway matches:
		//NOTE: we might read beyond the data - but at least we don't write
		//TODO: better protection to avoid accidental read beyond memory map???
		if (pos >= RxBuffer::PKTSYMB_MAX) bitsMatched = false;
	}

	//Try to match postamble:
	//TODO: Also use better protection in read access??
	if (success) {
		success = MatchSymbols(info.postamble, rxBuf.symbols, pos);
//		IRDebug::valu32 = info.postamble[0]+1<<12;
//		if (success) IRDebug::valu32 = 1023;
	}

/*
//	IRDebug::valu32 = pos;
	if (pos > IRDebug::valu32) {
		IRDebug::valu32 = pos;
		IRDebug::valIRMsg.protocol = info.protocol;
		IRDebug::valIRMsg.data = data;
		IRDebug::valIRMsg.nbits = bitCount;
	}
//	IRDebug::valu32 = validLen;
//	IRDebug::valu32 = bitCount;
*/

	//Return values on success:
	success = success && (validLen >= pos); //Did not read beyond valid data
	if (success && (result)) {
//		IRDebug::valu32 = 1024;
		result->protocol = info.protocol;
		result->data = data;
		result->nbits = bitCount;
	}
	return success;
}

//Message serialization
//******************************************************************************

//Might return null-terminated garbage if data exceeded
//Tag: LOWLEVELCODE
void BuildRawDefault(const PktInfo &pktInfo, uint32_t data, uint8_t nbits,
		PktRaw &dest) {
	const int PKTSYMB_MAX = PktRaw::PKTSYMB_MAX;
	const SymCount *symPtr;
	uint8_t dataMSB; //Try to coerce compiler to grab MSB in <=1 operation.
	int bitsPerSymb;
	int i, pos;

	if (pktInfo.encoding & IRProtocol::ENC_MASK_BITS_PER_SYMB
			!= IRProtocol::ENC_2BITS_PER_SYMB) {
		bitsPerSymb = 1;
	} else {
		bitsPerSymb = 2;
	}

	dest.symbols[PKTSYMB_MAX] = 0; //Safety
	dest.carrier = pktInfo.carrier;

	//Copy preamble:
	pos = 0;
	while (pktInfo.preamble[pos] != 0) {
		if (pos >= PKTSYMB_MAX) return;
		dest.symbols[pos] = pktInfo.preamble[pos];
		++pos;
	}

	//Copy data bits:
	while (nbits > 0) {
		//Try to coerce compiler to grab MSB in <=1 operation:
		dataMSB = data >> 24;

		symPtr = pktInfo.bits[dataMSB>>(8-bitsPerSymb)];

		i = 0;
		while (symPtr[i] != 0) {
			if (pos >= PKTSYMB_MAX) return;
			dest.symbols[pos] = symPtr[i];
			++pos; ++i;
		}
		data = data<<bitsPerSymb;
		nbits -= bitsPerSymb;
	}

	//Copy postamble:
	i = 0;
	while (pktInfo.postamble[i] != 0) {
		if (pos >= PKTSYMB_MAX) return;
		dest.symbols[pos] = pktInfo.postamble[i];
		++pos; ++i;
	}

	dest.symbols[pos] = 0; //Null-terminate the sequence
}

}; //namespace IRCtrl

//Last Line
