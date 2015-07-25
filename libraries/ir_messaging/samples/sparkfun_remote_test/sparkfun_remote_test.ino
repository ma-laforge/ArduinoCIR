/* sparkfun_remote_test.ino

Prints out decoded button when a message is received from the SparkFun IR remote.
*/

//Tell linker which libraries to include:
#include <arduino_tools_linkLib.h>
#include <arduino_timers_linkLib.h>
#include <ir_messaging_linkLib.h>

//Normal includes:
#include <irhw.h>
#include <ir_receiver.h>



//Device ID code:
const uint32_t CODE_DEVID = 0x10EF0000; //An NEC device remote

//Individual button codes:
const uint32_t CODE_POWER  = CODE_DEVID | 0xD827;
const uint32_t CODE_A      = CODE_DEVID | 0xF807;
const uint32_t CODE_B      = CODE_DEVID | 0x7887;
const uint32_t CODE_C      = CODE_DEVID | 0x58A7;
const uint32_t CODE_UP     = CODE_DEVID | 0xA05F;
const uint32_t CODE_DOWN   = CODE_DEVID | 0x00FF;
const uint32_t CODE_LEFT   = CODE_DEVID | 0x10EF;
const uint32_t CODE_RIGHT  = CODE_DEVID | 0x807F;
const uint32_t CODE_SELECT = CODE_DEVID | 0x20DF;
//NOTE: "|" concatenates upper 4 "device" hex codes with lower 4 "button" hex codes



//-----Instantiate HW-specific components-----
#define _HW_SELECTCONFIG 1 //1..2
const int PIN_IRRX = 4;               //IR receiver pin
const bool INVERT_IRRX_SIGNAL = true; //Depends on polarity of rx module

#if (_HW_SELECTCONFIG == 1)
   #define TIMER_IRRX 1                 //!*
   IRRX_CREATE_TMR16B(irRx, TIMER_IRRX, PIN_IRRX, INVERT_IRRX_SIGNAL)
#elif (_HW_SELECTCONFIG == 2)
   // ***ATmega1280/2560 only***
   #define TIMER_IRRX 4                 //!*
   IRRX_CREATE_TMR16B(irRx, TIMER_IRRX, PIN_IRRX, INVERT_IRRX_SIGNAL)
#endif
//!*Must be #define-d to work with pre-processor macro



void setup() {
   irRx.Configure();
   Serial.begin(9600);
   Serial.print("\n\n**********************");
   Serial.print("\nRX input pin: "); Serial.print(PIN_IRRX);
   Serial.print("\n");
}

void loop() {
   IRCtrl::IRMsg msg;

   if (irRx.PopMsg(&msg)) {
      if (IRCtrl::IRProtocol::NEC_RPT == msg.protocol) {
         //Repeat signal detected.
         //NOTE: NEC repeat signal has its own protocol... just easier that way.
         Serial.print(" REPEAT");
      } else {
         //Identify decoded signal (if supported):
         switch (msg.data) {
         case CODE_POWER:
            Serial.print("\nPOWER"); break;
         case CODE_A:
            Serial.print("\nA"); break;
         case CODE_B:
            Serial.print("\nB"); break;
         case CODE_C:
            Serial.print("\nC"); break;
         case CODE_UP:
            Serial.print("\nUP"); break;
         case CODE_DOWN:
            Serial.print("\nDOWN"); break;
         case CODE_LEFT:
            Serial.print("\nLEFT"); break;
         case CODE_RIGHT:
            Serial.print("\nRIGHT"); break;
         case CODE_SELECT:
            Serial.print("\nSELECT"); break;
         }
      }
   }
}

//Last line
