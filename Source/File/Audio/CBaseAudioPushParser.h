#ifndef __CBASEAUDIOPUSHPARSER_H__
#define __CBASEAUDIOPUSHPARSER_H__

#include "voSource2.h"
#include "CMemStream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef struct
{
	VO_U64	ullFramePos;
	VO_U64	ullFrameLen;
}tagPushFrameInfo;


class CBaseAudioPushParser
{
public:
	CBaseAudioPushParser();
	virtual ~CBaseAudioPushParser();

	VO_VOID Reset(){m_hFile.Reset();}

	virtual VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam );
	virtual VO_U32 Uninit();

	virtual VO_U32 Open(){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 Close(){return VO_RET_SOURCE2_OK;}

	virtual VO_U32 Start(){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 Pause(){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 Stop(){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 GetSample( VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample ){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 GetDuration(VO_U64 * pDuration){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 GetProgramCount( VO_U32 *pProgramCount){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 GetProgramInfo( VO_U32 uProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE eTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo ){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo){return VO_RET_SOURCE2_OK;}

	virtual VO_U32 Seek(VO_U64* pTimeStamp){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 SelectProgram( VO_U32 uProgram){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 SelectStream( VO_U32 uStream){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 SelectTrack( VO_U32 uTrack){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& Buffer ){return VO_RET_SOURCE2_OK;}

	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam){return VO_RET_SOURCE2_OK;}
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam){return VO_RET_SOURCE2_OK;}
protected:
	VO_SOURCE2_SAMPLECALLBACK	m_CallBack;
	CMemFileIO					m_hFile;
	VO_PTR						m_hCheck;
};

#ifdef _VONAMESPACE
}
#endif

#endif///<__CBASEAUDIOPUSHPARSER_H__
