#include "DRM_Conax_PlayReady.h"
#include "voDRMConaxPlayReady.h"
#include "cxdrmapi.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	int voCreateDRM(VOOSMPDRM** ppDRM, conaxDrmAPI* pParam)
	{
		if (0 == pParam)
			return VO_OSMP_DRM_ERR_PARAMETER;

		DRM_Conax_PlayReady *pDRM = new DRM_Conax_PlayReady(pParam);

		if (0 == pDRM) {
			return VO_OSMP_DRM_ERR_BASE;
		}

		*ppDRM = pDRM;

		return VO_OSMP_DRM_ERR_NONE;
	}

	int voDestroyDRM(VOOSMPDRM* pDRM)
	{
		if (0 == pDRM) {
			return VO_OSMP_DRM_ERR_PARAMETER;
		}

		delete pDRM;

		return VO_OSMP_DRM_ERR_NONE;
	}

#if defined __cplusplus
}
#endif