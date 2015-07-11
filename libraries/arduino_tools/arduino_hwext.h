/*arduino_hwext.h
Arduino hardware-access extensions
*/

#ifndef ARDUINO_HWEXT_H
#define ARDUINO_HWEXT_H

//TODO: use uintX_t instead of unsigned X?
typedef volatile unsigned char v_uint8_t;
typedef volatile unsigned int v_uint16_t;

//-----Define some useful hardware write macros-----
//_AHW: Arduino hardware macro prefix

//_AHW_WRITE[w]_[b]B(memref, lsb, value)
//   w: register width (bits)
//   b: # of CONTIGUOUS bits the register
//   memref: reference to register location
//   lsb: least-significant bit of contiguous bit field.
//   value: binary value to be translated/written to the register.

#define _AHW_WRITEw_bB(wtype, bmsk, memref, lsb, value) ({ \
	wtype tmp = (memref); \
	tmp &= ~((bmsk)<<(lsb)); \
	(memref) = tmp | (((value)&(bmsk))<<(lsb)); \
})
//WARNING! wtype cannot be wrapped in () for safety!
//(Compiler does not like that) -- be careful when using!

#define _AHW_WRITE8_1B(memref, lsb, value) _AHW_WRITEw_bB(uint8_t, 0x1, (memref), (lsb), (value))
#define _AHW_WRITE8_2B(memref, lsb, value) _AHW_WRITEw_bB(uint8_t, 0x3, (memref), (lsb), (value))
#define _AHW_WRITE8_3B(memref, lsb, value) _AHW_WRITEw_bB(uint8_t, 0x7, (memref), (lsb), (value))

#define _AHW_WRITE16_1B(memref, lsb, value) _AHW_WRITEw_bB(uint16_t, 0x1, (memref), (lsb), (value))
#define _AHW_WRITE16_2B(memref, lsb, value) _AHW_WRITEw_bB(uint16_t, 0x3, (memref), (lsb), (value))
#define _AHW_WRITE16_3B(memref, lsb, value) _AHW_WRITEw_bB(uint16_t, 0x7, (memref), (lsb), (value))

//_AHW_MAP_[b]AB(b[b-1], ..., b0)
//   Create a mapping array for a b-bit field in an array.
//   SEE: _AHW_WRITE[w]_[b]AB for writing
//   ex: _AHW_MAP_2AB(map, 0x2, 0x8) => const uint8_t map[] = {0x0, 0x8, 0x2, 0xA};
//       MSB is LOWER than MSB (& not consecutive) in this example
//   b: # of ARBITRARILY ARRANGED bits in the register

#define _AHW_MAP_2AB(label, b1, b0) (const uint8_t (label)[] = {0, (b0), (b1), (b1)|(b2)};)

//_AHW_WRITE[w]_[b]AB(memref, map, value)
//   w: register width (bits)
//   b: # of ARBITRARILY ARRANGED bits in the register
//   memref: reference to register location
//   map: array of bit data.
//   value: binary value to be translated/written to the register.

#define _AHW_WRITE8_2AB(memref, map, value) ({ \
	uint8_t tmp = (memref); \
	tmp &= ~(map)[0x3]; \
	(memref) = tmp | (map)[(value)&0x3]; \
})

#endif //#ifndef ARDUINO_HWEXT_H
//Last Line
