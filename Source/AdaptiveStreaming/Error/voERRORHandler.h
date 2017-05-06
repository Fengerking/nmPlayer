
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voERRORHandler.h

	Contains:	data type define header file

	Written by:	Leon Huang

	Change History (most recent first):
	2012-08-22		Leon Huang			Create file

*******************************************************************************/

#pragma once
#include "voSource2.h"
#include "voAdaptiveStreamParser.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct
{
	VO_HANDLE hHandle;

	VO_U32 (VO_API * Init)(VO_PTR* ppHandle, VO_SOURCE2_EVENTCALLBACK* pDrmCB, VO_ADAPTIVESTREAMPARSER_STREAMTYPE nType, VO_PTR pReserved);
	VO_U32 (VO_API * Uninit)(VO_PTR pHandle);

	VO_U32 (VO_API * CheckEvent)(VO_PTR pHandle, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2, VO_PTR pReserved);
	/**
	 * Set stop flag. To avoid the hung situation, Each app load this module should call this func first before exit or stop. 
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param bStop [in] the value of stop.
	 */
	VO_U32 (VO_API * SetStop)(VO_PTR pHandle, VO_BOOL bStop );

	VO_U32 (VO_API * SetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
	VO_U32 (VO_API * GetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_StreamingERRORHandler_API;

VO_S32 VO_API voGetStreamingErrorHandlerAPI(VO_StreamingERRORHandler_API * pDRMHandle, VO_U32 uFlag);

#if defined __cplusplus
}
#endif
