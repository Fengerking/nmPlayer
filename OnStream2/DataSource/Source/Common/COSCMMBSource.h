#pragma once
#include "COSBaseSource.h"

class COSCMMBSource :
	public COSBaseSource
{
public:
	COSCMMBSource(VO_SOURCE2_LIB_FUNC *pLibop);
	~COSCMMBSource(void);

	virtual int				LoadDll();

	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);


	virtual int				HandleEvent (int nID, int nParam1, int nParam2);

protected:
	char					m_szFileDll[256];
	char					m_szFileAPI[256];
};
