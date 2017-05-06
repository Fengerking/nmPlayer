#ifndef _COSVomeEngine_h
#define _COSVomeEngine_h

#include "COSBasePlayer.h"
#include "vompAPI.h"

class COSVomeEngine : public COSBasePlayer 
{
public:
	COSVomeEngine();
	virtual ~COSVomeEngine();
	
	virtual int		Init();
	virtual int		Uninit();

	virtual int		SetDataSource (void * pSource, int nFlag);
	virtual int		SendBuffer(int nSSType, VOMP_BUFFERTYPE * pBuffer);

	virtual int		GetVideoBuffer(VOMP_BUFFERTYPE ** ppBuffer);
	virtual int		GetAudioBuffer(VOMP_BUFFERTYPE ** ppBuffer);

	virtual int		Run (void);
	virtual int		Pause (void); 
	virtual int		Stop (void);
	virtual int		Flush (void);
	virtual int		GetStatus (int * pStatus);
	virtual int		GetDuration (int * pDuration);

	virtual int		SetView(void* pView);
	virtual	int		SetJavaVM(void *pJavaVM, void* obj);
	virtual void*	GetJavaObj();

	virtual int		GetPos (int * pCurPos);
	virtual int		SetPos (int nCurPos);
 
	virtual int		GetParam (int nID, void * pValue);
	virtual int		SetParam (int nID, void * pValue);

	virtual int		LoadDll (void);

	static	int 	vomtCallBack (void * pUserData, int nID, void * pParam1, void * pParam2);
	virtual int		HandleEvent (int nID, void * pParam1, void * pParam2);

protected:
	void *						m_hDll;
	void *						m_hPlay;

	VOMP_LIB_FUNC*				m_pLibFunc;
	VOMP_FUNCSET				m_VomeFuncSet;
};

#endif
