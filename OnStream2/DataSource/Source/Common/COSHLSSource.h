#ifndef __COSHLSSource_H__
#define __COSHLSSource_H__

#include "COSBaseSource.h"

class COSHLSSource : public COSBaseSource
{
public:
	COSHLSSource (VO_SOURCE2_LIB_FUNC *pLibop);
	virtual ~COSHLSSource (void);

	virtual int				Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag);
	virtual int				Uninit(void);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);


	virtual int				HandleEvent (int nID, int nParam1, int nParam2);

protected:
	virtual int				LoadDll();
};

#endif // __COSBaseSource_H__
