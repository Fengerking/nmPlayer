
#include "voDRM.h"
#include "DRMShell.h"


CDRMShell::CDRMShell(void)
	: m_javaCallback(NULL)
{
	memset( &m_drmCallback, 0, sizeof(VO_SOURCEDRM_CALLBACK) );
	memset( &m_DRM_OpenParam, 0, sizeof(VO_DRM_OPENPARAM) );
}


VO_U32 CDRMShell::Init(VO_DRM_OPENPARAM *pParam)
{
	m_drmCallback.pUserData = this;
	m_drmCallback.fCallback = vomeSourceDrmProc;

	m_javaCallback = pParam->pCallBack;

	return VO_ERR_DRM_OK;
}


VO_U32 CDRMShell::GetInternalAPI(VO_PTR *ppParam)
{
	*ppParam = &m_drmCallback;

	return VO_ERR_DRM_OK;
}

VO_U32 CDRMShell::vomeSourceDrmProc(VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved)
{
	return ( (CDRMShell *)pUserData )->vomeOnSourceDrm (nFlag, pParam, nReserved);
}