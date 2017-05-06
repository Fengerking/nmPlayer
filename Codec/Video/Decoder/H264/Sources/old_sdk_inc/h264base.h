/** 
 * \file h264base.h
 * \brief VisualOn H264 ID & sturcture, for SetParameter/GetParameter
 * \version 0.1
 * \date 10/31/2006 created
 * \author VisualOn
 */


#ifndef __VO_H264_BASE_H_
#define __VO_H264_BASE_H_

#include "sdkbase.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)
/*!
*the stream type that the decoder supports	 	
*/
typedef enum{
	VOH264_ANNEXB				= 0,	/*!<the bitstream format defined in 14496-10 Annexb */
	VOH264_14496_15				= 1,	/*!<the bitstream format defined in 14496-15,i.e. H264 RAW Data */
	VOH264_AVC					= 2,		/*!<the bitstream format defined in 14496-15,i.e. AVC Original Sample*/
	VOH264FILEFORMAT_END2           = 0x7fffffff,
}VOH264FILEFORMAT;
/*!
*the flush buffer stores the address of decoded frames that are not output yet	 	
*/
#define MAX_FRAMES 8

typedef struct{
	int frameCount;	/*!<the actual frameCount of the decoder buffer */
	VOCODECVIDEOBUFFER frameBuf[MAX_FRAMES]; /*!<the frame address of the decoded frame */
}VOH264FLUSHBUF;


typedef void  (VOCODECAPI *CALLBACK_OutputOneFrame)(VOCODECVIDEOBUFFER* outBuffer,int width,int height);
typedef enum{
	VOH264_iMX31_HD_DEBLOCK	=1,
	VOH264DEBLOCKFLAG_END2           = 0x7fffffff,

}VOH264DEBLOCKFLAG;

typedef struct{
	long	timeStamp;/*!<the timestamp by million seconds */
	long	timePerFrame;/*!<the time of each frame by million seconds*/
}VOTIMESTAMPINFO;

/*!
*the time info from SEI and VUI, used for Sanyo project
*/
typedef struct{
	long		num_units_in_tick;
	long		time_scale;
	long		cpb_removal_delay;
	long		fixed_frame_rate_flag;
}VOSEIINFO;

typedef enum{
   /*!<Severity1:bitstream errors that are resilient */
   VOH264_ERR_InvalidPOCType			=-1,
   VOH264_ERR_InvalidNumRefFrame		=-2,
   VOH264_ERR_LFDisableIdc				=-3,
   VOH264_ERR_QP						=-4,
   VOH264_ERR_SliceType					=-5,
   VOH264_ERR_IntraMBAIsNULL			=-6,
   VOH264_ERR_IntraModeIsNULL			=-7,
   VOH264_ERR_InvalidNumSliceGroupMapUnit	=-8,
   VOH264_ERR_IDRHasRemainedFrame		=-9,
   VOH264_ERR_DecPictureIsNotNULL		=-11,
   VOH264_ERR_InvalidB8Mode				=-13,
   VOH264_ERR_InvalidIntraPredMode		=-14,
   VOH264_ERR_InvalidIntraPredCMode		=-15,
   VOH264_ERR_InvalidRefIndex			=-16,
   VOH264_ERR_InvalidMbQPDelta			=-17,
   VOH264_ERR_InvalidDpbBufSize			=-18,
   VOH264_ERR_RemappingPicIdc			=-19,
   VOH264_ERR_InvalidFrameStoreType		=-20,
   VOH264_ERR_TOOMuchReorderIDC			=-22,
   VOH264_ERR_InvalidMCO				=-23,
   VOH264_ERR_InvalidMBMode				=-24,
   VOH264_ERR_InvalidBitstream			=-25 ,
   
   /*!<Severity2:bitstream errors that are resilient or ignored by decoder */
   VOH264_ERR_DPBIsNULL					=-101,
   VOH264_ERR_RefBufNotAllocated		=-102,
   VOH264_ERR_IntraMBBIsNULL			=-103,
   VOH264_ERR_NotSupportIPCM			=-104,
   VOH264_ERR_REFLISTGREAT16			=-105,
   VOH264_ERR_InvalidSEI				=-106,
   VOH264_ERR_REFFrameIsNULL			=-107,
   VOH264_ERR_InvalidSEQParSetID		=-108,
   VOH264_ERR_DecSliceLoss				=-109,
   VOH264_ERR_InvalidPicStruct			=-110,
   VOH264_ERR_NULLPOINT					=-111,
   VOH264_ERR_TooBigSEISize				=-112,

   /*!<Severity3:without sequence head or picture head,the decoder is hang and wait for the sps and pps*/
   VOH264_ERR_PPSIsNULL					=-200,
   VOH264_ERR_SPSIsNULL					=-201,
   VOH264_ERR_InvalidPPS				=-202,
   VOH264_ERR_InvalidSPS				=-203,

   /*!<Severity4:the error is not recoverable, exit the decoder,try again*/
   VOH264_ERR_MEMExit					=-300,

   /*!<Severity5:the bitstream is not supported by the decoder*/
   VOH264_ERR_NotSupportProfile			=-400,
   VOH264_ERR_NotSupportFMO				=-401,
   VOH264ErrorCode_END2           = 0x7fffffff,

}VOH264ErrorCode;
/**
 * H264 specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
typedef enum
{
	VOID_H264_STREAMFORMAT  	= VOID_H264_BASE | 0x0001,  /*!< The stream type that the decoder supports, the parameter is a signed long integer */
	VOID_H264_SEQUENCE_PARAMS	= VOID_H264_BASE | 0x0002,  /*!< sequence parameters, the parameter is a VOCODECDATABUFFER pointer*/
	VOID_H264_PICTURE_PARAMS	= VOID_H264_BASE | 0x0003,  /*!< picture parameters, the parameter is a VOCODECDATABUFFER pointer*/
	VOID_H264_FLUSH				= VOID_H264_BASE | 0x0004,  /*!<if it is set as a valid VOH264FLUSHBUF pointer,it will inform the decoder to output the buffer;if it is NULL,there is no output*/	
	VOID_H264_GETFIRSTFRAME		= VOID_H264_BASE | 0x0005,  /*!<it is set as 1,it will force the decoder to output the first frame immediately,the parameter is a Boolean(signed long integer)*/
	VOID_H264_DISABLEDEBLOCK	= VOID_H264_BASE | 0x0006,  /*!<it is set as 1,it will notify the decoder to turn off the deblock,the parameter is a Boolean(signed long integer)*/	
	VOID_H264_AVCCONFIG			= VOID_H264_BASE | 0x0007,  /*!<it is set as 1,it will notify the decoder to turn off the deblock,the parameter is a Boolean(signed long integer)*/	
	VOID_H264_CALLBACK_OUT		= VOID_H264_BASE | 0x0008,  /*!<it is a function of CALLBACK_OutputOneFrame used for VOH264_ANNEXB format*/
	VOID_H264_DEBLOCKFLAG		= VOID_H264_BASE | 0x0009,  /*!<if it is set as VOH264_iMX31_HD_DEBLOCK,it will notify the decoder to use imx31 HD deblock,the parameter is a VOH264DEBLOCKFLAG*/	
	VOID_H264_DEBLOCKROW		= VOID_H264_BASE | 0x000a,  /*!<the param indicates the interval of deblock rows,say,2 means deblock by 2 rows each time,0 means deblock by whole frame,the parameter is a long*/	
	VOID_H264_TIMESTAMP			= VOID_H264_BASE | 0x000b,  /*!<the timestamp of the decoding frame,the parameter is a pointer of VOTIMESTAMPINFO*/	
	VOID_H264_SEIINFO			= VOID_H264_BASE | 0x000c,  /*!<the time info from SEI and VUI,the parameter is a pointer of VOSEIINFO*/	

}
VOH264PARAMETERID;

#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_H264_BASE_H_ */


