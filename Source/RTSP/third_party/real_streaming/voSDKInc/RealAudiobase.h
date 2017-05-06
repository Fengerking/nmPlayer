/** 
 * \file RealAudiobase.h
 * \brief VisualOn RealAudio ID & sturcture, for SetParameter/GetParameter
 * \version 0.1
 * \date 10/31/2006 created
 * \author VisualOn
 */


#ifndef __VO_RealAudio_BASE_H_
#define __VO_RealAudio_BASE_H_


#include "sdkbase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif
#pragma pack(push, 4)
#include "RealCommonbase.h"
#define MAX_FRAMES 8

enum
{
	VORV_ERR_NOOUTBUFFER = -1,
};
typedef enum
{
	RA_AAC		= 0,

	RA_G2		,
	RA8_LBR		,
	RA8_HBR		,
	RA_SIPRO	,
}VA_VERSION;
typedef struct  
{
	VA_VERSION version;
	int	samplesPerFrame;//transform size
	int	frameSizeInBits;
	int cplStart;
	int cplQbits;
	int region;
	int	sample_rate;
	int channelNum;
}VORA_RAW_INIT_PARAM;
/**
 * RealAudio specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
#define VOID_RealAudio_BASE 0x00200000
typedef enum
{
	VOID_RealAudio_RESET			= VOID_RealAudio_BASE | 0x0002,  /*!<[IN] Reset the decoder,the param is a bool */	
	VOID_RealAudio_MAXOUTPUTSAMLES	= VOID_RealAudio_BASE | 0x0003,  /*!<[OUT]The max output size that the caller should support*/	
	VOID_RealAudio_INIT_PARAM		= VOID_RealAudio_BASE | 0x0004,	 /*!<[IN] it is same as VOID_COM_HEAD_DATA*/	
	VOID_RealAudio_RAW_INIT_PARAM	= VOID_RealAudio_BASE | 0x0005,	 /*!<[IN] it is VOVA_RAW_INIT_PARAM*/	
	VOID_RealAudio_BUF_INIT_PARAM	= VOID_RealAudio_BASE | 0x0006,	 /*!<[IN] it is VOVA_BUF_INIT_PARAM*/
	VOID_RealAudio_BLOCKSIZE		= VOID_RealAudio_BASE | 0x0006,	 /*!<[OUT] it is VOID_RealAudio_BLOCKSIZE*/

}
VORealAudioPARAMETERID;

#pragma pack(pop)


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif

#endif /* __VO_RealAudio_BASE_H_ */


