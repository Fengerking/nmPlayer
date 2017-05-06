#ifndef __LIST_T_H__
#define __LIST_T_H__

#include "voYYDef_SrcCmn.h"
#include <string.h>
#include "macro.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


template<class _Type>
class _List_node
{
public:
	_List_node()
		: _Next(0)
		, _Prev(0)
	{
	}

	_List_node(_List_node * _P_next_node, _List_node * _P_prev_node, _Type _Value)
		: _Next(_P_next_node)
		, _Prev(_P_prev_node)
		, _Val(_Value)
	{
	}

public:
	_List_node * _Next;
	_List_node * _Prev;
	_Type        _Val;
};



template<
	class _Type
>
class allocator_base;

template<
	class _Type,
	int _Size
>
class allocator;



template<
	class _Type,
	class _Allocator = allocator_base<_Type> 
>
class list_T : public _List_node<_Type>
{
	typedef _List_node<_Type> _List_node_T;

public:
	static _List_node_T * & _Nextnode(_List_node_T * _P_node)
	{
		return _P_node->_Next;
	}

	static _List_node_T * & _Prevnode(_List_node_T * _P_node)
	{
		return _P_node->_Prev;
	}

	static _Type _Value(_List_node_T * _P_node)
	{
		return _P_node->_Val;
	}

public:
	friend class iterator;

	class iterator
	{
	public:
		iterator()
			: _P_node(NULL)
		{
		}

		iterator(_List_node_T * _P_node)
			: _P_node(_P_node)
		{
		}

	public:
		_Type & operator*()
		{
			return _P_node->_Val;
		}

		_Type * operator->()
		{
			return (&**this);
		}

		iterator & operator++()
		{
			_P_node = _P_node->_Next;
			return (*this);
		}

		iterator operator++(int)
		{
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}

		iterator & operator--()
		{
			_P_node = _P_node->_Prev;
			return (*this);
		}

		iterator operator--(int)
		{
			iterator _Tmp = *this;
			--*this;
			return (_Tmp);
		}

		bool operator==(const iterator & _Compare) const
		{
			return (_P_node == _Compare._P_node);
		}

		bool operator!=(const iterator & _Compare) const
		{
			return (!(_P_node == _Compare._P_node));
		}

	public:
		_List_node_T * _P_node;
	};

public:
	list_T()
		: _Size(0)
	{
		_P_allocator = new _Allocator;
		_P_head = new _List_node_T(this, NULL, _Type());
		_Prevnode(this) = _P_head;
	}

	~list_T()
	{
		clear();
		SAFE_DELETE(_P_head);
		SAFE_DELETE(_P_allocator);
	}

public:
	_Type & front()
	{
		return (*begin());
	}

	const _Type front() const
	{
		return (*begin());
	}

	void push_front(const _Type & _Val)
	{
		_Insert(begin(), _Val);
	}

	void pop_front()
	{
		remove(begin());
	}

	_Type & back()
	{	
		return (*(--end()));
	}

	const _Type back() const
	{
		return (*(--end()));
	}

	void push_back(const _Type & _Val)
	{
		_Insert(end(), _Val);
	}

	void pop_back()
	{
		remove(--end());
	}

	iterator insert(iterator _Where, _Type & _Val)
	{
		_Insert(_Where, _Val);
		return (--_Where);
	}

	void remove(const _Type & _Val)
	{
		for(iterator iter=begin(); iter!=end(); ++iter)
		{
			if(*iter == _Val)
			{
				remove(iter);
				break;
			}
		}
	}

	void remove(iterator _Where)
	{
		_List_node_T * _P_node = _Where._P_node;
		if(_P_node == _P_head || _P_node == this)
			return;

		_Nextnode(_Prevnode(_P_node)) = _Nextnode(_P_node);
		_Prevnode(_Nextnode(_P_node)) = _Prevnode(_P_node);
		_P_allocator->deallocate(_P_node);

		_Decrease(1);
	}

	void clear()
	{
		_List_node_T * _P_node = _Nextnode(_P_head);
		_List_node_T * _P_next_node = NULL;
		for(; _P_node!=this; _P_node = _P_next_node)
		{
			_P_next_node = _Nextnode(_P_node);
			_P_allocator->deallocate(_P_node);
		}

		_Nextnode(_P_head) = this;
		_Prevnode(this) = _P_head;
		_Size = 0;
	}

	iterator begin()
	{
		return iterator(_Nextnode(_P_head));
	}

	iterator end()
	{
		return iterator(this);
	}

	bool empty( ) const 
	{ 
		return (_Size == 0);
	}

	int size() const
	{
		return _Size;
	}

	int max_size() const
	{
		_P_allocator->max_size();
	}

protected:
	void _Insert(iterator _Where, const _Type & _Val)
	{
		_List_node_T * _P_node = _Where._P_node;
		_List_node_T * _New_node = _P_allocator->allocate();
		if(_New_node == NULL)
			return;

		_Prevnode(_New_node) = _Prevnode(_P_node);
		_Nextnode(_New_node) = _P_node;
		_New_node->_Val = _Val;

		_Prevnode(_P_node) = _New_node;
		_Nextnode(_Prevnode(_New_node)) = _New_node;

		_Increase(1);
	}

	void _Increase(int _Count)
	{
		_Size += _Count;
	}

	void _Decrease(int _Count)
	{
		_Size -= _Count;
	}

public:
	_List_node_T   * _P_head;
	int              _Size;
	_List_node_T	firstNode;

public:
	_Allocator     * _P_allocator;
	_Allocator		firstAllocator;
};

//----------------------------     allocator     -------------------------

template<class _Type>
class allocator_base
{
public:
	typedef _List_node<_Type> _List_node_T;
	
public:
	virtual _List_node_T * allocate()
	{
		_List_node_T * newNode=new _List_node_T();
		return newNode;
	}

	virtual void deallocate(_List_node_T * _P_node)
	{
		SAFE_DELETE(_P_node);
	}

	virtual int max_size() const
	{
		return 0;
	}
};



template<
	class _Type,
	int _Size = 128
>
class allocator : public allocator_base<_Type>
{
typedef _List_node<_Type> _List_node_T;
public:
	allocator()
		: _P_buffer(NULL)
		, _Max_size(0)
		, _Index(0)
	{

	}

	~allocator()
	{
		for(int i=0; i<_Max_size; ++i)
		{
			SAFE_DELETE( _P_buffer[i]);
		}
		SAFE_DELETE_ARRAY(_P_buffer);
	}

public:
	virtual _List_node_T * allocate()
	{
		do
		{
			if(_Index >= _Max_size)
			{
				if(!preallocate())
					break;
			}

			return _P_buffer[_Index++];

		}while(0);

		return NULL;
	}

	virtual void deallocate(_List_node_T * _P_node)
	{
		_P_buffer[--_Index] = _P_node;
	}

	virtual int max_size() const
	{
		return _Max_size;
	}

protected:
	bool preallocate()
	{
		do
		{
			int _New_buffer_size = _Max_size + _Size;
			_List_node_T ** _P_new_buffer = new _List_node_T * [_New_buffer_size];
			if(_P_new_buffer == NULL)
				break;

			memset(_P_new_buffer, 0, _New_buffer_size * sizeof(_List_node_T *));
			for(int i=_Index; i<_New_buffer_size; ++i)
			{
				_P_new_buffer[i] = new _List_node_T();
				if(_P_new_buffer[i] == NULL)
					break;
			}

			SAFE_DELETE_ARRAY(_P_buffer);
			_P_buffer = _P_new_buffer;
			_Max_size = _New_buffer_size;

			return true;

		}while(0);

		return false;
	}

public:
	_List_node_T ** _P_buffer; 
	int _Max_size;
	int _Index;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__LIST_T_H__
