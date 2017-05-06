/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CLiveSource.cpp

Contains:	CLiveSource class file

Written by:	

Change History (most recent first):


*******************************************************************************/
#ifndef __CLIVESOURCE_H__
#define __CLIVESOURCE_H__

#include "CBaseSource.h"
#include "CBaseLiveSource.h"
#include "voLiveSource.h"
#include "CDataBuffer.h"
#include "vompType.h"
#include "voFile.h"

class CLiveSource : public CBaseSource
{
public:
	CLiveSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	virtual ~CLiveSource (void);

	virtual VO_U32		LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32		CloseSource (void);

	virtual VO_U32		GetSourceInfo (VO_PTR pSourceInfo);
	virtual VO_U32		GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo);

	virtual VO_U32		GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat);

	virtual VO_U32		GetFrameType (VO_U32 nTrack);

	virtual VO_U32		GetTrackData (VO_U32 nTrack, VO_PTR pTrackData);

	virtual VO_U32		SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);

	virtual VO_U32		GetSourceParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32		SetSourceParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);
	virtual VO_U32		SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);

	virtual VO_VOID		NotifyStatus ( VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2 );
	virtual VO_VOID		SendData ( VO_U16 nOutputType, VO_PTR pData );

protected:
	virtual VO_LIVESRC_FORMATTYPE GetLiveType( VO_U32 nType );
	virtual VO_U32		AddBuffer (bool bVideo, VOMP_BUFFERTYPE * pBuffer);

private:
	VO_CHAR					m_URL[1024];
	CBaseLiveSource*		m_pLiveSource;

	voCMutex				m_mtBuffer;
	VO_SOURCE_INFO			m_filInfo;
	VO_SOURCE_TRACKINFO		m_sAudioTrackInfo;
	VO_SOURCE_TRACKINFO		m_sVideoTrackInfo;
	int						m_nAudioTrack;
	int						m_nVideoTrack;

	VO_PBYTE				m_pVideoHeadData;
	VO_S32					m_nVideoHeadSize;
	VO_PBYTE				m_pAudioHeadData;
	VO_S32					m_nAudioHeadSize;

	voCDataBufferList *		m_pAudioTrack;
	voCDataBufferList *		m_pVideoTrack;

	long long				m_llMaxBufferTime;
	long long				m_llMinBufferTime;
	bool					m_bKeepAudioBuffer;
	VO_BOOL					m_bStop;

};

#endif
