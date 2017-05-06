#ifndef __ADPCMDECCHECK_HANDLE_
#define __ADPCMDECCHECK_HANDLE_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK)
extern void* g_hADPCMDecInst;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif