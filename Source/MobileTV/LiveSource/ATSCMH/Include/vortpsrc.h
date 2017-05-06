#ifndef __VORTPSRC_H__
#define __VORTPSRC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vortpbase.h"
#include "voMem.h"
#include "voLiveSource.h"
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
	VOSRCSENDDATA		pProc;
	VOSRCSTATUS			fStatus;
	VO_PTR				pUserData;
}VO_RTPSRCWRAP_INIT_INFO;

typedef struct
{
	RTPSRCPROC			pProc;
	VO_MEM_OPERATOR   *	pMemOP;
	VO_VOID           * pUserData;
}VO_RTPSRC_INIT_INFO;


/**
 * RTPSRC function set
 */

//VO_U32 VO_API voGetLiveSrcAPI(VO_LIVESRC_API * pRTPSource);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VORTPSRC_H__