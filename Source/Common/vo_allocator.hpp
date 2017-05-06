
#ifndef _VO_ALLOCATOR_HPP__

#define _VO_ALLOCATOR_HPP__

#include <new>

#include "voYYDef_SrcCmn.h"

#define _DESTRUCTOR(ty, ptr)	(ptr)->~ty()

template<class T>
class vo_allocator
{
public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

	template<class T2>
	struct rebind
	{
		typedef vo_allocator<T2> other;
	};

	pointer address( reference val ) const
	{
		return &val;
	}

	const_pointer address( const_reference val ) const
	{
		return &val;
	}

	vo_allocator()
	{
	}

	vo_allocator( const vo_allocator<T>& )
	{
	}

	template<class T2>
	vo_allocator( const vo_allocator<T2>& )
	{
	}

	template<class T2>
	vo_allocator<T2>& operator=( const vo_allocator<T2>& )
	{
		return (*this);
	}

	void deallocate( pointer _Ptr, size_t size = 1 )
	{
		delete(_Ptr);
	}

	pointer allocate( size_t count )
	{
		return (pointer)::operator new( count * sizeof(T) );
	}

	pointer allocate( size_t count, const void * )
	{
		return (pointer)::operator new( sizeof(const void *) * count );
	}

	void construct( pointer ptr_obj, const T& val )
	{
		void * ptr = ptr_obj;
		new(ptr) T(val);
	}

	void destroy( pointer ptr_obj )
	{
		_DESTRUCTOR( T, ptr_obj );
	}

	size_t max_size() const
	{
		size_t count = (size_t)(-1) / sizeof (T);
		return (0 < count ? count : 1);
	}
};

#endif