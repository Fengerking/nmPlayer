
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Lin2WinBase.h

	Contains:	Lin2WinBase header file

    Aim:        provide some common base struct or const variable 

	Written by:	XiaGuangTai

	Change History (most recent first):
	2008-01-11		gtxia			Create file

*******************************************************************************/


#ifndef _LIN_2_WIN_BASE_H
#define _LIN_2_WIN_BASE_H
#include <pthread.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef MAC
#include <sys/types.h>
#endif//
typedef enum {LH_FILE =0, LH_THREAD, LH_EVENT} LH_TYPE;

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)



#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004  


#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5


#define FILE_BEGIN         0
#define FILE_CURRENT       1
#define FILE_END           3

// the flow for the thread priority 	
#define THREAD_BASE_PRIORITY_LOWRT  15  // value that gets a thread to LowRealtime-1
#define THREAD_BASE_PRIORITY_MAX    2   // maximum thread base priority boost
#define THREAD_BASE_PRIORITY_MIN    (-2)  // minimum thread base priority boost
#define THREAD_BASE_PRIORITY_IDLE   (-15) // value that gets a thread to idle


#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)


#define INVALID_HANDLE_VALUE            0
#define INVALID_SET_FILE_POINTER	    (-1)

struct lh_info
{
	LH_TYPE mType;  // what kind of linux handle
	union
	{
		int	        mFd;    // file Id
		struct
		{
			pthread_t          mThId;  // thread id
			pthread_attr_t*    mAttr;  // thread attributes
		} mThrd;
		
		struct
		{
			pthread_cond_t lCond;
			pthread_mutex_t lMutex;
		} mEvt;             // for event
	} mData;
};
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
	
#endif // _LIN_2_WIN_BASE_H
