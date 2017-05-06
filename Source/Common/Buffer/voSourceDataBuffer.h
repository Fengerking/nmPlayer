#ifndef __VOSOURCEDATABUFFER_H__
#define __VOSOURCEDATABUFFER_H__
#include "voYYDef_SrcCmn.h"
#include "vo_singlelink_list.hpp"
#include "voType.h"
#include "voDSType.h"
#include "voProgramInfo.h"
#include "voCMutex.h"
#include "voLog.h"

 #ifdef _VONAMESPACE
 namespace _VONAMESPACE{
 #endif

template< class MEM >
_SAMPLE * create_source2_sample( const _SAMPLE * ptr_sample , MEM& alloc )
{
	_SAMPLE * ptr_obj = (_SAMPLE*)alloc.allocate( sizeof(_SAMPLE) );
	if (NULL == ptr_obj) {
		VOLOGE("!can not get memory of _SAMPLE");
		return NULL;
	}

	memset( ptr_obj , 0 , sizeof(_SAMPLE) );

	memcpy( ptr_obj, ptr_sample, sizeof(_SAMPLE) );

	if( ptr_sample->uSize )
	{
		ptr_obj->pBuffer = alloc.allocate( ptr_sample->uSize );
		if (NULL == ptr_obj->pBuffer) {
			VOLOGE("!can not get memory of %d", ptr_sample->uSize);
			return NULL;
		}
		
		memcpy( ptr_obj->pBuffer , ptr_sample->pBuffer , ptr_sample->uSize );
	}
	else
	{
		ptr_obj->pBuffer = NULL;
	}

	ptr_obj->pFlagData = NULL;
	if( (ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) &&
		ptr_sample->pFlagData )
	{
		CopyTrackInfoOP_T( static_cast<_TRACK_INFO*>(ptr_sample->pFlagData), reinterpret_cast<_TRACK_INFO**>(&ptr_obj->pFlagData) );
	}

	return ptr_obj;
}

template< class MEM >
VO_VOID destroy_source2_sample( const _SAMPLE * ptr_sample , MEM& alloc )
{
	if( ptr_sample->pBuffer )
	{
		alloc.deallocate( (VO_PBYTE)ptr_sample->pBuffer );
	}

	if( (ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) &&
		ptr_sample->pFlagData )
	{
		ReleaseTrackInfoOP_T( static_cast<_TRACK_INFO*>(ptr_sample->pFlagData) );
	}

	alloc.deallocate( (VO_PBYTE)ptr_sample );
}


class voSourceDataBuffer
{
public:
	voSourceDataBuffer( vo_allocator< VO_BYTE >& allocator , VO_S32 nMaxBufferTime = 10000 );
	virtual ~voSourceDataBuffer(void);

	virtual VO_S32		AddBuffer (_SAMPLE * pBuffer);
	virtual VO_S32		GetBuffer (_SAMPLE * pBuffer);

	virtual VO_U64		GetBuffTime (void);
	VO_S32				GetBuffCount (void);
	VO_BOOL				WillAvailable() { return m_bWillUsable; }

	VO_VOID				SetBuffSize(VO_S32 iMaxSize) { m_nMaxBufferTime = iMaxSize; }
	VO_S32				GetBuffSize() { return m_nMaxBufferTime; }

	virtual VO_S32		Flush (void);

	_SAMPLE *			PeekTheFirst();
	_SAMPLE *			PeekTheLast();

	VO_S32				RemoveTo( VO_U64 timestamp );
	VO_S32				CutFrom( VO_U64 timestamp );

protected:
	vo_singlelink_list< _SAMPLE * > m_list;
	vo_allocator< VO_BYTE >& m_allocator;

	_SAMPLE *			m_ptr_cache;
	VO_S32				m_nMaxBufferTime;

	VO_BOOL				m_bWillUsable;

	voCMutex			m_lock;
};

 #ifdef _VONAMESPACE
 }
 #endif

#endif