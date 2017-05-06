#ifndef __VODRM_COMMONAPI_H__
#define __VODRM_COMMONAPI_H__
#include "VOOSMPDRM.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	int voCreateDRM(VOOSMPDRM** ppDRM, conaxDrmAPI* pParam);

	int voDestroyDRM(VOOSMPDRM* pDRM);

#if defined __cplusplus
}
#endif

#endif //__VODRM_COMMONAPI_H__