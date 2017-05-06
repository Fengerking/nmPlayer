 /************************************************************************
 *                                                                       *
 *  VisualOn, Inc. Confidential and Proprietary, 2003-2009               *
 *                                                                       *
 ************************************************************************/

#ifndef __MEM_TEST_H__
#define __MEM_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>
#include <viMem.h>

/* For video memory share. */
VO_U32 VO_API Init(VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem);
VO_U32 VO_API GetBufByIndex (VO_S32 uID, VO_S32 nIndex);
VO_U32 VO_API Uninit(VO_S32 uID);
void GetVideoMemOperatorAPI(VO_MEM_VIDEO_OPERATOR * pVideoMemOperator);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_WMV9_H__
