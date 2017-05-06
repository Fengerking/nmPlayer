#ifndef __DRM_ADAPTER_H__
#define __DRM_ADAPTER_H__
#include "DRM.h"
#include "VOOSMPDRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class DRMAdapter
	: public CDRM 
{
public:
	DRMAdapter(void);
	~DRMAdapter(void);

	virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32	Uninit();

	virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NOT_IMPLEMENT; }

	virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);

	virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

private:
	VO_BOOL		m_bNeedDecrypt;

	VOOSMPDRM *	m_pDRMAPIs;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__DRM_ADAPTER_H__