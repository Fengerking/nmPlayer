#ifndef __VOSOURCEBUFFERMANAGER_AI_H__
#define __VOSOURCEBUFFERMANAGER_AI_H__

#include "MediaBuffer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

	#define VO_BUFFER_SETTINGS 0x80000000

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


class voSourceBufferManager_AI
{
public:
	voSourceBufferManager_AI(VO_S32 nBufferingTime = 5000, VO_S32 nMaxBufferTime = 20000, VO_S32 nStartBufferTime = 4000);
	~voSourceBufferManager_AI(void);

	VO_U32 AddBuffer(VO_U32 uType, VO_PTR ptr_obj);
	VO_U32 AddBuffers(VO_U32 uType, VO_PTR ptr_obj);
	VO_U32 GetBuffer(VO_U32 uType, VO_PTR ptr_obj) { return m_MediaBuffer.GetSample( static_cast<_SAMPLE*>(ptr_obj), static_cast<VO_SOURCE2_TRACK_TYPE>(uType) ); }

	void set_pos( VO_U64 pos ) { m_MediaBuffer.SetPos(&pos); }
	
	void Flush()
	{
		m_ullTimeLastAudio = 0;
		m_MediaBuffer.Flush();
	}

	VO_U32 SetParameter(VO_U32 uID, VO_PTR pParam) { return m_MediaBuffer.SetParameter(uID, pParam); }
	VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam) { return m_MediaBuffer.GetParameter(uID, pParam); }

private:
	MediaBuffer m_MediaBuffer;
	VO_U64		m_ullTimeLastAudio;
};

#ifdef _VONAMESPACE
}
#endif

#endif