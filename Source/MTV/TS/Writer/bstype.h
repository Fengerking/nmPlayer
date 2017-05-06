#ifndef _BS_TYPE_H_
#define _BS_TYPE_H_
#include "vobstype.h"
#ifdef _WIN32
#include <tchar.h>
#include <Windows.h>
#endif

//const uint8 BITVALMASK[] = { 0, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class BitsStream
{
public:

	BitsStream(uint8* p=0);
	void Init(uint8* p);

	uint8* Position();

	void SetPosition(uint8* pos);

	void SkipBits(int width);

	// size in byte
	int WriteByte(int size, uint8* pByte);

// 	void SkipBytes(int size)
// 	{
// 		pbyte += size;
// 	}


	// Write OP
	int WriteBits(int width, uint64 val);



	void Flush();
private:
	uint8*	pbyte;
	int		bitpos;
	uint8	bitsbuf;
	uint8*	test_byte;
};

#ifdef _VONAMESPACE
}
#endif

#endif // _BS_TYPE_H_