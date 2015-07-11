/*ir_message.cpp

*/

#include <ir_message.h>
#include <ir_packet.h>

namespace IRCtrl {

bool IRMsg::operator==(const IRMsg &ref) const {
	return (protocol == ref.protocol) && (data == ref.data)
		&& (nbits == ref.nbits);
}

void IRMsg::BuildRaw(PktRaw &dest) const {
	//NOTE: Cannot inline - because pktInfo is not known by .h file
	//      (circular dependency problems)
	if (protocol < IRProtocol::pktInfoCount)
		IRProtocol::pktInfo[protocol].BuildRaw(data, nbits, dest);
};

}; //namespace IRCtrl

//Last Line
