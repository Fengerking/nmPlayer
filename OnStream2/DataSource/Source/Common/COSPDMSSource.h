#ifndef __COSPDMSSource_H__
#define __COSPDMSSource_H__

#include "COSBaseSource.h"
#include "voPreHTTP.h"

typedef int (VO_API *pGetPreHTTPAPI)(VO_PREHTTP_API * pHandle, unsigned int uFlag);

class COSPDMSSource : public COSBaseSource
{
public:
	COSPDMSSource (VO_SOURCE2_LIB_FUNC *pLibop);
	virtual ~COSPDMSSource (void);

	virtual int				Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag);
	virtual int				Uninit(void);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);


	virtual int				HandleEvent (int nID, int nParam1, int nParam2);

protected:
	virtual int				LoadDll();

	virtual int				CheckLinkType(void * pSource, int nFlag);

	char					m_szFileDll[256];
	char					m_szFileAPI[256];
};

#endif // __COSBaseSource_H__
