	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCDataSource.h

	Contains:	voCDataSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __voCDataSource_H__
#define __voCDataSource_H__

#include "voFile.h"
#include "vompType.h"

#include "CBaseSource.h"
#include "fVideoHeadDataInfo.h"
#include "voCDataBuffer.h"

class voCDataSource : public CBaseSource
{
public:
	voCDataSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	virtual ~voCDataSource (void);

	virtual VO_U32		LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32		CloseSource (void);

	virtual VO_U32		GetSourceInfo (VO_PTR pSourceInfo);
	virtual VO_U32		GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo);

	virtual VO_U32		GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32		GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat);

	virtual VO_U32		GetTrackData (VO_U32 nTrack, VO_PTR pTrackData);
	virtual VO_U32		SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);

	virtual VO_U32		GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		AddTrack (const VO_PTR pSource, int nCodec, bool bAudio);
	virtual VO_U32		AddBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer);

	virtual VO_U64		GetBufferTime (bool bAudio);
	virtual VO_U32		Flush (void);

	virtual void		SetMaxBuffTime (VO_S32 nMaxTime) {m_llMaxBufferTime = nMaxTime;}
	virtual VO_S32		GetMaxBuffTime (void) {return (VO_S32)m_llMaxBufferTime;}
	virtual void		SetMinBuffTime (VO_S32 nMinTime) {m_llMinBufferTime = nMinTime;}
	virtual VO_S32		GetMinBuffTime (void) {return (VO_S32)m_llMinBufferTime;}

protected:
	voCMutex				m_mtBuffer;
	VO_SOURCE_INFO			m_filInfo;
	VO_SOURCE_TRACKINFO		m_sAudioTrackInfo;
	VO_SOURCE_TRACKINFO		m_sVideoTrackInfo;
	VO_VIDEO_HEADDATAINFO	m_nVideoHeaderInfo;
	VO_AUDIO_FORMAT			m_sAudioFormat;
	VO_VIDEO_FORMAT			m_sVideoFormat;
	int						m_nAudioTrack;
	int						m_nVideoTrack;

	int						m_nDropedVideo;

	VO_PBYTE				m_pVideoHeadData;
	VO_S32					m_nVideoHeadSize;
	VO_PBYTE				m_pAudioHeadData;
	VO_S32					m_nAudioHeadSize;

	VO_PBYTE				m_pVideoBufferData;
	VO_S32					m_nVideoBufferSize;
	VO_PBYTE				m_pAudioBufferData;
	VO_S32					m_nAudioBufferSize;

	voCDataBufferList *		m_pAudioTrack;
	voCDataBufferList *		m_pVideoTrack;

	VO_S64					m_llMaxBufferTime;
	VO_S64					m_llMinBufferTime;
	bool						m_bKeepAudioBuffer;
	bool						m_bVideoUpdown;

	VOMP_READBUFFER_FUNC *	m_pReadBuffer;
};

#endif // __voCDataSource_H__
