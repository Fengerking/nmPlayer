
#ifndef __VOSOURCE2PDWRAPPER_H__

#define __VOSOURCE2PDWRAPPER_H__

#include "voSourceBaseWrapper.h"
#include "vo_PD_manager.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class voSource2PDWrapper
	: public voSourceBaseWrapper
{
public:
	voSource2PDWrapper();
	virtual ~voSource2PDWrapper();

	virtual VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam );

	virtual VO_U32 Start();
	virtual VO_U32 Pause();
	virtual VO_U32 Stop();

	virtual VO_U32 Seek(VO_U64* pTimeStamp);

	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

protected:
	VO_U32		OnOpen();
	VO_U32		OnClose();
	VO_U32		OnGetDuration(VO_U64 * pDuration);
	VO_U32		OnGetTrackCnt(VO_U32 * pTraCnt);
	VO_U32		OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE_SAMPLE * pSample);
	VO_U32		OnGetTrackAvailable(VO_U32 uTrackIndex , VO_BOOL * beAlai);
	VO_U32		OnGetTrackInfo(VO_U32 uTrackIndex , VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32		OnGetTrackFourCC(VO_U32 uTrackIndex , VO_U32 * pFourCC);
	VO_U32		OnGetTrackMaxSampleSize(VO_U32 uTrackIndex , VO_U32 * pMaxSampleSize);
	VO_U32		OnGetAudioFormat(VO_U32 uTrackIndex , VO_AUDIO_FORMAT * pAudioFormat);
	VO_U32		OnGetVideoFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat);
	VO_U32		OnGetWaveFormatEx(VO_U32 uTrackIndex , VO_PBYTE * pExData);
	VO_U32		OnGetVideoBitMapInfoHead(VO_U32 uTrackIndex , VO_PBYTE * pBitMap);
	VO_U32		OnGetVideoUpSideDown(VO_U32 uTrackIndex , VO_BOOL * pUpSideDown);
	VO_U32		OnGetLanguage(VO_U32 uTrackIndex , VO_CHAR ** pLanguage);

private:
	VO_VOID		NotifyPDOpenEvent( VO_U32 nRet );
private:
	VO_PTR m_handle;
	VO_PTR m_hCheck;
	VOPDInitParam m_PD_param;

	//work around to store value 
	VO_S32 m_nBufferTime;
};

#ifdef _VONAMESPACE
}
#endif


#endif
