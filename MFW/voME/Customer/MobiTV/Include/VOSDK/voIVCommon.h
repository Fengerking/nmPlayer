	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voIVCommon.h

	Contains:	Image and Video type define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voIVCommon_H__
#define __voIVCommon_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voType.h"

/**
 * Defination of color format
 */
typedef enum
{
	VO_COLOR_YUV_PLANAR444			= 0,		/*!< YUV planar mode:444  vertical sample is 1, horizontal is 1  */
	VO_COLOR_YUV_PLANAR422_12		= 1,		/*!< YUV planar mode:422, vertical sample is 1, horizontal is 2  */
	VO_COLOR_YUV_PLANAR422_21		= 2,		/*!< YUV planar mode:422  vertical sample is 2, horizontal is 1  */
	VO_COLOR_YUV_PLANAR420			= 3,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VO_COLOR_YUV_PLANAR411			= 4,		/*!< YUV planar mode:411  vertical sample is 1, horizontal is 4  */
	VO_COLOR_YUV_PLANAR411V			= 37,		/*!< YUV planar mode:411  vertical sample is 4, horizontal is 1  */
	VO_COLOR_GRAY_PLANARGRAY		= 5,		/*!< gray planar mode, just Y value								 */
	VO_COLOR_YUYV422_PACKED			= 6,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 U0 Y1 V0  */
	VO_COLOR_YVYU422_PACKED			= 7,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y0 V0 Y1 U0  */
	VO_COLOR_UYVY422_PACKED			= 8,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y0 V0 Y1  */
	VO_COLOR_VYUY422_PACKED			= 9,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y0 U0 Y1  */
	VO_COLOR_YUV444_PACKED			= 10,		/*!< YUV packed mode:444, vertical sample is 1, horizontal is 1, data: Y U V	*/
	VO_COLOR_YUV_420_PACK			= 11, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, UV Packed*/
	VO_COLOR_YUV_420_PACK_2			= 35, 		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  , Y planar, VU Packed*/
	VO_COLOR_YVU_PLANAR420			= 12,
	VO_COLOR_YVU_PLANAR422_12		= 13,
	VO_COLOR_YUYV422_PACKED_2		= 14,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 U0 Y0 V0  */
	VO_COLOR_YVYU422_PACKED_2		= 15,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: Y1 V0 Y0 U0  */
	VO_COLOR_UYVY422_PACKED_2		= 16,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: U0 Y1 V0 Y0  */
	VO_COLOR_VYUY422_PACKED_2		= 17,		/*!< YUV packed mode:422, vertical sample is 1, horizontal is 2, data: V0 Y1 U0 Y0  */
	VO_COLOR_RGB565_PACKED			= 30,		/*!< RGB packed mode, data: B:5 G:6 R:5   						 */
	VO_COLOR_RGB555_PACKED			= 31,		/*!< RGB packed mode, data: B:5 G:5 R:5   						 */
	VO_COLOR_RGB888_PACKED			= 32,		/*!< RGB packed mode, data: B G R		 						 */
	VO_COLOR_RGB32_PACKED			= 33,		/*!< RGB packed mode, data: B G R A								 */
	VO_COLOR_RGB888_PLANAR			= 34,		/*!< RGB planar mode											 */
	VO_COLOR_YUV_PLANAR420_NV12		= 36,		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
	VO_COLOR_TYPE_MAX				= VO_MAX_ENUM_VALUE
}  VO_IV_COLORTYPE;


/**
 * Defination of rect structurel
 */
typedef struct _VO_RECT
{
	VO_S32	left;		/*!< Left  */
	VO_S32	top;		/*!< top  */
	VO_S32	right;		/*!< right  */
	VO_S32	bottom;		/*!< bottom  */
} VO_RECT;


/**
 * Defination of bitmap info header structure
 */
typedef struct _VO_BITMAPINFOHEADER
{
	VO_U32	biSize;					/*!< the size of the structure  */
	VO_S32	biWidth;				/*!< image with  */
	VO_S32	biHeight;				/*!< image height  */
	VO_U16	biPlanes;				/*!< plane num  */
	VO_U16	biBitCount;				/*!< bits count  */
	VO_U32	biCompression;			/*!< compression  */
	VO_U32	biSizeImage;			/*!< size of image  */
	VO_S32	biXPelsPerMeter;		/*!< X peles  */
	VO_S32	biYPelsPerMeter;		/*!< Y peles  */
	VO_U32	biClrUsed;				/*!< color used  */
	VO_U32	biClrImportant;			/*!< important  */
} VO_BITMAPINFOHEADER;



/**
 * Defination of Encoder quality level
 */
typedef enum
{
	VO_ENC_LOW_QUALITY      = 0,   /*!< low quality */
	VO_ENC_MID_QUALITY      = 1,   /*!< middle quality */
	VO_ENC_HIGH_QUALITY     = 2,    /*!< high quality */
	VO_ENC_QUALITY_MAX		= VO_MAX_ENUM_VALUE
}VO_IV_QUALITY;

/**
 * Video data buffer, usually used as iutput or output of video codec.
 */
typedef struct
{
	VO_PBYTE 			Buffer[3];			/*!< Buffer pointer */
	VO_S32				Stride[3];			/*!< Buffer stride */
	VO_IV_COLORTYPE		ColorType;			/*!< Color Type */
	VO_S64				Time;				/*!< The time of the buffer */
} VO_VIDEO_BUFFER;

/*!
* Defination of rotation type
*/
typedef enum
{
	VO_RT_DISABLE		= 0X00,  /*!< No rotation		*/
	VO_RT_90L			= 0X01,  /*!< Left rotation 90	*/
	VO_RT_90R			= 0X02,  /*!< Right rotation 90	*/
	VO_RT_180			= 0X04,  /*!< Left rotation 180	*/
	VO_RT_FLIP_Y		= 0X10,  /*!< Y direction flip	*/
	VO_RT_FLIP_X		= 0X20,  /*!< X direction flip	*/
	VO_RT_MAX			= VO_MAX_ENUM_VALUE
}VO_IV_RTTYPE;

/*!
* Defination of aspect ratio type
*/
typedef enum
{
	VO_RATIO_11			= 0X01,  /*!< Width and Height is 1 : 1		*/
	VO_RATIO_43			= 0X02,  /*!< Width and Height is 4 : 3		*/
	VO_RATIO_169		= 0X03,  /*!< Width and Height is 16 : 9	*/
	VO_RATIO_21			= 0X04,  /*!< Width and Height is 2 : 1		*/
	VO_RATIO_MAX		= VO_MAX_ENUM_VALUE
}VO_IV_ASPECT_RATIO;

/*!
* Defination of aspect ratio type
*/
typedef enum
{
	VO_ZM_LETTERBOX		= 0X01,  /*!< the xoom mode is letter box		*/
	VO_ZM_PANSCAN		= 0X02,  /*!< the xoom mode is pan scan			*/
	VO_ZM_FITWINDOW		= 0X03,  /*!< the xoom mode is fit to window	*/
	VO_ZM_ORIGINAL		= 0X04,  /*!< the xoom mode is original size	*/
	VO_ZM_MAX			= VO_MAX_ENUM_VALUE
}VO_IV_ZOOM_MODE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voIVCommon_H__
