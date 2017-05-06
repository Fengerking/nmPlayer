#ifndef __VOSOURCE2PARSERWRAPPER_H__
#define __VOSOURCE2PARSERWRAPPER_H__

#include "voYYDef_filcmn.h"
#include "voSourceBaseWrapper.h"
#include "voDRM2.h"
#include "voSource.h"
#include "CSourceIOSwitch.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class voSource2ParserWrapper : public voSourceBaseWrapper 
{
public:
	voSource2ParserWrapper();
	virtual ~voSource2ParserWrapper();

	VO_U32		Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam );

	VO_U32		Open();
	VO_U32		Seek(VO_U64* pTimeStamp);
	VO_U32		SelectTrack( VO_U32 nTrack);
	VO_U32		GetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32		SetParam(VO_U32 nParamID, VO_PTR pParam);

protected:
	VO_U32		OnOpen();
	VO_U32		OnClose();
	virtual VO_U32 OnGetDuration(VO_U64 * pDuration);
	virtual VO_U32 OnGetFileLength(VO_U64 * pLength);
	VO_U32		OnGetTrackCnt(VO_U32 * pTraCnt);
	VO_U32		OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE_SAMPLE * pSample);
	VO_U32		OnGetTrackAvailable(VO_U32 uTrackIndex , VO_BOOL * beAlai);
	VO_U32		OnGetTrackInfo(VO_U32 uTrackIndex , VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32		OnGetTrackFourCC(VO_U32 uTrackIndex , VO_U32 * pFourCC);
	VO_U32		OnGetTrackMaxSampleSize(VO_U32 uTrackIndex , VO_U32 * pMaxSampleSize);
	VO_U32		OnGetAudioFormat(VO_U32 uTrackIndex , VO_AUDIO_FORMAT * pAudioFormat);
	VO_U32		OnGetVideoFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat);
//	VO_U32		OnGetSubtitleFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat);	
	VO_U32		OnGetWaveFormatEx(VO_U32 uTrackIndex , VO_PBYTE * pExData);
	VO_U32		OnGetVideoBitMapInfoHead(VO_U32 uTrackIndex , VO_PBYTE * pBitMap);
	VO_U32		OnGetVideoUpSideDown(VO_U32 uTrackIndex , VO_BOOL * pUpSideDown);
	VO_U32		OnGetLanguage(VO_U32 uTrackIndex , VO_CHAR ** pLanguage);
	VO_U32 		GetBitrate();

protected:
	virtual VO_U32	SetTrackPos( VO_U32 nTrack , VO_U64 * pPos );

//	voSourceSwitchInfo			m_SwithInfo;
	voSourceSwitch				m_SrcIO;

	VO_U64						m_ullActualFileSize;	//for Push Play
	VO_PTR						m_Handle;

private:
	VO_FILE_OPERATOR			m_FileOpr;
};

#ifdef _VONAMESPACE
}
#endif

#endif
