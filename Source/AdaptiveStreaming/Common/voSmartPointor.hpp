#ifndef __VOSMARTPOINTOR_H__

#define __VOSMARTPOINTOR_H__

template< typename T >
class voSmartPointor
{
	typedef T* element_pointor;
public:
	voSmartPointor( T * ptr )
		:m_ptr_obj( ptr )
	{
		;
	}

	~voSmartPointor()
	{
		delete []m_ptr_obj;
	}

	operator element_pointor()
	{
		return m_ptr_obj;
	}

	operator void*()
	{
		return (void*)m_ptr_obj;
	}

	operator VO_PBYTE()
	{
		return (VO_PBYTE)m_ptr_obj;
	}

protected:
	element_pointor m_ptr_obj;
};


#endif