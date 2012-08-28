#ifndef BOARD_FLASH_H
#define BOARD_FLASH_H

#include <msp430.h>

class Flash
{
public:
	enum Segment
	{
		// don't write to SegA, contains calibration constants
		SegB = 0x1080,	// SSID to connect
		SegC = 0x1040, 	// WebProv Uname & Password
		SegD = 0x1000,	// Special Char at [0] and Mac after
		SegNULL = 0x0000
		
	};
	
	static char* ptrToSegment(const Segment seg) { return (char *)seg; }
	
	static void fillSegment(const Segment seg, const char filler)
	{
		char *Flash_ptr = (char *)seg;               // Initialize Flash pointer

		FCTL3 = FWKEY;                            // Clear Lock bit	
		FCTL1 = FWKEY + ERASE + EEI;              // Set Erase bit, allow interrupts	
		*Flash_ptr = 0;                           // Dummy write to erase Flash seg
		FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

		for (unsigned i = 64; i != 0; --i)
		{
			*Flash_ptr++ = filler;                   // Write value to flash
		}

		FCTL1 = FWKEY;                            // Clear WRT bit
		FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
	}
	
	static void writeToSegment(const Segment seg, const void* data, const unsigned len)
	{
		unsigned i;
		char *Flash_ptr = (char *)seg;               // Initialize Flash pointer
		char *data_ptr = (char *)data;
		FCTL3 = FWKEY;                            // Clear Lock bit	
		FCTL1 = FWKEY + ERASE + EEI;              // Set Erase bit, allow interrupts	
		*Flash_ptr = 0;                           // Dummy write to erase Flash seg
		FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

		for (i = 0; i < len; ++i)
		{
			*Flash_ptr++ = *data_ptr++;                   // Write value to flash
		}

		FCTL1 = FWKEY;                            // Clear WRT bit
		FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
	}
	
	static void copySegment(const Segment from, const Segment to)
	{
		char* Flash_ptrF = (char *)from;
		char* Flash_ptrT = (char *)to;
		
		FCTL3 = FWKEY;                            // Clear LOCK bit
		FCTL1 = FWKEY + ERASE + EEI;              // Set Erase bit, allow interrupts
		*Flash_ptrT = 0;                          // Dummy write to erase Flash seg to
		FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

		for (unsigned i = 64; i != 0; --i)
		{	
			*Flash_ptrT++ = *Flash_ptrF++;          // copy value segment D to seg B
		}

		FCTL1 = FWKEY;                            // Clear WRT bit	
		FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
	}
};

#endif
