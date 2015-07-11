/* ir_rx_sniff.ino

Simple sketch that dumps recieved signal to a serial output.
*/

//Tell linker which libraries to include:
#include <arduino_tools_linkLib.h>
#include <arduino_timers_linkLib.h>
#include <ir_messaging_linkLib.h>

//Normal includes:
#include <irhw.h>
#include <ir_receiver.h>
#include <ir_serialtools.h>

//-----Instantiate HW-specific components-----
#define TIMER_IRRX 4                  //!*
const int PIN_IRRX = 3;               //IR receiver pin
const bool INVERT_IRRX_SIGNAL = true; //Depends on polarity of rx module
//!*Must be #define-d to work with pre-processor macro

IRRX_CREATE_TMR16B(irRx, TIMER_IRRX, PIN_IRRX, INVERT_IRRX_SIGNAL)


void setup() {
   irRx.Configure();
   IRSerial::Configure();
   Serial.print("\n\n**********************");
   Serial.print("\nRX input pin: "); Serial.print(PIN_IRRX);
}

void loop() {
   if (irRx.MsgAvailable()) {
      IRCtrl::IRMsg msg;
      bool success = irRx.PopMsg(&msg);

      IRSerial::Print(irRx.rxBuf);

      if (success) {
         IRSerial::Print(msg); Serial.print("\n");
      } else {
         Serial.print("\nMessage not recognized.\n");
      }
   }
}

//Last line
