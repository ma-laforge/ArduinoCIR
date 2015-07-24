/*ir_serialtools.cpp
Implements tools to help send info through the serial monitor.
*/

#include <ir_serialtools.h>

#ifdef ENABLE_IR_SERIAL_TOOLS
#include <ir_receiver.h> //Access to structures

namespace IRSerial {

bool configured = false;

void Configure() {
	if (!configured) {
		Serial.begin(9600);
	}
	configured = true;
}

namespace ProtName {
	const char PN_None[] = "None";
	const char PN_NEC[] = "NEC";
	const char PN_NEC_RPT[] = "NEC (Repeat)";
	const char PN_Sony[] = "Sony";
	const char PN_RC5[] = "RC5";
	const char PN_RC6[] = "RC6";
	const char PN_Dish[] = "Dish";
	const char PN_Sharp[] = "Sharp";
	const char PN_Panasonic[] = "Panasonic";
	const char PN_JVC[] = "JVC";
	const char PN_Sanyo[] = "Sanyo";
	const char PN_Mitsubishi[] = "Mitsubishi";
	const char PN_RCMM[] = "RCMM";
	const char PN_Unknown[] = "Unknown";
};

const char *GetProtName(IRCtrl::IRProtocol::ID protocol) {
	const char *result;
	switch(protocol) {
	case IRCtrl::IRProtocol::NONE: result = ProtName::PN_None; break;
	case IRCtrl::IRProtocol::NEC: result = ProtName::PN_NEC; break;
	case IRCtrl::IRProtocol::NEC_RPT: result = ProtName::PN_NEC_RPT; break;
	case IRCtrl::IRProtocol::SONY: result = ProtName::PN_Sony; break;
	case IRCtrl::IRProtocol::RCMM: result = ProtName::PN_RCMM; break;
/*
	case IRCtrl::IRProtocol::RC5: result = ProtName::PN_RC5; break;
	case IRCtrl::IRProtocol::RC6: result = ProtName::PN_RC6; break;
	case IRCtrl::IRProtocol::DISH: result = ProtName::PN_Dish; break;
	case IRCtrl::IRProtocol::SHARP: result = ProtName::PN_Sharp; break;
	case IRCtrl::IRProtocol::PANASONIC: result = ProtName::PN_Panasonic; break;
	case IRCtrl::IRProtocol::JVC: result = ProtName::PN_JVC; break;
	case IRCtrl::IRProtocol::SANYO: result = ProtName::PN_Sanyo; break;
	case IRCtrl::IRProtocol::MITSUBISHI: result = ProtName::PN_Mitsubishi; break;
*/
	default: result = ProtName::PN_Unknown; break;
	}
	return result;
}

void Print(const ArduinoHw::Timer16b::Timer &reg) {
      Serial.print("\n   TCCRnA/B: 0b");
      Serial.print(reg.TCCRnA, BIN);
      Serial.print("/0b");
      Serial.print(reg.TCCRnB, BIN);
      Serial.print(", TIMSKn: 0x");
      Serial.print(reg.TIMSKn, HEX);
      Serial.print(", TCNTn: ");
      Serial.print(reg.TCNTn);
      Serial.print("\n   ICRn: ");
      Serial.print(reg.ICRn);
      Serial.print(", OCRnA: ");
      Serial.print(reg.OCRnA);
      Serial.print(", OCRnB: ");
      Serial.print(reg.OCRnB);
#ifdef _AHW_TIMERS_HAS16B3OC
      Serial.print(", OCRnC: ");
      Serial.print(reg.OCRnC);
#endif
}

int Print(const IRCtrl::SymCount *data, int max) {
	int pos = 0;
	while (data[pos] != 0) {
		if (pos >= max) {
			Serial.print ("Buffer overrun.");
			break;
		}
		Serial.print(data[pos]);
		Serial.print(",");
		++pos;
	}

	return pos;
}

void Print(const IRCtrl::PktInfo &data) {
	static const int PKTSYMB_MAX = IRCtrl::PktRaw::PKTSYMB_MAX;
	int count = 0;
	int bit;

	Serial.print("Protocol: ");
	Serial.print(GetProtName((IRCtrl::IRProtocol::ID) data.protocol));
	Serial.print(", Carrier: ");
	Serial.print(data.carrier);
	Serial.print("kHz");

	Serial.print("\n   Preamble: ");
	count = Print(data.preamble, IRCtrl::PktInfo::PREAMB_MAX);

	for (bit = 0; bit < IRCtrl::PktInfo::BITS_NUMELEM; ++bit) {
		Serial.print("\n   Bit[");
		Serial.print(bit);
		Serial.print("]: ");
		count = Print(data.bits[bit], IRCtrl::PktInfo::BIT_MAX);
	}
	Serial.print("\n   Postamble: ");
	count = Print(data.postamble, IRCtrl::PktInfo::PREAMB_MAX);
	Serial.print("\n");
}

void Print(const IRCtrl::PktRaw &data) {
	int count = 0;
	Serial.print("\nPktRaw: Carrier=");
	Serial.print(data.carrier);
	Serial.print("kHz\n");
	count = Print(data.symbols, IRCtrl::PktRaw::PKTSYMB_MAX);
	Serial.print("\nLength=");
	Serial.print(count);
}

void Print(const IRCtrl::RxBuffer &data) {
	int count = 0;
	Serial.print("\nRxBuffer: ");
	count = Print(data.symbols, IRCtrl::RxBuffer::PKTSYMB_MAX);
	Serial.print("\nLength=");
	Serial.print(count);
}

void Print(const IRCtrl::IRMsg &msg) {
	Serial.print("\nProtocol: ");
	Serial.print(GetProtName((IRCtrl::IRProtocol::ID) msg.protocol));
	Serial.print(", data: 0x"); Serial.print(msg.data, HEX);
	Serial.print(", #bits: "); Serial.print(msg.nbits);
}

}; //namespace IRSerial

#endif //#ifdef ENABLE_IR_SERIAL_TOOLS
//Last Line
