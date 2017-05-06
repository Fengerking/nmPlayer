	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIComp.h

	Contains:	CTIComp header file

	Written by:	David1
	Time: 2011/11/29

*******************************************************************************/
#ifndef __CTIComp_H__
#define __CTIComp_H__

#include <OMX_Component.h>
#include "CBaseComp.h"

class CTIComp : public CBaseComp
{
public:
  CTIComp(VO_CODEC_INIT_USERDATA * pUserData);
  virtual ~CTIComp(void);

	OMX_ERRORTYPE		SetCoreFile (OMX_STRING pCoreFile);
	OMX_ERRORTYPE		SetCompName (OMX_STRING pCompName);
	OMX_ERRORTYPE		ResetPort (int nIndex);

	OMX_ERRORTYPE		EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);
	OMX_ERRORTYPE		FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE		Flush (void);

	VO_U32				SetVideoInputData(VO_CODECBUFFER * pInput);
	VO_U32				GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);


friend class CTIPort;
protected:
	OMX_ERRORTYPE				CreateComponent(void);
	OMX_ERRORTYPE				CreatePorts (void);

protected: 
	VOMP_LISTENERINFO*			m_pListener;
};


#endif //__CTIComp_H__

