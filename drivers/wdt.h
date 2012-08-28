#ifndef BOARD_WDT_H
#define BOARD_WDT_H

#include <msp430.h>

class WDT
{
public:
	enum
	{
		WDTBITMASK = WDTPW|WDTNMI|WDTHOLD|WDTCNTCL,
	};
	
	static void enable(unsigned char mask) { WDTCTL = mask; }
	static void enable(void) {WDTCTL = WDTBITMASK;}
	static void disable(void) { WDTCTL = WDTBITMASK|WDTHOLD; }
	static void reset(void) { WDTCTL |= WDTCNTCL; }
	static void pet(void) { reset(); }
};

#endif