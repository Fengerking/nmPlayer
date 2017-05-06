#ifndef __VO_HTTPSVR_EXPORT__
#define __VO_HTTPSVR_EXPORT__

#if defined(WIN32)
#ifdef __cplusplus
#define DllExport extern "C" __declspec(dllexport)
#else
#define DllExport __declspec(dllexport)
#endif
DllExport int InitSvrAndStart(int nPort = 0 ); //nPort: server socktet port number
DllExport void StopServer();
#endif

#if defined(_POSIX)
#if defined(__cplusplus)
extern "C" {
#endif
int InitSvrAndStart(int nPort = 0 );
void StopServer();
#if defined(__cplusplus)
}
#endif
#endif


#endif