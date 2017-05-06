#ifndef __REFERENCECOUNT_H__
#define __REFERENCECOUNT_H__

template <typename T>
class reference_count
{
public:
	reference_count(T*p)
		: m_p(p)
		, m_i(1) {}

	virtual ~reference_count(void)
	{
		if (m_p)
			delete m_p;
	}

	T*	m_p;
	int	m_i;
};

template <typename T, typename D>
class reference_count_deleter
	: public reference_count<T>
{
public:
	reference_count_deleter(T* p, D dt)
		: reference_count<T>(p)
		, m_d(dt) {}

	~reference_count_deleter(void)
	{
		m_d(reference_count<T>::m_p);
		this->m_p = NULL;
	}

	D m_d;
};


#endif //__REFERENCECOUNT_H__