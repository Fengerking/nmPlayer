	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXThread.h

	Contains:	memory operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voOMXThread_H__
#define __voOMXThread_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <OMX_Core.h>

typedef void * voOMXThreadHandle;

typedef OMX_S32 (* voOMX_ThreadProc) (OMX_PTR pParam);

typedef enum voOMX_ThreadPriority
{
	VOOMX_THREAD_PRIORITY_TIME_CRITICAL		= 0x0,
	VOOMX_THREAD_PRIORITY_HIGHEST			= 0x1,
	VOOMX_THREAD_PRIORITY_ABOVE_NORMAL		= 0x2,
	VOOMX_THREAD_PRIORITY_NORMAL			= 0x3,
	VOOMX_THREAD_PRIORITY_BELOW_NORMAL		= 0x4,
	VOOMX_THREAD_PRIORITY_LOWEST			= 0x5,
	VOOMX_THREAD_PRIORITY_IDLE				= 0x6,
} voOMX_ThreadPriority;


/**
 * Create the thread
 * \param pHandle [out] the thread handle
 * \param pID [out] the thread id
 * \param fProc [in] the thread start address
 * \param pParam [in] the thread call back parameter
 * \param uFlag [in] the thread was create with the flagr
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadCreate (voOMXThreadHandle * pHandle, OMX_U32 * pID, 
								   voOMX_ThreadProc fProc, OMX_PTR pParam, OMX_U32 uFlag);


/**
 * close the thread
 * \param pHandle [in] the thread handle was created by voOMXThreadCreate
 * \param uExitCode [in] the return code after exit the thread
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadClose (voOMXThreadHandle hHandle, OMX_U32 uExitCode);


/**
 * Get the thread priority
 * \param pHandle [in] the thread handle was created by voOMXThreadCreate
 * \param pPriority [out] the priority to get
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadGetPriority (voOMXThreadHandle hHandle, voOMX_ThreadPriority * pPriority);


/**
 * Set the thread priority
 * \param pHandle [in] the thread handle was created by voOMXThreadCreate
 * \param uPriority [in] the priority to set
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadSetPriority (voOMXThreadHandle hHandle, voOMX_ThreadPriority uPriority);


/**
 * Suspend the thread
 * \param pHandle [in] the thread handle was created by voOMXThreadCreate
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadSuspend (voOMXThreadHandle hHandle);


/**
 * Resume the thread
 * \param pHandle [in] the thread handle was created by voOMXThreadCreate
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadResume (voOMXThreadHandle hHandle);

/**
 * Protect the thread
 * \param pHandle [in] the thread handle was created by voOMXThreadCreate
 * \return value OMX_ErrorNone
 */
OMX_ERRORTYPE	voOMXThreadProtect (voOMXThreadHandle hHandle);

/**
 * convert the thread priority
 * \param uPriority [in] the thread priority
 * \return value 
 */
OMX_ERRORTYPE	voOMXThreadConvertPriority (voOMX_ThreadPriority * pPriority, OMX_S32 * pPlatform, OMX_BOOL bPlatform);


/************************************************************************/
/* set thread name, only can be used in thread function                 */
/************************************************************************/
OMX_ERRORTYPE	voSetThreadName( const char * str_name );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voOMXThread_H__
