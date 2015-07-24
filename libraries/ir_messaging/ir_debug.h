/*ir_debug.h
Debugging module

NOTE: Not a big fan of how debug is implemented.
TODO: Give debug feature a little more thought.
*/

#ifndef IR_DEBUG_H
#define IR_DEBUG_H

//Definition of "ENABLE_IR_DEBUG_TOOLS" (ideally defined @ compilation - not here):
//#define ENABLE_IR_DEBUG_TOOLS //Compiles ir debug code
#ifdef ENABLE_IR_DEBUG_TOOLS

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

#endif //#ifdef ENABLE_IR_DEBUG_TOOLS
#endif //#ifndef IR_DEBUG_H
//Last Line
