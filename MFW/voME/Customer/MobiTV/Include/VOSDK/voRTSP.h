	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voRTSP.h

	Contains:	Video type and function define header file

	Written by:	Number Huang

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voRTSP_H__
#define __voRTSP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "voStreaming.h"
#include <voFile.h>

/**
The 3 param ID is for control to the RTSPEngine.
After open url successfully,user can play/pause/stop the engine via
SetParam(VO_RTSP_CONTROL_ID,...)
*/
typedef enum
{
	VO_RTSP_STOP				= 0x43000010, 
	VO_RTSP_PAUSE				= 0x43000012, 
	VO_RTSP_PLAY				= 0x43000013, 
}VO_RTSP_CONTROL_ID;
/**
 * Get File Read API interface
 * \param pReadHandle [out] Return the RTSP file read handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetRTSPReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voRTSP_H__
