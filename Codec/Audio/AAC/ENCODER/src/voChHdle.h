#ifndef __AACENCCHECK_HANDLE_
#define __AACENCCHECK_HANDLE_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK) 	
extern void* g_hAACEncInst;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif