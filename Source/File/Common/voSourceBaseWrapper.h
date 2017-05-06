
#ifndef __VO_SOURCE_BASE_WRAPPER_H__
#define __VO_SOURCE_BASE_WRAPPER_H__

#include "voYYDef_filcmn.h"
#include "voSource2.h"
#include "voString.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


const VO_U32 AAC_SampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};


class voSourceBaseWrapper
{
public:
	voSourceBaseWrapper();
	virtual ~voSourceBaseWrapper();

	virtual VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam );
	virtual VO_U32 Uninit();

	virtual VO_U32 Open();
	virtual VO_U32 Close();

	virtual VO_U32 Start();
	virtual VO_U32 Pause();
	virtual VO_U32 Stop();

	virtual VO_U32 GetSample( VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample );
	virtual VO_U32 GetDuration(VO_U64 * pDuration);
	virtual VO_U32 GetProgramCount( VO_U32 *pProgramCount);
	virtual VO_U32 GetProgramInfo( VO_U32 uProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo);
	virtual VO_U32 GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE eTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );
	virtual VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo);

	virtual VO_U32 Seek(VO_U64* pTimeStamp);
	virtual VO_U32 SelectProgram( VO_U32 uProgram);
	virtual VO_U32 SelectStream( VO_U32 uStream);
	virtual VO_U32 SelectTrack( VO_U32 uTrack);
	virtual VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& Buffer );

	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

protected:
	virtual VO_U32		OnOpen(){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnClose(){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetDuration(VO_U64 * pDuration){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetFileLength(VO_U64 * pLength){return VO_RET_SOURCE2_OK;}	
	virtual VO_U32		OnGetTrackCnt(VO_U32 * pTraCnt){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE_SAMPLE * pSample){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetTrackAvailable(VO_U32 uTrackIndex , VO_BOOL * beAlai){*beAlai = VO_TRUE;return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetTrackInfo(VO_U32 uTrackIndex , VO_SOURCE_TRACKINFO * pTrackInfo){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetTrackFourCC(VO_U32 uTrackIndex , VO_U32 * pFourCC){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetTrackMaxSampleSize(VO_U32 uTrackIndex , VO_U32 * pMaxSampleSize){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetAudioFormat(VO_U32 uTrackIndex , VO_AUDIO_FORMAT * pAudioFormat){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetVideoFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat){return VO_RET_SOURCE2_OK;}
//	virtual VO_U32		OnGetSubtitleFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pSubtitleFormat){return VO_RET_SOURCE2_OK;}	
	virtual VO_U32		OnGetWaveFormatEx(VO_U32 uTrackIndex , VO_PBYTE * pExData){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetVideoBitMapInfoHead(VO_U32 uTrackIndex , VO_PBYTE * pBitMap){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetVideoUpSideDown(VO_U32 uTrackIndex , VO_BOOL * pUpSideDown){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		OnGetLanguage(VO_U32 uTrackIndex , VO_CHAR ** pLanguage){return VO_RET_SOURCE2_OK;}
	virtual VO_U32		SetCurrentTimeStamp(VO_U64 Timestamp, VO_SOURCE2_TRACK_TYPE nOutPutType);
	virtual VO_U32		GetCurrentTimeStamp(VO_U64* pTimestamp);

protected:
	virtual VO_VOID		CreateStreamInfo( VO_U32 nTracks);
	VO_U32 NotifyEvent( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 );	
	VO_VOID PrintTrackInfo(VO_SOURCE2_TRACK_INFO *pTrackInfo);
	VO_VOID PrintStreamInfo(VO_SOURCE2_STREAM_INFO *pStreamInfo);
	VO_VOID PrintProgramInfo(VO_SOURCE2_PROGRAM_INFO *pProgramInfo);
	virtual VO_BOOL		ConvertData (VO_PBYTE pData, VO_U32 nSize);
	virtual VO_BOOL		ConvertHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize);
	virtual VO_U32 		GetBitrate(){return 0;}
private:
	virtual VO_BOOL     HEVC_ConvertHeadData(VO_PBYTE pHeadData, VO_U32 nHeadSize);

	virtual VO_S32		Config2ADTSHeader(VO_PBYTE pConfig, VO_U32 uConlen,
											VO_U32  uFramelen,VO_PBYTE pAdtsBuf,VO_U32 *pAdtsLen);
	VO_BOOL IsADTSBuffer(VO_PBYTE pAdtsBuf, VO_U32 buffersize);

protected:
	VO_U32						m_nFlag;
	VO_U32						m_nOpenFlag;
	VO_SOURCEDRM_CALLBACK2 *	m_ptr_drmcallback;
	VO_SOURCE2_EVENTCALLBACK *	m_ptr_EventCallback;	
	VO_TCHAR					m_Url[2048];
	VO_PTR						m_FileHandle;
	VO_U64						m_ullDuration;
	VO_U32						m_uAudioTrack;
	VO_U32						m_uVideoTrack;
	VO_U32						m_uSubtitleTrack;
	
	VO_SOURCE2_TRACK_INFO *		m_pAudioTrackInfo;
	VO_SOURCE2_TRACK_INFO *		m_pVideoTrackInfo;
	VO_SOURCE2_TRACK_INFO *		m_pSubtitleTrackInfo;

	VO_SOURCE2_PROGRAM_INFO *	m_pProgramInfo;
	VO_SOURCE2_STREAM_INFO *	m_pStreamInfo;

	VO_PBYTE					m_pHeadData;
	VO_U32						m_uHeadSize;
	VO_U32						m_uNalLen;
	VO_U32						m_uNalWord;
	VO_PBYTE					m_pVideoData;
	VO_U32						m_uVideoSize;
	VO_U32						m_uFrameSize;
	VO_PBYTE					m_pAudioData;
	VO_U32						m_nAudioSize;
	VO_PBYTE					m_pSubtitleData;
	VO_U32						m_nSubtitleSize;

	VO_BOOL						m_bAudioHeadDataSend;
	VO_BOOL						m_bVideoHeadDataSend;
	VO_BOOL						m_bSubtitleHeadDataSend;

	VO_U64						m_nCurrentTimeStamp;
	VO_LIB_OPERATOR*			m_pLibOp;
	VO_TCHAR *					m_pstrWorkPath;

	VO_U64						m_pVideoTimeStamp;
	VO_U64						m_pAudioTimeStamp;
	VO_U64						m_pSubtitleTimeStamp;
	
};

#ifdef _VONAMESPACE
}
#endif
/**/

#endif ///<__VO_SOURCE_BASE_WRAPPER_H__
