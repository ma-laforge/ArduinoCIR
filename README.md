[IRremote]: <http://www.righto.com/2009/08/multi-protocol-infrared-remote-library.html> "IRremote (Ken Shirriff)"

# ArduinoCIR: Communication using consumer IR "remote" signals

(Send/receive "TV remote" (+satbox +...) commands)

<a name="Description"></a>
## Description

ArduinoCIR provides the `ir_messaging` library, a ready-to-use solution for sending/receiving consumer IR (remote-control) signals on Arduino platforms.

<a name="Features"></a>
## Features

 - **Simultaneously** transmit & receive IR messages.
 - **(Untested)**: Build solutions supporting 2+ receivers or 2+ transmitters (Speed & hardware permitting).
 - User-selectable hardware units for timers (not hard-coded in library).

<a name="Protocols"></a>
### Supported Protocols

The list of supported protocols is listed below.

- **NEC**: (tx/rx).
- **RC-MM**: (tx only).
- **RC-5**: (untested tx/untested rx).
- **Sony SIRC**: (untested tx/rx).

Note:
 - Certain protocols might only be implemented for *either* transmit (tx) or receive (rx).
 - New protocols can be added to [libraries/ir\_messaging/ir\_protocol.cpp](libraries/ir_messaging/ir_protocol.cpp)/[.h](libraries/ir_messaging/ir_protocol.h).

## Table of Contents

 1. [Description](#Description)
 1. [Features](#Features)
    1. [Supported Protocols](#Protocols)
 1. [Purpose of Library](doc/purpose.md)
 1. [Supported Hardware](#Hardware)
 1. [Sample Sketches](libraries/ir_messaging/samples)
 1. [Usage Tips](doc/tips.md)
 1. [Wiring/Circuit Topology](doc/topology.md)
 1. [Software Documentation](doc/softwaredoc.md)
 1. [Resources/Acknowledgments](doc/resources_ack.md)
 1. [Known Limitations](#Limitations)

<a name="Hardware"></a>
### Supported Hardware

ArduinoCIR was designed to support easily multiple Atmel platforms.  At the moment, only a few are actually implemented:

 - Atmel ATmega328/328P
   - Arduino Uno
   - Arduino Redboard (&#x2705; Tested)
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
   - Arduino Mega 2560 (&#x2705; Tested)
   - Arduino Mega ADK (&#x2705; Tested)

<a name="Limitations"></a>
## Known Limitations

 - IR receiver currently only works using a 16-bit timer (`IRCtrl::Timer16b::RxHw`).
 - IR transmitter currently supports two resource allocations:
   - System clock (Timer0) + an 8-bit timer (`IRCtrl::Timer8b::TxHw`),
   - or *two* 16-bit timers (`IRCtrl::Timer16b::TxHw`).

### Compatibility

The `ir_messaging` library was tested on version 1.0.5+dfsg2-2 (Linux) of the Arduino IDE.

<a name="Disclaimer"></a>
## Disclaimer

This software is provided "as is", with no guarantee of correctness.
