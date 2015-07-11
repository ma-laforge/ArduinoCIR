/*ir_debug.cpp
Debugging module.
*/

#include <ir_debug.h>

#ifdef SERIAL_DEBUG
#include <ir_serialtools.h>

namespace IRDebug {

//Allocate memory for debug values:
volatile uint32_t valAPu32 = 0;
volatile uint32_t valu32 = 0;
IRCtrl::IRMsg valIRMsg;

void Configure() {
	IRSerial::Configure();
}

//Is debug dump requested by user? (any serial message is considered a request):
bool DumpRequested() {return Serial.available() > 0;};

void ProcessEvents() {
//-----Auto print-----
	if (valAPu32 != 0) {
		Serial.print("\nvalAPu32="); Serial.print(valAPu32);
		Serial.flush();
		valAPu32=0;
	}

//-----Print when user writes something-----
	if (DumpRequested()) {
		Serial.read(); //Pop data from serial buffer
   	Serial.print("\n\n---------------------------------");
		Serial.print("\nnow_msec()="); Serial.print(now_msec());
		Serial.print(", valu32=");	Serial.print(valu32, HEX);

		//Dump timer registers:
      Serial.print("\nTimer1:"); IRSerial::Print(ArduinoHw::timer1);
      Serial.print("\nTimer3:"); IRSerial::Print(ArduinoHw::timer3);
      Serial.print("\nTimer4:"); IRSerial::Print(ArduinoHw::timer4);
   }
}
}; //namespace IRDebug
#endif //#ifdef SERIAL_DEBUG
//Last Line
