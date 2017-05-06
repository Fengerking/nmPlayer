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

#ifndef __VO_LSP_API_H_
#define __VO_LSP_API_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voType.h"
#include "vompType.h"

/**
* Error code
*/
#define VO_ERR_PLAYER_OK				0
#define VO_ERR_PLAYER_BASE				0x90000000
#define VO_ERR_PLAYER_OUTOFMEMORY		(VO_ERR_PLAYER_BASE | 0x0001)		/*!< out of memory */
#define VO_ERR_PLAYER_NOT_IMPLEMENT		(VO_ERR_PLAYER_BASE | 0x0002)		/*!< feature not implemented */
#define VO_ERR_PLAYER_PARAM_ID_ERR		(VO_ERR_PLAYER_BASE | 0x0003)		/*!< invalid param ID */
#define VO_ERR_PLAYER_FAIL				(VO_ERR_PLAYER_BASE | 0x0004)		/*!< general error */
#define VO_ERR_PLAYER_NULL_POINTER		(VO_ERR_PLAYER_BASE | 0x0005)		/*!< pointer is NULL */


/**
* Parameter ID, used by voplayerSetParam,voplayerGetParam
*/
#define	VO_PLAYER_PID_BASE				0x52000000							/*!< The base param ID */

	
/**
* Event ID, used by VOLSPListener event callback function
*/
#define	VO_PLAYER_EID_BASE				0x53000000							/*!< The base event ID */
	
	
/**
*Call back function. Update the status from PLAYER
* \param nID [in] the id of special event, refer to VO_PLAYER_EID_BASE
* \param pParam1 [in/out] the first parameter
* \param pParam2 [in/out] the second parameter
*/
typedef int (* VOLSPListener) (void * pUserData, int nID, void * pParam1, void * pParam2);

	
/**
* PLAYER Open parameters, used by voplayerOpen
*/
typedef struct
{
	VOLSPListener			pListener;				/*!< The callback for event notify from player */
	VO_PTR					pUserData;				/*!< The user data used in VOLSPListener callback */
	VO_HANDLE				hDrawWnd;				/*!< The window's handle to draw video */
	VO_CHAR					szCfgFilePath[2014];	/*!< The config file path */
}VO_PLAYER_OPEN_PARAM;

	
/**
 * The input type of send buffer.
 */
typedef enum{
	VO_BUFFER_INPUT_VIDEO	=	0,	/*!< Video frame*/
	VO_BUFFER_INPUT_AUDIO	,		/*!< Audio frame*/
	VO_BUFFER_INPUT_STREAM	,		/*!< stream(such as TS) frame*/
}VO_BUFFER_INPUT_TYPE;

	
/**
* PLAYER function set
*/
typedef struct
{
	/**
	* Initialize a Player session.
	* This function should be called first for a session.
	* \param phPlayer [out] PLAYER handle.
	* \param pOpenParam [in] PLAYER open parameters
	* \retval VO_ERR_PLAYER_OK Succeeded 
	*/
	VO_U32 (VO_API * voplayerOpen) (VO_HANDLE* phPlayer, VO_PLAYER_OPEN_PARAM* pOpenParam);

	
	/**
	* Close the PLAYER session.
	* This function should be called last of the session.
	* \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	* \retval VO_ERR_PLAYER_OK Succeeded
	*/
	VO_U32 (VO_API * voplayerClose) (VO_HANDLE hPlayer);


	/**
	* Set the video draw area in the view window
	* \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	* \param nLeft [in] The left position of draw video area
	* \param nTop [in] The top position of draw video area
	* \param nRight [in] The right position of draw video area
	* \param nBottom [in] The bottom position of draw video area
	* \retval VO_ERR_PLAYER_OK Succeeded 
	*/
	VO_U32 (VO_API * voplayerSetDrawArea) (VO_HANDLE hPlayer, VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom);

	
	/**
	 * Set source URL
	 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	 * \param pSource [in] The Data source URL
	 * \retval VO_ERR_PLAYER_OK Succeeded
	 */
	VO_U32 (VO_API * voplayerSetDataSource) (VO_HANDLE hPlayer, VO_PTR pSource);

	/**
	 * Send the buffer into player
	 * \param hPlayer [in]  The PLAYER handle. Opened by voplayerOpen().
	 * \param bVideo [in] indicator of buffer type
	 * \param pBuffer [in] It is audio or video stream buffer.
	 * \retval VO_ERR_PLAYER_OK Succeeded
	 */
	VO_U32 (VO_API * voplayerSendBuffer) (VO_HANDLE hPlayer, VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);

	
	/**
	* start to play
	* \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	* \retval VO_ERR_PLAYER_OK Succeeded
	*/
	VO_U32 (VO_API * voplayerRun) (VO_HANDLE hPlayer);

	
	/**
	 * pause
	 * \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	 * \retval VO_ERR_PLAYER_OK Succeeded
	 */
	VO_U32 (VO_API * voplayerPause) (VO_HANDLE hPlayer);

	/**
	* stop playing
	* \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	* \retval VO_ERR_PLAYER_OK Succeeded 
	*/
	VO_U32 (VO_API * voplayerStop) (VO_HANDLE hPlayer);
	
	/**
	 * Get the status of player SDK
	 * \param hPlayer [in] The handle.which was created by vompInit.
	 * \param pStatus [out] The status of player.
	 * \retval VO_ERR_PLAYER_OK Succeeded
	 */
	VO_U32 (VO_API * voplayerGetStatus) (VO_HANDLE hPlayer, VOMP_STATUS * pStatus);
	
	/**
	* Get the special value from param ID
	* \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	* \param nParamID [in] The param ID
	* \param pParam [out] The return value depend on the param ID.
	* \retval VO_ERR_PLAYER_OK Succeeded
	*/
	VO_U32 (VO_API * voplayerGetParam) (VO_HANDLE hPlayer, VO_U32 nParamID, VO_PTR pParam);


	/**
	* Set the special value from param ID
	* \param hPlayer [in] The PLAYER handle. Opened by voplayerOpen().
	* \param nParamID [in] The param ID
	* \param pParam [in] The set value depend on the param ID.
	* \retval VO_ERR_PLAYER_OK Succeeded 
	*/
	VO_U32 (VO_API * voplayerSetParam) (VO_HANDLE hPlayer, VO_U32 nParamID, VO_PTR pParam);
} VO_PLAYER_API;


/**
* Get Live Streaming Player API interface
* \param pHandle [IN/OUT] Return the player API handle.
* \retval VO_ERR_PLAYER_OK Succeeded.
*/
VO_U32 VO_API voGetLSPAPI(VO_PLAYER_API* pHandle);

typedef VO_U32 (VO_API * VOGETLSPAPI) (VO_PLAYER_API* pHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_LSP_API_H_ */


