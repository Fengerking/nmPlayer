	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileMediaSource.cpp

	Contains:	CFileMediaSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"

#include "CFileMediaSource.h"

#include "CFileMediaExtractor.h"

#define LOG_TAG "CFileMediaSource"
#include "voLog.h"


CFileMediaSource::CFileMediaSource(CFileMediaExtractor * pExtractor, int nTrack, bool bAudio,
								   VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP)
	: CBaseMediaSource (pMediaBufferOP, pMetaDataOP)
	, m_pExtractor (pExtractor)
	, m_nTrack (nTrack)
	, m_bIsAudio (bAudio)
{

}

CFileMediaSource::~CFileMediaSource(void)
{

}

int CFileMediaSource::Start (void * pParam)
{
	m_pExtractor->Start (m_nTrack, pParam);

	return 0;
}

int CFileMediaSource::Stop (void)
{
	m_pExtractor->Stop (m_nTrack);

	return 0;
}

void * CFileMediaSource::GetFormat (void)
{
	//VOLOGI ("m_nTrack is %d", m_nTrack);

	return m_pExtractor->GetTrackMetaData (m_nTrack, 0);
}

int CFileMediaSource::Read (void ** ppBuffer, void * pOptions)
{
	return m_pExtractor->ReadSample (ppBuffer, pOptions, m_nTrack);
}
