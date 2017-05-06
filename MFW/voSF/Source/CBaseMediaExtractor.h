	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseMediaExtractor.h

	Contains:	CBaseMediaExtractor header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __CBaseMediaExtractor_H__
#define __CBaseMediaExtractor_H__

#include "voSF.h"

class CBaseMediaExtractor
{
public:
	CBaseMediaExtractor(VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP);
	virtual ~CBaseMediaExtractor(void);

	virtual int			GetTrackNum (void);
	virtual void *		GetTrackSource (int nTrack);
	virtual void *		GetTrackMetaData (int nTrack, int nFlag);
	virtual void *		GetMetaData (void);
	virtual int			GetFlag (void);

	virtual int			Start (void * pParam);
	virtual int			Stop (void);

	virtual	int			GetParam (int nParamID, void * pValue1, void * pValue2);
	virtual	int			SetParam (int nParamID, void * pValue1, void * pValue2);
	void                setMode(VOSF_MODE inMode);
protected:
	VOSF_MEDIABUFFERTYPE *	m_pMediaBufferOP;
	VOSF_METADATATYPE *		m_pMetaDataOP;
	VOSF_MODE               mMode;	
};

#endif // __CBaseMediaExtractor_H__
