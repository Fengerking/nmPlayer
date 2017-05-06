#ifndef __VOSOURCEBUFFERMANAGER_H__
#define __VOSOURCEBUFFERMANAGER_H__

#pragma once

#include "voSourceDataBuffer.h"
#include "voSourceVideoDataBuffer.h"
#include "voSourceSubtitleDataBuffer.h"
#include "vo_allocator.hpp"
#include "voCMutex.h"


#define VO_BUFFER_SETTINGS 0x80000000

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum VO_BUFFER_SETTINGS_TYPE
{
	VO_BUFFER_SETTING_STARTBUFFERTIME,
	VO_BUFFER_SETTING_BUFFERTIME,
	VO_BUFFER_SETTING_BUFFERING_STYLE,	//VO_BUFFERING_STYLE_TYPE
};

struct VO_BUFFER_SETTING_SAMPLE
{
	VO_BUFFER_SETTINGS_TYPE nType;
	VO_PTR pObj;
};

enum VO_BUFFERING_STYLE_TYPE
{
	VO_BUFFERING_AV,
	VO_BUFFERING_V,
	VO_BUFFERING_A
};

class voSourceBufferManager
{
public:
	voSourceBufferManager( VO_S32 nBufferingTime = 5000 , VO_S32 nMaxBufferTime = 10000 , VO_S32 nStartBufferTime = 2000 );
	virtual ~voSourceBufferManager();
	
	virtual VO_U32 AddBuffer(VO_U32 buffertype, VO_PTR ptr_obj);
	virtual VO_U32 GetBuffer(VO_U32 buffertype, VO_PTR ptr_obj);

	virtual VO_U32 SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam);
	
	virtual void Flush();

	void set_pos( VO_U64 pos );

protected:
	voSourceDataBuffer			m_audio;
	voSourceVideoDataBuffer		m_video;
	voSourceSubtitleDataBuffer	m_subtitle;

private:
	VO_BOOL CanGetData(VO_U32 uType);

	VO_U32 add_audio(_SAMPLE * ptr_sample) { return m_audio.AddBuffer(ptr_sample); }
	VO_U32 add_video(_SAMPLE * ptr_sample) { return m_video.AddBuffer(ptr_sample); }
	VO_U32 add_subtitle(_SAMPLE * ptr_sample) { return m_subtitle.AddBuffer(ptr_sample); }

	VO_U32 get_audio(_SAMPLE * ptr_sample) { return m_audio.GetBuffer(ptr_sample); }
	VO_U32 get_video(_SAMPLE * ptr_sample) { return m_video.GetBuffer(ptr_sample); }
	VO_U32 get_subtitle(_SAMPLE * ptr_sample) { return m_subtitle.GetBuffer(ptr_sample); }

	enum STATE_Manager {
		Manager_Starting,
		Manager_Seeking,
		Manager_Running,
		Manager_Caching
	};

	STATE_Manager m_eStateManager;

	VO_S32 m_nMaxBufferTime;
	VO_S32 m_BufferingTime;
	VO_S32 m_StartBufferTime;

	vo_allocator< VO_BYTE > m_alloc;

	VO_BUFFERING_STYLE_TYPE m_eBuffering_Style;

	VO_U64	m_ullWantedSeekPos;
	VO_U64	m_ullActualSeekPos;

	voCMutex m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif