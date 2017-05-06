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

#ifndef __voVideo_H__
#define __voVideo_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "voIVCommon.h"
#include "voMem.h"

#define	VO_PID_VIDEO_BASE			 0x41000000							/*!< The base param ID for video codec */
#define	VO_PID_VIDEO_FORMAT			(VO_PID_VIDEO_BASE | 0X0001)		/*!< Get / Set VO_VIDEO_FORMAT */
#define	VO_PID_VIDEO_QUALITY		(VO_PID_VIDEO_BASE | 0X0002)		/*!< Set enc quality. VO_IV_QUALITY */
#define	VO_PID_VIDEO_VIDEOMEMOP		(VO_PID_VIDEO_BASE | 0X0003)		/*!< Get / Set video memory operator. VO_MEM_VIDEO_OPERATOR */
#define	VO_PID_VIDEO_XDOWNSAMPLE	(VO_PID_VIDEO_BASE | 0X0004)		/*!< Video decoder X downsample */
#define	VO_PID_VIDEO_YDOWNSAMPLE	(VO_PID_VIDEO_BASE | 0X0005)		/*!< Video decoder Y downsample */
#define	VO_PID_VIDEO_ASPECTRATIO	(VO_PID_VIDEO_BASE | 0X0006)		/*!< Get video Aspect Ratio VO_IV_ASPECT_RATIO*/
#define	VO_PID_VIDEO_OUTPUTFRAMES	(VO_PID_VIDEO_BASE | 0X0007)		/*!< Set video output frames. VO_U32  */
#define	VO_PID_VIDEO_FRAMETYPE		(VO_PID_VIDEO_BASE | 0X0008)		/*!< Get the frame type with input buffer. VO_CODECBUFFER, userdata should fill frame type */
#define	VO_PID_VIDEO_BITRATE		(VO_PID_VIDEO_BASE | 0X0009)		/*!< The bitrate of video VO_U32* */
#define	VO_PID_VIDEO_DATABUFFER		(VO_PID_VIDEO_BASE | 0X000A)		/*!< The bitrate of video VO_VIDEO_BUFFER* */
#define	VO_PID_VIDEO_OUTPUTMODE		(VO_PID_VIDEO_BASE | 0X000B)		/*!< get/set the video buffer output mode. int * 0, display seq. 1, output immediately */
#define VO_PID_VIDEO_UPSIDEDOWN		(VO_PID_VIDEO_BASE | 0X000C)		/*!< check is the video upside-down*/
#define VO_PID_VIDEO_ParamVideoAvc	(VO_PID_VIDEO_BASE | 0X000D)		/*!< Get / Set OMX_VIDEO_PARAM_AVCTYPE */
#define VO_PID_VIDEO_DRM_FUNC		(VO_PID_VIDEO_BASE | 0X000E)		/*!< Set the DRM function pointer Param void * depend the DRM type. */
#define VO_PID_VIDEO_THUMBNAIL_MODE	(VO_PID_VIDEO_BASE | 0X000F)		/*!< Set Thumbnail mode, VO_BOOL */
#define VO_PID_VIDEO_S3D			(VO_PID_VIDEO_BASE | 0X0010)		/*!< Get S3D_params for 3D clip */
#define VO_PID_VIDEO_DIMENSION		(VO_PID_VIDEO_BASE | 0X0011)		/*!< Get decoded demesion */
#define VO_PID_VIDEO_MAXENCODERFRAMESIZE  (VO_PID_VIDEO_BASE | 0X0012)  /*!< Get max encoder frame size */
#define VO_PID_VIDEO_GETTHUMBNAIL	(VO_PID_VIDEO_BASE | 0X0013)  /*!< Indicate this instance is for getting the thumbnail */
/**
 * Video Coding type
 */
typedef enum VO_VIDEO_CODINGTYPE {
    VO_VIDEO_CodingUnused,     /*!< Value when coding is N/A */
    VO_VIDEO_CodingMPEG2,      /*!< AKA: H.262 */
    VO_VIDEO_CodingH263,       /*!< H.263 */
    VO_VIDEO_CodingS263,       /*!< H.263 */
    VO_VIDEO_CodingMPEG4,      /*!< MPEG-4 */
    VO_VIDEO_CodingH264,       /*!< H.264/AVC */
    VO_VIDEO_CodingWMV,        /*!< all versions of Windows Media Video */    
    VO_VIDEO_CodingRV,         /*!< all versions of Real Video */
    VO_VIDEO_CodingMJPEG,      /*!< Motion JPEG */
    VO_VIDEO_CodingDIVX,	   /*!< DIV3 */
    VO_VIDEO_CodingVP6,		   /*!< VP6 */
		VO_VIDEO_CodingVP8,		   /*!< VP8 */
		VO_VIDEO_CodingVP7,        /*!< VP7 */
		VO_VIDEO_CodingVC1,         /*VC1:WMV3,WMVA,WVC1*/
    VO_VIDEO_CodingH265,
    VO_VIDEO_Coding_Max		= VO_MAX_ENUM_VALUE
} VO_VIDEO_CODINGTYPE;


/**
 * Video Info Header Structure
 */
typedef struct _VO_VIDEOINFOHEADER
{
	VO_RECT				rcSource;          /*!< The bit we really want to use */
	VO_RECT				rcTarget;          /*!< Where the video should go */
	VO_U32				dwBitRate;         /*!< Approximate bit data rate */
	VO_U32				dwBitErrorRate;    /*!< Bit error rate for this stream */
	VO_S64				AvgTimePerFrame;   /*!< Average time per frame (100ns units) */
	VO_BITMAPINFOHEADER	bmiHeader;
} VO_VIDEOINFOHEADER;


/**
 * Frame type
 */
typedef enum
{
	VO_VIDEO_FRAME_I                = 0,   /*!< I frame */
	VO_VIDEO_FRAME_P                = 1,   /*!< P frame */
	VO_VIDEO_FRAME_B                = 2,   /*!< B frame */
	VO_VIDEO_FRAME_S                = 3,   /*!< S frame */
	VO_VIDEO_FRAME_NULL             = 4,   /*!< NULL frame*/
    VO_VIDEO_FRAMETYPE_MAX			= VO_MAX_ENUM_VALUE
}
VO_VIDEO_FRAMETYPE;

/**
 * General video format info
 */
typedef struct
{
	VO_S32				Width;		 /*!< Width */
	VO_S32				Height;		 /*!< Height */
	VO_VIDEO_FRAMETYPE	Type;		/*!< Frame type, such as I frame, P frame */
} VO_VIDEO_FORMAT;

/**
 * General video dimension info
 */
typedef struct
{
	VO_S32				nTop;
	VO_S32				nLeft;
	VO_U32				nWidth;		 /*!< Width */
	VO_U32				nHeight;		 /*!< Height */
} VO_VIDEO_DECDIMENSION;

typedef void* voANativeWindow;
/**
 * General video output info
 */
#define	VO_VIDEO_OUTPUT_MORE		0X00000001
#define	VO_VIDEO_OUTPUT_RETRY		0X00000002

typedef struct
{
	VO_VIDEO_FORMAT		Format;			/*!< video format */
	VO_U32				InputUsed;		/*!< input buffer used */
	VO_U32				Flag	;		/*!< the output status.  */
	VO_U32				Resevered;		/*!< resevered */
} VO_VIDEO_OUTPUTINFO;

typedef enum
{
	// MPEG2
	VO_VIDEO_MPEG2ProfileSimple = 0,			/**< Simple Profile */
	VO_VIDEO_MPEG2ProfileMain,					/**< Main Profile */
	VO_VIDEO_MPEG2Profile422,					/**< 4:2:2 Profile */
	VO_VIDEO_MPEG2ProfileSNR,					/**< SNR Profile */
	VO_VIDEO_MPEG2ProfileSpatial,				/**< Spatial Profile */
	VO_VIDEO_MPEG2ProfileHigh,					/**< High Profile */

	// H263
	VO_VIDEO_H263ProfileBaseline = 0x1000000, 
	VO_VIDEO_H263ProfileH320Coding, 
	VO_VIDEO_H263ProfileBackwardCompatible, 
	VO_VIDEO_H263ProfileISWV2, 
	VO_VIDEO_H263ProfileISWV3, 
	VO_VIDEO_H263ProfileHighCompression, 
	VO_VIDEO_H263ProfileInternet, 
	VO_VIDEO_H263ProfileInterlace, 
	VO_VIDEO_H263ProfileHighLatency, 

	// MPEG4
	VO_VIDEO_MPEG4ProfileSimple = 0x2000000, 
	VO_VIDEO_MPEG4ProfileSimpleScalable, 
	VO_VIDEO_MPEG4ProfileCore, 
	VO_VIDEO_MPEG4ProfileMain, 
	VO_VIDEO_MPEG4ProfileNbit, 
	VO_VIDEO_MPEG4ProfileScalableTexture, 
	VO_VIDEO_MPEG4ProfileSimpleFace, 
	VO_VIDEO_MPEG4ProfileSimpleFBA, 
	VO_VIDEO_MPEG4ProfileBasicAnimated, 
	VO_VIDEO_MPEG4ProfileHybrid, 
	VO_VIDEO_MPEG4ProfileAdvancedRealTime, 
	VO_VIDEO_MPEG4ProfileCoreScalable, 
	VO_VIDEO_MPEG4ProfileAdvancedCoding, 
	VO_VIDEO_MPEG4ProfileAdvancedCore, 
	VO_VIDEO_MPEG4ProfileAdvancedScalable, 
	VO_VIDEO_MPEG4ProfileAdvancedSimple, 

	// WMV
	VO_VIDEO_WMVFormatUnused = 0x3000000,		/**< Format unused or unknown */
	VO_VIDEO_WMVFormat7,						/**< Windows Media Video format 7 */
	VO_VIDEO_WMVFormat8,						/**< Windows Media Video format 8 */
	VO_VIDEO_WMVFormat9,						/**< Windows Media Video format 9 */
	VO_VIDEO_WMVFormatWVC1,						/**< Windows WVC1 */

	// RV
	VO_VIDEO_RVFormatUnused = 0x4000000,		/**< Format unused or unknown */
	VO_VIDEO_RVFormat8,							/**< Real Video format 8 */
	VO_VIDEO_RVFormat9,							/**< Real Video format 9 */
	VO_VIDEO_RVFormatG2,						/**< Real Video Format G2 */

	// AVC
	VO_VIDEO_AVCProfileBaseline = 0x5000000,	/**< Baseline profile */
	VO_VIDEO_AVCProfileMain,					/**< Main profile */
	VO_VIDEO_AVCProfileExtended,				/**< Extended profile */
	VO_VIDEO_AVCProfileHigh,					/**< High profile */
	VO_VIDEO_AVCProfileHigh10,					/**< High 10 profile */
	VO_VIDEO_AVCProfileHigh422,					/**< High 4:2:2 profile */
	VO_VIDEO_AVCProfileHigh444,					/**< High 4:4:4 profile */

	// DivX
	VO_VIDEO_DivX311 = 0x6000000,				/**< DivX 3.11 */
	VO_VIDEO_DivX4,								/**< DivX 4.x */
	VO_VIDEO_DivX5,								/**< DivX 5.x */
	VO_VIDEO_DivX6,								/**< DivX 6.x */

	VO_VIDEO_PROFILETYPE_MAX = VO_MAX_ENUM_VALUE
} VO_VIDEO_PROFILETYPE;

typedef enum
{
	// MPEG2
	VO_VIDEO_MPEG2LevelLL = 0,			/**< Low Level */
	VO_VIDEO_MPEG2LevelML,				/**< Main Level */
	VO_VIDEO_MPEG2LevelH14,				/**< High 1440 */
	VO_VIDEO_MPEG2LevelHL,				/**< High Level */

	// H263
	VO_VIDEO_H263Level10 = 0x1000000, 
	VO_VIDEO_H263Level20, 
	VO_VIDEO_H263Level30, 
	VO_VIDEO_H263Level40, 
	VO_VIDEO_H263Level45, 
	VO_VIDEO_H263Level50, 
	VO_VIDEO_H263Level60, 
	VO_VIDEO_H263Level70, 

	// MPEG4
	VO_VIDEO_MPEG4Level0 = 0x2000000,	/**< Level 0 */
	VO_VIDEO_MPEG4Level0b,				/**< Level 0b */
	VO_VIDEO_MPEG4Level1,				/**< Level 1 */
	VO_VIDEO_MPEG4Level2,				/**< Level 2 */
	VO_VIDEO_MPEG4Level3,				/**< Level 3 */
	VO_VIDEO_MPEG4Level4,				/**< Level 4 */
	VO_VIDEO_MPEG4Level4a,				/**< Level 4a */
	VO_VIDEO_MPEG4Level5,				/**< Level 5 */

	// WMV, Not Applicable

	// RV, Not Applicable

	// AVC
	VO_VIDEO_AVCLevel1 = 0x5000000,		/**< Level 1 */
	VO_VIDEO_AVCLevel1b,				/**< Level 1b */
	VO_VIDEO_AVCLevel11,				/**< Level 1.1 */
	VO_VIDEO_AVCLevel12,				/**< Level 1.2 */
	VO_VIDEO_AVCLevel13,				/**< Level 1.3 */
	VO_VIDEO_AVCLevel2,					/**< Level 2 */
	VO_VIDEO_AVCLevel21,				/**< Level 2.1 */
	VO_VIDEO_AVCLevel22,				/**< Level 2.2 */
	VO_VIDEO_AVCLevel3,					/**< Level 3 */
	VO_VIDEO_AVCLevel31,				/**< Level 3.1 */
	VO_VIDEO_AVCLevel32,				/**< Level 3.2 */
	VO_VIDEO_AVCLevel4,					/**< Level 4 */
	VO_VIDEO_AVCLevel41,				/**< Level 4.1 */
	VO_VIDEO_AVCLevel42,				/**< Level 4.2 */
	VO_VIDEO_AVCLevel5,					/**< Level 5 */
	VO_VIDEO_AVCLevel51,				/**< Level 5.1 */

	VO_VIDEO_LEVELTYPE_MAX = VO_MAX_ENUM_VALUE
} VO_VIDEO_LEVELTYPE;

typedef struct
{
	VO_VIDEO_PROFILETYPE	Profile;
	VO_VIDEO_LEVELTYPE		Level;
} VO_VIDEO_PROFILELEVEL;

/**
 * Video Decoder Function Set.
 */
typedef struct
{
	/**
	 * Init the video decoder module and return decoder handle
	 * \param phDec [OUT] Return the video decoder handle
	 * \param vType	[IN] The decoder type if the module support multi codec.
	 * \param pUserData	[IN] The init param. It is memory operator or allocated memory
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Init) (VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);

	/**
	 * Set compressed video data as input.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param pInput [IN] The input buffer param.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetInputData) (VO_HANDLE hDec, VO_CODECBUFFER * pInput);

	/**
	 * Get the uncompressed yuv video data
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param pOutBuffer [OUT] The dec module filled the buffer pointer and stride.
	 * \param pOutInfo [OUT] The dec module filled video format and used the input size.
	 *						 pOutInfo->InputUsed is total used the input size.
	 * \retval  VO_ERR_NONE Succeeded.
	 *			VO_ERR_INPUT_BUFFER_SMALL. The input was finished or the input data was not enough.
	 */
	VO_U32 (VO_API * GetOutputData) (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo);

	/**
	 * Set the param for special target.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetParam) (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Get the param for special target.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [OUT] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Uninit the decoder.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Uninit) (VO_HANDLE hDec);
} VO_VIDEO_DECAPI;


/**
 * Video Encoder Function Set.
 */
typedef struct
{
	/**
	 * Init the video encoder module and return encoder handle
	 * \param phEnc [OUT] Return the video encoder handle
	 * \param vType	[IN] The encoder type if the module support multi codec.
	 * \param pUserData	[IN] The init param. It is memory operator or allocated memory
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Init) (VO_HANDLE * phEnc,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);

	/**
	 * Encode the raw video data.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param pInput [IN] The input buffer param.
	 * \param pOutput [Out] The compressed video data.
	 * \param pType [Out] The video frame type.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Process) (VO_HANDLE hDec, VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType);

	/**
	 * Set the param for special target.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetParam) (VO_HANDLE hEnc, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Get the param for special target.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [OUT] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_HANDLE hEnc, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Uninit the encoder.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Uninit) (VO_HANDLE hEnc);
} VO_VIDEO_ENCAPI;


/**
 * Video Render CallBack function. It was called before render video.
 * \param pUserData [IN]] The user data pointer which was set by caller.
 * \param pVideoBuffer [IN] The video buffer info.
 * \param pVideoSize [IN] The video size.
 * \param nStart [IN] The video time.
 * \retval VO_ERR_NONE Succeeded.
 *		   VO_ERR_FINISH, the render will not render video.
 */
typedef VO_S32 (VO_API * VOVIDEOCALLBACKPROC) (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize, VO_S32 nStart);

#ifdef __cplusplus
}
#endif

#endif
