	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataVideoPort.h

	Contains:	voCOMXDataVideoPort header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXDataVideoPort_H__
#define __voCOMXDataVideoPort_H__

#include "voCOMXDataOutputPort.h"

class voCOMXDataVideoPort : public voCOMXDataOutputPort
{
public:
	voCOMXDataVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXDataVideoPort(void);

	virtual	void			SetConfigFile (CBaseConfig * pCfgFile);

	virtual OMX_ERRORTYPE	GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam);

	virtual OMX_ERRORTYPE	SendBuffer (OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE	SetTrackPos (OMX_S64 * pPos);

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer);
	virtual OMX_ERRORTYPE	ReconfigPort (VO_LIVESRC_TRACK_INFOEX * pInfoEx);

protected:
	bool					m_bDropFrame;
};

#endif //__voCOMXDataVideoPort_H__
