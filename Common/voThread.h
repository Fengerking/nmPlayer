	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voThread.h

	Contains:	memory operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voThread_H__
#define __voThread_H__

#include "voYYDef_Common.h"
#include "voIndex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

typedef void * voThreadHandle;

typedef VO_U32 (* voThreadProc) (VO_PTR pParam);

/**
 * Thread create function
 * \param pHandle [in/out] the handle of thread
 * \param nID [in] the id of thread
 * \param fProc [in] the function entry pointer
 * \param pParam [in] the parameter in call function.
 */
typedef int (* VOTDThreadCreate) (void ** pHandle, int * pID, void * fProc, void * pParam);

#ifdef _METRO
typedef unsigned int (__stdcall *VOTHREAD_START_ROUTINE)(VO_PTR lpThreadParameter);
#endif //_METRO
    
typedef enum voThreadPriority
{
	VO_THREAD_PRIORITY_TIME_CRITICAL	= 0x0,
	VO_THREAD_PRIORITY_HIGHEST			= 0x1,
	VO_THREAD_PRIORITY_ABOVE_NORMAL		= 0x2,
	VO_THREAD_PRIORITY_NORMAL			= 0x3,
	VO_THREAD_PRIORITY_BELOW_NORMAL		= 0x4,
	VO_THREAD_PRIORITY_LOWEST			= 0x5,
	VO_THREAD_PRIORITY_IDLE				= 0x6,
} voThreadPriority;

#ifdef _IOS

#ifndef THREAD_PRIORITY_IDLE
#define THREAD_PRIORITY_IDLE 15
#endif
	
#ifndef THREAD_PRIORITY_LOWEST
#define THREAD_PRIORITY_LOWEST 21
#endif

#ifndef THREAD_PRIORITY_BELOW_NORMAL
#define THREAD_PRIORITY_BELOW_NORMAL 26
#endif

#ifndef THREAD_PRIORITY_NORMAL
#define THREAD_PRIORITY_NORMAL 31
#endif
	
#ifndef THREAD_PRIORITY_ABOVE_NORMAL
#define THREAD_PRIORITY_ABOVE_NORMAL 36
#endif
	
#ifndef THREAD_PRIORITY_HIGHEST
#define THREAD_PRIORITY_HIGHEST 41
#endif
	
#ifndef THREAD_PRIORITY_TIME_CRITICAL
#define THREAD_PRIORITY_TIME_CRITICAL 47
#endif

#endif

/**
 * Create the thread
 * \param pHandle [out] the thread handle
 * \param pID [out] the thread id
 * \param fProc [in] the thread start address
 * \param pParam [in] the thread call back parameter
 * \param uFlag [in] the thread was create with the flagr
 * \return value VO_ErrorNone
 */
VO_U32	voThreadCreate (voThreadHandle * pHandle, VO_U32 * pID, 
						voThreadProc fProc, VO_PTR pParam, VO_U32 uFlag);


/**
 * close the thread
 * \param pHandle [in] the thread handle was created by voThreadCreate
 * \param uExitCode [in] the return code after exit the thread
 * \return value VO_ErrorNone
 */
VO_U32	voThreadClose (voThreadHandle hHandle, VO_U32 uExitCode);


/**
 * Get the thread priority
 * \param pHandle [in] the thread handle was created by voThreadCreate
 * \param pPriority [out] the priority to get
 * \return value VO_ErrorNone
 */
VO_U32	voThreadGetPriority (voThreadHandle hHandle, voThreadPriority * pPriority);


/**
 * Set the thread priority
 * \param pHandle [in] the thread handle was created by voThreadCreate
 * \param uPriority [in] the priority to set
 * \return value VO_ErrorNone
 */
VO_U32	voThreadSetPriority (voThreadHandle hHandle, voThreadPriority uPriority);


/**
 * Suspend the thread
 * \param pHandle [in] the thread handle was created by voThreadCreate
 * \return value VO_ErrorNone
 */
VO_U32	voThreadSuspend (voThreadHandle hHandle);


/**
 * Resume the thread
 * \param pHandle [in] the thread handle was created by voThreadCreate
 * \return value VO_ErrorNone
 */
VO_U32	voThreadResume (voThreadHandle hHandle);

/**
 * Protect the thread
 * \param pHandle [in] the thread handle was created by voThreadCreate
 * \return value VO_ErrorNone
 */
VO_U32	voThreadProtect (voThreadHandle hHandle);

/**
 * set the thread name
 * \param uThreadID [in] the thread id was created by voThreadCreate
 * \return value VO_ErrorNone
 */
void voThreadSetName(VO_U32 uThreadID, const char* threadName);

/**
 * convert the thread priority
 * \param uPriority [in] the thread priority
 * \return value 
 */
VO_U32	voThreadConvertPriority (voThreadPriority * pPriority, VO_S32 * pPlatform, VO_BOOL bPlatform);

/**
 * Get the current thread ID
 * \return current thread ID 
 */
VO_S32	voThreadGetCurrentID (void);
        
#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif // __voThread_H__
