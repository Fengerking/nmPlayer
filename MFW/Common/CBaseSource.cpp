	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "CBaseSource.h"

CBaseSource::CBaseSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: CBaseNode (hInst, pMemOP)
	, m_nFormat (nFormat)
	, m_nDuration (0)
	, m_nCurPos (0)
	, m_nError (0)
	, m_nSourceType (VO_SOURCE_TYPE_NONE)
	, m_pDrmCB (pDrmCB)
	, m_pIFileBasedDrm (NULL)
	, m_LiveSrcType(VO_LIVESRC_FT_MAX)
	, mIsVDrmDataAppended(false)
	, mIsADrmDataAppended(false)
	
{
}

CBaseSource::~CBaseSource ()
{
	CloseSource ();
}

VO_U32 CBaseSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	return 0;
}

VO_U32 CBaseSource::CloseSource (void)
{
	return 0;
}

VO_U32 CBaseSource::Start(void)
{
	return 	SetParam (VO_PID_COMMON_START, 0);
}

VO_U32 CBaseSource::Pause(void)
{
	return 	SetParam (VO_PID_COMMON_PAUSE, 0);
}

VO_U32 CBaseSource::Stop(void)
{
	return 	SetParam (VO_PID_COMMON_STOP, 0);
}

VO_U32 CBaseSource::GetSourceInfo (VO_PTR pSourceInfo)
{
	return 0;
}

VO_U32 CBaseSource::GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo)
{
	return 0;
}

VO_U32 CBaseSource::GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat)
{
	return 0;
}

VO_U32 CBaseSource::GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat)
{
	return 0;
}

VO_U32 CBaseSource::GetFrameType (VO_U32 nTrack)
{
	return 0;
}

VO_U32 CBaseSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	return 0;
}

VO_BOOL CBaseSource::CanSeek (void)
{
	return VO_TRUE;
}

VO_U32 CBaseSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	return 0;
}

VO_U32 CBaseSource::GetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	return 0;
}

VO_U32 CBaseSource::SetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	return 0;
}

VO_U32 CBaseSource::GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	return 0;
}

VO_U32 CBaseSource::SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	return 0;
}

VO_U32 CBaseSource::SetCallBack (VO_PTR pCallBack, VO_PTR pUserData)
{
	return 0;
}

char* CBaseSource::GetMetaDataText (int nIndex)
{
	return 0;
}

bool CBaseSource::isVDrmDataAppended()
{
	return mIsVDrmDataAppended;
}

bool CBaseSource::isADrmDataAppended()
{
	return mIsADrmDataAppended;
}
