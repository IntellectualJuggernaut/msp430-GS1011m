#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#include <msp430.h>
#include "Shared.h"

#ifndef NULL
#define NULL 0
#endif

#define NOP() __no_operation()

#define _csHigh (P2OUT |= CSPIN) //GPIO_setOutputHighOnPin(CSBaseAddress,2,CSPIN)
#define n_csHigh (P2OUT &= ~CSPIN) //GPIO_setOutputLowOnPin(CSBaseAddress,2,CSPIN)
#define select() n_csHigh
#define deselect() _csHigh
#define hostWakeUPSignal() (0 != (P2IN & HSPINMASK)) //GPIO_getInputPinValue(CSBaseAddress,2,HSPIN)
#define cs() select()
#define ncs() deselect()
#define hs() hostWakeUPSignal()
#define resetHSFlag() (P2IFG &= ~HSPIN)

#ifndef CSPIN
#define CSPIN (BIT1)
#endif
#ifndef HSPIN
#define HSPIN (BIT2)
#endif
#ifndef HDPIN
#define HDPIN (BIT3)
#endif
#ifndef HSPINMASK
#define HSPINMASK (HSPIN|HDPIN)
#endif

#define CAM_CHANNEL 0
#define WIFI_TX_CHANNEL 1
#define WIFI_RX_CHANNEL 2

#ifndef MOSI
#define MOSI (BIT1)
#define MISO (BIT2)
#define MCLK (BIT3)
#define CSPINs (BIT1)
#define HSPINs (BIT2)
#define HSPIN2s (BIT3)
#endif

#define ASPINMASK (BIT3|BIT1|BIT2)

#define LEDGreenOn() (P5OUT |= BIT3)
#define LEDGreenOff() (P5OUT &= ~BIT3)
#define LEDGreenToggle() (P5OUT ^= BIT3)

#define LEDBlueOn() (P5OUT |= BIT2)
#define LEDBlueOff() (P5OUT &= ~BIT2)
#define LEDBlueToggle() (P5OUT ^= BIT2)

#define LEDErrOn() LEDGreenOn()
#define LEDErrOff() LEDGreenOff()
#define LEDErrToggle() LEDGreenToggle()


#define interrupts() __enable_interrupt()
#define noInterrupts() __disable_interrupt()


#define SMCLK_XT1 0
#define SMCLK_XT2 0

#if SMCLK_XT1 && SMCLK_XT2
#error fuck you
#endif

#if SMCLK_XT2
	// here we are using VLO for ACLK, so wdt is every 5.5 seconds
	#define WDTCTLBITS (WDTPW|WDTTMSEL|WDTSSEL|WDTIS0|WDTNMI)
#else
	// still using original board... just disable
	#define WDTCTLBITS (WDTPW|WDTHOLD|WDTNMI)
#endif



#define wdt_enable() \
	WDTCTL = WDTCTLBITS

#define wdt_disable() \
	WDTCTL = (WDTCTLBITS | WDTHOLD)

#define wdt_reset() \
	WDTCTL |= (WDTPW|WDTCNTCL)


#define LPM() __low_power_mode_3()
#define LPM_EXIT() __low_power_mode_off_on_exit()

#ifndef HWREG
#define HWREG(x)                                                              \
        (*((volatile unsigned int *)(x)))
#endif
#ifndef HWREGB
#define HWREGB(x)                                                             \
        (*((volatile unsigned char *)(x)))
#endif

#if DEBUG != 0
#define INLINE inline
#else
#define INLINE _Pragma("inline=forced")
#endif

#define PACK _Pragma("pack(1)")
#define UNPACK _Pragma("pack()")

#define ROM const
#define vROM __ro_placement volatile ROM


#define far __data20
#define FAR far
#define near __data16
#define NEAR near

#define APPREG HWREGB( _TAR )

enum ServerSignal 
{
	HTTPSERVER_S = BIT5,
	MJPEGSERVER_S = BIT6,
};


extern void setup();
extern uint8_t loop();

#endif