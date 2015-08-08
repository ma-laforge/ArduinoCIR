/*arduino_ext.h
Arduino extensions

Defines a few useful types to have for many applications.
*/

#ifndef ARDUINO_EXT_H
#define ARDUINO_EXT_H

//TODO: Do we really want to include Arduino.h instead of stdint.h?
//      Depends on how to define NULL...  I think you can use stddef.h for that.
//Define uintX_t:
#include <Arduino.h>
//#include <stdint.h>

//const int NULL = 0; //Null macro defined through Arduino.h already.

//Time-related constants (default accuracy):
typedef uint32_t time_t; //As in <ctime> / time.h (seconds).
typedef uint32_t msec_t; //millisecond data type
typedef uint32_t usec_t; //microsecond data type
typedef uint32_t hz_t;   //Frequency, in Hz

//Space-sensitive time-related constants:
//(Not compatible with base functions - used for readability of optimized code)
typedef uint16_t usec_u16t; //Stores a little over 65 ms
typedef uint32_t usec_u32t; //Stores a little over 4000 sec
typedef uint64_t usec_u64t;
typedef uint16_t msec_u16t; //Stores a little over 65 sec
typedef uint32_t msec_u32t; //Stores a little over 4M sec
typedef uint64_t msec_u64t;

typedef int16_t usec_s16t; //Stores a little over +/-32 ms
typedef int32_t usec_s32t; //Stores a little over +/-2000 sec
typedef int64_t usec_s64t;
typedef int16_t msec_s16t; //Stores a little over +/-32 sec
typedef int32_t msec_s32t; //Stores a little over +/-2M sec
typedef int64_t msec_s64t;

//-----Wrapper for time functions (with nice associated data type)-----
inline msec_t now_msec() {return millis();}
inline usec_t now_usec() {return micros();}

//Don't call native functions.... They count cycles (cannot deal with interrupts).
//inline void wait_msec(msec_t wTime) {delay(wTime);}
//inline void wait_usec(usec_t wTime) {delayMicroseconds(wTime);}

inline void wait_msec(msec_t wTime) {
	msec_t tstart = now_msec();
	while (now_msec()-tstart < wTime) ;
}

inline void wait_usec(usec_t wTime) {
	usec_t tstart = now_usec();
	while (now_usec()-tstart < wTime) ;
}

//Integer division with rounding (macro to ensure pre-evaluated)
//TODO: would inline do just as well?
#define INTDIV_RND(x, y) (((((x)<<1)/(y))+1)>>1)

#endif //#ifndef ARDUINO_EXT_H
//Last Line
