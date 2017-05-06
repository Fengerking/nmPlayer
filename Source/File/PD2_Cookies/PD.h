// PD.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"

#include "voType.h"
#include "voSource.h"
#include "vo_PD_manager.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
VO_S32 VO_API voGetPDReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

#ifdef _PD_SOURCE2
VO_S32 VO_API voGetPD2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag);
#endif //_SOURCE2

    
#ifdef __cplusplus
}
#endif /* __cplusplus */