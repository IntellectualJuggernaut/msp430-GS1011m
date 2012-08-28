//
//  Shared.h
//  
//
//  Created by Clayton Thomas on 7/26/12.
//
//

#ifndef _Shared_h
#define _Shared_h

#include <inttypes.h>
//#include "util/binary.h"

#define F_KILO 1000L
#define F_MEGA 1000000L
	
	
#ifndef F_CPU
#define F_CPU (4*F_MEGA)
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))
	
#define sei() __enable_interrupt()
#define cli() __disable_interrupt()
	
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )
	
#define delayMicros(_usd) __delay_cycles( microsecondsToClockCycles((_usd)) )
#define delay(_msd) delayMicros( F_KILO * (_msd) )
	
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
	
#define bitRead(value, bit) ((value) & ((1UL << (bit))))
#define bitCheck(value, bit) (bitRead(value, bit) != 0)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
	
#define maskRead(value, mask) ((value)&(mask))	
#define maskCheck(value, mask) (maskRead((value), (mask)) == (mask))
#define maskSet(value, mask) ((value) |= (mask))
#define maskClear(value, bit) ((value) &= ~(mask))
	
	
#ifndef _BV
#define _BV(bit) (1UL << (bit))
#endif
	
#define _SFRB(x)                                                             \
        (*((volatile unsigned char *)(&x)))

#define sbi(reg, bit) (_SFRB((reg)) |= _BV((bit)))
#define cbi(reg, bit) (_SFRB((reg)) &= ~_BV((bit)))
	
typedef unsigned int word;
	
#define bit(b) (1UL << (b))
	
typedef uint8_t boolean;
typedef uint8_t byte;
typedef uint8_t Byte;
#define BytePtr(_vptr) ((uint8_t*)(_vptr))

#if DEBUG != 0
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min); static_assert(in_min <= x && x <= in_max, "x is not an element of it's own range")
#else
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif


#ifndef DLog
#if DEBUG != 0 && DLOGGING != 0
#include <stdio.h>
// simply print string
#define DLogv(fmt, ...) printf( fmt, ##__VA_ARGS__)
#else // debug
//#define DLog(fmt, ...) 
//#define DLogf(fmt, ...)
#define DLogv(fmt, ...)
#endif // debug

// print function name with trailing new line
#define DLog(fmt, ...) DLogv(fmt "\n", ##__VA_ARGS__) 
// print function name without the new line
#define DLogf(fmt, ...) DLogv("%s: " fmt, __PRETTY_FUNCTION__, ##__VA_ARGS__) //printf("%s: " fmt, __PRETTY_FUNCTION__, ##__VA_ARGS__)

#endif// dlog 

#endif
