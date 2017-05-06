#ifndef _VCAMERA_BASE_H_
#define _VCAMERA_BASE_H_

#include "voMMRecord.h"
#include "CPerfTest.h"

enum Internal_Retun_Code
{
	I_VORC_REC_NO_ERRORS				= 0x0001,
	I_VORC_REC_MEMORY_FULL				= 0x0002,
	I_VORC_REC_DISK_FULL				= 0x0004,
	I_VORC_REC_WRITE_FAILED				= 0x0008,
	I_VORC_REC_ENC_FAILED				= 0x0010,
	I_VORC_REC_CC_ERROR					= 0x0020,
	I_VORC_REC_FW_INIT_ERROR			= 0x0040,
	I_VORC_REC_TIME_EXPIRED				= 0x0080,
	I_VORC_REC_SIZE_EXPIRED				= 0x0100,
	I_VORC_REC_AUDIO_ENC_INT_FAILED		= 0x0200,
	I_VORC_REC_VIDEO_ENC_INT_FAILED		= 0x0400,
};

typedef struct _OutputBuffer
{
	unsigned char* pOutBuf[3];
	int OutBufSize[3];
	VO_IV_COLORTYPE OutType;
}OutputBuffer;

typedef struct _InputBuffer
{
	unsigned char* pInBuf;
	int nSrcVideoWidth;
	int nSrcVideoHeight;
	int nDstVideoWidth;
	int nDstVideoHeight;
	VO_IV_COLORTYPE InType;
	VO_IV_RTTYPE RotateType;
}InputBuffer;

#endif

