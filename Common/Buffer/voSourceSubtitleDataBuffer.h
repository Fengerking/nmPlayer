#pragma once

#include "SubtitleFunc.h"
#include "vo_singlelink_list.hpp"
#include "voSource2.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voSourceSubtitleDataBuffer
{
public:
	voSourceSubtitleDataBuffer( vo_allocator< VO_BYTE >& allocator );
	virtual ~voSourceSubtitleDataBuffer();

	virtual VO_S32		AddBuffer (voSubtitleInfo * pBuffer);
	virtual VO_S32		GetBuffer (voSubtitleInfo * pBuffer);

	virtual VO_U64		GetBuffTime ();
	virtual VO_S32		GetBuffCount ();

	virtual VO_S32		Flush ();

	virtual VO_S32		RemoveTo( VO_U64 timestamp );

protected:
	virtual vo_singlelink_list< voSubtitleInfo* >::iterator FindNearestItem( VO_U64 time );

protected:
	vo_singlelink_list< voSubtitleInfo* > m_list;
	vo_allocator< VO_BYTE >& m_alloc;

	voSubtitleInfo * m_ptr_cache;

	voCMutex			m_lock;
};
    
#ifdef _VONAMESPACE
}
#endif
