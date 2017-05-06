#ifndef __VOSOURCESUBTITLEDATABUFFER_H__
#define __VOSOURCESUBTITLEDATABUFFER_H__
#pragma once

#include "voSourceDataBuffer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class voSourceSubtitleDataBuffer :
	public voSourceDataBuffer
{
public:
	voSourceSubtitleDataBuffer( vo_allocator< VO_BYTE >& allocator );
	virtual ~voSourceSubtitleDataBuffer();

	virtual VO_S32		AddBuffer (_SAMPLE * pBuffer);
	virtual VO_S32		GetBuffer (_SAMPLE * pBuffer);
protected:
	virtual vo_singlelink_list< _SAMPLE* >::iterator FindNearestItem( VO_U64 time );
};

#ifdef _VONAMESPACE
}
#endif

#endif