	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CExtVOMEPlayer.h

	Contains:	CExtVOMEPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/

#ifndef __CExtVOMEPlayer_H__
#define __CExtVOMEPlayer_H__

#include "CVOMEPlayer.h"

#include "vo_http_live_streaming.h"


class CExtVOMEPlayer : public CVOMEPlayer
{
public:
	CExtVOMEPlayer (void);
	virtual ~CExtVOMEPlayer (void);

	virtual int			BuildGraph (void);
	virtual int			Run (void);
	virtual int			Pause (void);
	virtual int			Stop (void);

	virtual int			SetPos (int	nPos);
	virtual int			GetPos (int * pPos);
	virtual int			GetDuration (int * pPos);

	virtual int			Reset (void);
	virtual int			Uninit (void);

	OMX_ERRORTYPE 		SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack);
	OMX_ERRORTYPE 		EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex);
	OMX_ERRORTYPE 		GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles);
	OMX_ERRORTYPE 		GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames);
	OMX_ERRORTYPE 		AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	OMX_ERRORTYPE 		RemoveComponent (OMX_COMPONENTTYPE * pComponent);
	OMX_ERRORTYPE 		ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel);


protected:
	OMX_COMPONENTTYPE *		m_pCompSrc;
	OMX_COMPONENTTYPE *		m_pCompAD;
	OMX_COMPONENTTYPE *		m_pCompVD;
	OMX_COMPONENTTYPE *		m_pCompAR;
	OMX_COMPONENTTYPE *		m_pCompVR;

	OMX_COMPONENTTYPE *		m_pCompClock;

public:
	voOMXThreadHandle		m_hThreadMedia;
	OMX_U32					m_nMediaThreadID;
	bool					m_bMediaStop;

	static int	ReadMediaDataProc (OMX_PTR pParam);
	int			ReadMediaDataLoop (void);
	
	VO_VOID data_callback( OMX_BUFFERHEADERTYPE * ptr_buffer );

protected:
	vo_http_live_streaming m_streaming;
};

#endif // __CExtVOMEPlayer_H__
