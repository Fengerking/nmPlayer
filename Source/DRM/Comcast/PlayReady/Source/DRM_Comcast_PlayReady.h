#ifndef __CDRM_COMCAST_PLAYREADY_H__
#define __CDRM_COMCAST_PLAYREADY_H__
#include "DRM.h"


class CDRM_Comcast_PlayReady :
	public CDRM
{
public:
	CDRM_Comcast_PlayReady(void);
	virtual ~CDRM_Comcast_PlayReady(void);

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

private:
	void *		m_pHandle;

	unsigned	m_uSizeFullHeader; // for Envelope
};

#endif //__CDRM_COMCAST_PLAYREADY_H__