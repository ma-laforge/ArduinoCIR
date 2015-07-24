/* ir_repeater.ino

Simple sketch that re-transmits valid incomming IR messages.

NOTE:
-Supports ATmega1280/2560 only.  Repeater sketch is flaky when using system
 clock to modulate transmitter (IRTX_CREATE_TMR8BSCLK).
*/

//Tell linker which libraries to include:
#include <arduino_tools_linkLib.h>
#include <arduino_timers_linkLib.h>
#include <ir_messaging_linkLib.h>

//Normal includes:
#include <irhw.h>
#include <ir_transmitter.h>
#include <ir_receiver.h>
#include <ir_serialtools.h> //Just to configure serial... not really needed

//-----Instantiate HW-specific components-----
const int PIN_IRRX = 4;               //IR receiver pin
const bool INVERT_IRRX_SIGNAL = true; //Depends on polarity of rx module

#define TIMER_IRTXMODULATOR 1         //!*
#define TIMER_IRTXCARRIER 3           //!*Dictates IR transmit pin
#define TIMER_IRRX 4                  //!*
IRTX_CREATE_TMR16B(irTx, TIMER_IRTXMODULATOR, TIMER_IRTXCARRIER)
IRRX_CREATE_TMR16B(irRx, TIMER_IRRX, PIN_IRRX, INVERT_IRRX_SIGNAL)
//!*Must be #define-d to work with pre-processor macro



void setup() {
   irTx.Configure();
   irRx.Configure();
   IRSerial::Configure();

   Serial.print("\n\n**********************");
   Serial.print("\nRX input pin: "); Serial.print(PIN_IRRX);
   Serial.print(", TX output pin: "); Serial.print(irTx.GetOutputPin());
}

void loop() {
   static IRCtrl::IRMsg msg;

   if (irRx.PopMsg(&msg)) {
      irTx.Send(msg);
   }
}

//Last line
