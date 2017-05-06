#pragma once
#include "vo_singlelink_list.hpp"
#include "voType.h"
#include "voSource2.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

template< class MEM >
VO_SOURCE2_SAMPLE * create_source2_sample( const VO_SOURCE2_SAMPLE * ptr_sample , MEM& alloc )
{
	VO_SOURCE2_SAMPLE * ptr_obj = ( VO_SOURCE2_SAMPLE * )alloc.allocate( sizeof(VO_SOURCE2_SAMPLE) );

	memcpy( ptr_obj , ptr_sample , sizeof( VO_SOURCE2_SAMPLE ) );

	if( ptr_sample->uSize )
	{
		ptr_obj->pBuffer = alloc.allocate( ptr_sample->uSize );
		memcpy( ptr_obj->pBuffer , ptr_sample->pBuffer , ptr_sample->uSize );
	}

	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT )
	{
		if( ptr_sample->pFlagData )
		{
			VO_SOURCE2_TRACK_INFO * ptr_info = ( VO_SOURCE2_TRACK_INFO * )ptr_sample->pFlagData;
			VO_SOURCE2_TRACK_INFO * ptr_temp = ( VO_SOURCE2_TRACK_INFO * )alloc.allocate( sizeof(VO_SOURCE2_TRACK_INFO) );

			ptr_obj->pFlagData = (VO_PBYTE)ptr_temp;
			memcpy( ptr_temp , ptr_info , sizeof( VO_SOURCE2_TRACK_INFO ) );

			ptr_temp->pHeadData = 0;

			if( ptr_info->uHeadSize )
			{
				ptr_temp->pHeadData = alloc.allocate( ptr_info->uHeadSize );
				memcpy( ptr_temp->pHeadData , ptr_info->pHeadData , ptr_info->uHeadSize );
			}
		}
	}

	return ptr_obj;
}

template< class MEM >
VO_VOID destroy_source2_sample( const VO_SOURCE2_SAMPLE * ptr_sample , MEM& alloc )
{
	if( ptr_sample->pBuffer )
	{
		alloc.deallocate( (VO_PBYTE)ptr_sample->pBuffer );
	}

	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT )
	{
		if( ptr_sample->pFlagData )
		{
			VO_SOURCE2_TRACK_INFO * ptr_info = ( VO_SOURCE2_TRACK_INFO * )ptr_sample->pFlagData;

			if( ptr_info->pHeadData )
				alloc.deallocate( (VO_PBYTE)ptr_info->pHeadData );

			alloc.deallocate( (VO_PBYTE)ptr_info );
		}
	}

	alloc.deallocate( (VO_PBYTE)ptr_sample );
}

class voSourceDataBuffer
{
public:
	voSourceDataBuffer( vo_allocator< VO_BYTE >& allocator , VO_S32 nMaxBufferTime = 10000 );
	virtual ~voSourceDataBuffer(void);

	virtual VO_S32		AddBuffer (VO_SOURCE2_SAMPLE * pBuffer);
	virtual VO_S32		GetBuffer (VO_SOURCE2_SAMPLE * pBuffer);

	virtual VO_U64		GetBuffTime (void);
	virtual VO_S32		GetBuffCount (void);

	virtual VO_S32		Flush (void);

	virtual VO_S32		GetFirstBufferTimeStamp( VO_U64 * ptr_timestamp );

	virtual VO_S32		RemoveTo( VO_U64 timestamp );

protected:
	vo_singlelink_list< VO_SOURCE2_SAMPLE * > m_list;
	vo_allocator< VO_BYTE >& m_allocator;

	VO_SOURCE2_SAMPLE * m_ptr_cache;
	VO_S32				m_nMaxBufferTime;

	voCMutex			m_lock;
};
    
#ifdef _VONAMESPACE
}
#endif
