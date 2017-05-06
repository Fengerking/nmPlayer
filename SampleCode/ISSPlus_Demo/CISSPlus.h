#ifndef __PUSHMP4_H__
#define __PUSHMP4_H__

#include "voType.h"
#include "voSource.h"
#include "voLiveSourcePlus.h"

#include "DllLoader.h"

typedef signed long (VO_API *GetISSLiveSrcPlusAPI) (VO_LIVESRC_PLUS_API* pHandle);

class CISSPlus: public CDllLoader
{
public:
	CISSPlus(void);
	~CISSPlus(void);

private:
	VO_LIVESRC_PLUS_API m_ReadHandle;
	VO_PTR m_hFile;
	VO_LIVESRC_CALLBACK m_callback;
	VO_U32 m_videoCodec;
	VO_U32 m_audioCodec;

public:
	int PushBuffer(unsigned char *buf, int nSize,int nFlag = 1);
	void SetFileName(char *name);
	void Open();
	void Close();
	int GetFileTracks();
	int GetTrackType(int id);
	
	int HeaderParser(unsigned char *buf, int nSize);
	int ChunkParser(VO_SOURCE_TRACKTYPE trackType, unsigned char *buf, int nSize);
	int GetStreamInfo(int id, VO_LIVESRC_PLUS_STREAM_INFO **streamInfo);
	int GetStreamCount(VO_U32 *nCount);
	int GetDrmInfo(VO_LIVESRC_PLUS_DRM_INFO  **ppDRMInfo);
	int SetCodecType(VO_SOURCE_TRACKTYPE typeType, VO_U32 nCodec);

	static VO_S32 SendEvent (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	static VO_S32 SendData (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);


	FILE  *video_ff ;
	FILE  *audio_ff;

};

#endif