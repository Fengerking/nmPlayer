	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseMediaSource.cpp

	Contains:	CBaseMediaSource class file

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

#include "CBaseMediaSource.h"

#define LOG_TAG "CBaseMediaSource"
#include "voLog.h"

CBaseMediaSource::CBaseMediaSource(VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP)
	: m_pMediaBufferOP (pMediaBufferOP)
	, m_pMetaDataOP (pMetaDataOP)
	, mpMemVideoOP(NULL)
{

}

CBaseMediaSource::~CBaseMediaSource(void)
{

}

int CBaseMediaSource::Start (void * pParam)
{
	//VOLOGI ();

	return 0;
}

int CBaseMediaSource::Stop (void)
{
	return 0;
}

void * CBaseMediaSource::GetFormat (void)
{
	return NULL;
}

int CBaseMediaSource::Read (void ** ppBuffer, void * pOptions)
{
	*ppBuffer = NULL;

	return VOSF_ERR_NOT_IMPLEMENT;
}


int CBaseMediaSource::GetParam (int nParamID, void * pValue1, void * pValue2)
{
	return VOSF_ERR_WRONG_PARAM_ID;
}

int CBaseMediaSource::SetParam (int nParamID, void * pValue1, void * pValue2)
{
	if(nParamID == VOSF_PID_MemVideoOperater)
	{
		mpMemVideoOP = (VO_MEM_VIDEO_OPERATOR *)pValue1;
		return VOSF_ERR_NONE;
	}

	return VOSF_ERR_WRONG_PARAM_ID;
}
