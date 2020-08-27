# Sample Sketches: ir\_messaging library

4 sample sketches are provided under the [libraries/ir\_messaging/samples](libraries/ir_messaging/samples) sub-directory:

 - **ir\_tx\_onbutton**: Transmits a hard-coded IR message when a button is pressed.
 - **ir\_rx\_sniff**: Prints recieved signal to serial output (serial monitor).
 - **sparkfun\_remote\_test**: Prints received signal to serial output (specific to "SparkFun IR Control Kit").
 - **ir\_repeater**: Simple sketch that re-transmits valid incomming IR messages.
	- NOTE: Does not respect protocol-dependent periodicity (or signalling gap)
     between IR messages.  Take the NEC volume up/down commands for example:
     The ir\_repeater sketch will recieve the first volume message, then immediately
     re-transmit it (~68ms) once fully decoded.  By the time the volume message
     re-transmission is complete, the receiver ISR will have already read in the
     trailing NEC "repeat" message (only ~11ms) - as per the 110ms periodicidy
     of messages in the NEC protocol.  Thus, without enforcing protocol-dependent
     periodicity, the first "repeat" message will be re-transmitted too early for
     proper detection by the end-user equipment.

