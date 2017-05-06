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

#ifndef __voJpeg_H__
#define __voJpeg_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voImage.h>
#include <voVideo.h>
#include <viMem.h>

typedef struct
{
	int		start_X;
	int		start_Y;
	int		end_X;
	int 	end_Y;
} JPEG_IMAGE_FIELD;

typedef enum
{
	VO_JPEG_DECSTEP_ENABLE = 1,	/* Set JPEG decode for step by step model, if nonzero, enable it, default unable */
	VO_JPEG_DECSTEP_NUMBER	= 2,    /* Set JPEG decode for one step at least decode lines, default 16  */
	VO_JPEG_INPUT_MODE		= 3,    /* Set JPEG input mode. if 0, buffer mode, else if 1, file mode, default 0  */
	VO_JPEG_FILE_PATH		= 4,    /* Set JPEG input file  path and name */
	VO_JPEG_VIDEO_TYPE		= 5,	/* Get JEPG output video type, the paramter is VOVIDEOTYPE */
	VO_JPEG_OUTPUT_FIELD   = 6,    /* Set JPEG decode output image field, default it is all field, the parameter is  IMAGE_FIELD*/
	VO_JPEG_OUTPUT_BUFFER	= 7,    /* Set and get JPEG output image data buffer, the parameter is VOCODECVIDEOBUFFER */
	VO_JPEG_OUTPUT_ZOOMOUT = 8,    /* Set JPEG decode output image zoom out, it should 1, 2, 4, 8, 8*n */
	VO_JPEG_BUFFER_LIMITED = 9,    /* Set JPEG decode can allocate the buffer size */
	VO_JPEG_FLUSH_MEMORY	= 10,	/* free JPEG decode memory */
	VO_JPEG_INPUT_BUFFER   = 11,    /* Set JPEG input buffer  */
	VO_JPEG_FILE_HANDLE    = 12,    //zou
	VO_JPEG_OUTPUT_SCANLINES = 13,  //zou
	VO_JPEG_NUM_COMPONENT      = 14, //zou
	VO_JPEG_SCANLINE_HEIGHT_Y    = 15, //zou
	VO_JPEG_SCANLINE_HEIGHT_UV    = 16, //zou
    VO_PID_DEC_JPEG_SET_NORMAL_CHROMA = 17,
	VO_VOJPEGDECSPECID_MAX = VO_MAX_ENUM_VALUE
} VOJPEGDECSPECID;

typedef enum
{
	VO_NORTH_LATITITUDE =	'N',     /* North latitude */
	VO_SOUTH_LATITITUDE =	'S',     /* South latitude */
	VO_Latitude_ref_MAX = VO_MAX_ENUM_VALUE
} Latitude_ref;

typedef enum
{
	VO_EAST_LONGITUDE   =	'E',     /* East longtitude */
	VO_WEST_LONGITUDE   =	'W',	 /* West longtitude */
	VO_Longitude_ref_MAX = VO_MAX_ENUM_VALUE
} Longitude_ref;

#define		SEA_LEVEL						0  /* above sea level */
#define		SEA_LEVEL_REF					1  /* below sea level */
	
/**
 * JPEG specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
#define VO_PID_DEC_JPEG_BASE            VO_PID_COMMON_BASE | VO_INDEX_DEC_JPEG
typedef enum
{
	VO_PID_JPEG_QUALITY	= VO_PID_DEC_JPEG_BASE | 0x0001,  /*!< JPEG specific parameter id 1, the parameter is a LONG integer, 
														 from 1 to 100, define the quliaty of JPEG, default 75 */
	VO_PID_PRE_JPEGSIZE	= VO_PID_DEC_JPEG_BASE | 0x0002,  /*!< JPEG specific parameter id 2, 
														 it's for getting predictable output encoded JPEG size, integer type  */
	VO_PID_EXIFINFO_SUPPT = VO_PID_DEC_JPEG_BASE | 0x0003,  /*!< JPEG specific parameter id 3, encoder it's for setting jpeg picture add exif infomation, 
														 decode it is setting for decode exif header infomation, integer type,										
														 if nonzero, add or get exif, else no exif.  default 0, no exif */
	VO_PID_THUMPIC_SUPPT	= VO_PID_DEC_JPEG_BASE | 0x0004,  /*!< JPEG specific parameter id 4, it's setting jpeg picture support thumbnail data, integer type,
														 if nonzero, add thumbnail, else no thumbnail.  default 0, no thumbnail */
	VO_PID_GET_THUMDATA	= VO_PID_DEC_JPEG_BASE | 0x0005,  /*!< JPEG specific parameter id 5, it's for getting jpeg picture thumbnail data, 
														 the input parameter is the pointer of the structure VOCODECDATABUFFER */
	VO_PID_PICTURE_DATE	= VO_PID_DEC_JPEG_BASE | 0x0006,  /*!< JPEG specific parameter id 6, it's for setting picture encode data and time, 
														 string type: "YYYY:MM:DD HH:MM:SS" such as: "2006:06:06 12:00:00" */
	VO_PID_THUMPIC_WIDTH	= VO_PID_DEC_JPEG_BASE | 0x0007,  /*!< JPEG specific parameter id 7, 
														 it's for setting thumbnail picture width, default 160 */
	VO_PID_THUMPIC_HEIGHT	= VO_PID_DEC_JPEG_BASE | 0x0008,  /*!< JPEG specific parameter id 8, 
														 it's for setting thumbnail picture height, default 120 */
	VO_PID_PICXRESOLUTION = VO_PID_DEC_JPEG_BASE | 0x0009,  /*!< JPEG specific parameter id 9, it is pointer of a 2dim int array(one rational), the value:array[0]/array[1]
														 it's for setting or getting image resoltion in width direction, default {72, 1}  */
    VO_PID_PICYRESOLUTION	= VO_PID_DEC_JPEG_BASE | 0x0010,  /*!< JPEG specific parameter id 10, it is pointer of a 2dim array,
														 it's for setting or getting image resoltion in height direction, default {72, 1} */
	VO_PID_MACHINE_MAKE	= VO_PID_DEC_JPEG_BASE | 0x0011,  /*!< JPEG specific parameter id 11, 
														 it's for setting  or getting the company who manufacture the handset, string type, default no make */
	VO_PID_MACHINE_MODEL	= VO_PID_DEC_JPEG_BASE | 0x0012,  /*!< JPEG specific parameter id 12, 
														 it's for setting or getting handset model, string type, default no model */
	VO_PID_GPS_LATIREF    = VO_PID_DEC_JPEG_BASE | 0x0013,  /*!< JPEG specific parameter id 13, it's for setting or getting GPS Latitude reference, 
														 it's reference to North latitude and South Latitude,  Latitude_ref type, default NORTH_LATITITUDE */ 	
	VO_PID_GPS_LATITUDE   = VO_PID_DEC_JPEG_BASE | 0x0014,  /*!< JPEG specific parameter id 14, it's for setting or getting GPS Latitude value, 
														 a pointer of 6 dim int array for degrees(2 dim), minutes(2 dim) and seconds(2 dim)  */
    VO_PID_GPS_LONGIREF   = VO_PID_DEC_JPEG_BASE | 0x0015,  /*!< JPEG specific parameter id 15, it's for setting or getting GPS longitude reference, 
														 it's reference to North east and west longitude,  Longitude_ref type, default EAST_LONGITUDE */ 	
	VO_PID_GPS_LONGITUDE  = VO_PID_DEC_JPEG_BASE | 0x0016,  /*!< JPEG specific parameter id 16, it's for setting or getting GPS longitude value,
														 a pointer of 6 dim int array for degrees(2 dim), minutes(2 dim) and seconds(2 dim)  */
	VO_PID_GPS_ALITUDEREF = VO_PID_DEC_JPEG_BASE | 0x0017,  /*!< JPEG specific parameter id 17, it's for setting or getting GPS alitude reference, 
														 it's reference to above sea level and below sea level, default SEA_LEVEL(above sea level) */ 	
	VO_PID_GPS_ALITUDE    = VO_PID_DEC_JPEG_BASE | 0x0018,  /*!< JPEG specific parameter id 18, it's for setting or getting GPS alitude value,
														 a pointer of 2 dim int array, the value: array[0]/array[1]  */
	VO_PID_GPS_MAPDATUM   = VO_PID_DEC_JPEG_BASE | 0x0019, 	/*!< JPEG specific parameter id 19, it's for setting  or getting Indicates the geodetic survey data 
													     used by the GPS receiver, string type, for example "SHANGHAI,CHINA" */
	VO_PID_GPS_DATESTAMP  = VO_PID_DEC_JPEG_BASE | 0x0020, 	/*!< JPEG specific parameter id 19, it's for setting  or getting Indicates the GPS data,
														 string type, "YYYY:MM:DD" such as: "2006:06:06" */	
	VO_PID_JPEG_PROGMODE	= VO_PID_DEC_JPEG_BASE | 0x0021,  /*!< JPEG specific parameter id 21, it's for getting JPEG mode, it is progressive mode or sequential mode 
														 integer type, if 1:progressive mode, or 0: sequential mode  */
	VO_PID_GPS_INFO		= VO_PID_DEC_JPEG_BASE | 0x0022,  /*!< JPEG specific parameter id 22, it's for setting  or getting Indicates the gps info in file */

	VO_PID_JPEG_WIDTH   = VO_PID_DEC_JPEG_BASE | 0x0023,  /*!< JPEG specific parameter id 23, it's for getting width of image */

	VO_PID_JPEG_HEIGHT   = VO_PID_DEC_JPEG_BASE | 0x0024,  /*!< JPEG specific parameter id 24, it's for getting height of image*/
	VO_PID_JPEG_REVERSAL  = VO_PID_DEC_JPEG_BASE | 0x0025,  /*!< JPEG specific parameter id 25, it's for doing reversal of image*/

	VO_PID_JPEG_WINAUTHOR	= VO_PID_DEC_JPEG_BASE | 0x0100,  /*!< JPEG specific parameter id 100, it's for setting  or getting jpeg author */
	VO_PID_MAX              = VO_MAX_ENUM_VALUE
}
VOJPEGPARAMETERID;


/**
* JPEG Decoder specific return code 
* \see VOCOMMONRETURNCODE
*/
#define VO_ERR_DEC_JPEG_BASE              VO_ERR_BASE | VO_INDEX_DEC_JPEG
#define VO_ERR_ENC_JPEG_BASE              VO_ERR_BASE | VO_INDEX_ENC_JPEG
typedef enum
{
	VO_ERR_JPEGDEC_INBUFFERPOINT_ERR		= VO_ERR_DEC_JPEG_BASE | 0x0001,  /*!< JPEG Decoder invalid input buffer address return code 1 */
	VO_ERR_JPEGDEC_UNSUPPORT_FEATURE		= VO_ERR_DEC_JPEG_BASE | 0x0002,  /*!< JPEG Decoder unspport JPEG feature, return code 2 */
	VO_ERR_JPEGDEC_DECODE_HEADER_ERR		= VO_ERR_DEC_JPEG_BASE | 0x0003,  /*!< JPEG Decoder decode header error, return code 3 */
	VO_ERR_JPEGDEC_DECODE_TAIL_ERR		= VO_ERR_DEC_JPEG_BASE | 0x0004,  /*!< JPEG Decoder decode frame error, return code 4 */
	VO_ERR_JPEGDEC_DECODE_UNFINISHED		= VO_ERR_DEC_JPEG_BASE | 0x0005,  /*!< JPEG Decoder decode frame unfinshed, return code 5 */
	VO_ERR_JPEGDEC_NO_EXIFMARKER			= VO_ERR_DEC_JPEG_BASE | 0x0006,  /*!< JPEG file no exif marker, return code 6 */
	VO_ERR_JPEGDEC_NO_IDITEM				= VO_ERR_DEC_JPEG_BASE | 0x0007,  /*!< JPEG file no data for given ID  */
	VO_ERR_JPEGDEC_OPENFILE_ERROR			= VO_ERR_DEC_JPEG_BASE | 0x0008,  /*!< open JPEG file error */
	VO_ERR_JPEGDEC_NOTHUMPIC				= VO_ERR_DEC_JPEG_BASE | 0x0009,  /*!< JPEG file no thumb picture */
	VO_ERR_JPEGDEC_UNKNOWN_ERR			= VO_ERR_DEC_JPEG_BASE | 0x00FF,  /*!< MJPEG Decoder un known error, return code FF */
	VO_ERR_MAX                          = VO_MAX_ENUM_VALUE
}
VOJPEGDECRETURNCODE;
typedef enum
{
	VO_ERR_JPEGENC_INBUFFERPOINT_ERR		= VO_ERR_ENC_JPEG_BASE | 0x0001,  /*!< JPEG Encoder invalid input buffer address return code 1 */
	VO_ERR_JPEGENC_WIDTH_INVALID			= VO_ERR_ENC_JPEG_BASE | 0x0002,  /*!< JPEG Encoder encode width invalid, return code 2 */
	VO_ERR_JPEGENC_HEIGHT_INVALID			= VO_ERR_ENC_JPEG_BASE | 0x0003,  /*!< JPEG Encoder encode height invalid, return code 3 */
	VO_ERR_JPEGENC_UNSUPPORT_COLOR		= VO_ERR_ENC_JPEG_BASE | 0x0004,  /*!< JPEG Encoder encode unsupport image color type, return code 4 */
	VO_ERR_JPEGENC_ENCODE_HEADER_ERR		= VO_ERR_ENC_JPEG_BASE | 0x0005,  /*!< JPEG Encoder encode header error, return code 5 */
	VO_ERR_JPEGENC_ENCODE_FRAME_ERR		= VO_ERR_ENC_JPEG_BASE | 0x0006,  /*!< JPEG Encoder encode frame error, return code 6 */
	VO_ERR_JPEGENC_THUMbWIDTH_INVALID		= VO_ERR_ENC_JPEG_BASE | 0x0007,  /*!< JPEG Encoder encode thumbnail width invalid, maybe >200 or <0 return code 7 */
	VO_ERR_JPEGENC_THUMbHEIGHT_INVALID	= VO_ERR_ENC_JPEG_BASE | 0x0008,  /*!< JPEG Encoder encode thumbnail height invalid, maybe >200 or <0 return code 8 */
	VO_ERR_JPEGENC_ENCUNFINISHED			= VO_ERR_ENC_JPEG_BASE | 0x0009,  /*!< JPEG Encoder encode unfinished, should continue to call function */
	VO_ERR_JPEGENC_UNKNOWN_ERR			= VO_ERR_ENC_JPEG_BASE | 0x00FF,  /*!< JPEG Encoder unknown error, return code FF */
}
VOJPEGENCRETURNCODE;

/**
 * Get image decorder API interface
 * \param pDecHandle [out] Return the H264 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetJPEGDecAPI (VO_IMAGE_DECAPI * pDecHandle,VO_U32 uFlag);

/**
 * Get image encoder API interface
 * \param pEncHandle [out] Return the H264 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetJPEGEncAPI (VO_IMAGE_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voJpeg_H__
