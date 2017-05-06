/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		COMXALEngine.h
 
 Contains:	VisualOn OpenMAX AL wrapper class file
 
 Written by:	Jim Lin
 
 Change History (most recent first):
 2012-03-24		Jim			Create file
 *******************************************************************************/


#ifndef _COMXAL_ENGINE_H_
#define _COMXAL_ENGINE_H_

#include "voNPWrap.h"
#include "voIndex.h"

class COMXALEngine 
{
public:
	COMXALEngine();
	virtual ~COMXALEngine();
	
	int Init(void* pInitParam);
	int Uninit();
	int SetView(void* pView);
	int Open(void* pSource, int nFlag);
	int Close();
	int Run();
	int Pause();
	int Stop();
	int GetPos();
	int SetPos(int nPos);
	int GetParam(int nParamID, void* pValue);
	int SetParam(int nParamID, void* pValue);
	
private:
	virtual int		LoadDll (void);
	
protected:
	void*					m_hDll;
	void*					m_hWrap;

	VONP_LIB_FUNC*		m_pLibFunc;
	VO_NP_WRAPPER_API	m_NPFuncSet;

	VO_LOG_PRINT_CB* 		m_pbVOLOG;
  VO_TCHAR *          m_pPlayerPath;
};

#endif // end _COMXAL_ENGINE_H_
