/* ir_simple.ino

Simple sketch transmits a hard-coded IR message when a button is pressed.
*/

//Tell linker which libraries to include:
#include <arduino_tools_linkLib.h>
#include <arduino_timers_linkLib.h>
#include <ir_messaging_linkLib.h>

//Normal includes:
#include <irhw.h>
#include <ir_transmitter.h>
#include <ir_serialtools.h> //Just to configure serial... not really needed

//-----Message to be sent on button-down-----
//const IRCtrl::IRMsg msg(IRCtrl::IRProtocol::NEC, 0xE13E31CE, 32); //NEC: Volume down
const IRCtrl::IRMsg msg(IRCtrl::IRProtocol::NEC, 0x5EA1D827, 32); //Yamaha: Volume down

//-----Instantiate HW-specific components-----
#define _HW_SELECTCONFIG 1 //1..2
const int PIN_BUTTON = 2;            //Pushbutton pin

#if (_HW_SELECTCONFIG == 1)
	#define TIMER_IRTXCARRIER 2          //!*Dictates IR transmit pin
	IRTX_CREATE_TMR8BSCLK(irTx, TIMER_IRTXCARRIER)
#elif (_HW_SELECTCONFIG == 2)
	// ***ATmega1280/2560 only***
	#define TIMER_IRTXMODULATOR 1        //!*
	#define TIMER_IRTXCARRIER 3          //!*Dictates IR transmit pin
	IRTX_CREATE_TMR16B(irTx, TIMER_IRTXMODULATOR, TIMER_IRTXCARRIER)
#endif
//!*Must be #define-d to work with pre-processor macro



//-----State machine constants for handling button events-----
//How long to invalidate the button after pressing the button (debounce)
const msec_t BUTTONINVALID_PERIOD = 100;
const int BUTTONSTATE_VALID = 0;
const int BUTTONSTATE_INVALID = 1;
msec_t buttonInvalidStart = 0;  //Time @ which button was invalidated (debounced).
int buttonState = 0;            //Internal state of pushbutton abstraction.
int buttonSignal = 0;           //Electrical signal read on button pin.

void setup() {
   //Assign IOs:
   pinMode(PIN_BUTTON, INPUT);
   irTx.Configure();
   IRSerial::Configure();

   //Initialize state machines:
   buttonState = BUTTONSTATE_VALID;
   buttonInvalidStart = now_msec();

   Serial.print("\n\n**********************");
   Serial.print("\nPush button pin: "); Serial.print(PIN_BUTTON);
   Serial.print(", TX output pin: "); Serial.print(irTx.GetOutputPin());
}

void loop() { 
   //State machine to handle/debounce button events:
   switch (buttonState) {
   case BUTTONSTATE_VALID:
      // read the state of the pushbutton value:
      buttonSignal = digitalRead(PIN_BUTTON);

      if (buttonSignal == HIGH) {
         buttonInvalidStart = now_msec();
         buttonState = BUTTONSTATE_INVALID;
         irTx.Send(msg);
      }
   break;
   case BUTTONSTATE_INVALID:
      if (now_msec()-buttonInvalidStart > BUTTONINVALID_PERIOD) {
         buttonState = BUTTONSTATE_VALID;
      }
   break;
   }
}

//Last line
