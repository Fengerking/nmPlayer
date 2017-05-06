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

#ifndef __VO_H265_H__
#define __VO_H265_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>

#define VO_ERR_DEC_H265_BASE                 (VO_ERR_BASE | VO_INDEX_DEC_H265)        /*!<H265 decoder error code base*/
#define VO_ERR_ENC_H265_BASE                 (VO_ERR_BASE | VO_INDEX_ENC_H265)        /*!<H265 encoder error code base*/    

#define VO_PID_DEC_H265_BASE                 (VO_PID_COMMON_BASE | VO_INDEX_DEC_H265) /*!< H.265 decoder parameter ID base*/
#define VO_PID_ENC_H265_BASE                 (VO_PID_COMMON_BASE | VO_INDEX_ENC_H265) /*!< H.265 encoder parameter ID base*/



typedef enum{
    VO_H265_ANNEXB                            = 0,    /*!<the bitstream format defined in 14496-10 Annexb */
    VO_H265_14496_15                          = 1,    /*!<the bitstream format defined in 14496-15,i.e.H265 RAW Data */
    VO_H265_AVC                               = 2,    /*!<the bitstream format defined in 14496-15,i.e. AVC Original Sample*/
    VO_H265_ANNEXB_COMPLETE                   = 3,/*!<the bitstream format defined in 14496-10 Annexb, but it is one or multiple complete NALU*/
    VO_H265_FF_MAX                            = VO_MAX_ENUM_VALUE
}VO_H265FILEFORMAT;


typedef enum{
    /*!< Decoder */
    VO_H265_ERR_InvalidPOCType                = VO_ERR_DEC_H265_BASE | 0x0001,  /*!< Decoder error of configuration */
    VO_H265_ERR_InvalidRefPic                 = VO_ERR_DEC_H265_BASE | 0x0002,  /*!< Decoder error of RefPic */
    VO_H265_ERR_PPSIsNULL					  = VO_ERR_DEC_H265_BASE | 0x0003,  /*!< Decoder error of missing pps */
	VO_H265_ERR_SPSIsNULL					  = VO_ERR_DEC_H265_BASE | 0x0004,  /*!< Decoder error of missing sps */
	VO_H265_ERR_SPSERROR					  = VO_ERR_DEC_H265_BASE | 0x0005,  /*!< Decoder error of SPS */
	VO_H265_ERR_PPSERROR					  = VO_ERR_DEC_H265_BASE | 0x0006,  /*!< Decoder error of PPS */
	VO_H265_ERR_VPSERROR					  = VO_ERR_DEC_H265_BASE | 0x0007,  /*!< Decoder error of VPS */
	VO_H265_ERR_NALUERROR					  = VO_ERR_DEC_H265_BASE | 0x0009,  /*!< Decoder error of NALU */
	VO_H265_ERR_SLICEHEADERERROR			  = VO_ERR_DEC_H265_BASE | 0x000a,  /*!< Decoder error of sliceheader */
	VO_H265_ERR_RPSERROR			          = VO_ERR_DEC_H265_BASE | 0x000b,  /*!< Decoder error of RPS */
	VO_H265_ERR_RERERENCEERROR			      = VO_ERR_DEC_H265_BASE | 0x000c,  /*!< Decoder error of reference*/
	VO_H265_ERR_SCALINGLISTERROR			  = VO_ERR_DEC_H265_BASE | 0x000d,  /*!< Decoder error of scalinglist*/
	VO_H265_ERR_ENTRYERROR			          = VO_ERR_DEC_H265_BASE | 0x000E,  /*!< Decoder error of entry*/
    /*!< Encoder */
    VO_ERR_ENC_H265_CONFIG                    = VO_ERR_ENC_H265_BASE | 0x0001,  /*!< Encoder error of configuration */

    VO_H265_ERR_MAX                           = VO_MAX_ENUM_VALUE
}VO_H265ErrorCode;

#if 0
typedef enum{

    EOF_BYSPEC                                = 0,/*!< normal output, not ASAP but following the spec buffer management */
    EOF_NORMAL_ASAP                           = 1,/*!< output ASAP */
    EOF_NORMAL_BASELINE                       = 3,/*!< output by spec for baseline*/
    EOF_NEGTIVEPOC_ASAP                       = 100,/*!< for rare case, the poc is negative */
}EOUTPUTFLAG;
#endif

enum
{
    VO_PID_DEC_H265_STREAMFORMAT              = VO_PID_DEC_H265_BASE | 0x0001,  /*!< The stream type that the decoder supports, the parameter is a LONG integer */
    VO_PID_DEC_H265_SEQUENCE_PARAMS           = VO_PID_DEC_H265_BASE | 0x0002,  /*!< sequence parameters, the parameter is a VOCODECDATABUFFER pointer*/
    VO_PID_DEC_H265_PICTURE_PARAMS            = VO_PID_DEC_H265_BASE | 0x0003,  /*!< picture parameters, the parameter is a VOCODECDATABUFFER pointer*/
    VO_PID_DEC_H265_FLUSH                     = VO_PID_DEC_H265_BASE | 0x0004,  /*!<if it is set as a valid VO_H265FLUSHBUF pointer,it will inform the decoder to output the buffer;if it is NULL,there is no output*/    
    VO_PID_DEC_H265_GETFIRSTFRAME             = VO_PID_DEC_H265_BASE | 0x0005,  /*!<it is set as 1,it will force the decoder to output the first frame immediately,the parameter is a Boolean(LONG integer)*/
    VO_PID_DEC_H265_DISABLEDEBLOCK            = VO_PID_DEC_H265_BASE | 0x0006,  /*!<it is set as 1,it will notify the decoder to turn off the deblock,the parameter is a Boolean(LONG integer)*/    
    VO_PID_DEC_H265_AVCCONFIG                 = VO_PID_DEC_H265_BASE | 0x0007,  /*!<it is set as 1,it will notify the decoder to turn off the deblock,the parameter is a Boolean(LONG integer)*/    
    VO_PID_DEC_H265_CALLBACK_OUT              = VO_PID_DEC_H265_BASE | 0x0008,  /*!<it is a function of CALLBACK_OutputOneFrame used for VO_H265_ANNEXB format*/
    VO_PID_DEC_H265_DEBLOCKFLAG               = VO_PID_DEC_H265_BASE | 0x0009,  /*!<the parameter is EDeblcokFlag*/    
    VO_PID_DEC_H265_DEBLOCKROW                = VO_PID_DEC_H265_BASE | 0x000a,  /*!<the param indicates the interval of deblock rows,say,2 means deblock by 2 rows each time,0 means deblock by whole frame,the parameter is a long*/    
    VO_PID_DEC_H265_TIMESTAMP                 = VO_PID_DEC_H265_BASE | 0x000b,  /*!<the timestamp of the decoding frame,the parameter is a pointer of VOTIMESTAMPINFO*/    
    VO_PID_DEC_H265_SCALE                     = VO_PID_DEC_H265_BASE | 0x000c,  /*!<SCALE is used for thumbnail,4 means zoom out the size to 1/4*/
    VO_PID_DEC_H265_ENABLE_JUMP2I             = VO_PID_DEC_H265_BASE | 0x0020,  /*!<Jump to next I frame if error,1:enable,0:disable,default:1*/    
    VO_PID_DEC_H265_ENABLE_OUTPUTASAP         = VO_PID_DEC_H265_BASE | 0x0023,  /*!<output the frames asap, rather than following the DPB rule,1:enable,0:disable,default:1*/    
    VO_PID_DEC_H265_ENABLE_COPYINPUT          = VO_PID_DEC_H265_BASE | 0x0024,  /*!<copy input data to internal buffer,1:enable,0:disable,default:0*/    
    VO_PID_DEC_H265_STREAMINGMODE             = VO_PID_DEC_H265_BASE | 0x0025,  /*!<enbale it to enhance the error resilience,0:disable,1:enable,default:0*/    
    VO_PID_DEC_H265_ENABLE_SEI                = VO_PID_DEC_H265_BASE | 0x0026,  /*!<enbale SEI info,check the VO_H265_SEIFLAG*/    
    VO_PID_DEC_H265_MULTICORE_NUM             = VO_PID_DEC_H265_BASE | 0x0027,  /*!<set the core number of current cpu,great than 1 will enable multithread*/    
    VO_PID_DEC_H265_PARSE_ROWNUM              = VO_PID_DEC_H265_BASE | 0x0028,  /*!<the number of row parsed one time*/    
    VO_PID_DEC_H265_VUI                       = VO_PID_DEC_H265_BASE | 0x0029,  /*!<get the TH265VideoUsebilityInfo*/    
    VO_PID_DEC_H265_OPT_FLAG                  = VO_PID_DEC_H265_BASE | 0x0030,  /*!<for some unregular request, enforce the decoder to apply some special flag,the value is EH265SPECIALFlag*/    
    VO_PID_DEC_H265_T35_USERDATA              = VO_PID_DEC_H265_BASE | 0x0031,  /*!<get the T35 user data*/    
    VO_PID_DEC_H265_INPUTPARAM                = VO_PID_DEC_H265_BASE | 0x0032,  /*!<get input parameters*/
    VO_PID_DEC_H265_FLUSH_PICS                = VO_PID_DEC_H265_BASE | 0x0040,  /*!< Flush the codec buffer.VO_U32 *, 1 Flush, 0 No * */
	VO_PID_DEC_H265_FASTMODE                  = VO_PID_DEC_H265_BASE | 0x0041,  /*!<Set/get fast mode, the parameter is address of VO_U32*/                  
    VO_PID_ENC_H265_PROFILE                   = VO_PID_ENC_H265_BASE | 0x0001,  /*!<Set profile, the parameter is address of VO_S32*/    
    VO_PID_ENC_H265_LEVEL                     = VO_PID_ENC_H265_BASE | 0x0002,  /*!<Set level, the parameter is address of VO_S32*/    
    VO_PID_ENC_H265_BITRATE                   = VO_PID_ENC_H265_BASE | 0x0004,  /*!<Set bit rate, the parameter is address of VO_S32*/
    VO_PID_ENC_H265_FRAMERATE                 = VO_PID_ENC_H265_BASE | 0x0005,  /*!<Set nFrame rate, the parameter is address of float*/
    VO_PID_ENC_H265_KEYFRAME_INTERVAL         = VO_PID_ENC_H265_BASE | 0x0006,  /*!<Set key nFrame interval, the parameter is address of VO_S32*/
    VO_PID_ENC_H265_VIDEO_QUALITY             = VO_PID_ENC_H265_BASE | 0x0007,  /*!<Set encoder video quality, the parameter is address of VOVIDEOENCQUALITY*/    
    VO_PID_ENC_H265_FORCE_KEY_FRAME           = VO_PID_ENC_H265_BASE | 0x0008,  /*!<Force currenct nFrame coded as key nFrame, parameter is address of VO_S32, 0-disable, 1-enable*/
    VO_PID_ENC_H265_OUTBUF_SIZE               = VO_PID_ENC_H265_BASE | 0x0009,  /*!<Return the max out buffer size needed by encoder, the parameter is address of VO_S32 */
    VO_PID_ENC_H265_INPUT_ROTATION            = VO_PID_ENC_H265_BASE | 0x000b,  /*!<Set input video rotation type, (invalid)*/
    VO_PID_ENC_H265_RATECONTROL               = VO_PID_ENC_H265_BASE | 0x000c,  /*!<Set bitrate control method, the paramter is AVCBitRateControl*/        
    VO_PID_ENC_H265_WIDTH                     = VO_PID_ENC_H265_BASE | 0x000e,  /*!<Set nWidth of video, the paramter is address of VO_S32*/
    VO_PID_ENC_H265_HEIGHT                    = VO_PID_ENC_H265_BASE | 0x0010,  /*!<Set nHeight of video, the paramter is address of VO_S32*/
    
    VO_ID_H265_MAX                            = VO_MAX_ENUM_VALUE
};
/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the H265 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API yyGetHEVCDecFunc (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/**
 * Get video encoder API interface
 * \param pEncHandle  [IN/OUT] Return the H265 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetH265EncAPI (VO_VIDEO_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_H265_H__
