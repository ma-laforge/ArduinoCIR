/* ir_repeater.ino

Simple sketch that re-transmits valid incomming IR messages.
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
#define _HW_SELECTCONFIG 1 //1..2
const int PIN_IRRX = 4;               //IR receiver pin
const bool INVERT_IRRX_SIGNAL = true; //Depends on polarity of rx module

#if (_HW_SELECTCONFIG == 1)
   #define TIMER_IRRX 1                 //!*
   #define TIMER_IRTXCARRIER 2          //!*Dictates IR transmit pin
   IRRX_CREATE_TMR16B(irRx, TIMER_IRRX, PIN_IRRX, INVERT_IRRX_SIGNAL)
   IRTX_CREATE_TMR8BSCLK(irTx, TIMER_IRTXCARRIER)
#elif (_HW_SELECTCONFIG == 2)
   // ***ATmega1280/2560 only***
   #define TIMER_IRRX 4                 //!*
   #define TIMER_IRTXMODULATOR 1        //!*
   #define TIMER_IRTXCARRIER 3          //!*Dictates IR transmit pin
   IRRX_CREATE_TMR16B(irRx, TIMER_IRRX, PIN_IRRX, INVERT_IRRX_SIGNAL)
   IRTX_CREATE_TMR16B(irTx, TIMER_IRTXMODULATOR, TIMER_IRTXCARRIER)
#endif
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
