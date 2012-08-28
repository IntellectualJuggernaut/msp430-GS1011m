#ifndef BOARD_SVS_H
#define BOARD_SVS_H

#include <msp430.h>

class SVS
{
public:
	enum
	{
		VDiff = VLD0,
		V1_25 = VLD3|VLD2|VLD1|VLD0,
		V3_7 = V1_25-VDiff,
		V3_5 = V3_7-VDiff,
		V3_35 = V3_5-VDiff,
		V3_2 = V3_35-VDiff,
		V3_05 = V3_2-VDiff,
		V2_9 = V3_05-VDiff,
		V2_8 = V2_9-VDiff,
		V2_65 = V2_8-VDiff,
		V2_5 = V2_65-VDiff,
		V2_4 = V2_5-VDiff,
		V2_3 = V2_4-VDiff,
		V2_2 = V2_3-VDiff,
		V2_1 = VLD1,
		V1_9 = VLD0,
	};
	enum
	{
		VOFF = 0,
		MaxV = V3_7,
		MinV = VLD0,
		StartV = V3_2,
	};
	
	static void init(const unsigned char start = V3_7)
	{
		unsigned char ret = start;
		SVSCTL = ret;
		while (ret != 0)
		{
			ret -= VDiff;
			if (0 == (SVSCTL&SVSFG))
				return;
			
			SVSCTL = ret;
			// might not need this too much		
			//__delay_cycles(64);
		}
	}
	static void enablePORON(void) { SVSCTL |= PORON; }
	static void diablePORON(void) { SVSCTL &= ~PORON; }
	static void end(void) { SVSCTL = 0; }
	static bool sigPwrChng(void) { return  0 != (SVSCTL & SVSFG);}
	static void clearInterrupt(void) { SVSCTL &= ~SVSFG; }
	static unsigned char currentVoltage(void) { return (SVSCTL & 0xF0); }
	static void isr(void)
	{
		SVSCTL -= VDiff;
		clearInterrupt();
		if (currentVoltage() == MinV)
		{
			// need to die or something... 
			// this is crazy low... 
			// nothing should be working by now
		}
	}
};

#endif