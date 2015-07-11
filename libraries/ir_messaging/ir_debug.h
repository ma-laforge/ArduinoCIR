/*ir_debug.h
Debugging module

NOTE: Not a big fan of how debug is implemented.
TODO: Give debug feature a little more thought.
*/

//Definition of "SERIAL_DEBUG" (should be defined @ compilation - not here):
#define SERIAL_DEBUG //Enables debug code
#ifdef SERIAL_DEBUG

#ifndef IR_DEBUG_H
#define IR_DEBUG_H

#include <ir_message.h>

namespace IRDebug {

//Automatically printed when non-zero through ProcessEvents():
extern volatile uint32_t valAPu32;

//Normal debug values (must manually print):
extern volatile uint32_t valu32;
extern IRCtrl::IRMsg valIRMsg;

void Configure(); //Configure module
bool DumpRequested(); //=true when user writes something to serial
void ProcessEvents();
}; //namespace IRDebug

#endif //#ifndef IR_DEBUG_H
#endif //#ifdef SERIAL_DEBUG
//Last Line
