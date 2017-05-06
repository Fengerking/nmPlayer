/**
 * ISO Media
 * Base classes
 */

#pragma once

#include "voYYDef_TS.h"
#include "vobstype.h"
#include "isomio.h"

#ifdef _XMLDUMP
#include "xmlwr.h"
#endif //_XMLDUMP

namespace ISOM {

class Container;

//====================================================================

class Object
{
public:
	Object();
	virtual ~Object();

protected:
	virtual bool IsContainer() const 
	{ 
		return false; 
	}

private:
	Object* sibling;
	Container* parent;

public:
	Container* Parent() const
	{
		return parent;
	}

	void SetParent(Container* p)
	{
		parent = p;
	}

	Object* NextSibling() const
	{
		return sibling;
	}

	void SetSibling(Object* s)
	{
		sibling = s;
	}

protected:
	uint32 address;
	uint32 size;

public:
	uint32 GetAddress() const 
	{ 
		return address; 
	}

	uint32 GetSize() const 
	{ 
		return size; 
	}

	void SetAddress(uint32 a) 
	{ 
		address = a; 
	}

	void SetSize(uint32 s) 
	{
		size = s;
	}

#ifdef _DEBUG
public:
	static unsigned int AliveObjects;
	static unsigned int MaxObjects;
#endif //_DEBUG

#ifdef _XMLDUMP
public:
	virtual bool Dump(XmlWriter& w) const;
	virtual const char* GetXmlTag() const { return "obj"; }
	virtual bool DumpAttributes(XmlWriter& w) const;
	virtual bool DumpChildren(XmlWriter& w) const { return true; }
#endif //_XMLDUMP

public:
	virtual int NodeCount() const
	{
		return 1;
	}

	virtual uint32 NodeSize() const
	{
		return size;
	}

public:
	virtual bool Dump(Writer& w) const;
};

//====================================================================

class Container : public Object
{
public:
	Container();
	virtual ~Container();

protected:
	virtual bool IsContainer() const 
	{ 
		return true; 
	}

private:
	Object* firstchild;
	Object* lastchild;

public:
	Object* FirstChild() const
	{ 
		return firstchild; 
	}

	Object* NextChild(Object* current) const 
	{ 
		return current->NextSibling(); 
	}

	Object* LastChild() const
	{ 
		return lastchild; 
	}

	int ChildrenCount() const
	{
		int c = 0;
		Object* b = FirstChild();
		while (b)
		{
			++c;
			b = NextChild(b);
		}
		return c;
	}

	Object* AddChild(Object* obj)
	{
		Object* lc = lastchild;
		if (lc)
			lc->SetSibling(obj);
		else
			firstchild = obj;
		lastchild = obj;
		obj->SetParent(this);
		return obj;
	}

	Object* RemoveChild(Object* obj);

public:
	uint32 CalcChildrenSize() const;


#ifdef _XMLDUMP
public:
	virtual bool DumpChildren(XmlWriter& w) const;
#endif //_XMLDUMP

public:
	virtual int NodeCount() const
	{
		int c = 0;
		Object* b = FirstChild();
		while (b)
		{
			c += b->NodeCount();
			b = NextChild(b);
		}
		return c;
	}

	virtual uint32 NodeSize() const
	{
		uint32 c = 0;
		Object* b = FirstChild();
		while (b)
		{
			c += b->NodeSize();
			b = NextChild(b);
		}
		return c;
	}

public:
	virtual bool Dump(Writer& w) const;
	virtual bool DumpChildren(Writer& w) const;

};


//====================================================================

#define WRAP_CHILDREN_FUNCTIONS(T) \
	int Get##T##Count() const \
	{ \
		return ChildrenCount(); \
	} \
\
	T* First##T() const \
	{ \
		return (T*)FirstChild(); \
	} \
\
	T* Last##T() const \
	{ \
		return (T*)LastChild(); \
	} \
\
	T* Next##T(T* current) const \
	{ \
		return (T*)NextChild(current); \
	} \
\
	T* Add##T(T* child) \
	{ \
		return (T*)AddChild(child); \
	}

#define WRAP_PARENT_FUNCTIONS(T) \
	T* Get##T() const \
	{ \
		return (T*)Parent(); \
	}

#define WRAP_TREE_FUNCTIONS(C, P) \
	WRAP_CHILDREN_FUNCTIONS(C) \
	WRAP_PARENT_FUNCTIONS(P)

} //namespace ISOM
