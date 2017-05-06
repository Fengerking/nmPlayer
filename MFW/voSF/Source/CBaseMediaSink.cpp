/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseMediaSink.cpp

Contains:	CBaseMediaSink class file

Written by:	

Change History (most recent first):


*******************************************************************************/

#include "CBaseMediaSink.h"
#include "voFile.h"

CBaseMediaSink::CBaseMediaSink(FILE* fd, int format, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, 
							   VOSF_METADATATYPE * pMetaDataOP, VOSF_MEDIASOURCETYPE * pMediaSourceOP)
	: m_fd(fd),
	  m_format(VO_FILE_FFVIDEO_MPEG4),
	  m_pMediaBufferOP(pMediaBufferOP),
	  m_pMetaDataOP(pMetaDataOP),
	  m_pMediaSourceOP(pMediaSourceOP)
{

}

CBaseMediaSink::~CBaseMediaSink()
{

}