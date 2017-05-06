#include "AES128_CommonAPI.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef _WINDOWS
	int voCreateDRM(VOOSMPDRM** ppDRM, const TCHAR *pWorkPath)
#else
	int voCreateDRM(VOOSMPDRM** ppDRM, const char *pWorkPath)
#endif
	{
		AES128_CommonAPI *pDRM = new AES128_CommonAPI(pWorkPath);

		if (NULL == pDRM) {
			VOLOGE("Out of memory");
			return VO_OSMP_DRM_ERR_BASE;
		}

		*ppDRM = pDRM;

		return VO_OSMP_DRM_ERR_NONE;
	}


	int voDestroyDRM(VOOSMPDRM* pDRM)
	{
		if (NULL == pDRM) {
			VOLOGE("empty pointor");
			return VO_OSMP_DRM_ERR_PARAMETER;
		}

		AES128_CommonAPI *pAES128 = (AES128_CommonAPI *)pDRM;

		delete pAES128;

		return VO_OSMP_DRM_ERR_NONE;
	}

#if defined __cplusplus
}
#endif