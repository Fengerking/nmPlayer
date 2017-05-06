	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseMediaExtractor.cpp

	Contains:	CBaseMediaExtractor class file

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

#include "CBaseMediaExtractor.h"

#define LOG_TAG "CBaseMediaExtractor"
#include "voLog.h"

CBaseMediaExtractor::CBaseMediaExtractor(VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP)
	: m_pMediaBufferOP (pMediaBufferOP)
	, m_pMetaDataOP (pMetaDataOP)
	, mMode(PlayMode)
{

}

CBaseMediaExtractor::~CBaseMediaExtractor(void)
{

}

int CBaseMediaExtractor::GetTrackNum (void)
{
	return 0;
}

void * CBaseMediaExtractor::GetTrackSource (int nTrack)
{
	return NULL;
}

void * CBaseMediaExtractor::GetTrackMetaData (int nTrack, int nFlag)
{
	return NULL;
}

void * CBaseMediaExtractor::GetMetaData (void)
{
	return NULL;
}

int CBaseMediaExtractor::GetFlag (void)
{
	return NULL;
}

int CBaseMediaExtractor::Start (void * pParam)
{
	return 0;
}

int CBaseMediaExtractor::Stop (void)
{
	return 0;
}

int CBaseMediaExtractor::GetParam (int nParamID, void * pValue1, void * pValue2)
{
	return VOSF_ERR_WRONG_PARAM_ID;
}

int CBaseMediaExtractor::SetParam (int nParamID, void * pValue1, void * pValue2)
{
	return VOSF_ERR_WRONG_PARAM_ID;
}

void CBaseMediaExtractor::setMode(VOSF_MODE inMode)
{
	mMode = inMode;
}
