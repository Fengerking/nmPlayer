#pragma once

#include "voSourceDataBuffer.h"
#include "voSourceVideoDataBuffer.h"
#include "voSourceSubtitleDataBuffer.h"
#include "vo_allocator.hpp"
#include "CDllLoad.h"
#include "voCaptionParser.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voSourceBufferManager
{
public:
	voSourceBufferManager( VO_S32 nBufferingTime = 5000 , VO_S32 nMaxBufferTime = 10000 , VO_S32 nStartBufferTime = 2000 );
	virtual ~voSourceBufferManager();

	VO_U32 AddBuffer( VO_U32 buffertype , VO_PTR ptr_obj );
	VO_U32 GetBuffer( VO_U32 buffertype , VO_PTR ptr_obj );

	void SetLibOp( VO_SOURCE2_LIB_FUNC * pLibOp );

	void Flush();

	void set_pos( VO_U64 pos );

protected:
	VO_U32 add_audio( VO_SOURCE2_SAMPLE * ptr_sample );
	VO_U32 add_video( VO_SOURCE2_SAMPLE * ptr_sample );
	VO_U32 add_subtitle( voSubtitleInfo * ptr_sample );

	VO_U32 get_audio( VO_SOURCE2_SAMPLE * ptr_sample );
	VO_U32 get_video( VO_SOURCE2_SAMPLE * ptr_sample );
	VO_U32 get_subtitle( voSubtitleInfo * ptr_sample );

	VO_U32 addbuffer_duringseek( VO_U32 buffertype , VO_PTR ptr_obj );

	VO_VOID load_cc();

protected:

	VO_S32 m_nMaxBufferTime;
	VO_S32 m_BufferingTime;
	VO_S32 m_StartBufferTime;

	vo_allocator< VO_BYTE > m_alloc;

	voSourceDataBuffer m_audio;
	voSourceVideoDataBuffer m_video;
	voSourceSubtitleDataBuffer m_subtitle;

	CDllLoad m_loader;
	VO_BOOL m_isccloaded;

	VO_CAPTION_PARSER_API m_ccapi;
	VO_PTR m_hcc;

	VO_BOOL m_isbuffering;
	VO_BOOL m_isstartbuffering;

	VO_BOOL m_has_eos;

	voCMutex m_lock;

	VO_BOOL m_ispureaudio;
	VO_U64 m_videorecovertimestamp;
	VO_U64 m_lastaudiotimestamp;

	VO_BOOL m_isseek;
	VO_U64	m_seekpos;
};
    
#ifdef _VONAMESPACE
}
#endif