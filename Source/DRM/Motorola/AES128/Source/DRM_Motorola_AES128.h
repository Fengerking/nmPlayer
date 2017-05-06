#ifndef __CDRM_MOTOROLA_AES128_H__
#define __CDRM_MOTOROLA_AES128_H__
#include "DRM_API.h"
#include "DRMStreaming.h"

class CDRM_Motorola_AES128 :
	public CDRMStreaming
{
public:
	CDRM_Motorola_AES128(void);
	virtual ~CDRM_Motorola_AES128(void);

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam);

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

private:
	DRM_Callback*	m_pAPI;
	DRMHANDLE		m_hDRM;

	VO_CHAR m_PreKeyURI[2048];

	VO_U32	m_uLen;
	VO_BYTE m_Buf[752];
};

#endif //__CDRM_MOTOROLA_AES128_H__