#ifndef _VO_BSC_API_H_
#define _VO_BSC_API_H_

#include "voAudio.h"
#include "voVideo.h"
#include "vobstype.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
* Find patterns like 00 00 01 B3
*/
class PatternFinder
{
public:
	PatternFinder(uint32 pattern=1, uint32 mask=0xffffffff)
	{
		SetPattern(pattern, mask);
	}

	void SetPattern(uint32 pattern, uint32 mask=0xffffffff)
	{
		this->pattern = pattern;
		this->mask = mask;
		current = 0x5a5a5a5a; //choose a value never used in common patterns. common patterns include 000001xx, fffx
	}

	uint8* Find(uint8* pStart, uint8* pEnd)
	{
		uint8* p = pStart;
		while (p < pEnd)
		{
			current <<= 8;
			current |= *p++;
			if ((current & mask) == pattern)
				return p;
		}
		return 0;
	}

	int Unmatched()
	{
		uint32 subpattern = pattern;
		uint32 submask = mask >> 8;
		int unmatched = 1;
		while (submask)
		{
			subpattern >>= 8;
			if ((current & submask) == subpattern)
				return unmatched;
			submask >>= 8;
			++unmatched;
		}
		return unmatched;
	}

	uint8* ExportPattern(uint8* buffer, int size)
	{
		uint32 t = pattern;
		uint8* p = buffer + size;
		for (int i = 0; i < size; i++)
		{
			*(--p) = (uint8)t;
			t >>= 8;
		}
		return p;
	}

	uint32 GetCurrent() { return current; }

private:
	uint32 pattern;
	uint32 mask;
	uint32 current;
};

#ifdef _VONAMESPACE
}
#endif

#endif // end of _VO_BSC_API_H_