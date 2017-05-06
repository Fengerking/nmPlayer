#ifndef __CDRM_DISCRETIX_PLAYREADY_H__
#define __CDRM_DISCRETIX_PLAYREADY_H__
#include "DRMStreaming.h"
#include "DiscretixDRMAPI.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class DRM_Discretix_PlayReady
	: public CDRMStreaming
{
public:
	DRM_Discretix_PlayReady(void);
	virtual ~DRM_Discretix_PlayReady(void);

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

public:
	static DiscretixDRMAPI m_API;

private:
	enum HLS_DRM_Type {
		HLS_DRM_UNKNOW,
		HLS_DRM_HARMONIC,
		HLS_DRM_DX
	};

	struct DXHarmonic {
		VO_U32			uID;
		void *			hDRM;
		unsigned long	uOffsetHamonic;
	};

	DXHarmonic	m_Harmonics[3];

	void* m_hDRM;

	HLS_DRM_Type m_eHLS_DRM;

	voCMutex				m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_DISCRETIX_PLAYREADY_H__