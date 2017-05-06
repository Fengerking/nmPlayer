#ifndef __CDRM_VISUALON_EMPTY_H__
#define __CDRM_VISUALON_EMPTY_H__
#include "DRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDRM_VisualOn_AES128 :
	public CDRM
{
public:
	CDRM_VisualOn_AES128(void);
	virtual ~CDRM_VisualOn_AES128(void);

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

private:
	void * m_pHandle;

	VO_BYTE m_Buf[16];
	VO_BOOL m_bBufvalid;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_VISUALON_EMPTY_H__