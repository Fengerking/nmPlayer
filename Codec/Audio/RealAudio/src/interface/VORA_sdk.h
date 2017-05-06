
#ifndef __VORA_SDK_H__
#define __VORA_SDK_H__

#include "voRADecID.h"
#include "voRealAudio.h"
#include "ra_decode.h"

typedef struct  
{
	VO_AUDIO_CODECAPI				RealDecAPI;
	VORA_RAW_INIT_PARAM				rawParam;
	VORA_INIT_PARAM*				rmParam;
	
	void*							bufPara;
	void*							customData;

	unsigned char					*inbuf;
	int								inlength;

	void*							hCheck;

	VO_MEM_OPERATOR					*pvoMemop;
	VO_LIB_OPERATOR					*pLibOperator;
}VORA_SDK;

typedef struct  
{
	ra_decode*		pDecoder;
	int maxoutSize;
	
}VORA_RMFmtParam;

VO_U32 VO_API voRealAudioDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData );
VO_U32 VO_API voRealAudioDecUninit(VO_HANDLE hCodec);
VO_U32 VO_API voRealAudioDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput);
VO_U32 VO_API voRealAudioDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);															
VO_U32 VO_API voRealAudioDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
VO_U32 VO_API voRealAudioDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);

VO_U32 VO_API voRealAudioRAWDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData );
VO_U32 VO_API voRealAudioRAWDecUninit(VO_HANDLE hCodec);
VO_U32 VO_API voRealAudioRAWDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);															
VO_U32 VO_API voRealAudioRAWDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
VO_U32 VO_API voRealAudioRAWDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);

VO_U32 VO_API voRealAudioRMFmtDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData );
VO_U32 VO_API voRealAudioRMFmtDecUninit(VO_HANDLE hCodec);
VO_U32 VO_API voRealAudioRMFmtDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);															
VO_U32 VO_API voRealAudioRMFmtDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
VO_U32 VO_API voRealAudioRMFmtDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);


VO_U32 VO_API voRealAudioRMBufDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData );
VO_U32 VO_API voRealAudioRMBufDecUninit(VO_HANDLE hCodec);
VO_U32 VO_API voRealAudioRMBufDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo);															
VO_U32 VO_API voRealAudioRMBufDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
VO_U32 VO_API voRealAudioRMBufDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
void* rm_memory_malloc(VO_MEM_OPERATOR *pMemOP, void* pUserMem, unsigned int ulSize);
UINT32 rm_memory_free(VO_MEM_OPERATOR *pMemOP, void* pUserMem, void* ptr);
#endif /* __VORA_SDK_H__*/

