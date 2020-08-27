## ArduinoCIR: Wiring/Circuit Topology

Very little external circuitry is required for IR communication.  Ken Shirriff provides a good reference circuit for use with his own IRremote library.  The only major caveat to using his topology is with regards to the default choice of I/O pins (see section [Hardware/Pinout Selection](#HWPinout)).  Given all this good work, please visit Ken's IRremote page for the wiring diagram.

 - **IRremote (Ken Shirriff)**: <http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html>

Similarly, a wiring diagram for the *push button* signal required by the `ir_tx_onbutton` sketch can be found on Ken's record/playback project page (again, see section [Hardware/Pinout Selection](#HWPinout) for default pinout):

 - **Record/playback project (Ken Shirriff)**: <http://www.righto.com/2009/09/arduino-universal-remote-record-and.html>

<a name="HWPinout"></a>
### Hardware/Pinout Selection

The `ir_messaging` library is relatively flexible.  The IR **receiver** software can be configured to listen on any available pin.  However, the **transmitter output pin** is controlled by whatever timer generates the **output carrier**:

 - ATmega328/328P
   - Timer2 &rArr; Pin 3.
 - ATmega1280/2560
   - Timer1 &rArr; Pin 11.
   - Timer2 &rArr; Pin 9.
   - Timer3 &rArr; Pin 5.
   - Timer4 &rArr; Pin 6.

Please refer to the appropriate [datasheet](resources_ack.md#Datasheets) for more details regarding the capabilities of your particular Arduino platform.

