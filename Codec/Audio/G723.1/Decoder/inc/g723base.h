/** 
* \file g723base.h
* \brief VisualOn G.723.1 Codec ID & sturcture, for SetParameter/GetParameter
* \version 0.1
* \date 11/25/2008 created
* \author VisualOn
*/


#ifndef __VO_G723_BASE_H_
#define __VO_G723_BASE_H_

#include "sdkbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)
	/*!
	* the bitrate that the decoder supports
	*/
	typedef enum { 
		VOG723_MDNONE		= -1,	/*!< Invalid mode */
		VOG723_MD63		= 0,	/*!< 5.30kbps   */
		VOG723_MD53		= 1,    /*!< 6.30kbps   */       
	}VOG723MODE;

	/*!
	* the frame type that the decoder supports
	*/
	typedef enum {
		VOG723_DEFAULT  		= 0,	/*!< packed the bitstream*/
		VOG723_ITU             		= 1,    /*!< unpacked the bitstream*/
	}VOG723FRAMETYPE;

	/**
	* G723 specific parameter id 
	* \see VOCOMMONPARAMETERID
	*/
	typedef enum
	{
		VOID_G723_FRAMETYPE		= VOID_G723_BASE | 0x0001,  
		/*!< the type of frame,the default value is VOI_RFC3267, the parameter is a LONG integer */
		VOID_G723_MODE			= VOID_G723_BASE | 0x0002, 
		/*!< the mode of frame,the default value is VOG723_MD122, the parameter is a LONG integer */
		VOID_G723_DTX           	= VOID_G723_BASE | 0x0003, /*DTX Parameter ID*/
		VOID_G723_FLUSH			= VOID_G723_BASE | 0x0004,  /*!< reset AMR decoder */

	}VOG723PARAMETERID;


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_G723_BASE_H_ */


