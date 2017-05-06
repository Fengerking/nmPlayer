/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef __voCCRRR_H__
#define __voCCRRR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIVCommon.h"
#include "voMem.h"
#include "viMem.h"

#define VO_CCRRR_PMID_ShowOverlay	(VO_INDEX_SNK_CCRRR | 0x0001) //Show or hide overlay , *pValue = 0 hide overlay *pValue = 1 show overlay 

#define VO_PID_CCRRR_BASE            VO_PID_COMMON_BASE | VO_INDEX_SNK_CCRRR

typedef enum
{
	VO_PID_CCRRR_ALPHAVALUE	      = VO_PID_CCRRR_BASE | 0x0001,  /*!<Set alpha channel value for rgb32, default is 0 */
	VO_PID_CCRRR_MAX              = VO_MAX_ENUM_VALUE
}
VOCCRRRPARAMETERID;	
	
/**
 * CCRR module property
 */
typedef struct
{
	VO_U32		nRender;		/*!< 1 render video in CCRR. */
	VO_U32		nOverlay;		/*!< 1 is overlay or write video memory directly. */
	VO_U32		nKeyColor;		/*!< the dest override key color. */
	VO_U32		nRotate;		/*!< Supports rotate type VO_IV_RTTYPE */
	VO_U32		nOutBuffer;		/*!< 1 provide the output buffer */
	VO_U32		nFlag;			/*!< Resever */
	VO_CHAR		szName[64];		/*!< the name of CCRR */
} VO_CCRRR_PROPERTY;

/**
 * CCRR function set
 */
typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phCCRRR, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hCCRRR);
	VO_U32 (VO_API * GetProperty) (VO_HANDLE hCCRRR, VO_CCRRR_PROPERTY * pProperty);
	VO_U32 (VO_API * GetInputType) (VO_HANDLE hCCRRR, VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	VO_U32 (VO_API * GetOutputType) (VO_HANDLE hCCRRR, VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	VO_U32 (VO_API * SetColorType) (VO_HANDLE hCCRRR, VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	VO_U32 (VO_API * SetCCRRSize) (VO_HANDLE hCCRRR, VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);
	VO_U32 (VO_API * Process) (VO_HANDLE hCCRRR, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);
	VO_U32 (VO_API * WaitDone) (VO_HANDLE hCCRRR);
	VO_U32 (VO_API * SetCallBack) (VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	VO_U32 (VO_API * GetVideoMemOP) (VO_HANDLE hCCRRR, VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	VO_U32 (VO_API * SetParam) (VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);
	VO_U32 (VO_API * GetParam) (VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue);
} VO_VIDEO_CCRRRAPI;

/**
 * Get video render API interface
 * \param pCCRRR [IN/OUT] Return the video color conversion and resize API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API yyGetColorConvertFunc (VO_VIDEO_CCRRRAPI * pCCRRR, VO_U32 uFlag);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // _CCRRRENDER_H_
