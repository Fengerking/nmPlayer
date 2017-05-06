#ifndef __CDRM_H__
#define __CDRM_H__

#include "voDRM2.h"
#include "voSource2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDRM
{
public:
	CDRM(void);
	virtual ~CDRM(void);

	virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32	Uninit();

	virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam) = 0;
	virtual VO_U32	GetInternalAPI(VO_PTR* ppParam);

	virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32	GetParameter(VO_U32 uID, VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat) = 0;
	static VO_U32	fSourceDrmProc(VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 eSourceFormat) { return ( (CDRM *)pUserData )->OnSourceDrm(nFlag, pParam, (VO_DRM2_SOURCE_FORMAT)eSourceFormat); }

protected:
	VO_S32 SendEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2) { return m_cEventCallBack.SendEvent ? m_cEventCallBack.SendEvent(m_cEventCallBack.pUserData, nID, nParam1, nParam2) : VO_ERR_DRM2_BASE; }

	VO_DRM_OPENPARAM		m_DRM_OpenParam;
	VO_SOURCEDRM_CALLBACK2	m_drmCallback;

	VO_SOURCE2_IO_HTTP_VERIFYCALLBACK	m_cbVerify;

	VO_TCHAR	m_szPackPath[512];
	VO_BOOL	m_bExit;

private:
	VO_SOURCE2_EVENTCALLBACK	m_cEventCallBack;
};


VO_U32 getRPRightsManagementHeader_PIFF(VO_BYTE* pProtectionSystemSpecificHeader, VO_U32 uProtectionSystemSpecificHeader, VO_PBYTE* ppRightsManagementHeader, VO_U32* puRightsManagementHeader);


#ifdef _VONAMESPACE
}
#endif

#endif // __CDRM_H__

