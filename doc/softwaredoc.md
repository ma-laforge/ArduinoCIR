# ArduinoCIR: Software Documentation

## Libraries (Dependencies)

NOTE: All libraries listed below are included as part of the `ir_messaging` project.

 - **`ir_messaging`**: Provides consumer IR (remote-control) communication for Arduino platforms.
 - **`arduino_tools`**: Defines types/functions/macros that extend the Arduino base tools.
 - **`arduino_timers`**: Defines tools/abstractions to simplify control of Arduino timers.  Needs to be extended to support different chipsets.

## Class Hierarchy

The following lists a few key classes:

 - **`IRCtrl::IRMsg` class**: High-level description of a consumer IR (remote-control) message.
 - **`IRCtrl::TxHw` virtual class**: Implementations allocate timers, I/O pins, & control low-level IR transmit.
 - **`IRCtrl::RxHw` virtual class**: Implementations allocate timers, I/O pins, & control low-level IR receive.
 - **`IRCtrl::PktInfo` class**: Describes low-level signalling of consumer IR packets.
 - **`IRCtrl::IRProtocol::pktInfo[]`**: Details how each consumer IR protocol transmits/receives IR messages.

And some low-level classes that might be useful for supporting more hardware:

 - **`ArduinoHW::Timer16b::Timer` class**: Facilitates the control of 16-bit timers (hopefully works for most arduino platforms).

