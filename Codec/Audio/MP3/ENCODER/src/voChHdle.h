#ifndef __MP3ENCCHECK_HANDLE_
#define __MP3ENCCHECK_HANDLE_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined (_WIN32_WCE)  || defined(WIN32) || defined (LCHECK)
extern void* g_hMp3EncInst;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif