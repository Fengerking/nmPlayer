/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voSMIL_H__
#define __voSMIL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voSource2.h"

#define INIT_CBS_FLAG						0x0001			//The smil type flag
#define LENGTH_64			64
typedef struct VO_CBS_SMIL_VIDEO
{
	VO_CHAR	pSrc[MAXURLLEN];					// video url
	VO_U64	clipBegin;						//the clip bigin time
	VO_U64	clipEnd;							//the clip end time
	VO_CHAR	pGuid[LENGTH_64];					// Guid str	
	VO_CHAR pClosedCaptionURL[MAXURLLEN];		// contain the URL for closedcaption
	VO_BOOL	pIsLive;							// Is live content	
	VO_CHAR	pTitle[MAXURLLEN];					// Title 
	VO_CHAR	pContantDAta[LENGTH_64];			// contant data	
	VO_BOOL	bEpisodeFlag;						// the flag parsed from bEpisodeFlag.	
	VO_CBS_SMIL_VIDEO*	pNext;				// pNext point to the next VO_CBS_SMIL_VIDEO
}VO_CBS_SMIL_VIDEO;	


#if 0
typedef struct
{
//callback instance
	VO_PTR pUserData;

/**
 * Callback function. The source will send the data out..
 * \param pUserData [in] The user data which was set by Open().
 * \param nOutputType [in] The output data type, see VO_SOURCE2_OUTPUT_TYPE
 * \param pData [in] The data pointer, it MUST be converted to correct struct pointer according nType, see VO_SOURCE2_OUTPUT_TYPE
 *                   the param type is depended on the nOutputType, for VO_SOURCE2_TT_VIDEO & VO_SOURCE2_TT_AUDIO please use VO_SOURCE2_SAMPLE
 *                                                                  for VO_SOURCE2_TT_SUBTITLE please use voSubtitleInfo
 */
	VO_U32 (VO_API * SendData) (VO_PTR pUserData, VO_SMIL_STRUCT* pData);

}VO_SMIL_DATACALLBACK;
#endif

typedef struct
{
/**
 * Initial a smil session and Set the init type( nFlag etc. ) into the module
 * For this function please only set the nFalg to define the module type, and copy the pParam, do not do anything else
 * This function should be called first for a session.
 * \param phHandle [out] source handle.
 * \param nFlag [in] define the module type.
 * \param pParam [in] user define.
 */
VO_U32 (VO_API * Init) (VO_HANDLE * phHandle, VO_U32 nFlag, VO_PTR pParam );

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Uninit) (VO_HANDLE hHandle);

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pData [in] The data smil parser need to parse. 
 * \param nSize [in] The size of the data which will be parsed by smil parser . 
 * \param pSmilStruct [out] The new data struct generated from the pData.The struct type shoud match the nFlag setted by Init(). 
 *  The FLAG/STRUCT MAP :  INIT_CBS_FLAG-----VO_CBS_SMIL_VIDEO
 */
VO_U32 (VO_API * Parse) ( VO_HANDLE hHandle, VO_PBYTE pData, VO_U32 nSize,  VO_PTR* ppSmilStruct);
} VO_SMIL_API;



VO_S32 VO_API voGetSMILAPI(VO_SMIL_API* pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voSMIL_H__




