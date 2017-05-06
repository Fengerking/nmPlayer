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

#ifndef __VO_H264New_H__
#define __VO_H264New_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>
typedef enum{
	VO_H264_ANNEXB				= 0,	/*!<the bitstream format defined in 14496-10 Annexb */
	VO_H264_14496_15			= 1,	/*!<the bitstream format defined in 14496-15,i.e. H264 RAW Data */
	VO_H264_AVC					= 2,	/*!<the bitstream format defined in 14496-15,i.e. AVC Original Sample*/
	VO_H264_ANNEXB_COMPLETE = 3,/*!<the bitstream format defined in 14496-10 Annexb, but it is one or multiple complete NALU*/
	VO_H264_FF_MAX				= VO_MAX_ENUM_VALUE
}VO_H264FILEFORMAT;

#define VO_ERR_ENC_H264_BASE                 (VO_ERR_BASE | VO_INDEX_ENC_H264) /*!< H.264 encoder error code base*/	
#define VO_ERR_DEC_H264_BASE                 (VO_ERR_BASE | VO_INDEX_DEC_H264) /*!< H.264 decoder error code base*/	
//VOH264ERROR(VO_H264_ERR_NotSupportFMO);
typedef enum{
	VO_H264_ERR_PPSIsNULL					= VO_ERR_DEC_H264_BASE | 0x0001,
	VO_H264_ERR_SPSIsNULL					= VO_ERR_DEC_H264_BASE | 0x0002,
	VO_H264_ERR_InvalidPPS				    = VO_ERR_DEC_H264_BASE | 0x0003,
	VO_H264_ERR_InvalidSPS				    = VO_ERR_DEC_H264_BASE | 0x0004,
	VO_H264_ERR_MEMExit					    = VO_ERR_DEC_H264_BASE | 0x0005,
	VO_H264_ERR_NotSupportProfile			= VO_ERR_DEC_H264_BASE | 0x0006,
	VO_H264_ERR_NotSupportFMO				= VO_ERR_DEC_H264_BASE | 0x0007,
	VO_H264_ERR_RBSP				        = VO_ERR_DEC_H264_BASE | 0x0008,
	VO_H264_ERR_FORBIDDEN_BIT				= VO_ERR_DEC_H264_BASE | 0x0009,
	VO_H264_ERR_NALU_TYPE				= VO_ERR_DEC_H264_BASE | 0x000A,
	VO_H264_ERR_NALU_SMALL				= VO_ERR_DEC_H264_BASE | 0x000B,
	VO_H264_ERR_SLICE_TYPE				= VO_ERR_DEC_H264_BASE | 0x000C,
	VO_H264_ERR_NUMREFFRAME				= VO_ERR_DEC_H264_BASE | 0x000D,
	VO_H264_ERR_PIC_STRUCT				= VO_ERR_DEC_H264_BASE | 0x000E,
	VO_H264_ERR_LIST_REORDERING		= VO_ERR_DEC_H264_BASE | 0x000F,
	VO_H264_ERR_SLICE_HEADER  		= VO_ERR_DEC_H264_BASE | 0x0010,
	VO_H264_ERR_POC           		= VO_ERR_DEC_H264_BASE | 0x0011,
	VO_H264_ERR_NO_STRUCTURE   		= VO_ERR_DEC_H264_BASE | 0x0012,
	VO_H264_ERR_MAX_SLICE_NUM   	= VO_ERR_DEC_H264_BASE | 0x0013,
	VO_H264_ERR_MEMORY_MANAGEMENT	= VO_ERR_DEC_H264_BASE | 0x0014,
	VO_H264_ERR_REORDER_LIST    	= VO_ERR_DEC_H264_BASE | 0x0015,
	VO_H264_ERR_DUPLICATE_FRAMENUM= VO_ERR_DEC_H264_BASE | 0x0016,
	VO_H264_ERR_DPB_EMPTY         = VO_ERR_DEC_H264_BASE | 0x0017,
	VO_H264_ERR_DPB_NO_OUTPUT     = VO_ERR_DEC_H264_BASE | 0x0018,
	VO_H264_ERR_MB_TYPE           = VO_ERR_DEC_H264_BASE | 0x0019,
	VO_H264_ERR_B8MODE            = VO_ERR_DEC_H264_BASE | 0x001A,
	VO_H264_ERR_MAX_CPB_CNT       = VO_ERR_DEC_H264_BASE | 0x001B,
	VO_H264_ERR_FIRST_MB_OVERFLOW = VO_ERR_DEC_H264_BASE | 0x001C,
	VO_H264_ERR_CBP_TOO_LARGE     = VO_ERR_DEC_H264_BASE | 0x001D,
	VO_H264_ERR_QP_TOO_LARGE      = VO_ERR_DEC_H264_BASE | 0x001E,
	VO_H264_ERR_POS               = VO_ERR_DEC_H264_BASE | 0x001F,
	VO_H264_ERR_NZ_CAVLC          = VO_ERR_DEC_H264_BASE | 0x0020,
	VO_H264_ERR_TOTALZEROS_CAVLC  = VO_ERR_DEC_H264_BASE | 0x0021,
	VO_H264_ERR_SEI				        = VO_ERR_DEC_H264_BASE | 0x0022,
	VO_H264_ERR_MB_NUM		        = VO_ERR_DEC_H264_BASE | 0x0023,
	VO_H264_ERR_REF_NULL	        = VO_ERR_DEC_H264_BASE | 0x0024,
	VO_H264_ERR_AVC_NULL	        = VO_ERR_DEC_H264_BASE | 0x0025,
	VO_H264_ERR_INIT_LIST	        = VO_ERR_DEC_H264_BASE | 0x0026,
	VO_H264_ERR_INVALIDMCO	      = VO_ERR_DEC_H264_BASE | 0x0027,
	/*!< Encoder*/
    VO_ERR_ENC_H264_CONFIG               = VO_ERR_ENC_H264_BASE | 0x0001,  /*!< Encoder error of configuration */
    VO_ERR_ENC_H264_VIDEOTYPE            = VO_ERR_ENC_H264_BASE | 0x0002,  /*!< Encoder unsupport video type */
    VO_ERR_ENC_H264_FRAME                = VO_ERR_ENC_H264_BASE | 0x0003,  /*!< Error in nFrame */
    	
	VO_H264_ERR_MAX							= VO_MAX_ENUM_VALUE
}VO_H264ErrorCode;

#define VO_ID_H264_BASE 0x40100000


#define VO_PID_ENC_H264_BASE              (VO_PID_COMMON_BASE | VO_INDEX_ENC_H264) /*!< H.264 encoder parameter ID base*/

typedef enum{
	EDF_IMX31_HD							= 1,/*!< for imx31 HD */
	EDF_DUAL_CORE							= 2,/*!< for dual core */
	EDF_ONLYMBEDGE							= 4,/*!< only deblcok the edge of MB */
	EDF_ONLYTOPFIELD						= 8,/*!< only deblcok the top field for the de-interlace of Sony */
	VO_H264_EDF_MAX							= VO_MAX_ENUM_VALUE
}EDeblcokFlag;

typedef enum{

	EOF_BYSPEC									= 0,/*!< normal output, not ASAP but following the spec buffer management */
	EOF_NORMAL_ASAP								= 1,/*!< output ASAP */
	EOF_NORMAL_BASELINE							= 3,/*!< output by spec for baseline*/
	EOF_NEGTIVEPOC_ASAP							= 100,/*!< for rare case, the poc is negative */
}EOUTPUTFLAG;
/*
The structure is to expose some special info of the decoder, 
which is used for the application to do something extra, like extending
**/
typedef struct
{
	int				aspect_ratio_idc;   
	int				sar_width;                                    
	int				sar_height;                                   	                                
	
}TH264VideoUsebilityInfo;

/*
The flag is used for telling the decoder some special encoder info.
eg., in some cases that the caller knows the encoder info, then the decoder can optimize it accordingly.
**/
typedef enum
{
	OPT_ONLY_B_SPACIAL = 1,			/*!< the encoded stream use B frame with spactial direct pred mode */
	OPT_DUALCORE_DUMP  = 2,			/*!< dump the log with thread safe on mul-core platform*/
	OPT_DISABLE_ERRORLOG  = 4,		/*!< disable to output the error log*/
	OPT_STRICT_FRAMENUM   = 8,		/*!< check the frame number strictly to check frame drop*/
	OPT_ENABLE_DEINTERLACE   = 16,		/*!< enable the SE de-interlaceing algorithm, which drops one field of B frame*/
	OPT_FULLPIX_NONREF   = 32,		/*!< only full pixel MC on non-ref frame*/
	OPT_FULLPIX_ALL      = 64,		/*!< only full pixel MC on any frame*/
	OPT_ENABLE_DEINTERLACE_IP      = 128,		/*!<enable the SE de-interlaceing algorithm, which drops one field of I or P frame */
	OPT_DISABLE_DECODE      = 256,		/*!<disable decode slice for some special use like to get the SEI info*/
	VO_H264_OPT_MAX					= VO_MAX_ENUM_VALUE
}EH264OPTFlag;

/*
The flag is used for control the SEI info.
**/
typedef enum
{
	VHSF_DISABLE		= 0,		/*!< disable SEI decoding */
	VHSF_ENABLE			= 1,		/*!< enable SEI decoding */
	VHSF_GET_T35_USERDDATA	= 1<<1,		/*!< enable SEI decoding,and get the T35 user data*/
	VHSF_MAX			= VO_MAX_ENUM_VALUE

}VO_H264_SEIFLAG;
enum
{
	VO_ID_H264_STREAMFORMAT  	= VO_ID_H264_BASE | 0x0001,  /*!< The stream type that the decoder supports, the parameter is a LONG integer */
	VO_ID_H264_SEQUENCE_PARAMS	= VO_ID_H264_BASE | 0x0002,  /*!< sequence parameters, the parameter is a VOCODECDATABUFFER pointer*/
	VO_ID_H264_PICTURE_PARAMS	= VO_ID_H264_BASE | 0x0003,  /*!< picture parameters, the parameter is a VOCODECDATABUFFER pointer*/
	VO_ID_H264_FLUSH			= VO_ID_H264_BASE | 0x0004,  /*!<if it is set as a valid VO_H264FLUSHBUF pointer,it will inform the decoder to output the buffer;if it is NULL,there is no output*/	
	VO_ID_H264_GETFIRSTFRAME	= VO_ID_H264_BASE | 0x0005,  /*!<it is set as 1,it will force the decoder to output the first frame immediately,the parameter is a Boolean(LONG integer)*/
	VO_ID_H264_DISABLEDEBLOCK	= VO_ID_H264_BASE | 0x0006,  /*!<it is set as 1,it will notify the decoder to turn off the deblock,the parameter is a Boolean(LONG integer)*/	
	VO_ID_H264_AVCCONFIG		= VO_ID_H264_BASE | 0x0007,  /*!<it is set as 1,it will notify the decoder to turn off the deblock,the parameter is a Boolean(LONG integer)*/	
	VO_ID_H264_CALLBACK_OUT		= VO_ID_H264_BASE | 0x0008,  /*!<it is a function of CALLBACK_OutputOneFrame used for VO_H264_ANNEXB format*/
	VO_ID_H264_DEBLOCKFLAG		= VO_ID_H264_BASE | 0x0009,  /*!<the parameter is EDeblcokFlag*/	
	VO_ID_H264_DEBLOCKROW		= VO_ID_H264_BASE | 0x000a,  /*!<the param indicates the interval of deblock rows,say,2 means deblock by 2 rows each time,0 means deblock by whole frame,the parameter is a long*/	
	VO_ID_H264_TIMESTAMP		= VO_ID_H264_BASE | 0x000b,  /*!<the timestamp of the decoding frame,the parameter is a pointer of VOTIMESTAMPINFO*/	
	VO_ID_H264_SCALE			= VO_ID_H264_BASE | 0x000c,  /*!<SCALE is used for thumbnail,4 means zoom out the size to 1/4*/
	VO_ID_H264_ENABLE_JUMP2I	= VO_ID_H264_BASE | 0x0020,  /*!<Jump to next I frame if error,1:enable,0:disable,default:1*/	
	VO_ID_H264_ENABLE_PARTICAIL_DECODE	= VO_ID_H264_BASE | 0x0021,  /*!<go on decoding if slice lost,1:enable,0:disable,default:0*/	
	VO_ID_H264_ENABLE_FIRST_FRAME_NON_INTRA	= VO_ID_H264_BASE | 0x0022,  /*!<the first frame can be non-intra,1:enable,0:disable,default:0*/	
	VO_ID_H264_ENABLE_OUTPUTASAP		  = VO_ID_H264_BASE | 0x0023,  /*!<output the frames asap, rather than following the DPB rule,1:enable,0:disable,default:1*/	
	VO_ID_H264_ENABLE_COPYINPUT						 = VO_ID_H264_BASE | 0x0024,  /*!<copy input data to internal buffer,1:enable,0:disable,default:0*/	
	VO_ID_H264_STREAMINGMODE				 = VO_ID_H264_BASE | 0x0025,  /*!<enbale it to enhance the error resilience,0:disable,1:enable,default:0*/	
	VO_ID_H264_ENABLE_SEI					 = VO_ID_H264_BASE | 0x0026,  /*!<enbale SEI info,check the VO_H264_SEIFLAG*/	
	VO_ID_H264_MULTICORE_NUM				 = VO_ID_H264_BASE | 0x0027,  /*!<set the core number of current cpu,great than 1 will enable multithread*/	
	VO_ID_H264_PARSE_ROWNUM					 = VO_ID_H264_BASE | 0x0028,  /*!<the number of row parsed one time*/	
	VO_ID_H264_VUI							 = VO_ID_H264_BASE | 0x0029,  /*!<get the TH264VideoUsebilityInfo*/	
	VO_ID_H264_OPT_FLAG						 = VO_ID_H264_BASE | 0x0030,  /*!<for some unregular request, enforce the decoder to apply some special flag,the value is EH264SPECIALFlag*/	
	VO_ID_H264_T35_USERDATA					 = VO_ID_H264_BASE | 0x0031,  /*!<get the T35 user data*/	
    VO_ID_H264_INPUTPARAM                    = VO_ID_H264_BASE | 0x0032,  /*!<get input parameters*/
    VO_ID_H264_FLUSH_PICS                    = VO_ID_H264_BASE | 0x0040,  /*!< Flush the codec buffer.VO_U32 *, 1 Flush, 0 No * */
    VO_ID_H264_INTERLACE                     = VO_ID_H264_BASE | 0x0041,  /*!<get interlace*/
	VO_PID_ENC_H264_PROFILE               = VO_PID_ENC_H264_BASE | 0x0001,  /*!<Set profile, the parameter is address of VO_S32*/	
	VO_PID_ENC_H264_LEVEL                 = VO_PID_ENC_H264_BASE | 0x0002,  /*!<Set level, the parameter is address of VO_S32*/	
    VO_PID_ENC_H264_BITRATE               = VO_PID_ENC_H264_BASE | 0x0004,  /*!<Set bit rate, the parameter is address of VO_S32*/
    VO_PID_ENC_H264_FRAMERATE             = VO_PID_ENC_H264_BASE | 0x0005,  /*!<Set nFrame rate, the parameter is address of float*/
    VO_PID_ENC_H264_KEYFRAME_INTERVAL     = VO_PID_ENC_H264_BASE | 0x0006,  /*!<Set key nFrame interval, the parameter is address of VO_S32*/
    VO_PID_ENC_H264_VIDEO_QUALITY         = VO_PID_ENC_H264_BASE | 0x0007,  /*!<Set encoder video quality, the parameter is address of VOVIDEOENCQUALITY*/	
    VO_PID_ENC_H264_FORCE_KEY_FRAME       = VO_PID_ENC_H264_BASE | 0x0008,  /*!<Force currenct nFrame coded as key nFrame, parameter is address of VO_S32, 0-disable, 1-enable*/
    VO_PID_ENC_H264_OUTBUF_SIZE           = VO_PID_ENC_H264_BASE | 0x0009,  /*!<Return the max out buffer size needed by encoder, the parameter is address of VO_S32 */
    VO_PID_ENC_H264_INPUT_ROTATION        = VO_PID_ENC_H264_BASE | 0x000b,  /*!<Set input video rotation type, (invalid)*/
    VO_PID_ENC_H264_RATECONTROL           = VO_PID_ENC_H264_BASE | 0x000c,  /*!<Set bitrate control method, the paramter is AVCBitRateControl*/		
    VO_PID_ENC_H264_WIDTH                 = VO_PID_ENC_H264_BASE | 0x000e,  /*!<Set nWidth of video, the paramter is address of VO_S32*/
    VO_PID_ENC_H264_HEIGHT              = VO_PID_ENC_H264_BASE | 0x0010,  /*!<Set nHeight of video, the paramter is address of VO_S32*/
	
	VO_ID_H264_MAX				= VO_MAX_ENUM_VALUE
};
/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the H264 Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/**
 * Get video encoder API interface
 * \param pEncHandle  [IN/OUT] Return the H264 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetH264EncAPI (VO_VIDEO_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_H264_H__
