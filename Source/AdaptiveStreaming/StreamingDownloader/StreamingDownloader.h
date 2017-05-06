/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		StreamingDownloader.h

	Contains:	CStreamingDownloader class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/

#ifndef _STREAMING_DOWNLOADER_H
#define _STREAMING_DOWNLOADER_H

#include "voSource2.h"
#include "voSource2_IO.h"
#include "voAdaptiveStreamParser.h"
#include "ChunkDownloader.h"
#include "DownloadList.h"
#include "vo_thread.h"
#include "AdaptDataSource.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


typedef enum{

	STATUS_IDLE,
	STATUS_INIT,
	STATUS_OPEN,
	STATUS_START,
	STATUS_MAX
}SDOWNLOADER_STATUS;


class CStreamingDownloader :public CDllLoad, public vo_thread, public AdaptDataSource
{
public:
	CStreamingDownloader();
	~CStreamingDownloader();

	virtual VO_U32 Init(VO_SOURCE2_EVENTCALLBACK* pCallback, VO_SOURCE2_INITPARAM * pParam);
	virtual VO_U32 Uninit();
	
	virtual VO_U32 Open(VO_PTR pSource, VO_U32 uFlag, VO_PTCHAR pLocalDir);
	virtual VO_U32 Close();
	static VO_S32 OnUpdateCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

	virtual VO_U32 StartDownload();
	virtual VO_U32 StopDownload();

//	virtual VO_U32 GetProgramInfo(VO_SOURCE2_PROGRAM_INFO** ppProgramInfo);
	virtual VO_U32 SelectStream(VO_U32 nStreamID);
	virtual VO_U32 SelectTrack(VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType);
	virtual VO_U32 GetDuration(VO_U64* pDuration);
	virtual VO_U32 SetParam( VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 GetParam( VO_U32 nParamID, VO_PTR pParam);

	virtual VO_U32 AddManifestInfo( VO_ADAPTIVESTREAM_PLAYLISTDATA* pPlaylistdata);
	virtual VO_U32 GetManifestInfo( list_T<MANIFEST_INFO*>** pManifestList);
protected:
	virtual void thread_function();
	virtual VO_U32 AsyncOpen();
	virtual VO_U32 CheckStreamingType( VO_ADAPTIVESTREAM_PLAYLISTDATA * pData ,VO_ADAPTIVESTREAMPARSER_STREAMTYPE* pType);
	virtual VO_U32 ReleaseManifestInfo(list_T<MANIFEST_INFO*>*	pInfolist);
	virtual VO_U32 CommetSelection(VO_U32 bSelect);
	virtual VO_VOID Reset();
	virtual VO_U32 IsTypeSupport();
	virtual VO_U32 IsLinkSupport(VO_CHAR* pUrl, VOOSMP_SRC_SOURCE_FORMAT& type);
	virtual VO_U32 SwitchToNextStatus(SDOWNLOADER_STATUS eStatus);
	virtual VO_U32 CheckStatus(SDOWNLOADER_STATUS eStatus);

private:
	CChunkDownloader*				m_pTrunkdownloader;
	CDownloadList*					m_pDownloadlist;

	VO_TCHAR						m_LocalPath[MAXURLLEN];
	list_T<MANIFEST_INFO*> 			m_nManifestList;
	
	VO_SOURCE2_IO_API * 				m_pIO;
	//for http callback, cookie
	VO_SOURCE2_HTTPHEADER * 			m_pIOHttpHeader;
	//for http poxy setting
	VO_SOURCE2_HTTPPROXY * 			m_pIOHttpProxy;
	
	VO_LOG_PRINT_CB * 				m_pVologCB;

	VO_ADAPTIVESTREAM_PLAYLISTDATA	m_sPlaylistData;

	VO_SOURCE2_EVENTCALLBACK		m_sUpdateCallback;
	
	VO_BOOL 						m_bStop;
	VO_BOOL 						m_bManifestFlag;
	VO_TCHAR						m_WorkPath[MAXURLLEN];

	SDOWNLOADER_STATUS				m_eStatus;
	VO_PTR							m_hCheck;

public:
	VO_SOURCE2_EVENTCALLBACK		m_sEventCallBack;
	VO_U64							m_nDuration;
};

#ifdef _VONAMESPACE
}
#endif

#endif


