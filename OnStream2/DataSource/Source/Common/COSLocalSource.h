#ifndef __COSLocalSource_H__
#define __COSLocalSource_H__

#include "COSBaseSource.h"

class COSLocalSource : public COSBaseSource
{
public:
	COSLocalSource (VO_SOURCE2_LIB_FUNC *pLibop, int	nFF);
	virtual ~COSLocalSource (void);

	virtual int				Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag);
	virtual int				Uninit(void);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

	virtual int				HandleEvent (int nID, int nParam1, int nParam2);

protected:
	virtual int				LoadDll();

	virtual int				UpdataAPI();

	int						m_nFF;

	char					m_szFileDll[256];
	char					m_szFileAPI[256];

};

#endif // __COSMP4Source_H__
