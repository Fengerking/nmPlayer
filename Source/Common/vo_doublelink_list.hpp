#ifndef __VO_DOUBLELINK_LIST_HPP__

#define __VO_DOUBLELINK_LIST_HPP__

#include "vo_allocator.hpp"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

template < class T >
class vo_doublelink_listitem
{
public:
	vo_doublelink_listitem( const T& obj )
		:m_obj(obj)
		,m_ptr_next(0)
		,m_ptr_pre(0)
	{
	}


	T m_obj;
	vo_doublelink_listitem< T > * m_ptr_next;
	vo_doublelink_listitem< T > * m_ptr_pre;
};

template < class T >
class vo_doublelink_listitem_iterator
{
public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

	vo_doublelink_listitem_iterator( vo_doublelink_listitem< T > * ptr_obj )
		:m_ptr_obj( ptr_obj )
	{
	}

	vo_doublelink_listitem_iterator( const vo_doublelink_listitem_iterator< T >& ref_obj )
	{
		m_ptr_obj = ref_obj.m_ptr_obj;
	}

	reference operator*()
	{
		return m_ptr_obj->m_obj;
	}

	pointer operator->()
	{
		return &(m_ptr_obj->m_obj);
	}

	vo_doublelink_listitem_iterator<T>& operator++()
	{
		m_ptr_obj = m_ptr_obj->m_ptr_next;
		return *this;
	}

	vo_doublelink_listitem_iterator<T> operator++(int)
	{
		vo_doublelink_listitem_iterator<T> temp = *this;
		m_ptr_obj = m_ptr_obj->m_ptr_next;
		return temp;
	}

	bool operator==( const vo_doublelink_listitem_iterator<T>& m_ref )
	{
		return (m_ptr_obj == m_ref.m_ptr_obj);
	}

	bool operator!=( const vo_doublelink_listitem_iterator<T>& m_ref )
	{
		return (m_ptr_obj != m_ref.m_ptr_obj);
	}

protected:
	vo_doublelink_listitem< T > * m_ptr_obj;
};

template < class T , class MEM = vo_allocator< vo_doublelink_listitem<T> > >
class vo_doublelink_list
{
public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

	typedef vo_doublelink_listitem< T > listitem;

	typedef vo_doublelink_listitem_iterator< T > iterator;

	vo_doublelink_list()
		:m_ptr_header(0)
		,m_ptr_tail(0)
		,m_itemcount(0)
	{
	}

	~vo_doublelink_list()
	{
		reset();
	}

	void push_front( const T& obj )
	{
		add_item_front( obj );
	}

	void push_back( const T& obj )
	{
		add_item_back( obj );
	}

	template< class Comp >
	void insert_front_sort( const T& obj , Comp compare )
	{
		listitem * ptr_item = m_ptr_header;

		while( ptr_item )
		{
			if( compare( ptr_item->m_obj , obj ) < 0 )
			{
				ptr_item =  ptr_item->m_ptr_next;
			}
			else
			{
				add_item_before( ptr_item , obj );
				return;
			}
		}

		add_item_back( obj );
	}

	template< class Comp >
	void insert_back_sort( const T& obj , Comp compare )
	{
		listitem * ptr_item = m_ptr_tail;

		while( ptr_item )
		{
			if( compare( ptr_item->m_obj , obj ) > 0 )
			{
				ptr_item =  ptr_item->m_ptr_pre;
			}
			else
			{
				add_item_after( ptr_item , obj );
				return;
			}
		}

		add_item_front( obj );
	}

	void pop_front()
	{
		del_front();
	}

	iterator begin()
	{
		return iterator( m_ptr_header );
	}

	iterator end()
	{
		return iterator( 0 );
	}

	iterator last()
	{
		return iterator( m_ptr_tail );
	}

	size_t count()
	{
		return m_itemcount;
	}

	bool empty()
	{
		return m_itemcount == 0;
	}

	void reset()
	{
		while( m_ptr_header )
			del_front();
	}

private:
	void add_item_front( const T& obj )
	{
		listitem * ptr = (listitem *)alloctor.allocate(1);
		alloctor.construct( ptr , obj );

		if( m_ptr_header == 0 )
			m_ptr_header = m_ptr_tail = ptr;
		else
		{
			ptr->m_ptr_next = m_ptr_header;
			m_ptr_header->m_ptr_pre = ptr;
			m_ptr_header = ptr;
		}

		m_itemcount++;
	}

	void add_item_back( const T& obj )
	{
		listitem * ptr = (listitem *)alloctor.allocate(1);
		alloctor.construct( ptr , obj );

		if( m_ptr_header == 0 )
			m_ptr_header = m_ptr_tail = ptr;
		else
		{
			m_ptr_tail->m_ptr_next = ptr;
			ptr->m_ptr_pre = m_ptr_tail;
			m_ptr_tail = ptr;
		}

		m_itemcount++;
	}

	void add_item_before( listitem * ptr_item , const T& obj )
	{
		listitem * ptr = (listitem *)alloctor.allocate(1);
		alloctor.construct( ptr , obj );

		if( m_ptr_header == ptr_item )
		{
			ptr->m_ptr_next = m_ptr_header;
			m_ptr_header->m_ptr_pre = ptr;
			m_ptr_header = ptr;
		}
		else
		{
			ptr->m_ptr_next = ptr_item;
			ptr_item->m_ptr_pre->m_ptr_next = ptr;
			ptr->m_ptr_pre = ptr_item->m_ptr_pre;
			ptr_item->m_ptr_pre = ptr;
		}

		m_itemcount++;
	}

	void add_item_after( listitem * ptr_item , const T& obj )
	{
		listitem * ptr = (listitem *)alloctor.allocate(1);
		alloctor.construct( ptr , obj );

		if( m_ptr_tail == ptr_item )
		{
			m_ptr_tail->m_ptr_next = ptr;
			ptr->m_ptr_pre = m_ptr_tail;
			m_ptr_tail = ptr;
		}
		else
		{
			ptr->m_ptr_next = ptr_item->m_ptr_next;
			ptr_item->m_ptr_next->m_ptr_pre = ptr;
			ptr_item->m_ptr_next = ptr;
			ptr->m_ptr_pre = ptr_item;
		}

		m_itemcount++;
	}

	void del_front()
	{
		if( m_ptr_header == m_ptr_tail )
		{
			if( m_ptr_header )
			{
				alloctor.destroy( m_ptr_header );
				alloctor.deallocate( m_ptr_header );
			}

			m_ptr_header = m_ptr_tail = 0;
		}
		else
		{
			listitem * ptr = m_ptr_header;
			m_ptr_header = m_ptr_header->m_ptr_next;
			m_ptr_header->m_ptr_pre = 0;
			alloctor.destroy( ptr );
			alloctor.deallocate( ptr );
		}

		if( m_itemcount > 0 )
			m_itemcount--;
	}

private:
	listitem * m_ptr_header;
	listitem * m_ptr_tail;

	size_t m_itemcount;

	MEM alloctor;
};

#ifdef _VONAMESPACE
}
#endif

#endif