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

#ifndef __viMem_H__
#define __viMem_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voIndex.h>
#include <voVideo.h>

/**
 * Video Memory info structure.
 */
typedef struct
{
	/**IN PARAM*/
	VO_S32				Stride;				/*!< Buffer Stride */
	VO_S32				Height;				/*!< Buffer Height */
	VO_IV_COLORTYPE		ColorType;			/*!< Color Type,default:YUV_PLANAR420 */
	VO_S32				FrameCount;			/*!< Frame counts */
	VO_S32				Flag;				/*!< reserved: for special use */
	/**OUT PARAM*/
	VO_VIDEO_BUFFER *	VBuffer;			/*!<virtual Address Buffer array. */
	VO_VIDEO_BUFFER *	PBuffer;			/*!<reserved: physical Address Buffer array.*/
} VO_MEM_VIDEO_INFO;


/**
 * Video Memory operator function set.
 */
typedef struct
{
	/**
	 * Init the video memory with requirement.
	 * \param uID [IN] The caller module ID>
	 * \param pVideoMem	[IN] The video memory requirement.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Init) (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem);

	/**
	 * Get available video buffer by index
	 * \param uID [IN] The caller module ID>
	 * \param nIndex [IN] The index of the video buffer
	 * \retval VO_ERR_NONE is available, other is not available..
	 */
	VO_U32 (VO_API * GetBufByIndex) (VO_S32 uID, VO_S32 nIndex);

	/**
	 * Uninit and release the video memory
	 * \param uID [IN] The caller module ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Uninit) (VO_S32 uID);
} VO_MEM_VIDEO_OPERATOR;

/**
When calling GetOutputData,the decoder engine may return the following value because of shared memory are all
locked
*/
enum{
	VO_MEMRC_NO_YUV_BUFFER			=-2233/*!< Hex:0xF747,when caller get the return,the usual handle is to go on calling OutPutData*/
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __viMem_H__
