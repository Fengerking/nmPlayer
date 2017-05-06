/*
 * Extends result codes with DRM specific errors.
 */

#ifndef __DRM_Result_h__
#define __DRM_Result_h__

/* TODO: Include all direct base libraries as they are added */
#include <MKBase/Result.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* DRM ERRORS                                                                */
/******************************************************************************/

/* Helper macros for HTTP code creation */

#define MK_C_DRM         MK_C(0x100)
#define MK_S_DRM(aCode)  MK_S(MK_C_DRM, aCode) 
#define MK_E_DRM(aCode)  MK_E(MK_C_DRM, aCode)

/* Success codes */

#define  MK_S_DRM_COND_MDAT_ICMPLT MK_S_DRM(0x001) /* Incomplete mdat */

/* Error codes */

#define MK_E_DRM_FAIL       MK_E_DRM(0x000) /* Unspecified failure */
#define MK_E_DRM_CRIT       MK_E_DRM(0x001) /* Unspecified critical/abortive failure */

#define MK_E_DRM_CRYPTFAIL  MK_E_DRM(0x008) /* Platform crypto-function failed */

#define MK_E_DRM_PKEYCRPT   MK_E_DRM(0x200) /* Public key corrupt */
#define MK_E_DRM_KEYNFOUND  MK_E_DRM(0x201) /* AES key not found */

#define MK_E_DRM_CNTCRPT    MK_E_DRM(0x300) /* Content corrupt */
#define MK_E_DRM_CNTNSUP    MK_E_DRM(0x301) /* Unsupported content version */
#define MK_E_DRM_CNTEXP     MK_E_DRM(0x302) /* Content expired */

#define MK_E_DRM_LICCRPT    MK_E_DRM(0x400) /* License corrupt */
#define MK_E_DRM_LICNSUP    MK_E_DRM(0x401) /* Unsupported license version */
#define MK_E_DRM_LICNSTART  MK_E_DRM(0x402) /* License not yet valid/active */
#define MK_E_DRM_LICEXP     MK_E_DRM(0x403) /* License expired */
#define MK_E_DRM_LICWRONG   MK_E_DRM(0x404) /* License does not match user, device or content */
#define MK_E_DRM_LICMISSING MK_E_DRM(0x405) /* A license needed for playback is missing */

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Resolves DRM (and sub-library) result-codes. Returns non-NULL if the
 * result-code was resolved and NULL otherwise.ific code matched).
 */
const MK_Char* MK_DRM_GetCategoryDesc(MK_Result aRes, const MK_Char** aName);
const MK_Char* MK_DRM_GetResultDesc(MK_Result aRes, const MK_Char** aName);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
