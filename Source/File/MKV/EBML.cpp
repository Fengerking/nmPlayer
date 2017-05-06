#include "EBML.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

VO_U8 TranslateVINT( VO_U8 * pByte , VO_U8 * bytelength , VO_S64 * pResult )
{
	VO_U8 firstbyte = pByte[0];
	VO_U8 actrualsize = 0 ;

	*pResult = 0;

	if( firstbyte >= 0x80 )
	{
		actrualsize = 1;
	}
	else if( firstbyte >= 0x40 )
	{
		actrualsize = 2;
	}
	else if( firstbyte >= 0x20 )
	{
		actrualsize = 3;
	}
	else if( firstbyte >= 0x10 )
	{
		actrualsize = 4;
	}
	else if( firstbyte >= 0x8 )
	{
		actrualsize = 5;
	}
	else if( firstbyte >= 0x4 )
	{
		actrualsize = 6;
	}
	else if( firstbyte >= 0x2 )
	{
		actrualsize = 7;
	}
	else if( firstbyte >= 0x1 )
	{
		actrualsize = 8;
	}

	if (actrualsize == 0)///<it is forbidden,so return size == 0
	{
		return 0;
	}
	firstbyte = ((VO_U8)(firstbyte<<actrualsize))>>actrualsize;

	if( actrualsize > *bytelength )
	{
		*bytelength = actrualsize;
		return 1;
	}

	*bytelength = actrualsize;

	VO_PBYTE ptr = (VO_PBYTE)pResult;
	VO_S32 index = 0;

	for( index = 0 ; index < actrualsize - 1 ; index++ )
	{
		ptr[index] = pByte[ actrualsize - index - 1];
	}

	ptr[index] = firstbyte;

	return 0;
}

VO_U8 TranslateVSINT( VO_U8 * pByte , VO_U8 * bytelength , VO_S64 * pResult )
{
	VO_U8 retval;
	retval = TranslateVINT( pByte , bytelength , pResult );

	if( retval == 1 )
		return 1;

	*pResult = *pResult - gc_vsintsub[ *bytelength - 1 ];

	return 0;
}

#ifdef _VONAMESPACE
}
#endif

