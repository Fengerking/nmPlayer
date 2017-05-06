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
#ifndef __VO_IOMX_DEC_H__
#define __VO_IOMX_DEC_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "voVideo.h"

typedef	void *	VO_IOMX_DATA;	// decoded data handle

#define	VO_PID_IOMXDEC_BASE				 0x41200000
#define VO_PID_IOMXDEC_SetSurface		(VO_PID_IOMXDEC_BASE | 0x00001) /*!<Set the surface. eclair, froyo, GB Surface*, HC, ICS ANativeWindow* */
#define VO_PID_IOMXDEC_RenderData		(VO_PID_IOMXDEC_BASE | 0x00002)	/*!<Render data. VO_VIDEO_BUFFER* */
#define VO_PID_IOMXDEC_CancelData		(VO_PID_IOMXDEC_BASE | 0x00003)	/*!<Cancel data. VO_VIDEO_BUFFER* */
#define VO_PID_IOMXDEC_ForceOutputAll	(VO_PID_IOMXDEC_BASE | 0x00004)	/*!<Force output all frames in IOMX decoder. NULL parameter */
#define VO_PID_IOMXDEC_IsWorking		(VO_PID_IOMXDEC_BASE | 0x00005)	/*!<query if IOMX working or not, VO_BOOL* 
																			if working, we need recreate IOMX decoder if BA happens	*/
#define VO_PID_IOMXDEC_SetCrop			(VO_PID_IOMXDEC_BASE | 0x00006)	/*!<Set Crop. VO_RECT* */
#define VO_PID_IOMXDEC_PROBE			(VO_PID_IOMXDEC_BASE | 0x00007)	/*!<IOMX hardware codec probe* */



#define VO_ERR_IOMXDEC_BASE						(VO_ERR_BASE | 0x1200000)
#define VO_ERR_IOMXDEC_PlatformUnsupported		(VO_ERR_IOMXDEC_BASE | 0x00001)	/*!<OS, Hardware platform unsupported */
#define VO_ERR_IOMXDEC_CodecUnsupported			(VO_ERR_IOMXDEC_BASE | 0x00002)	/*!<Codec, profile, level, resolution, settings unsupported */
#define VO_ERR_IOMXDEC_TimestampOrder			(VO_ERR_IOMXDEC_BASE | 0x00003)	/*!<Timestamp order error, DTS/PTS */
#define VO_ERR_IOMXDEC_Hardware					(VO_ERR_IOMXDEC_BASE | 0x00004)	/*!<After OMX_EventError, all SetInputData/GetOutputData return VO_ERR_IOMXDEC_Hardware */
#define VO_ERR_IOMXDEC_NeedRetry				(VO_ERR_IOMXDEC_BASE | 0x00005) /*!<currently no valid buffer, but you can retry then get valid buffer 
																					if from IOMXDecSetInputData, we need call IOMXDecGetOutputData and retry IOMXDecSetInputData to same buffer again
																					if from IOMXDecGetOutputData, we need call IOMXDecSetInputData then retry	*/

#define VO_COLOR_IOMX_PRIVATE			0x7F000001

// GetOutputData
// VO_VIDEO_BUFFER::Buffer[0] : VO_IOMX_DATA
// VO_VIDEO_BUFFER::ColorType : VO_COLOR_IOMX_PRIVATE

/**
* Get IOMX video decoder API interface
* \param pDecHandle [OUT] return the IOMX video decoder API handle.
* \param uFlag [IN] reserved
* \retval VO_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetIOMXDecAPI(VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif	//__VO_IOMX_DEC_H__
