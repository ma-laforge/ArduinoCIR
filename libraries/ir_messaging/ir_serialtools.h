/*ir_serialtools.h
Declares tools to help send info through the serial monitor.
*/

#ifndef IR_SERIALTOOLS_H
#define IR_SERIALTOOLS_H

#include <irhw.h>
#include <ir_message.h>
#include <ir_packet.h>

namespace IRSerial {
void Configure(); //Configure module
void Print(const ArduinoHw::Timer16b::Timer &reg);
void Print(const IRCtrl::PktInfo &data);
void Print(const IRCtrl::PktRaw &pkt);
void Print(const IRCtrl::RxBuffer &data);
void Print(const IRCtrl::IRMsg &msg);
}; //namespace IRSerial

#endif //#ifndef IR_SERIALTOOLS_H
//Last Line
