#ifndef _ABSTRACT_HEADER_H
#define _ABSTRACT_HEADER_H

#include "voitypes.h"

#ifdef _HEAD_MTYPE
#include <streams.h>
#endif //_HEAD_MTYPE

class AbstractHeader
{

public:

#ifdef _HEAD_DECODE
	virtual bool Decode(uint8* data, uint32 size) = 0;
#endif //_HEAD_DECODE

#ifdef _HEAD_ENCODE
	virtual bool Encode(uint8* data, uint32& size) = 0;
#endif //_HEAD_ENCODE
	
#ifdef _HEAD_MTYPE
	virtual HRESULT BuildMediaType(CMediaType* pmt) = 0;
#endif //_HEAD_MTYPE

#if defined(_HEAD_DECODE) && defined(_HEAD_MTYPE)

	HRESULT ParseAndBuildMediaType(CMediaType* pmt, uint8* data, uint32 size)
	{
		if (!Decode(data, size))
			return E_FAIL;
		return BuildMediaType(pmt);
	}

#endif //defined(_HEAD_DECODE) && defined(_HEAD_MTYPE)

};

#endif // _ABSTRACT_HEADER_H
