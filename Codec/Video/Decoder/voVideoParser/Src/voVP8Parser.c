#include <stdlib.h>
#include "string.h"
#include "voVP8Parser.h"

VO_S32 voVP8Init(VO_HANDLE *pParHandle)
{
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER*)malloc(sizeof(VO_VIDEO_PARSER));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0xFF,sizeof(VO_VIDEO_PARSER));
	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voVP8Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_VIDEO_PARSER *pVideoInfo = (VO_VIDEO_PARSER *)pParHandle;
	VO_U32 show_frame=0,first_partition_length_in_bytes=0;
	VO_U32 horiz_scale=0,vert_scale=0;
	VO_U8* inBuffer= pInData->Buffer;
	VO_U32 BufferLen = pInData->Length; 
	pVideoInfo->nFrame_type = (VO_VIDEO_FRAMETYPE)(inBuffer[0] & 1);

	pVideoInfo->isRefFrame = 1;

	if(pVideoInfo->nFrame_type == VO_VIDEO_FRAME_I)
	{
		if(BufferLen<7)
			return VO_ERR_VIDEOPARSER_INPUTDAT;
		pVideoInfo->nProfile = (inBuffer[0] >> 1) & 7;
		show_frame = (inBuffer[0] >> 4) & 1;
		first_partition_length_in_bytes = (inBuffer[0] | (inBuffer[1] << 8) | (inBuffer[2] << 16)) >> 5;
		inBuffer += 3;
		if (first_partition_length_in_bytes > BufferLen)
			return VO_ERR_VIDEOPARSER_INPUTDAT;
		if (inBuffer[0] != 0x9d || inBuffer[1] != 0x01 || inBuffer[2] != 0x2a)
			return VO_ERR_VIDEOPARSER_NOTSUPPORT;

		pVideoInfo->nWidth = (inBuffer[3] | (inBuffer[4] << 8)) & 0x3fff;
		horiz_scale = inBuffer[4] >> 6;
		pVideoInfo->nHeight = (inBuffer[5] | (inBuffer[6] << 8)) & 0x3fff;
		vert_scale = inBuffer[6] >> 6;		
	}
	else
	{
		if(BufferLen<3)
			return VO_ERR_VIDEOPARSER_INPUTDAT;
		pVideoInfo->nProfile = (inBuffer[0] >> 1) & 7;
		show_frame = (inBuffer[0] >> 4) & 1;
		first_partition_length_in_bytes = (inBuffer[0] | (inBuffer[1] << 8) | (inBuffer[2] << 16)) >> 5;
		if(first_partition_length_in_bytes > BufferLen)
			return VO_ERR_VIDEOPARSER_INPUTDAT;
	}
	return VO_RETURN_FMHEADER;
}

VO_S32 voVP8GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER *)pParHandle;
	switch(nID)
	{
	case VO_PID_VIDEOPARSER_VERSION:
		*((VO_U32*)pValue) = pParser->nVersion;
		break;
	case VO_PID_VIDEOPARSER_PROFILE:
		*((VO_U32*)pValue) = pParser->nProfile;
		break;
	case VO_PID_VIDEOPARSER_LEVEL:
		*((VO_U32*)pValue) = pParser->nLevel;
		break;
	case VO_PID_VIDEOPARSER_FRAMETYPE:
		*((VO_U32*)pValue) = pParser->nFrame_type;
		break;
	case VO_PID_VIDEOPARSER_WIDTH:
		*((VO_U32*)pValue) = pParser->nWidth;
		break;
	case VO_PID_VIDEOPARSER_HEIGHT:
		*((VO_U32*)pValue) = pParser->nHeight;
		break;
	case VO_PID_VIDEOPARSER_INTERLANCE:
		*((VO_U32*)pValue) = 0;
		break;
	case VO_PID_VIDEOPARSER_REFFRAME:
		*((VO_U32*)pValue) = 1;
		break;
	case VO_PID_VIDEOPARSER_ISVC1:
		*((VO_U32*)pValue) = 0;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	if(*((VO_U32*)pValue)  == 0xFFFFFFFF)
		return VO_ERR_VIDEOPARSER_NOVALUE;

	return VO_ERR_NONE;
}

VO_S32 voVP8Uninit(VO_HANDLE pParHandle)
{
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER *)pParHandle;
	if(pParser)
	{
		free(pParser);
		pParser = NULL;
	}
	return 0;
}