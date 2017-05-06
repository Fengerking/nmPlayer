#ifndef __VOSOURCEBUFFERMANAGER_AI_H__
#define __VOSOURCEBUFFERMANAGER_AI_H__

#pragma once
#include "voSourceBufferManager.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum VO_BUFFER_PID_TYPE
{
	VO_BUFFER_PID_DURATION_A,	/*!< <G> get the duration of audio in buffer*/
	VO_BUFFER_PID_DURATION_V,	/*!< <G> get the duration of video in buffer*/
	VO_BUFFER_PID_DURATION_T,	/*!< <G> get the duration of subtitle in buffer*/
	VO_BUFFER_PID_MAX_SIZE,		/*!< <S/G> set/get the maximum length of buffer*/
	VO_BUFFER_PID_IS_RUN		/*!< <G> check whether the buffer will output data*/
};

class voSourceBufferManager_AI :
	public voSourceBufferManager
{
public:
	voSourceBufferManager_AI(VO_S32 nBufferingTime = 5000, VO_S32 nMaxBufferTime = 20000, VO_S32 nStartBufferTime = 2000);
	~voSourceBufferManager_AI(void);

	virtual VO_U32 AddBuffer(VO_U32 uType, VO_PTR ptr_obj);
	virtual VO_U32 GetBuffer(VO_U32 uType, VO_PTR ptr_obj);
	
	virtual void Flush();

private:
	VO_BOOL	m_bBA_Audio;
	VO_BOOL	m_bBA_Video;

	VO_BOOL	m_bPureAudio;
	VO_U64	m_lastaudiotimestamp;
};

#ifdef _VONAMESPACE
}
#endif

#endif