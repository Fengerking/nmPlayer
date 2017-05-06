#ifndef __SHAREDPTR_H__
#define __SHAREDPTR_H__
#include "reference_count.hpp"


template <typename T>
class shared_ptr
{
public:
	explicit shared_ptr(T* p)
		: m_refer_counter( new reference_count<T>(p) ) {}

	template<typename D>
	explicit shared_ptr(T* p, D dt)
		: m_refer_counter( new reference_count_deleter<T, D>(p, dt) )
	{	// construct with _Px, deleter
	}

	explicit shared_ptr()
		: m_refer_counter( new reference_count<T>(NULL) ) {}

//	shared_ptr(nullptr_t)
//		: m_refer_counter( new reference_count<T>(NULL) ) {}

	~shared_ptr()
	{
		decrease();
	}


	shared_ptr(const shared_ptr& rhs)
		: m_refer_counter(rhs.m_refer_counter)
	{
		increase();
	}

	shared_ptr& operator=(const shared_ptr& rhs)
	{
		shared_ptr(rhs).swap(*this);
		return (*this);
	}

	shared_ptr& operator=(T* p)
	{
		shared_ptr(p).swap(*this);
		return (*this);
	}

	T* operator->() const
	{
		return m_refer_counter->m_p;
	}

	T* get() const
	{	// return pointer to resource
		return m_refer_counter->m_p;
	}

	operator bool () const
	{	// test if shared_ptr object owns no resource
		return (get() != 0);
	}

private:
	void increase()
	{
		m_refer_counter->m_i++;
	}

	void decrease()
	{
		m_refer_counter->m_i--;

		if (0 == m_refer_counter->m_i)
		{
			delete m_refer_counter;
		}
	}

	void swap(shared_ptr<T>& _Other)
	{
		reference_count<T>* _tmp	= m_refer_counter;
		m_refer_counter				= _Other.m_refer_counter;
		_Other.m_refer_counter		= _tmp;
	}

	reference_count<T>*	m_refer_counter;
};

template<class T, class U>
bool operator==(const shared_ptr<T>& _Left, const shared_ptr<U>& _Right)
{	// test if shared_ptr == shared_ptr
	return ( _Left.get() == _Right.get() );
}

template<class T, class U>
bool operator!=(const shared_ptr<T>& _Left, const shared_ptr<U>& _Right)
{	// test if shared_ptr != shared_ptr
	return ( _Left.get() != _Right.get() );
}

#endif //__SHAREDPTR_H__
