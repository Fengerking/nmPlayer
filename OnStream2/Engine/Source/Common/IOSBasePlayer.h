#ifndef __IOSBASEPLAYER_H__
#define __IOSBASEPLAYER_H__

#include "voType.h"
#include "voOnStreamType.h"

class IOSBasePlayer 
{
public:
	virtual int		Init() = 0;
	virtual int		Uninit() = 0;

	virtual int		SetDataSource (void * pSource, int nFlag) = 0;

	virtual int		Run (void) = 0;
	virtual int		Pause (void) = 0; 
	virtual int		Stop (void) = 0;
	virtual int		Flush (void) = 0;
	virtual int		GetStatus (int * pStatus) = 0;
	virtual int		GetDuration (int * pDuration) = 0;

	virtual int		SetView(void* pView) = 0;
	virtual	int		SetJavaVM(void *pJavaVM, void* obj) = 0;
	virtual void*	GetJavaObj() = 0;

	virtual int		GetPos (int * pCurPos) = 0;
	virtual int		SetPos (int nCurPos) = 0;
 
	virtual int		GetParam (int nID, void * pValue) = 0;
	virtual int		SetParam (int nID, void * pValue) = 0;
	
	virtual int		GetSEISample (VOOSMP_SEI_INFO * pSample) = 0;
	virtual int		GetSubtileSample (voSubtitleInfo * pSample) = 0;
	virtual int		GetSubLangNum(int *pNum) = 0;
	virtual int		GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem) = 0;
	virtual int		GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo) = 0;
	virtual int		SelectLanguage(int Index) = 0;
};

#endif
