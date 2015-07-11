/*ir_protocol.h
Defines classes and constants that describe IR signalling protocols.

NOTE:
	-*Implementation of what is provided in ir_packet.h*
	-Should be limited to actually describing protocols themselves
	 (not generic structures & constants).
	-Used directly by IRMsg in the ir_message module.

http://www.sbprojects.com/knowledge/ir/
Carriers (kHz)    Period (us)
36                27.777
38                26.316

Modulation ID     Description
PDE               Pulse Distance Encoding (vary width of 0)
PWE               Pulse Width Encoding (vary width of 1)
PPE               Pulse-Position Encoding (2 bits/pulse)
TODO: Verify that naming is ok; not sure about proper nomenclature

Protocol      Carrier   Modulation   Time base        Time base...
               Freq                                 Carrier-Relative
               (kHz)                   (ms)         (Tcarrier/Tunit)

JVC:            38      PDE          1.05/2
Mitsubishi:     38      PDE          ~ 1/2
NEC:            38      PDE          2.25/4
NRC17:          38      Manchester   1/2                  20
   (Nokia)
Sharp:          38      PDE          1/3.125
SIRC:           40      PWE          1.2/2
   (Sony)
Philips
   RC5:         36      Manchester   8/9                  32
   RC6:         36      Manchester   4/9                  16
   RC-MM:       36      PPE
    0 0    166.7 µs (6 cycles)    277.8 µs (10 cycles)
    0 1    166.7 µs (6 cycles)    444.4 µs (16 cycles)
    1 0    166.7 µs (6 cycles)    611.1 µs (22 cycles)
    1 1    166.7 µs (6 cycles)    777.8 µs (28 cycles)
   RECS80:    Not common?
   (+ST)
RCA:          56        PDE          1/2
*/

#ifndef IR_PROTOCOL_H
#define IR_PROTOCOL_H

namespace IRCtrl {
	class PktInfo; //Forwarded declaration

//List of supported protocols... Recognized by IRMsg object:
//NOTE: Order must match global array "pktInfo[]"
namespace IRProtocol {
	enum ID {
		UNKNOWN     = -1,
		NONE        = 0,
		NEC         = 1,
		NEC_RPT,          //Repeat message has a shorter preamble than regular
                        //NEC messages.  Registering a separate protocol for
                        //the repeat signal as a simple solution.
		SONY,
		RCMM,
		RC5,
/*
		RC6,
		DISH,
		SHARP,
		PANASONIC,
		JVC,
		SANYO,
		MITSUBISHI,
*/
		ID_LASTPLUS1,
		PROTOCOL_COUNT = ID_LASTPLUS1-1
	};

	//Corresponding list of pktInfo (Recognized by IRMsg object):
	extern const PktInfo pktInfo[];
	extern const int pktInfoCount;
}; //namespace IRProtocol
}; //namespace IRCtrl

#endif //#ifndef IR_PROTOCOL_H
//Last Line
