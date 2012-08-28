#ifndef _SOCK_STRUCT_H_
#define _SOCK_STRUCT_H_

#include <msp430.h>
#include <inttypes.h>


struct Socket
{
	enum { Base_Address = TBR_ };
	enum 
	{
		closed = (uint8_t)0,
		open = (uint8_t)1<<0,
		connected = (uint8_t)1<<1,
		STATE_MASK = open|connected
	};
	enum
	{
		none = (uint8_t)0,
		connreq = (uint8_t)1<<2,
		dataavail = (uint8_t)1<<3,
		EVENT_MASK = connreq|dataavail
	};
	static 
	Socket& table(const uint16_t& cid) { return *((Socket*)((volatile uint8_t*)(cid + Base_Address))); }
	
	uint8_t getCID() const { return (uint8_t)((uint16_t)this-Base_Address); }
	void clearState() { (*(volatile uint8_t*)this) &= ~(STATE_MASK); }
	void setState(const uint8_t& state) { clearState(); (*(volatile uint8_t*)this) |= state; }
	uint8_t getState() const { return (*(volatile uint8_t*)this)&STATE_MASK; }
	
	void clearEvent() { (*(volatile uint8_t*)this) &= ~(EVENT_MASK); }
	void setEvent(const uint8_t& event) { clearEvent(); (*(volatile uint8_t*)this) |= event; }
	uint8_t getEvent() const { return (*(volatile uint8_t*)this)&EVENT_MASK; }
	
	void clearRemoteCID() { (*(volatile uint8_t*)this) &= ~(0xF0); }
	void setRemoteCID(const uint8_t& rcid) { clearRemoteCID(); (*(volatile uint8_t*)this) |= (rcid <<4); }
	uint8_t getRemoteCID() const { return (*(volatile uint8_t*)this)>>4; }
	
	void invalidate() { (*(volatile uint8_t*)this) = 0; }
private:
	Socket(const Socket& rhs);
	Socket& operator =(const Socket& rhs);
};

#endif