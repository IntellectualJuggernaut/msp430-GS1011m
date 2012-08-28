//
//  StaticQueue.h
//  GSLibHelpers
//
//  Created by Clayton Thomas on 8/10/12.
//  Copyright (c) 2012 Clayton Thomas. All rights reserved.
//

#ifndef GSLibHelpers_StaticQueue_h
#define GSLibHelpers_StaticQueue_h


#include <stddef.h>
#include <inttypes.h>

template <typename _Tp, size_t sz>
class fifo_node_alloc
{
public:
	typedef _Tp value_type;			//Element type
	typedef _Tp* pointer;				//Pointer to element
	typedef _Tp& reference;			//Reference to element
	typedef const _Tp* const_pointer;	//Constant pointer to element
	typedef const _Tp& const_reference;		//Constant reference to element
	typedef size_t size_type;		//Quantities of elements
	typedef ptrdiff_t difference_type;	//Difference between two pointers
public:
	enum { BlockSize = (sizeof(_Tp) & 0x01) ? (sizeof(_Tp) + 0x01) : (sizeof(_Tp))};
	struct Block { char buffer[BlockSize]; };
	
	enum { ALLOC_QUANTA = BlockSize };
	enum { ALLOC_SIZE = sz*ALLOC_QUANTA };
	
	fifo_node_alloc( ) : head(0), tail(sz)
	{}
	
	
	size_type max_size() const { return size_type(sz); }
	
	inline pointer address(reference r) const { return &r; }
    inline const_pointer address(const_reference r) const { return &r; }
	
//	void construct(pointer ptr, const value_type &  t) const  { new ((void*)ptr) _Tp(t); }
//	void destroy(pointer p) const { ((_Tp*)p)->~_Tp(); }
	
	template <typename CTp>
	static void construct(CTp* ptr, const CTp & t) { (*ptr) = CTp(t); }
	
	template <typename CTp>
	static void destroy(CTp* p) { ((_Tp*)p)->~_Tp(); }
	
	pointer allocate (size_type n, const_pointer hint=0)
	{
		if (head != tail)
		{
			Block* next = &mBuffer[head++];
			if (head >= sz)
				head = 0;
			return (pointer)(next->buffer);
		}
		return 0;
	}
	void deallocate (pointer p, size_type n)
	{
		do {
			putBack();
		} while (--n);
	}
	
private:
	void putBack()
	{
		++tail;
		if (tail >= sz)
			tail = 0;
	}
	uint8_t head, tail;
	Block mBuffer[sz];
};

class empty_fifo_lock
{
public:
	static void lock() {}
	static void unlock() {}
};

struct fifo_node_base
{
	fifo_node_base* next;
};

template <typename _Tp>
struct fifo_node : public fifo_node_base
{
	_Tp data;
};

template <typename _Tp,
			size_t sz = 4,
			typename Mutex = empty_fifo_lock>
class fifo_queue
{
public:
	typedef _Tp value_type;			//Element type
	typedef _Tp* pointer;				//Pointer to element
	typedef _Tp& reference;			//Reference to element
	typedef const _Tp* const_pointer;	//Constant pointer to element
	typedef const _Tp& const_reference;		//Constant reference to element
	typedef size_t size_type;		//Quantities of elements
	typedef ptrdiff_t difference_type;	//Difference between two pointers
	typedef fifo_node<_Tp> Node;
	typedef fifo_node_alloc<Node, sz> allocator_type;
	fifo_queue() : tail(0), count(0) {head.next = 0;}
	
	bool
	empty() const
	{ return size() == 0; }
	
	bool
	full() const
	{ return size() == sz; }
	
	/**  Returns the number of elements in the %queue.  */
	size_type
	size() const
	{ return count; }
	
	/**
	 *  Returns a read/write reference to the data at the first
	 *  element of the %queue.
	 */
	reference
	front()
	{return ((Node*)(head.next))->data;}
	
	/**
	 *  Returns a read-only (constant) reference to the data at the first
	 *  element of the %queue.
	 */
	const_reference
	front() const
	{return ((Node*)(head.next))->data;}
	
	/**
	 *  Returns a read/write reference to the data at the last
	 *  element of the %queue.
	 */
	reference
	back()
	{ return ((Node*)tail)->data; }
	
	/**
	 *  Returns a read-only (constant) reference to the data at the last
	 *  element of the %queue.
	 */
	const_reference
	back() const
	{ return ((Node*)tail)->data; }
	
	/**
	 *  @brief  Add data to the end of the %queue.
	 *  @param  x  Data to be added.
	 *
	 *  This is a typical %queue operation.  The function creates an
	 *  element at the end of the %queue and assigns the given data
	 *  to it.  The time complexity of the operation depends on the
	 *  underlying sequence.
	 */
	bool
	push(const value_type& __x)
	{ if (!full()) {push_back(__x); return true;} return false;}
	
	/**
	 *  @brief  Removes first element.
	 *
	 *  This is a typical %queue operation.  It shrinks the %queue by one.
	 *  The time complexity of the operation depends on the underlying
	 *  sequence.
	 *
	 *  Note that no data is returned, and if the first element's
	 *  data is needed, it should be retrieved before pop() is
	 *  called.
	 */
	void
	pop()
	{ if (!empty()) pop_front(); }
	
protected:
	//static
	//void construct(pointer ptr, const value_type &  t)  {  *(ptr) = _Tp(t); }
//	static
//	void destroy(pointer p) { ((_Tp*)p)->~_Tp(); }
	Node* getNode() { return (Node*)alloc.allocate(1); }
	void push_back(const value_type& __x)
	{
		m.lock();
		{
			Node* newnode = getNode();
			alloc.construct((pointer)&newnode->data, __x);
			
			if (head.next == 0)
				head.next = (fifo_node_base*)newnode;
			if (tail == 0)
			{
				tail = (fifo_node_base*)newnode;
			}
			else
			{
				tail->next = (Node*)newnode;
				tail = tail->next;
			}
			tail->next = 0;
			++count;
		}
		m.unlock();
	}
	void pop_front()
	{
		m.lock();
		{
			Node* p = static_cast<Node*>(head.next);
			if (p)
			{
				head.next = head.next->next;
				alloc.destroy((value_type*)&p->data);
				alloc.deallocate(p,1);
			}
			--count;
		}
		m.unlock();
	}
	
	fifo_node_base head, *tail;
	allocator_type alloc;
	Mutex m;
	unsigned char count;
};

#endif
