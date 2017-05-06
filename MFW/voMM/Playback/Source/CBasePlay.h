	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBasePlay.h

	Contains:	CBasePlay header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CBasePlay_H__
#define __CBasePlay_H__

#include "voMMPlay.h"
#include "CvoBaseObject.h"
#include "voCMutex.h"

class CBaseConfig;
class IVOFileBasedDRM;

class CBasePlay : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CBasePlay (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CBasePlay (void);

	virtual VO_U32		SetCallBack (VOMMPlayCallBack pCallBack, VO_PTR pUserData);
	virtual VO_U32		SetViewInfo (VO_PTR hView, VO_RECT * pRect);
	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32 		Run (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		GetDuration (VO_U32 * pDuration);
	virtual VO_U32 		GetCurPos (VO_S32 * pPos);
	virtual VO_U32 		SetCurPos (VO_S32 nPos);
	virtual VO_U32 		SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 		GetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		SetConfig (CBaseConfig * pConfig);
	virtual void		SetDrmCB (VO_SOURCEDRM_CALLBACK * pDrmCB) {m_pDrmCallBack = pDrmCB;}

protected:
	voCMutex					m_csPlay;
	VO_PTR						m_hInst;
	VO_MEM_OPERATOR *			m_pMemOP;
	VO_FILE_OPERATOR *			m_pFileOP;

	VOMMPlayCallBack			m_pCallBack;
	VO_PTR						m_pUserData;

	VO_PTR						m_hView;
	VO_RECT						m_rcView;

	CBaseConfig *				m_pConfig;
	VO_LIB_OPERATOR *			m_pLibOP;
	VO_SOURCEDRM_CALLBACK *		m_pDrmCallBack;

	IVOFileBasedDRM*			m_IFileBasedDRM;
	VO_FILE_OPERATOR *			m_pFileBaseOP;
};

#endif // __CBasePlay_H__
