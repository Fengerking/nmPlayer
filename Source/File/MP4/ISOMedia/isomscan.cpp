#include "isomscan.h"
#include "mpxutil.h"
#include "voLog.h"
//using namespace ISOMS;
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
Scanner::Scanner(Reader* r)
: reader(r)
{
}

bool Scanner::ReadAtom(Atom& atom)
{
	memset(&atom,0,sizeof(Atom));
	uint8 headbuf[8];
	if (reader->Read(headbuf, 8))
	{
		memcpy(&atom.size, headbuf, 4);
		memcpy(&atom.tag, headbuf + 4, 4);
		Util::Swap32(&atom.size);
		Util::Swap32(&atom.tag);
#if 1 // Jason, 4/11/2011, support 64-bit size format, not 64-bit value
		if (atom.size == 1)
		{
			if (reader->Read(headbuf, 8))
			{
				reader->Move(-8); //restore
				memcpy(&atom.size, headbuf, 8);
				Util::Swap64(&atom.size);
			}
		}
#endif
		return true;
	}
	return false;
}

uint32 Scanner::FindTag(uint32 tag)
{
	Atom atom;
	while (ReadAtom(atom))
	{
		if (atom.tag == tag)
			return atom.size - 8;
		if (atom.size < 8) // some box has size 0, such as 'skip'
			break;
		if (!reader->Move(atom.size - 8))
			break;
	}
	return 0;
}

uint32 Scanner::FindTag(uint32 tag, ABSPOS endPos)
{
	Atom atom;
	while (ReadAtom(atom))
	{
		if (atom.tag == tag)
			return atom.size - 8;
		if (atom.size < 8) // some box has size 0, such as 'skip'
			break;
		if (!reader->Move(atom.size - 8))
			break;
		if (reader->Position() >= endPos)
			break;
	}
	return 0;
}
