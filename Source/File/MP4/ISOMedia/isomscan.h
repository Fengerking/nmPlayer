/**
 * A fast scan version to parse ISO Media file
 * @created 4/27/2006
 */

#ifndef _ISOM_SCAN_H
#define _ISOM_SCAN_H

#include <stdio.h>
#ifdef _XMLDUMP
#include "../MP4Base/xmlwr.h"
#endif //_XMLDUMP

#include "../MP4Base/mpxobj.h"
#include "../MP4Base/mpxio.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//using namespace MPx;

//namespace ISOMS {

struct Atom
{
	uint64 size;
	uint32 tag;
};

class Scanner
{
public:
	Scanner(Reader* r);
	bool ReadAtom(Atom& atom);
	uint32 FindTag(uint32 tag); //return size of data part
	uint32 FindTag(uint32 tag, ABSPOS endPos); 

	Reader* GetReader() { return reader; }

protected:
	Reader* reader;
};


#ifdef _VONAMESPACE
}
#endif
//} //namespace ISOMS

#endif // _ISOM_SCAN_H
