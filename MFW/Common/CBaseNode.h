	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseNode.h

	Contains:	CBaseNode header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-06-06		JBF			Create file

*******************************************************************************/

#ifndef __CBaseNode_H__
#define __CBaseNode_H__

#include "CDllLoad.h"
#include "CBaseConfig.h"

#include "voCMutex.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class CBaseNode : public CDllLoad
{
public:
	CBaseNode (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CBaseNode (void);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

	virtual VO_U32		SetConfig (CBaseConfig * pConfig) {m_pConfig = pConfig; return 0;}
	virtual VO_U32		SetErrorText (VO_TCHAR * pErrorText) {m_pError = pErrorText; return 0;}
	virtual void		SetVOSDK (VO_BOOL bVOSDK) {m_bVOUsed = bVOSDK;}
	virtual VO_BOOL		GetVOSDK () { return m_bVOUsed;}
	virtual void		SetForThumbnail (VO_BOOL bForThumbnail) {m_bForThumbnail = bForThumbnail;}
	virtual void		SetOMXComp (VO_BOOL bOMXComp) {m_bOMXComp = bOMXComp;}
	virtual void		SetDumpLogLevel (VO_U32 nLevel) {m_nDumpLogLevel = nLevel;}

protected:
	VO_PTR				m_hInst;
	voCMutex			m_Mutex;

	VO_MEM_OPERATOR	*	m_pMemOP;
	VO_LOG_PRINT_CB *	m_pcbVOLOG;

	CBaseConfig *		m_pConfig;
	VO_CHAR				m_szCfgItem[128];
	VO_BOOL				m_bVOUsed;
	VO_BOOL				m_bForThumbnail;
	VO_BOOL				m_bOMXComp;
	VO_TCHAR *			m_pError;
	VO_U32				m_nDumpLogLevel;
};

#endif // __CBaseNode_H__
