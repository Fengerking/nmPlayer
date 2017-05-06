#ifndef __CDRMShell_H__
#define __CDRMShell_H__
#include "voDRM2.h"


class CDRMShell
{
public:
	CDRMShell(void);
	virtual ~CDRMShell(void) {};

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit() = 0;

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) = 0;
	virtual VO_U32 GetInternalAPI(VO_PTR* ppParam);

	virtual VO_U32 SetParameter(VO_U32 uID, VO_PTR pParam) = 0;
	virtual VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam) = 0;

	static VO_U32 vomeSourceDrmProc(VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved);
	virtual VO_U32 vomeOnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved) = 0;

protected:
	VO_DRM_OPENPARAM		m_DRM_OpenParam;
	VO_SOURCEDRM_CALLBACK	m_drmCallback;

	VOSOURCEDRMCALLBACK		m_javaCallback;
};

#endif // __CDRMShell_H__

