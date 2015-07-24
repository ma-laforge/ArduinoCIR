/*ir_serialtools.h
Declares tools to help send info through the serial monitor.
*/

#ifndef IR_SERIALTOOLS_H
#define IR_SERIALTOOLS_H

//Definition of "ENABLE_IR_SERIAL_TOOLS" (ideally defined @ compilation - not here):
#define ENABLE_IR_SERIAL_TOOLS //Compiles code from ir_serialtools
#ifdef ENABLE_IR_SERIAL_TOOLS

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

#endif //#ifdef ENABLE_IR_SERIAL_TOOLS
#endif //#ifndef IR_SERIALTOOLS_H
//Last Line
