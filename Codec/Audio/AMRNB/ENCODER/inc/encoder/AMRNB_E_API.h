	/*!
	*																		
	*		VisualOn, Inc. Confidential and Proprietary, 2005				
	*																		
	*/
/** 
	* \file		AMRNB_E_API.h
	* \brief	VisualOn AMR_NarrowBand Encoder API
	* \version 0.1
	* \date 6/30/2006 updated, ver 0.1
 	* \date 6/26/2006 created
*/

#ifndef __EAMRNBLIB_H__
#define __EAMRNBLIB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void *g_hAMRNBEncInst;
/*!
 * the bitrate that the decoder supports
 */
typedef enum _VoiAMRNBMode { 
	VOI_MDNONE = -1,	/*!< Invalid mode */
	VOI_MD475	= 0,	/*!< 475bps */
	VOI_MD515	= 1,     /*!< 515bps */       
	VOI_MD59	= 2,	/*!< 59bps */
	VOI_MD67	= 3,	/*!< 67bps */
	VOI_MD74	= 4,	/*!< 74bps */
	VOI_MD795	= 5,	/*!< 795bps */
	VOI_MD102	= 6,	/*!< 10.2kbps */
	VOI_MD122	= 7,    /*!< 12.2kbps */	        
	VOI_MDDTX	= 8,	/*!< DTX mode */
	VOI_N_MODES 	= 9    /*!< Invalid mode */
}VoiAMRNBMode;
/*!
 * the frame type that the decoder supports
 */
typedef enum _VoiAMRNBFrameType{
	VOI_RFC3267 = 0,	/*!< the frame type is the head (defined in RFC3267) + rawdata*/
	VOI_RAWDATA	= 1,	/*!< the frame type is raw data without head */
	VOI_IF1		= 2,    /*!< the frame type is the IF1 head + rawdata*/        
	VOI_IF2		= 3,	/*!< the frame type is the IF2 head + rawdata*/
}VoiAMRNBFrameType;
/*!
 * Structure of input parameters 
 */
typedef struct _AMRNBELibParam {
	unsigned char*		inputStream;/*!< the buffer of the input data,It should be allocated by the client of the libary*/
	int			inputSize;/*!< the size of the input data,it is 320 bytes always*/
	VoiAMRNBMode		mode;/*!< [IN/OUT]the bitrate mode of the bitstream,if the frameType is VOI_RAWDATA,the mode shall be set,otherwise it can be VOI_MDNONE. */
	VoiAMRNBFrameType	frameType;/*!< the type of frame,the default value is VOI_RFC3267*/
	unsigned short* 	outputStream;/*!< the buffer of output data.It should be allocated by the client of the libary.The buffer size should be larger than 512 Bytes*/
	int			outputSize;/*!< [OUT]the decoded frame size.*/
	void*		hEnc;
} AMRNBELibParam;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EAMRNBLIB_H__
