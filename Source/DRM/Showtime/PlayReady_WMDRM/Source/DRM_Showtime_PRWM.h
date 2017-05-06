#ifndef __CDRM_SHOWTIME_PRWM_H__
#define __CDRM_SHOWTIME_PRWM_H__
#include "DRMStreaming.h"
#include "drm_api.h"
#include "voCMutex.h"

class CDRM_Showtime_PRWM :
	public CDRMStreaming
{
public:
	CDRM_Showtime_PRWM(void);
	virtual ~CDRM_Showtime_PRWM(void);

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32 SetParameter(VO_U32 uID, VO_PTR pParam);

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

protected:
	VO_U32 VO_Rights_Acquire();

private:
	DrmAgent_Context	m_Context;
	DrmAgent_Handle		m_Handle;

	VO_BYTE*			m_pBuffer;

	VO_CHAR								m_szSID[256];
	VO_CHAR								m_szURLServer[1024];

	voCMutex		m_lock;
};

#endif //__CDRM_SHOWTIME_PRWM_H__