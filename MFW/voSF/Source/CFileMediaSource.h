	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileMediaSource.h

	Contains:	CFileMediaSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __CFileMediaSource_H__
#define __CFileMediaSource_H__

#include "CBaseMediaSource.h"


class CFileMediaExtractor;

class CFileMediaSource : CBaseMediaSource
{
public:
	CFileMediaSource(CFileMediaExtractor * pExtractor, int nTrack, bool bAudio,
						VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP);
	virtual ~CFileMediaSource(void);

	virtual int			Start (void * pParam);
	virtual int			Stop (void);
	virtual void *		GetFormat (void);
	virtual int			Read (void ** ppBuffer, void * pOptions);

protected:
	CFileMediaExtractor *	m_pExtractor;
	int						m_nTrack;
	bool					m_bIsAudio;
};

#endif // __CFileMediaSource_H__