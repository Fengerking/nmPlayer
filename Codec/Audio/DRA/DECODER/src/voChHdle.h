#ifndef __DRADECCHECK_HANDLE_
#define __DRADECCHECK_HANDLE_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK)
extern void* g_hDRADecInst;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif