[ProtektivePak]: <http://protektivepak.descoindustries.com/ProtektivePakCatalog/PPKimpregnated/CircuitBoardShippers/37054/#.VZ9BOfm9gu8>

# Usage Tips

## Atmel Timer0: Time & Delays

On what appears to be all Atmel chipsets, the Arduino software uses Timer0 to measure time (`millis()`, `micros()`).  It should probably use Timer0 to implement delays (`delay()`, `delayMicroseconds()`) in an interrupt-insensitive way as well... but it just counts cycles at the moment.  In any case: better not appropriate this timer for anything else.

NOTE: The `arduino_tools` library implements `wait_msec()` & `wait_usec()` (replacing `delay()` & `delayMicroseconds()`) with a busy-wait on elapsed time.

## Atmel: Timer Priorities

Interrupt priority on on Atmel timers appear to match their number (highest priority for Timer0, then Timer1, ...).  To avoid potential signal jitter issues, select the lowest available timer (ex: `timer1`) to modulate the transmitter output (`TxHw::tmrMod`).

## Register Writes

To guarantee proper initialization of the Arduino platforms, it appears hardware register writes must be executed *after* construction of static objects.  Thus, register writes performed during the construction phase of static global objects simply do not persist.  I believe the intent was for users to perform these register writes within Arduino's `setup()` function.

## Interference on `ir_repeater`

The biggest issue with the `ir_repeater` sample project is its inherent succeptibility to interference between the originating IR remote and the repeated signal.  Most IR controller diodes are designed to drive significant power over a broad range of angles.  Without proper isolation, the repeated signal might leak back into the arduino's IR receiver... and if that does not cause an interference problem, the originating signal from the consumer IR remote will then likely collide with the repeated signal - confusing the IR receiver of the end-user equipment.

To solve this problem, the originating signal from the consumer IR remote should be physically isolated from the repeated signal.

I found that boxes to store ESD-sensitive devices, like the [37054 Protektive Pak][ProtektivePak] are ideal for the task.  Simply place the receiver module (possibly with the arduino itself) in the box, and solder the transmit diode to the end of a ~1m+, 18AWG speaker wire.  The speaker wire will make it easy to effectively direct the repeated signal to the end-user equipment.  If the consumer IR remote is pointed away from this direction and shrouded sufficiently by the ESD box (where the arduino reciever module is listening), interference should no longer be a problem.

