[IRremote]: http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html "IRremote (Ken Shirriff)"

# ArduinoCIR: Consumer IR Control

## Description

ArduinoCIR provides the ir\_messaging library, a ready-to-use solution for sending/receiving consumer IR (remote-control) signals on the Arduino platform.

<a name="SupportedHW"></a>
### Supported Hardware

Only a single chipset is supported at the moment: the ATmega2560.  Nonetheless, ir\_messaging was designed to be extensible to other Arduino platforms.  Of course, no effort was yet placed on the mechanics of *how* to conditionally build for different platforms.

- Arduino Mega 2560 / Arduino Mega ADK.

### Supported Protocols

The list of supported protocols is listed below.  Note that certain protocols might only be implemented for either transmit (tx) or receive (rx).

- **NEC**: (tx/untested rx).
- **RC-MM**: (tx only).
- **RC-5**: (untested tx/untested rx).
- **Sony SIRC**: (untested tx/rx).

New protocols can be added to [libraries/ir\_messaging/ir\_protocol.cpp](libraries/ir\_messaging/ir\_protocol.cpp)/[.h](libraries/ir\_messaging/ir\_protocol.h).

### Purpose (Is there not IRremote already?)

Indeed, the IRremote library already exists to interface Arduino platforms with consumer IR products.  The main differating factor of the ir\_messaging library is that it does not hardcode which timer/IO pin is used.

Here are a few advantages of ir\_messaging:

- *Designed* to simultaneously transmit & receive IR messages.
	- Last version of IRremote I checked used the same timer hardware for both transmit & receive (contention).  Moreover, the trival time-interleaved (tx &hArr; rx) solution is not very robust.
- Easier to select which timers & I/O pins to use for transmit/receive function.
	- No need to read/modify core library files.
	- Can be easily deduced from sample sketches (+ minimal info about hardware timers).
- Uses `IRMsg` class to simplify API (protocol, data & message length stay together).
- More modular approach to supporting different chipsets.
- Seems to be a more robust solution (but cannot remember exact reason why that is).

Untested advantages:

- Could easily support asynchronous (non-blocking) transmit (not yet made available).
	- Uses ISR to modulate transmit carrier.
- Build solutions supporting 2+ receivers or 2+ transmitters (Speed & hardware permitting).

### Any reason to use IRremote instead?

Definitely.  There are a few reasons to use IRremote instead of the ir\_messaging library.  Some that come to mind are listed below:

- At the moment, IRremote supports more IR protocols.
	- Potentially a problem if the project *must* communicate with an un-supported protocol.
	- NOTE: Possible to add support for other protocols in ir\_messaging.
- At the moment, IRremote supports more Arduino platforms.
	- NOTE: Can use ATmega2560/ADK implementation as a model for adding platforms to ir\_messaging.
- IRremote uses `delayMicroseconds()` (Timer0) to modulate the transmit carrier (mark/space).
	- Does not need a dedicated timer to implement modulation (less hardware).
- Might not be as comfortable making customizations to ir\_messaging library.
	- Implementation makes heavy use of C++ constructs (ex: objects).
	- Code is divided into more files (This might not fit the way you work).
	- Object/module hierarchy is more elaborate than IRremote.

## Sample Sketches

ir\_messaging provides 3 sample sketches under the [libraries/ir\_messaging/samples](libraries/ir_messaging/samples) sub-directory:

- **ir\_repeater**: Simple sketch that re-transmits valid incomming IR messages.
- **ir\_rx\_sniff**: Simple sketch that dumps recieved signal to serial output.
- **ir\_tx\_onbutton**: Simple sketch that transmits a hard-coded IR message when a button is pressed.

### Sample Sketch Pinouts

The sample sketches listed above come pre-configured for the ATmega2560/ADK using the following I/O configuration:

- **IR receiver module**: Pin 3.
- **IR transmitter diode**: Pin 5.
- **Push button**: Pin 2.

If, for whatever reason, this pin list cannot be located: simply load one of the sample sketches and read the serial monitor upon device reset.  The sample sketches all dump the relevant software-configured pins.

## Usage Tips

### Atmel `timer0`: Time & Delays

On what appears to be all Atmel chipsets, the Arduino software uses `timer0` to measure time & implement delays (`delay()`, `delayMicroseconds()`, `millis()`, `micros()`).  Better not appropriate this timer for anything else.

### Atmel: Timer Priorities

Interrupt priority on on Atmel timers appear to match their number (highest priority for `timer0`, then `timer1`, ...).  To avoid potential signal jitter issues, select the lowest available timer (ex: `timer1`) to modulate the transmitter output (`TxHw::tmrMod`).

### Register Writes

To guarantee proper initialization of the Arduino platforms, it appears hardware register writes must be executed *after* construction of static objects.  Thus, register writes performed during the construction phase of static global objects simply do not persist.  I believe the intent was for users to perform these register writes within Arduino's "setup()" function.

### Interference on "ir\_repeater"

The biggest issue with the ir\_repeater sample project is its inherent succeptibility to interference between the originating IR remote and the repeated signal.  Most IR controller diodes are designed to drive significant power over a broad range of angles.  Without proper isolation, the repeated signal might leak back into the arduino's IR receiver... and if that does not cause an interference problem, the originating signal from the consumer IR remote will then likely collide with the repeated signal - confusing the IR receiver of the end-user equipment.

To solve this problem, the originating signal from the consumer IR remote should be physically isolated from the repeated signal.

I found that boxes to store ESD-sensitive devices, like the [37054 Protektive Pak](http://protektivepak.descoindustries.com/ProtektivePakCatalog/PPKimpregnated/CircuitBoardShippers/37054/#.VZ9BOfm9gu8) are ideal for the task.  Simply place the receiver module (possibly with the arduino itself) in the box, and solder the transmit diode to the end of a ~1m+, 18AWG speaker wire.  The speaker wire will make it easy to effectively direct the repeated signal to the end-user equipment.  If the consumer IR remote is pointed away from this direction and shrouded sufficiently by the ESD box (where the arduino reciever module is listening), interference should no longer be a problem.

## Circuit Topology

Very little external circuitry is required for IR communication.  Ken Shirriff provides a good reference circuit for use with his own IRremote library.  The only major caveat to using his topology is with regards to the default choice of I/O pins (see section [Hardware/Pinout Selection](#HWPinout)).  Given all this good work, please visit Ken's IRremote page for the wiring diagram.

- **IRremote (Ken Shirriff)**: <http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html>

Similarly, a wiring diagram for the *push button* signal required by the ir\_tx\_onbutton sketch can be found on Ken's record/playback project page (again, see section [Hardware/Pinout Selection](#HWPinout) for default pinout):

- **Record/playback project (Ken Shirriff)**: <http://www.righto.com/2009/09/arduino-universal-remote-record-and.html>

<a name="HWPinout"></a>
### Hardware/Pinout Selection

The ir\_messaging library is very flexible.  It allows the user to control which timers/pin drive the IR transmitter, and which are used by the IR receiver.

Please refer to the appropriate [datasheet](#ArduinoDatasheets) for more details regarding the capabilities of your particular Arduino platform.

## Libraries (Dependencies)

NOTE: All libraries listed below are included as part of the ir\_messaging project.

- **arduino\_tools**: Defines types/functions/macros that extend the Arduino base tools.
- **arduino\_timers**: Defines tools/abstractions to simplify control of Arduino timers.  Needs to be extended to support different chipsets.
- **ir\_messaging**: Provides consumer IR (remote-control) communication for Arduino platforms.

## Class Hierarchy

The following lists a few key classes:

- **`IRCtrl::IRMsg` class**: High-level description of a consumer IR (remote-control) message.
- **`IRCtrl::TxHw` virtual class**: Implementations allocate timers, I/O pins, & control low-level IR transmit.
- **`IRCtrl::RxHw` virtual class**: Implementations allocate timers, I/O pins, & control low-level IR receive.
- **`IRCtrl::PktInfo` class**: Describes low-level signalling of consumer IR packets.
- **`IRCtrl::IRProtocol::pktInfo[]`**: Details how each consumer IR protocol transmits/receives IR messages.

And some low-level classes that might be useful for supporting more hardware:
- **`ArduinoHW::Timer16b::Timer` class**: Facilitates the control of 16-bit timers (hopefully works for most arduino platforms).

## Compiling

The ir\_messaging library was tested on the version 1.0.5+dfsg2-2 (Linux) of the Arduino IDE.

## Known Limitations

- IR receiver currently only works using a 16-bit timer (`IRCtrl::Timer16b::RxHw`).
- IR transmitter currently only works using *two* 16-bit timers (`IRCtrl::Timer16b::TxHw`).
- ...

## Resources/Acknowledgments

### Ken Shirriff

Ken Shirriff's IRremote library provides an interesting solution to consumer IR (remote-control) communication.  Ken's library is very inspiring:

- **IRremote on Github**: <https://github.com/shirriff/Arduino-IRremote>
- **Description**: <http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html>

A few more interesting blogs from Ken's site:

- **Sony codes & LIRC**: <http://www.righto.com/2010/03/understanding-sony-ir-remote-codes-lirc.html>
- **Arduino PWM**: <http://www.righto.com/2009/07/secrets-of-arduino-pwm.html>

### SB-Projects

The SB-Projects website contains useful information on selected consumer IR (remote-control) protocols:

- **IR Remote Control Theory**: <http://www.sbprojects.net/knowledge/ir/index.php>

<a name="ArduinoDatasheets"></a>
### Arduino Datasheets

The following list provides hyperlinks to some useful Arduino datasheets.  Please note the chipset corresponding to your particular Arduino platform.

- **ATmega2560**: <http://www.atmel.com/devices/atmega2560.aspx?tab=documents>

## Disclaimer

This software is provided "as is", with no guarantee of correctness.  Use at own risk.
