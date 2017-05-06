/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseMediaSink.h

Contains:	CBaseMediaSink class file

Written by:	

Change History (most recent first):


*******************************************************************************/

#ifndef _CBASEMEDIASINK_H_
#define _CBASEMEDIASINK_H_

#include "voSF.h"
#include <stdio.h>

class CBaseMediaSink
{
public:
	CBaseMediaSink(FILE* fd, int format, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, 
					VOSF_METADATATYPE * pMetaDataOP, VOSF_MEDIASOURCETYPE * pMediaSourceOP);
	virtual ~CBaseMediaSink();

	virtual int	AddSource(void* pSource)= 0;
	virtual int Start(void *pMetaData = 0) = 0;
	virtual int Stop() = 0;
	virtual int Pause() = 0;
	virtual bool ReachEOS() = 0;

protected:
	FILE*					m_fd;
	int						m_format;
	VOSF_MEDIABUFFERTYPE *	m_pMediaBufferOP;
	VOSF_METADATATYPE *		m_pMetaDataOP;
	VOSF_MEDIASOURCETYPE*	m_pMediaSourceOP;

};
#endif