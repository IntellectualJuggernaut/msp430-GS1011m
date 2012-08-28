#ifndef BOARD_DMA_H
#define BOARD_DMA_H

#include <msp430.h>

#ifndef __data16_write_addr
#define __data16_write_addr(reg, addr) *(unsigned short*)reg = addr
#endif

#define DMA_TRIGGERSOURCE_0          (0*0x0001u)  /* DMA channel transfer select 0:  DMA_REQ (sw)*/
#define DMA_TRIGGERSOURCE_1          (1*0x0001u)  /* DMA channel transfer select 1:  Timer_A (TACCR2.IFG) */
#define DMA_TRIGGERSOURCE_2          (2*0x0001u)  /* DMA channel transfer select 2:  Timer_B (TBCCR2.IFG) */
#define DMA_TRIGGERSOURCE_3          (3*0x0001u)  /* DMA channel transfer select 3:  USCIA0 receive */
#define DMA_TRIGGERSOURCE_4          (4*0x0001u)  /* DMA channel transfer select 4:  USCIA0 transmit */
#define DMA_TRIGGERSOURCE_5          (5*0x0001u)  /* DMA channel transfer select 5:  DAC12_0CTL.DAC12IFG */
#define DMA_TRIGGERSOURCE_6          (6*0x0001u)  /* DMA channel transfer select 6:  ADC12 (ADC12IFG) */
#define DMA_TRIGGERSOURCE_7          (7*0x0001u)  /* DMA channel transfer select 7:  Timer_A (TACCR0.IFG) */
#define DMA_TRIGGERSOURCE_8          (8*0x0001u)  /* DMA channel transfer select 8:  Timer_B (TBCCR0.IFG) */
#define DMA_TRIGGERSOURCE_9          (9*0x0001u)  /* DMA channel transfer select 9:  USCIA1 receive */
#define DMA_TRIGGERSOURCE_10         (10*0x0001u) /* DMA channel transfer select 10: USCIA1 transmit */
#define DMA_TRIGGERSOURCE_11         (11*0x0001u) /* DMA channel transfer select 11: Multiplier ready */
#define DMA_TRIGGERSOURCE_12         (12*0x0001u) /* DMA channel transfer select 12: USCIB0 receive */
#define DMA_TRIGGERSOURCE_13         (13*0x0001u) /* DMA channel transfer select 13: USCIB0 transmit */
#define DMA_TRIGGERSOURCE_14         (14*0x0001u) /* DMA channel transfer select 14: previous DMA channel DMA2IFG */
#define DMA_TRIGGERSOURCE_15         (15*0x0001u) /* DMA channel transfer select 15: ext. Trigger (DMAE0) */

#define DMA_TRANSFER_SINGLE              (DMADT_0)
#define DMA_TRANSFER_BLOCK               (DMADT_1)
#define DMA_TRANSFER_BURSTBLOCK          (DMADT_2)
#define DMA_TRANSFER_REPEATED_SINGLE     (DMADT_4)
#define DMA_TRANSFER_REPEATED_BLOCK      (DMADT_5)
#define DMA_TRANSFER_REPEATED_BURSTBLOCK (DMADT_6)

#define DMA_DIRECTION_UNCHANGED (DMASRCINCR_0)
#define DMA_DIRECTION_DECREMENT (DMASRCINCR_2)
#define DMA_DIRECTION_INCREMENT (DMASRCINCR_3)

#define DMA_SIZE_SRCWORD_DSTWORD ( !(DMASRCBYTE | DMADSTBYTE) )
#define DMA_SIZE_SRCBYTE_DSTWORD (DMASRCBYTE)
#define DMA_SIZE_SRCWORD_DSTBYTE (DMADSTBYTE)
#define DMA_SIZE_SRCBYTE_DSTBYTE (DMASRCBYTE | DMADSTBYTE)

#define DMA_TRIGGER_RISINGEDGE ( !(DMALEVEL) )
#define DMA_TRIGGER_HIGH       (DMALEVEL)



#ifndef HWREG
#define HWREG(x)                                                              \
        (*((volatile unsigned int *)(x)))
#endif
#ifndef HWREGB
#define HWREGB(x)                                                             \
        (*((volatile unsigned char *)(x)))
#endif

namespace dmaimpl
{
	enum offsets
	{
		CTL0_OFS = 0,
		CTL1_OFS = 2,
		IV_OFS = 4,
		CHCTL_OFS = 0x01D0 - 0x0120u,//0xBC,// THIS IS NOT CORRECT//0x01D0 - (ptrdiff_t)__BASE_REG__,
		SA_OFS = CHCTL_OFS + 2,
		DA_OFS = SA_OFS + 4,
		SZ_OFS = DA_OFS + 4,
		CH_OFS = SZ_OFS + 2,
		CH2CH_OFS = 0x0C,
	};
};

template <unsigned short CHANNEL_NUM>
class DMA 
{
  public:

	enum
	{
		BASE_ADDRESS = 0x0120u,
		CHANNEL = CHANNEL_NUM,
		
		CHANNEL_OFS = CHANNEL * dmaimpl::CH2CH_OFS, // inter channel offset, i.e. offset between ch 1 and ch 2 registers
		
		CHCTL_OFS = dmaimpl::CHCTL_OFS + CHANNEL_OFS,// THIS IS NOT CORRECT//0x01D0 - (ptrdiff_t)__BASE_REG__,
		SA_OFS = dmaimpl::SA_OFS + CHANNEL_OFS,
		DA_OFS = dmaimpl::DA_OFS + CHANNEL_OFS,
		
		SZ_OFS = dmaimpl::SZ_OFS + CHANNEL_OFS,
		
		CHCTL = BASE_ADDRESS + CHCTL_OFS,	// ex DMA1CTL = base + offset of DMA0CTL + CHNum(=1) * channel2channel offset
		CHSA = BASE_ADDRESS + SA_OFS,
		CHDA = BASE_ADDRESS + DA_OFS,
		CHSZ = BASE_ADDRESS + SZ_OFS,
		CHTS_SHIFT = CHANNEL*4
	};
	
	static void setup(const unsigned transferModeSelect,
					 const unsigned char triggerSourceSelect,
					 const unsigned char transferUnitSelect,
					 const unsigned char triggerTypeSelect)
	{
		//Reset and Set DMA Control 0 Register
		HWREGB(CHCTL) =
			transferModeSelect //Set Transfer Mode
				| transferUnitSelect //Set Transfer Unit Size
					| triggerTypeSelect; //Set Trigger Type
		// still need trigger source
		DMACTL0 &= ~(0xF << CHTS_SHIFT);
		DMACTL0 |= (triggerSourceSelect << CHTS_SHIFT);
	}
	
	static void setTransferSize(const unsigned& transferSize) { HWREG(CHSZ) = transferSize; }
	static void setSourceAddr(const unsigned long& srcAddress,
							  const unsigned int& directionSelect)
	{
		__data16_write_addr((unsigned short)(CHSA), srcAddress);
		// reset bits before selecting
		HWREGB(CHCTL) &= ~(DMASRCINCR_3);
    	HWREGB(CHCTL) |= directionSelect;
	}
	static void setDstAddress (const unsigned long& dstAddress,
							   const unsigned int& directionSelect)
	{
		__data16_write_addr((unsigned short)(CHDA), dstAddress);
		// reset bits before selecting
		HWREGB(CHCTL) &= ~(DMADSTINCR_3);
    	HWREGB(CHCTL) |= (directionSelect << 2);
	}
	static void enable() { HWREGB(CHCTL) |= DMAEN; }
	static void disable() { HWREGB(CHCTL) &= ~DMAEN; }
	static void start() { HWREGB(CHCTL) |= DMAEN | DMAREQ; }
	static void clearInterrupt() { HWREGB(CHCTL) &= ~DMAIFG; }
	static void enableInterrupt() { clearInterrupt(); HWREGB(CHCTL) |= DMAIE; }
	static void disableInterrupt() { HWREGB(CHCTL) &= ~DMAIE; }
	static bool getInterruptStatus() { return 0 != ( HWREGB(CHCTL) & DMAIFG ); }
	static bool ISR() { return true; }
};


#endif
