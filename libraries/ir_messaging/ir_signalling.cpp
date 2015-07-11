/*ir_signalling.h
Defines low-level classes and constants, and functions used to transmit/recieve
physical IR signals.
*/

#include <ir_signalling.h>
#include <ir_debug.h>

namespace IRCtrl {

//Message decoding tools
//******************************************************************************

//Converts usec_s16t array into a 32-bit bit sequence of symbols.
//-tolTbl: Symbol tolerance table; supports tblLen <=8
//-Will move pos pointer forward in the sequence.
//-Returns # of bits decoded (0: error)
uint8_t GetBitSeq(const SymbolTol tolTbl[], uint8_t tblLen,
		const usec_s16t msg[], int &pos, uint32_t &result) {
	uint8_t nFound = 0;
	int _pos = pos; //Local copy - too much optimization?
	uint8_t i;
	result = 0;

	while (msg[_pos] != 0) {
		//Protect from runaway matches:
		if (_pos >= RxBuffer::PKTSYMB_MAX) return nFound;
//IRDebug::valAPu32 = _pos;
		if (msg[_pos] <= -PktLimits::SIGFREE_MIN) return nFound;
		i = 0;
		while (true) {
			if (i >= tblLen) return nFound;

			//Symbol durations must be ~integer multiple of symbol width:
			if (abs(tolTbl[i].nominal-abs(msg[_pos])) <= tolTbl[i].absTol) {
				result = result << i+1;
				if (msg[_pos]>=0) result |= ((uint8_t)0xFF)>>(7-i);
				nFound += i+1;
				break;
			}
			++i;
		}

		++_pos;
		pos = _pos; //Move position forward for each successful match
	}

	return nFound;
}
}; //namespace IRCtrl

//Last Line
