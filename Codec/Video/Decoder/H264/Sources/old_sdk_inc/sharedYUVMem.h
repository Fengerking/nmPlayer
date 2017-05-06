	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		viMem.h

	Contains:	base function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef		 __SHAREDYUVMEM_H__
#define	__SHAREDYUVMEM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "defines.h"
#include "sdkbase.h"
typedef struct
{
	unsigned char*		 					Buffer[3];			/*!< Buffer pointer */
	long								Stride[3];			/*!< Buffer stride */
	VOVIDEOTYPE			ColorType;			/*!< Color Type */
	avdInt64							UserData;			/*!< user data pointer */
} VOVIDEO_BUFFER;
typedef struct
{
	/**IN PARAM*/
	long				Stride;				/*!< Buffer Stride */
	long				Height;				/*!< Buffer Height */
	VOVIDEOTYPE		ColorType;			/*!< Color Type,default:YUV_PLANAR420 */
	long				FrameCount;			/*!< Frame counts */
	long				Flag;				/*!< reserved: for special use */
	/**OUT PARAM*/
	/**OUT PARAM*/
	VOVIDEO_BUFFER *	VBuffer;			/*!<virtual Address Buffer array. */
	VOVIDEO_BUFFER *	PBuffer;			/*!<reserved: physical Address Buffer array.*/
	
}
VOMEM_VIDEO_INFO;
enum{
	VOMEMRC_CURBUF_ISLOCKED		=-2232,/*!< Hex:0xF748,indicates the buffer is locked by render*/
	VOMEMRC_NO_YUV_BUFFER			=-2233/*!< Hex:0xF747,when caller get the return,the usual handle is to call GetParam(VOID_COM_VIDEO_SHAREDMEMWAITOUTPUT)*/
};
enum{
	VOID_COM_VIDEO_SHAREDMEMORYOPERATOR	= VOID_COM_BASE | 0x0111,   /*!< SharedVideo memory operation for frame buffer,the value is a pointer to VOMEM_VIDEO_OPERATOR*/
	VOID_COM_VIDEO_SHAREDMEMWAITOUTPUT	= VOID_COM_BASE | 0x0112   /*!< Wait for the output data due to the lack of available buffer,the value is a  pointer to VIDEOBUFFER*/

};
/**
CODEC ID
*/
enum
{
	VOVIDEO_CodingUnused,     /**< Value when coding is N/A */
	VOVIDEO_CodingMPEG2,      /**< AKA: H.262 */
	VOVIDEO_CodingH263,       /**< H.263 */
	VOVIDEO_CodingS263,       /**< H.263 */
	VOVIDEO_CodingMPEG4,      /**< MPEG-4 */
	VOVIDEO_CodingH264,       /**< H.264/AVC */
	VOVIDEO_CodingWMV,        /**< all versions of Windows Media Video */
	VOVIDEO_CodingRV,         /**< all versions of Real Video */
	VOVIDEO_CodingMJPEG,      /**< Motion JPEG */
	VOVIDEO_CodingDIVX,	   /**< DIV3 */
	VOVIDEO_CodingMax = 0x7FFFFFFF
};
typedef struct
{	
/**
 * Init the Shared Memory Operator 
 * \param uID [in] CODEC ID
 * \param pVideoMem[in/out] video memory
 * \retval VORC_OK Succeeded.
 */

	unsigned long (VOAPI * Init) (long uID, VOMEM_VIDEO_INFO * pVideoMem);
 /**
 * Get the buffer by Index 
 * \param uID [in] CODEC ID
 * \param nIndex[in] index of the shared buffers,the range is [0..FrameCount-1]
 * \retval VORC_OK Succeeded.
			   VOMEMRC_CURBUF_ISLOCKED:the buffer is locked by render
 */
	unsigned long (VOAPI * GetBufByIndex) (long uID, long nIndex);
/**
 * UnInit the Shared Memory Operator 
 * \param uID [in] CODEC ID
 * \retval VORC_OK Succeeded.
 */
	unsigned long (VOAPI * Uninit) (long uID);
} VOMEM_VIDEO_OPERATOR;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __SHAREDYUVMEM_H__
