[IRremote]: <http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html> "IRremote (Ken Shirriff)"

# ArduinoCIR: Consumer IR Control

## Description

ArduinoCIR provides the ir\_messaging library, a ready-to-use solution for sending/receiving consumer IR (remote-control) signals on Arduino platforms.

<a name="SupportedHW"></a>
### Supported Hardware

ArduinoCIR was designed to support multiple Atmel platforms.  At the moment, only a few are supported:

- Atmel ATmega328/328P
	- Arduino Uno
	- Arduino Redboard (Tested)
	- Arduino Pro
	- Arduino Fio
	- Arduino Mini
	- Arduino Nano
	- Arduino LilyPad
	- Arduino BT
	- Arduino Ethernet
- Atmel ATmega1280
	- Arduino Mega
- Atmel ATmega2560
	- Arduino Mega 2560 (Tested)
	- Arduino Mega ADK (Tested)

### Supported Protocols

The list of supported protocols is listed below.  Note that certain protocols might only be implemented for either transmit (tx) or receive (rx).

- **NEC**: (tx/rx).
- **RC-MM**: (tx only).
- **RC-5**: (untested tx/untested rx).
- **Sony SIRC**: (untested tx/rx).

New protocols can be added to [libraries/ir\_messaging/ir\_protocol.cpp](libraries/ir\_messaging/ir\_protocol.cpp)/[.h](libraries/ir\_messaging/ir\_protocol.h).

### Purpose (Is there not IRremote already?)

Indeed, the IRremote library already exists to interface Arduino platforms with consumer IR products.  The main differating factor of the ir\_messaging library is that it does not hardcode the allocation of hadware timers.

Here are a few advantages of ir\_messaging:

- *Designed* to simultaneously transmit & receive IR messages.
	- Includes sample IR repeater project (**ir\_repeater**).
	- Last version of IRremote I checked used the same timer hardware for both transmit & receive (contention).  Moreover, the trival time-interleaved (tx &hArr; rx) solution is not very robust.
- Supports transmission (only) of **RC-MM** codes used by certain cable box models made by Motorola.
	- Might need to repeat each transmitted message twice in rapid succession, as per the protocol.
- Easier to select which timers & I/O pins to use for transmit/receive function.
	- No need to read/modify core library files.
	- Hardware select can be easily deduced from sample sketches (+ minimal knowldege of hardware timers).
- Uses `IRMsg` class to simplify API (protocol, data & message length stay together).
- More modular approach to supporting different chipsets.
- Seems to be a *slightly* more robust solution (but cannot remember exact reason why that is).

Untested advantages:

- Could easily support asynchronous (non-blocking) transmit (not yet made available).
	- Uses ISR to modulate transmit carrier.
- Build solutions supporting 2+ receivers or 2+ transmitters (Speed & hardware permitting).

### Any reason to use IRremote instead?

Definitely.  There are a few reasons to use IRremote instead of the ir\_messaging library.  Some that come to mind are listed below:

- At the moment, IRremote supports more IR protocols.
	- Only an advantage if your project *must* communicate with a given (un-supported) protocol.
	- NOTE: Possible to add support for other protocols in ir\_messaging.
- At the moment, IRremote supports more Arduino platforms.
- Might not be as comfortable making customizations to the ir\_messaging library.
	- Implementation makes heavy use of C++ constructs (ex: objects).
	- Code is divided into more files (This might not fit the way you work).
	- Object/module hierarchy is more elaborate than IRremote.
- Currently generates smaller binaries.
- Larger community for support.

## Sample Sketches

ir\_messaging provides 3 sample sketches under the [libraries/ir\_messaging/samples](libraries/ir_messaging/samples) sub-directory:

- **ir\_tx\_onbutton**: Simple sketch that transmits a hard-coded IR message when a button is pressed.
- **ir\_rx\_sniff**: Simple sketch that dumps recieved signal to serial output.
- **ir\_repeater**: Simple sketch that re-transmits valid incomming IR messages.
	- NOTE: Does not respect protocol-dependent periodicity (or signalling gap) between IR messages.  Take the NEC volume up/down commands for example:  The ir\_repeater sketch will recieve the first volume message, then immediately re-transmit it (~68ms) once fully decoded.  By the time the volume message re-transmission is complete, the receiver ISR will have already read in the trailing NEC "repeat" message (only ~11ms) - as per the 110ms periodicidy of messages in the NEC protocol.  Thus, without enforcing protocol-dependent periodicity, the first "repeat" message will be re-transmitted too early for proper detection by the end-user equipment.

## Usage Tips

### Atmel Timer0: Time & Delays

On what appears to be all Atmel chipsets, the Arduino software uses Timer0 to measure time (`millis()`, `micros()`).  It should probably use Timer0 to implement delays (`delay()`, `delayMicroseconds()`) in an interrupt-insensitive way as well... but it just counts cycles at the moment.  In any case: better not appropriate this timer for anything else.

NOTE: The arduino\_tools library implements `wait_msec()` & `wait_usec()` (replacing `delay()` & `delayMicroseconds()`) with a busy-wait on elapsed time.

### Atmel: Timer Priorities

Interrupt priority on on Atmel timers appear to match their number (highest priority for Timer0, then Timer1, ...).  To avoid potential signal jitter issues, select the lowest available timer (ex: `timer1`) to modulate the transmitter output (`TxHw::tmrMod`).

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

The ir\_messaging library is relatively flexible.  The IR receiver software can be configured to listen on any available pin.  However, the **transmitter output pin** is controlled by whatever timer generates the **output carrier**:

- ATmega328/328P
	- Timer2: Pin 3.
- ATmega1280/2560
	- Timer1: Pin 11.
	- Timer2: Pin 9.
	- Timer3: Pin 5.
	- Timer4: Pin 6.

Please refer to the appropriate [datasheet](#Datasheets) for more details regarding the capabilities of your particular Arduino platform.

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
- IR transmitter currently supports two resource allocations:
	- System clock (Timer0) + an 8-bit timer (`IRCtrl::Timer8b::TxHw`),
	- or *two* 16-bit timers (`IRCtrl::Timer16b::TxHw`).
- ...

## Resources/Acknowledgments

<a name="Shirriff_et_al"></a>
### Ken Shirriff (et al.)

Ken Shirriff's IRremote library provides an interesting solution to consumer IR (remote-control) communication.  Ken's library is very inspiring:

- **IRremote on Github**: <https://github.com/shirriff/Arduino-IRremote>
- **Description**: <http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html>

A few more interesting blogs from Ken's site:

- **Sony codes & LIRC**: <http://www.righto.com/2010/03/understanding-sony-ir-remote-codes-lirc.html>
- **Arduino PWM**: <http://www.righto.com/2009/07/secrets-of-arduino-pwm.html>

### SB-Projects

The SB-Projects website contains useful information on selected consumer IR (remote-control) protocols:

- **IR Remote Control Theory**: <http://www.sbprojects.net/knowledge/ir/index.php>

<a name="Datasheets"></a>
### Datasheets

The following lists a few datasheets might be of use.

- **ATmega328/328P**: <http://www.atmel.com/devices/atmega328p.aspx>
- **ATmega1280/2560**: <http://www.atmel.com/devices/atmega2560.aspx>

## Disclaimer

This software is provided "as is", with no guarantee of correctness.  Use at own risk.
