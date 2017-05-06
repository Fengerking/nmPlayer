#ifndef __VO_SINGLELINK_LIST_HPP__

#define __VO_SINGLELINK_LIST_HPP__

#include "vo_allocator.hpp"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

template < class T >
class vo_singlelink_listitem
{
public:
	vo_singlelink_listitem( const T& obj )
		:m_obj(obj)
		,m_ptr_next(0)
	{
	}


	T m_obj;
	vo_singlelink_listitem< T > * m_ptr_next;
};

template < class T >
class vo_singlelink_listitem_iterator
{
public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

	vo_singlelink_listitem_iterator( vo_singlelink_listitem< T > * ptr_obj )
		:m_ptr_obj( ptr_obj )
	{
	}

	vo_singlelink_listitem_iterator( const vo_singlelink_listitem_iterator< T >& ref_obj )
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

	vo_singlelink_listitem_iterator<T>& operator++()
	{
		m_ptr_obj = m_ptr_obj->m_ptr_next;
		return *this;
	}

	vo_singlelink_listitem_iterator<T> operator++(int)
	{
		vo_singlelink_listitem_iterator<T> temp = *this;
		m_ptr_obj = m_ptr_obj->m_ptr_next;
		return temp;
	}

	bool operator==( const vo_singlelink_listitem_iterator<T>& m_ref )
	{
		return (m_ptr_obj == m_ref.m_ptr_obj);
	}

	bool operator!=( const vo_singlelink_listitem_iterator<T>& m_ref )
	{
		return (m_ptr_obj != m_ref.m_ptr_obj);
	}

protected:
	vo_singlelink_listitem< T > * m_ptr_obj;
};

template < class T , class MEM = vo_allocator< vo_singlelink_listitem<T> > >
class vo_singlelink_list
{
public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;

	typedef vo_singlelink_listitem< T > listitem;

	typedef vo_singlelink_listitem_iterator< T > iterator;

	vo_singlelink_list()
		:m_ptr_header(0)
		,m_ptr_tail(0)
		,m_itemcount(0)
	{
	}

	~vo_singlelink_list()
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
	void insert_sort( const T& obj , Comp compare )
	{
		listitem * ptr_item = m_ptr_header;
		listitem * ptr_lastitem = m_ptr_header;

		while( ptr_item )
		{
			if( compare( ptr_item->m_obj , obj ) < 0 )
			{
				ptr_lastitem = ptr_item;
				ptr_item =  ptr_item->m_ptr_next;
			}
			else
			{
				add_item_between( ptr_lastitem , ptr_item , obj );
				return;
			}
		}

		add_item_back( obj );
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

	void removeto( iterator itere )
	{
		iterator iter = begin();

		while( iter != itere )
		{
			iter++;
			del_front();
		}
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
			m_ptr_tail = ptr;
		}

		m_itemcount++;
	}

	void add_item_between( listitem * pre , listitem * after , const T& obj )
	{
		listitem * ptr = (listitem *)alloctor.allocate(1);
		alloctor.construct( ptr , obj );

		if( pre == m_ptr_header && pre == after )
		{
			if( m_ptr_header == 0 )
				m_ptr_header = m_ptr_tail = ptr;
			else
			{
				ptr->m_ptr_next = m_ptr_header;
				m_ptr_header = ptr;
			}
		}
		else
		{
			pre->m_ptr_next = ptr;
			ptr->m_ptr_next = after;
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