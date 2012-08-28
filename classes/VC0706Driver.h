#ifndef VC0706_DRIVER_H
#define VC0706_DRIVER_H
#include <msp430.h>
#include <inttypes.h>
//#include <intrinsics.h>

#define UART_TX (BIT6)
#define UART_RX (BIT7)

#define F_BRCLK (1048576)//(4000000)//
#define BAUD (115200)

#define BAUD_VALUE (int)(F_BRCLK/BAUD)
#define BAUDH ((BAUD_VALUE >> 8) & 0xFF)
#define BAUDL (BAUD_VALUE & 0xFF)

#define UCBRS_VALUE (int)(((float)(1.f*F_BRCLK/BAUD) - (float)(BAUD_VALUE)) * 8)
#define UCBRS (UCBRS_VALUE<<1)

#define RXTRIG (DMA2TSEL_9)
#define TXTRIG (DMA2TSEL_10)
#define CAM_BUF_LEN (1024)

class VC0706Driver
{
public:
	enum { IDLE = 0, SENDING, RECEIVING, };
	
	VC0706Driver() : _sleeping(0),_state(0),_buffer(0), rxLen(0){}
	static void init();
	void open();
	void close();
	
	void reset();
	void sleep(); 	// might replace with power off
	void wake();	// might replace with power on
	
	void stopFBUF();
	void getFBUFLen(unsigned& len);
	uint8_t* readFBUF(const unsigned& startAddress,const unsigned& readLen);
	void startFBUF();
	void swapBuffers() { _buffer = !_buffer; }
	
	void runCommand(const uint8_t* buf, const unsigned& len, const unsigned& retLen);
	
	static void send(const uint8_t* buf, const unsigned& len);
	static void recv(uint8_t* buf, const unsigned& len);
	
	bool ISR();
	
	uint8_t _sleeping;
	volatile uint8_t _state;
	uint8_t _buffer;
	unsigned rxLen;
	uint8_t rxBuf[2][CAM_BUF_LEN+10];
};



void VC0706Driver::init()
{
	UCA1CTL1 |= UCSWRST;
    P3DIR &= ~UART_RX;
    P3DIR |= UART_TX;
    P3SEL |= ( UART_TX | UART_RX );
    UCA1CTL1 |= UCSSEL_2;
    UCA1BR0 = BAUDL;
    UCA1BR1 = BAUDH;
    UCA1MCTL = UCBRS;
    UCA1CTL0 = 0;
    UCA1CTL1 &= ~UCSWRST;
}

void VC0706Driver::open()
{
	_state = IDLE;
	this->wake();
	const uint8_t smallImage[] = {0x56,0,0x31,0x05,0x04, 0x01, 0, 0x19, 0x22};
	const uint8_t fullCompression[] = {0x56,0,0x31,0x05,0x01, 0x01, 0x12, 0x04, 0xFF};
	this->runCommand(smallImage,sizeof(smallImage),5);
	this->runCommand(fullCompression,sizeof(fullCompression),5);
//	this->reset();
//	for (volatile unsigned i = 10; i--;)
//		__delay_cycles(1000);
}
void VC0706Driver::close()
{
	this->sleep();
	DMA2CTL = 0;
	UCA1CTL1 |= UCSWRST;
}

void VC0706Driver::reset()
{
	const uint8_t cmd[] = {0x56, 0, 0x26, 0};
	this->runCommand(cmd,sizeof(cmd),5);
}
void VC0706Driver::sleep()
{
	const uint8_t cmd[] = {0x56,0,0x3E, 0x03, 0x00,0x01,0x01};
	this->runCommand(cmd,sizeof(cmd),5);
	_sleeping = 1;
}
void VC0706Driver::wake()
{
	const uint8_t cmd[] = {0x56,0,0x3E, 0x03,0x00,0x01,0x00};
	this->runCommand(cmd,sizeof(cmd),5);
	_sleeping = 0;
}

void VC0706Driver::stopFBUF()
{
	const uint8_t cmd[] = {0x56,0,0x36,0x01,0x00};
	while (this->_state != IDLE);
	this->runCommand(cmd,sizeof(cmd),5);
}

void VC0706Driver::getFBUFLen(unsigned& len)
{
	const uint8_t cmd[] = {0x56,0,0x34,0x01,0x00};
	this->runCommand(cmd,sizeof(cmd),9);
	len = (rxBuf[_buffer][7] << 8) | (rxBuf[_buffer][8]);
}

uint8_t* VC0706Driver::readFBUF(const unsigned& startAddress,const unsigned& lentoread)
{
	const uint8_t cmd[] = {
		0x56,0x00,0x32,0x0C,0x00,0x0A,					// current frame, mcu transfer
		0x00,0x00,(startAddress>>8),(startAddress&0xFF),// start address
		0x00,0x00,(lentoread>>8),(lentoread&0xFF),		// read length
		0x00,0x00										// read delay
	};
	this->runCommand(cmd,sizeof(cmd),lentoread+10);
	return &this->rxBuf[_buffer][5];
}
void VC0706Driver::startFBUF()
{
	const uint8_t cmd[] = {0x56,0,0x36,0x01,0x02};
	this->runCommand(cmd,sizeof(cmd),5);
}

void VC0706Driver::runCommand(const uint8_t* buf, const unsigned& len, const unsigned& retLen)
{
	rxLen = retLen;
	this->_state = SENDING;
	VC0706Driver::send(buf, len);
	UC1IFG &= ~UCA1TXIFG;
	UC1IFG |= UCA1TXIFG;
	__low_power_mode_3();
}

void VC0706Driver::send(const uint8_t* buf, const unsigned& len)
{
	__istate_t s = __get_interrupt_state();
	__disable_interrupt();
	DMA2CTL &= ~DMAEN;
	DMACTL0 &= ~RXTRIG;
	DMACTL0 |= TXTRIG;		// TX
	
	DMA2SA = (unsigned)buf;
	DMA2DA = (unsigned)&UCA1TXBUF;
	DMA2SZ = len;
	 
	DMA2CTL = DMADT_0		// single transfer
			| DMADSTINCR_0	// dst unchanged
			| DMASRCINCR_3 	// src increment
			| DMADSTBYTE	// dst byte
			| DMASRCBYTE	// src byte
			| DMAIE			// interrtupt enabled
			| DMAEN;			// enabled
	__set_interrupt_state(s);
}

void VC0706Driver::recv(uint8_t* buf, const unsigned& len)
{
	DMA2CTL &= ~DMAEN;
	DMACTL0 &= ~TXTRIG;
	DMACTL0 |= RXTRIG;		// RX
	
	DMA2SA = (unsigned)&UCA1RXBUF;
	DMA2DA = (unsigned)buf;
	DMA2SZ = len;
	
	DMA2CTL = DMADT_0		// single transfer
			| DMADSTINCR_3	// dst increment
			| DMASRCINCR_0 	// src unchanged
			| DMADSTBYTE	// dst byte
			| DMASRCBYTE	// src byte
			| DMAIE			// interrtupt enabled
			| DMAEN;		// enabled
}

bool VC0706Driver::ISR()
{
	switch(this->_state)
	{
	default:
	case IDLE:
		return true;
	case SENDING:
		{
			this->_state = RECEIVING;
			VC0706Driver::recv(&this->rxBuf[_buffer][0],this->rxLen);
			return false;
		}
	case RECEIVING:
		{
			this->_state = IDLE;
			return true;
		}
	};
}

#endif