	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileSink.h

	Contains:	CFileSink header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CFileSink_H__
#define __CFileSink_H__

#include "CBaseNode.h"
#include "voSink.h"

class CFileSink : public CBaseNode
{
public:
	CFileSink (VO_PTR hInst, VO_FILE_FORMAT nFileFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CFileSink (void);

	virtual VO_U32		Init (VO_FILE_SOURCE* pSource, VO_U32 nAudioCoding, VO_U32 nVideoCoding);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		AddSample (VO_SINK_SAMPLE * pSample);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_SINK_OPENPARAM	m_paramOpen;
	VO_PTR				m_hSink;
	VO_SINK_WRITEAPI	m_apiSink;
	VO_FILE_FORMAT		m_nFormat;
};

#endif // __CFileSink_H__
