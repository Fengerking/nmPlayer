	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileSource.h

	Contains:	CFileSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CFileSource_H__
#define __CFileSource_H__

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "voFile.h"

class CFileSource
{
public:
	CFileSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CFileSource (void);

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

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	virtual VO_U32		FreeLib (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetCallBack (VO_PTR pCallBack, VO_PTR pUserData);

protected:
	VO_PTR				m_hInst;
	VO_SOURCE_READAPI	m_funFileRead;
	VO_SOURCE_OPENPARAM	m_paramOpen;
	VO_FILE_SOURCE		m_Source;
	VO_PTR				m_hFile;
	VO_BOOL				m_bForceClosed;

	VO_SOURCE_INFO		m_filInfo;
	VO_S32				m_nVideoTrack;
	VO_S32				m_nAudioTrack;

	VO_BOOL	*			m_pFirstSample;

	VO_TCHAR			m_szDllFile[128];
	VO_TCHAR			m_szAPIName[128];
	VO_PTR				m_pAPIEntry;

#ifdef _WIN32
	HMODULE			m_hDll;
#else
	VO_PTR			m_hDll;
#endif // _WIN32
};

#endif // __CFileSource_H__
