#ifndef __VORTPSRC_H__
#define __VORTPSRC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vortpbase.h"
#include "voMem.h"

/**
* Error code
*/
#define VORC_RTPSRC_BASE    0x8078000
enum _VORC_RTPSRC
{
	VORC_RTPSRC_OK               = 0,
	VORC_RTPSRC_ERROR            = VORC_RTPSRC_BASE | 0x0001,
	VORC_RTPSRC_INVALID_ARG      = VORC_RTPSRC_BASE | 0x0002,
	VORC_RTPSRC_OUT_OF_MEMORY    = VORC_RTPSRC_BASE | 0x0003,
	VORC_RTPSRC_NOT_IMPLEMENT	 = VORC_RTPSRC_BASE | 0x0004,
	VORC_RTPSRC_NETWORK_TIMEOUT  = VORC_RTPSRC_BASE | 0x0005,
	VORC_RTPSRC_BAD_NETWORK      = VORC_RTPSRC_BASE | 0x0006,
	VORC_RTPSRC_NEED_RETRY       = VORC_RTPSRC_BASE | 0x0007,
};
typedef int VORC_RTPSRC;

typedef struct
{
	VO_S32			nChannelID;
	VO_VOID       *	pData;
	VO_U32          nDataSize;
	VO_VOID       *	pUserData;
}VO_RTPSRC_DATA;

typedef void (VO_API *RTPSRCPROC)(VO_RTPSRC_DATA * pData);

typedef struct
{
	RTPSRCPROC			pProc;
	VO_MEM_OPERATOR   *	pMemOP;
	VO_VOID           * pUserData;
}VO_RTPSRC_INIT_INFO;


/**
 * RTPSRC function set
 */
typedef struct
{
	VORC_RTPSRC (VO_API * Open) (VO_PTR * ppHandle, char * szSDPData, VO_RTPSRC_INIT_INFO * pInitInfo);

	VORC_RTPSRC (VO_API * Close) (VO_PTR pHandle);

	VORC_RTPSRC (VO_API * Start) (VO_PTR pHandle);

	VORC_RTPSRC (VO_API * Stop) (VO_PTR pHandle);

	VORC_RTPSRC (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	VORC_RTPSRC (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	VORC_RTPSRC (VO_API * SendData) (VO_PTR pHandle, VO_PBYTE pData, VO_U32 nSize);

} VO_RTPSRC_API;

VORC_RTPSRC VO_API voGetRTPSourceAPI(VO_RTPSRC_API * pRTPSource);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VORTPSRC_H__