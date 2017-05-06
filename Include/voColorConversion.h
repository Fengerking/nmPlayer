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


#ifndef __VO_COLOR_CONVERSION_H_
#define __VO_COLOR_CONVERSION_H_

#include <voImage.h>
#include <voVideo.h>
#include <viMem.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)


/**
 * Color Conversion specific return code 
 * \see VOCOMMONRETURNCODE
 */
#define VO_ERR_CC_BASE              VO_ERR_BASE | VO_INDEX_SNK_CCRRV

typedef enum
{
	VO_ERR_CC_UNSUPORT_INTYPE	= VO_ERR_CC_BASE | 0x0001,  /*!< Color Conversion unsuport input type */
	VO_ERR_CC_UNSUPORT_OUTTYPE	= VO_ERR_CC_BASE | 0x0002,  /*!< Color Conversion unsuport output type */
	VO_ERR_CC_UNSUPORT_ROTATION	= VO_ERR_CC_BASE | 0x0003,  /*!< Color Conversion unsuport rotation type*/
	VO_ERR_CC_UNSUPORT_RESIZE  	= VO_ERR_CC_BASE | 0x0004,  /*!< Color Conversion unsuport resize*/
	VO_ERR_CC_UNINI			  	= VO_ERR_CC_BASE | 0x0005,  /*!< Color Conversion uninitial error*/
	VO_ERR_CC_PARAMETER_ERR	  	= VO_ERR_CC_BASE | 0x0006,  /*!< Error input parameters*/
	VO_ERR_MAX                  = VO_MAX_ENUM_VALUE
}VOCCRETURNCODE;
/*!
* Defination of buffer allocated by CC lib.This is used for GPU architecture.  
*
*/
typedef struct{
	VO_S32		inHeight;/*!<[IN] input buffer height in pixel */
	VO_S32		inWidth;/*!<[IN] input buffer width in pixel */
	VO_BYTE*	inBuffer;/*!<[OUT] input buffer */
	VO_S32		outHeight;/*!<[IN] output buffer height in pixel of the largest screen*/
	VO_S32		outWidth;/*!<[IN] output buffer width in pixel of the largest screen*/
	VO_BYTE*	outBuffer;/*!<[OUT] output buffer*/	
	VO_S32		outWidth2;/*!<[OUT] if it is set non-zero value,that means there is another outbuffer*/
	VO_BYTE*	outBuffer2;/*!<[OUT] if it is set non-zero value,that means it is uesed */
}BufferByCC;
/**
 * Color conversion specific parameter id 
 * \see VOCOMMONPARAMETERID
*/
#define VO_PID_CC_BASE            VO_PID_COMMON_BASE | VO_INDEX_SNK_CCRRV
typedef enum
{
	VO_PID_CC_INIINSTANCE	= VO_PID_CC_BASE | 0x0001,  /*!<it will initialize the color conversion instance,the parameter is point of ClrConvData */
	VO_PID_CC_CONVPARM		= VO_PID_CC_BASE | 0x0002,  /*!<Set conversion matrix,the parameter is CONVPARM */
	VO_PID_CC_CONERTMATRIX	= VO_PID_CC_BASE | 0x0003,  /*!<Custom YUV to RGB conversion matrix, the parameter point of conversion matrix */	
	VO_PID_CC_FRAMEDONE		= VO_PID_CC_BASE | 0x0004,  /*!<indicate one Frame is done,it is used for GPU architecture,the parameter is Boolean(1/0)*/		
	VO_PID_CC_ALLOCBUF		= VO_PID_CC_BASE | 0x0005,  /*!<have the lib to allocate the in/out buffer,it is used for GPU architecture,the parameter is point of BufferByCC*/
	VO_PID_MAX              = VO_MAX_ENUM_VALUE
}
VOCCPARAMETERID;

typedef enum
{
	ITU_R_BT_601	        = 0,  /*!< ITU R BT 601 */
	ITU_R_BT_709			= 1,  /*!< ITU R BT 709 */
	VO_CONVPARM_MAX         = VO_MAX_ENUM_VALUE
}CONVPARM;

/*!
* Defination of rotation type of output  
*
*/
typedef enum
{
	ROTATION_DISABLE	    = 0X00,  /*!< No rotation */
	ROTATION_90L		    = 0X01,  /*!< Left rotation 90 */
	ROTATION_90R            = 0X02,  /*!< Right rotation 90 */
	ROTATION_180            = 0X04,  /*!< Rotation 180*/
	FLIP_Y                  = 0X10,   /*!< Flip Y */
	FLIP_X                  = 0X20,   /*!< Flip X */
	VO_ROTATIONTYPE_MAX     = VO_MAX_ENUM_VALUE
}ROTATIONTYPE;

/*!
* Defination of anti-aliasing level  
*
*/
typedef enum
{
	NO_ANTIALIASING			 = 0,  /*!< No anti-aliasing */
	LOW_ANTIALIASING		 = 1,  /*!< low level anti-aliasing */
	HIGH_ANTIALIASING        = 2,  /*!< high level anti-aliasing */
	THREE_ANTIALIASING       = 3,  /*!< fast anti-aliasing */
	VO_ANTI_ALIASING_MAX     = VO_MAX_ENUM_VALUE
}ANTI_ALIASING;
			     
/*!
* Defination of color conversion configure struction
*
*/ 
typedef struct {
	VO_IV_COLORTYPE		nInType;              /*!< Input type */	
	VO_S32				nInWidth;             /*!< Input width(image size) */
	VO_S32				nInHeight;            /*!< Input height(image size) */
	VO_S32				nInStride;            /*!< Input memory stride(byte unit) */
	VO_U8	            *pInBuf[3];           /*!< Input memory address */			
	VO_IV_COLORTYPE		nOutType;             /*!< Output type */  
	VO_S32              nOutWidth;            /*!< Output width(image size) */
	VO_S32              nOutHeight;           /*!< Output height(image size) */
	VO_S32				nOutStride;           /*!< Output memory stride(byte unit) */
	VO_U8	            *pOutBuf[3];          /*!< Output memory address */			
	VO_S32				nIsResize;            /*!< Do scaling flag, 0-don't do scaling 1-do scaling */
	ROTATIONTYPE        nRotationType;        /*!< Output rotation type */
	ANTI_ALIASING       nAntiAliasLevel;      /*!< Anti-aliasing level */
	VO_S32              nInUVStride;          /*!< Input memory uvstride(byte unit)*/
	VO_S32              nOutUVStride;          /*!< Output memory uvstride(byte unit)*/
	VO_U8               *mb_skip;             /*!< mb skip pointer*/
} ClrConvData;					     


typedef struct
{
	/**
	* Init the image decoder module and return decorder handle
	* \param phDec [OUT] Return the video decoder handle
	* \param vType	[IN] The image coding type if the module support multi codec.
	* \param pUserData	[IN] The init param. It is memory operator or alloced memory
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * CCInit) (VO_HANDLE * hCodec,VO_CODEC_INIT_USERDATA * pUserData);

	/**
	* Decoder the comprossed image data to raw image data
	* \param hDec [IN]] The Dec Handle which was created by Init function.
	* \param pInput [IN] The input buffer param.
	* \param pOutput [OUT] The output buffer param.
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * CCProcess) (VO_HANDLE hCodec,VO_PTR pData);

	/**
	* Set the param for special target.
	* \param hDec [IN]] The Dec Handle which was created by Init function.
	* \param uParamID [IN] The param ID.
	* \param pData [IN] The param value depend on the ID>
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * CCSetParam) (VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);

	/**
	* Get the param for special target.
	* \param hDec [IN]] The Dec Handle which was created by Init function.
	* \param uParamID [IN] The param ID.
	* \param pData [OUT] The param value depend on the ID>
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * CCGetParam) (VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);

	/**
	* Uninit the decoder.
	* \param hDec [IN]] The Dec Handle which was created by Init function.
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * CCUninit) (VO_HANDLE hCodec);
} VO_CLRCONV_DECAPI;

/**
* Get image  API interface
* \param pDecHandle [out] Return the color conversion handle.
* \retval VO_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetClrConvAPI (VO_CLRCONV_DECAPI * pDecHandle, VO_U32 uFlag);

#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* VO_COLOR_CONVERSION_H_ */
