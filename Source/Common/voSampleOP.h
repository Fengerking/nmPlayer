
#ifndef __VO_SAMPLEOP_H__
#define __VO_SAMPLEOP_H__

#include "voYYDef_SrcCmn.h"
#include "voSource2.h"
#include "voDSType.h"
#include "vo_singlelink_list.hpp"
#include "voProgramInfo.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif




template< class MEM >
_SAMPLE * CreateSampleOP_T( const _SAMPLE * ptr_sample , MEM& alloc )
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
VO_VOID DestroySampleOP_T( const _SAMPLE * ptr_sample , MEM& alloc )
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

// End define
#ifdef _VONAMESPACE
}
#else
#endif /* _VONAMESPACE */

#endif
