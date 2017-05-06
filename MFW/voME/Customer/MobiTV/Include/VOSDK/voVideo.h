/** 
 * \file voVideo.h
 * \brief VisualOn Video type and function define header file
 * \version 0.1
 * \date 7/07/2009 updated, ver 0.1
 * \date 3/10/2009 created
*/

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
    VO_VIDEO_CodingVP6,		  /*!< VP6 */
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
 * General video output info
 */
typedef struct
{
	VO_VIDEO_FORMAT		Format;			/*!< video format */
	VO_U32				InputUsed;		/*!< input buffer used */
	VO_U32				Resevered;		/*!< resevered */
} VO_VIDEO_OUTPUTINFO;

/**
 * Video Deocder Function Set.
 */
typedef struct
{
	/**
	 * Init the video decoder module and return decorder handle
	 * \param phDec [OUT] Return the video decoder handle
	 * \param vType	[IN] The deocder type if the module support multi codec.
	 * \param pUserData	[IN] The init param. It is memory operator or alloced memory
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
	 *			VO_ERR_INPUT_BUFFER_SMALL. The input was finished or the input data was not enought.
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
	 * \param pUserData	[IN] The init param. It is memory operator or alloced memory
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Init) (VO_HANDLE * phEnc,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);

	/**
	 * Encode the raw video data.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param pInput [IN] The input buffer param.
	 * \param pOutput [Out] The compressed video data.
	 * \param pType [Out] The videoframe type.
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
 * \param pUserData [IN]] The user data pointer which was setted by caller.
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
