/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003 -     		*
 *																		*
 ************************************************************************/
/*******************************************************************************
 *	File:		voOSEngWrapper.h
 *	
 *	Contains:	voOSEngWrapper header file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-03-20		Jeff			Create file
 *	
 ******************************************************************************/

#ifndef _CONSTREAM_WRAPPER_
#define _CONSTREAM_WRAPPER_

#include "CDllLoad.h"
#include "voOnStreamEngine.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class voOSEngWrapper : public CDllLoad
{
public:
	voOSEngWrapper();
	virtual ~voOSEngWrapper (void);
    
	virtual int         Init(int nPlayerType, void* pInitParam, int nInitParamFlag);
	virtual int			Uninit();
    virtual int         SetView(void* pView);
	virtual int 		Open (void * pSource, int nFlag);
    
    virtual int         SelectLanguage (int nIndex);
	virtual int         GetLanguage (VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);
    
    virtual int			Close();
    
	virtual int 		Run(void);
	virtual int 		Pause(void);
	virtual int 		Stop(void);
    
	virtual int			GetPos();
	virtual int 		SetPos(int nCurPos);
    
    virtual int 		GetSubtitleSample(voSubtitleInfo* pSample);
    
	virtual int			GetParam(int nParamID, void* pParam);
	virtual int			SetParam(int nParamID, void* pParam);
    
    virtual int		    GetSEISample(VOOSMP_SEI_INFO * pSample);
    
private:
    voOnStreamEngnAPI   m_cOSFuncSet;
    void *				m_pOSHandle;
    void *              m_pLogCB;
};

#endif //_CONSTREAM_WRAPPER_