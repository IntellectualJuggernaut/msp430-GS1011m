#ifndef BOARD_SPI_H
#define BOARD_SPI_H

#include <msp430.h>

#ifndef HWREG
#define HWREG(x)                                                              \
        (*((volatile unsigned int *)(x)))
#endif
#ifndef HWREGB
#define HWREGB(x)                                                             \
        (*((volatile unsigned char *)(x)))
#endif
#include <stdio.h>
template <unsigned short CHANNEL_NUM, const unsigned short __BASE_ADDR = 0x0068u>
class SPI
{
  public:
	
	enum
	{
		BASE_ADDRESS = __BASE_ADDR,
		CHANNEL = CHANNEL_NUM,
		CH_OFS = 0x00D8u - BASE_ADDRESS,
		UCBxCTL0 = BASE_ADDRESS + CHANNEL*CH_OFS,
		UCBxCTL1 = UCBxCTL0 + 1,
		UCBxBR0 = UCBxCTL1 + 1,
		UCBxBR1 = UCBxBR0 + 1,
		UCBxI2CIE = UCBxBR1 + 1,
		UCBxSTAT = UCBxI2CIE + 1,
		
		UCBxRXBUF = UCBxSTAT + 1,
		UCBxTXBUF = UCBxRXBUF + 1,
		
		UCBxIE = 0x0001u + (0x0005u * CHANNEL),
		UCBxIFG = 0x0003u + (0x0004u * CHANNEL),
	};
	enum
	{
		UCBxRXIFG = (unsigned char)(0x04),
		UCBxTXIFG = (unsigned char)(0x08),
		UCBxRXIE = UCBxRXIFG,
		UCBxTXIE = UCBxTXIFG,
		UCBxTXPIN = (unsigned char)(BIT4 << (2*CHANNEL)),
		UCBxRXPIN = (unsigned char)(BIT5 << (2*CHANNEL)),
	};
	
	inline static void write(const uint8_t& _data)
	{
		while( !getTXInterruptStatus() );
  		HWREGB(UCBxTXBUF) = _data;
	}
	inline static uint8_t read()
	{
		while( !getRXInterruptStatus() );
		return HWREGB(UCBxRXBUF);
	}
	inline static uint8_t transfer(const uint8_t& _data = 0xF5);
	inline static void begin();
	inline static void lock() { HWREGB(UCBxCTL1) |= UCSWRST; } 
	inline static void unlock() { HWREGB(UCBxCTL1) &= ~UCSWRST; }
	inline static void end();
	inline static unsigned long receiveBufferForDMA() { return HWREGB(UCBxRXBUF); }
	inline static unsigned long transmitBufferForDMA() { return HWREGB(UCBxTXBUF); }

	static void clearRXInterrupt() { HWREGB(UCBxIFG) &= ~UCBxRXIFG; }
	static void setRXFlag() { HWREGB(UCBxIFG) |= UCBxRXIFG; }
	static void enableRXInterrupt() { clearRXInterrupt(); HWREGB(UCBxIE) |= UCBxRXIE; }
	static void disableRXInterrupt() { HWREGB(UCBxIE) &= ~UCBxRXIE; }
	static bool getRXInterruptStatus() { return 0 != ( HWREGB(UCBxIFG) & UCBxRXIFG ); }
	static void clearTXInterrupt() { HWREGB(UCBxIFG) &= ~UCBxTXIFG; }
	static void setTXFlag() { HWREGB(UCBxIFG) |= UCBxTXIFG; }
	static void enableTXInterrupt() { clearTXInterrupt(); HWREGB(UCBxIE) |= UCBxTXIE; }
	static void disableTXInterrupt() { HWREGB(UCBxIE) &= ~UCBxTXIE; }
	static bool getTXInterruptStatus() { return 0 != ( HWREGB(UCBxIFG) & UCBxTXIFG ); }

};

template <unsigned short CHANNEL_NUM, const unsigned short __BASE_ADDR>
uint8_t SPI<CHANNEL_NUM,__BASE_ADDR>::transfer(const uint8_t& _data)
{
//  while( !getTXInterruptStatus() );
//  HWREGB(UCBxTXBUF) = _data;
//  while( !getRXInterruptStatus() );
//  return HWREGB(UCBxRXBUF);
	write(_data);
	return read();
}

template <unsigned short CHANNEL_NUM, const unsigned short __BASE_ADDR>
void SPI<CHANNEL_NUM,__BASE_ADDR>::begin()
{
    lock();
    
    HWREGB(UCBxCTL0) = 
          UCCKPL | 
          //UCCKPH |
          UCMSB |
          UCMST |
          UCSYNC;

    HWREGB(UCBxCTL1) |= UCSSEL_3; 
    HWREGB(UCBxBR0) = 0x08;//0x02 if SMCLK = 1MHz // 0x08 if SMCLK = 8MHz
    HWREGB(UCBxBR1) = 0x00;
    unlock();
    //while(!getTXInterruptStatus())
      ;
}

template <unsigned short CHANNEL_NUM, const unsigned short __BASE_ADDR>
void SPI<CHANNEL_NUM,__BASE_ADDR>::end()
{
  lock();
  HWREGB(UCBxCTL0) = 0;
  unlock();
}

#endif