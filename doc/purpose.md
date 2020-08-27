# ArduinoCIR: Purpose of Library (Is there not `IRremote` already?)

Indeed, the `IRremote` library already exists to interface Arduino platforms with consumer IR products.  The main differating factor of the `ir_messaging` library is that it does not hardcode the allocation of hadware timers.

Here are a few advantages of `ir_messaging`:

 - *Designed* to simultaneously transmit & receive IR messages.
   - Includes sample IR repeater project (**`ir_repeater`**).
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

# Any reason to use `IRremote` instead?

Definitely.  There are a few reasons to use IRremote instead of the `ir_messaging` library.  Some that come to mind are listed below:

 - At the moment, IRremote supports more IR protocols.
   - Only an advantage if your project *must* communicate with a given (un-supported) protocol.
   - NOTE: Possible to add support for other protocols in `ir_messaging`.
 - At the moment, IRremote supports more Arduino platforms.
 - Might not be as comfortable making customizations to the `ir_messaging` library.
   - Implementation makes heavy use of C++ constructs (ex: objects).
   - Code is divided into more files (This might not fit the way you work).
   - Object/module hierarchy is more elaborate than IRremote.
 - Currently generates smaller binaries.
 - Larger community for support.

