#include "mpxobj.h"

//using namespace MPx;
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _DEBUG
unsigned int Object::AliveObjects = 0;
unsigned int Object::MaxObjects = 0;
#endif //_DEBUG


//====================================================================

Object::Object()

{
	parent = 0;
	sibling = 0;
	address = 0;
	size = 0;
#ifdef _DEBUG
	++AliveObjects;
	if (MaxObjects < AliveObjects)
		MaxObjects = AliveObjects;
#endif //_DEBUG
}

Object::~Object()
{
#ifdef _DEBUG
	--AliveObjects;
#endif //_DEBUG
}

bool Object::FastDump(Writer& w) const
{
	uint8* buf = new uint8[GetSize()];
	if (buf == NULL)
		return false;
	MemStream ms(buf, GetSize());
	Stream* oldStream = w.GetStream();
	w.SetStream(&ms); //since writer maybe WriterMSB, WriterLSB, just use original writer
	bool b = Dump(w);
	w.SetStream(oldStream);
	if (b)
		b = w.Write(buf, GetSize());
	delete buf;
	return b;
}


//====================================================================

Container::Container()
: firstchild(0),
  lastchild(0)
{
}

Container::~Container()
{
	RemoveChildren();
}

uint32 Container::CalcChildrenSize() const
{
	uint32 sum = 0;
	Object* obj = FirstChild();
	while (obj)
	{
		sum += obj->GetSize();
		obj = NextChild(obj);
	}
	return sum;
}

void Container::RemoveChildren()
{
	Object* obj = FirstChild();
	while (obj)
	{
		Object* t = obj;
		obj = NextChild(obj);
		delete t;
	}
	firstchild = 0;
	lastchild = 0;
}

Object* Container::RemoveChild(Object* obj)
{
	Object* pprev = 0;
	Object* pnext = 0;
	Object* b = FirstChild();
	while (b)
	{
		pnext = NextChild(b);
		if (b == obj)
		{
			if (pprev)
			{
				pprev->SetSibling(pnext); //even if pnext==null, 3/4/2005
				if (!pnext)
					lastchild = pprev;
			}
			else
			{
				if (pnext)
					firstchild = pnext;
				else
					firstchild = lastchild = 0;
			}
			break; //3/4/2005
		}
		pprev = b;
		b = pnext;
	}
	return obj;
}

//====================================================================

#ifdef _XMLDUMP

#if 0
bool Object::Dump(XmlWriter& w) const
{
	bool b = w.StartTag(GetXmlTag());
	b &= DumpAttributes(w);
	b &= w.EndTag();
	return b;
}
#endif

bool Object::Dump(XmlWriter& w) const
{
	bool b = w.StartTag(GetXmlTag());
	b &= DumpAttributes(w);
	b &= w.CloseTag();
	b &= DumpChildren(w);
	b &= w.EndTag(GetXmlTag());
	return b;
}

bool Object::DumpAttributes(XmlWriter& w) const
{
	bool b = w.OutAttribute("addr", address);
	b &= w.OutAttribute("size", size);
	return b;
}

bool Container::DumpChildren(XmlWriter& w) const
{
	bool b = true;
	Object* obj = FirstChild();
	while (obj)
	{
		b &= obj->Dump(w);
		obj = NextChild(obj);
	}
	return b;
}

#endif //_XMLDUMP

bool Object::Dump(Writer& w) const
{
	return false;
}

bool Container::Dump(Writer& w) const
{
	bool b = DumpChildren(w);
	return b;
}

bool Container::DumpChildren(Writer& w) const
{
	bool b = true;
	Object* obj = FirstChild();
	while (obj)
	{
		b &= obj->Dump(w);
		obj = NextChild(obj);
	}
	return b;
}



