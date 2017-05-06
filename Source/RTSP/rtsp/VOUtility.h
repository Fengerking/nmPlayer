#ifndef __VO_RTSP_UTILITY_H_
#define __VO_RTSP_UTILITY_H_
#include "utility.h"
//#define LL_DEBUG LL_RTSP_ERR

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifndef VOAPI
#define VOAPI __cdecl
#endif

//namespace VOUtility{
void  VOAPI voLog(int id,const char* fileName,const char* logInfo,...);
//};
typedef enum{
	LL_FATAL			= 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
	LL_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
	LL_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
	LL_WARNING,     /// A warning. An operation completed with an unexpected result.
	LL_NOTICE,      /// A notice, which is an information with just a higher priority.
	LL_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
	LL_DEBUG,       /// A debugging message.
	LL_TRACE        /// A tracing message. This is the lowest priority.
}LOG_Level2;

#ifdef _VONAMESPACE
}
#endif
#endif//__VO_RTSP_UTILITY_H_