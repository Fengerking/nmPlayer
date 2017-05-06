#include <string.h>
#include <stdlib.h>

#include "voMPEG2Parser.h"
#include "voVideoParser.h"
#include "voReadBits.h"


typedef struct
{
	VO_VOID* pCodecUser;
	VO_VIDEO_PROFILETYPE nProfile;
	VO_VIDEO_LEVELTYPE nLevel;
	VO_VIDEO_FRAMETYPE nFrame_type;
	VO_U32 nWidth;
	VO_U32 nHeight;
	VO_U32 isInterlace;
	VO_U32 isRefFrame;
	VO_CODECBUFFER SQHeader;

	//private state
	VO_U32 sqState;
	VO_U32 fmState;
} VO_MPEG2_VIDEO_INFO;

#define MPEG2_SC_LEN 4
/* sequence header code is 0x000001b3  4byte*/
static int voSearchMpeg2SHC(unsigned char const *input, int length)
{
	/*find 0x000001b3 */
	unsigned char const *head = input;
	unsigned char const *end = input + length - MPEG2_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	do {
		if (head[0]) {/* faster if most of bytes are not zero*/	
			head++;
			continue;
		}
		/* find 00xx*/
		if (head[1]){
			head += 2; // skip 2 bytes;
			continue;
		}
		/* find 0000xx */
		if (head[2] != 0x01){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find 000001xx*/
		head += 4; /* update pointer first, since it will be used in all cases below*/
		switch (head[-1]){
		case 0xB3:
			/* find sequence header*/
			return (int)(head - input) - 4;
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	} while (head < end);
	/* not finding start code */
	return -1;
}

static int voSearchMpeg2VOPSC(unsigned char const *input, int length)
{
	/*find 0x00000100 */
	unsigned char const *head = input;
	unsigned char const *end = input + length - MPEG2_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	do {
		if (head[0]) {/* faster if most of bytes are not zero*/	
			head++;
			continue;
		}
		/* find 00xx*/
		if (head[1]){
			head += 2; // skip 2 bytes;
			continue;
		}
		/* find 0000xx */
		if (head[2] != 0x01){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find 000001xx*/
		head += 4; /* update pointer first, since it will be used in all cases below*/
		switch (head[-1]){
		case 0x00:
			/* find sequence header*/
			return (int)(head - input) - 4;
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	} while (head < end);
	/* not finding start code */
	return -1;
}

static VO_S32 voParserMpeg2SH(PARSER_BITSTREAM *pBS, VO_MPEG2_VIDEO_INFO *pVideoHeadDataInfo)
{
	VO_U32 i;

	if (ParserReadBits(pBS, 16) != 0 || ParserReadBits(pBS, 16) != 0x1B3) // start_code
	{
		return -1;
	}
	pVideoHeadDataInfo->nWidth = ParserReadBits(pBS, 12);
	pVideoHeadDataInfo->nHeight = ParserReadBits(pBS, 12);


	//skip 38 bits
	ParserFlushBits(pBS, 16);
	ParserFlushBits(pBS, 16);
	ParserFlushBits(pBS, 6);

	if (ParserEndOfBits(pBS))
	{
		return -1;
	}
	if (1 == ParserReadBits(pBS, 1))	//intra_quantiser_matrix
	{
		//skip 8*64 bits
		for (i = 32; i != 0; --i)
		{
			ParserFlushBits(pBS, 16);
		}
	}

	if (ParserEndOfBits(pBS))
	{
		return -1;
	}
	if (1 == ParserReadBits(pBS, 1))	//non_intra_quantiser_matrix
	{
		//skip 8*64 bits
		for (i = 32; i != 0; --i)
		{
			ParserFlushBits(pBS, 16);
		}
	}

	if (ParserEndOfBits(pBS))
	{
		return 0;		//mpeg1, no Sequence extension
	}

	//Sequence extension code is 0x000001B5, ID is 0001
	if (0 == ParserReadBits(pBS, 16)
		&& 0x01B5 == ParserReadBits(pBS, 16)
		&& 0x1 == ParserReadBits(pBS, 4))
	{
		if (ParserEndOfBits(pBS))
		{
			return -1;
		}
		ParserFlushBits(pBS, 1);
		switch (ParserReadBits(pBS, 3))	//profile
		{
		case 0x101:
			pVideoHeadDataInfo->nProfile = VO_VIDEO_MPEG2ProfileSimple;
			break;
		case 0x100:
			pVideoHeadDataInfo->nProfile = VO_VIDEO_MPEG2ProfileMain;
			break;
		case 0x011:
			pVideoHeadDataInfo->nProfile = VO_VIDEO_MPEG2ProfileSNR;
			break;
		case 0x010:
			pVideoHeadDataInfo->nProfile = VO_VIDEO_MPEG2ProfileSpatial;
			break;
		case 0x001:
			pVideoHeadDataInfo->nProfile = VO_VIDEO_MPEG2ProfileHigh;
			break;
		default:
			pVideoHeadDataInfo->nProfile = VO_VIDEO_MPEG2ProfileSimple;
		}

		switch (ParserReadBits(pBS, 4))	//level
		{
		case 0x1010:
			pVideoHeadDataInfo->nLevel = VO_VIDEO_MPEG2LevelLL;
			break;
		case 0x1000:
			pVideoHeadDataInfo->nLevel = VO_VIDEO_MPEG2LevelML;
			break;
		case 0x0110:
			pVideoHeadDataInfo->nLevel = VO_VIDEO_MPEG2LevelH14;
			break;
		case 0x0100:
			pVideoHeadDataInfo->nLevel = VO_VIDEO_MPEG2LevelHL;
			break;
		default:
			pVideoHeadDataInfo->nLevel = VO_VIDEO_MPEG2LevelLL;
		}
		
		pVideoHeadDataInfo->isInterlace = !ParserReadBits(pBS, 1);	//interlace flag
		ParserFlushBits(pBS, 16);
		ParserFlushBits(pBS, 2);
		if (ParserEndOfBits(pBS))
		{
			return -1;
		}
		if (!ParserReadBits(pBS, 1))	//marker
		{
			return -2;
		}
		ParserFlushBits(pBS, 15);
		if (ParserEndOfBits(pBS))
		{
			return -1;
		}
		ParserFlushBits(pBS, 1);
	}

	return 0;	
}

static VO_S32 voParserMpeg2VOP(PARSER_BITSTREAM *pBS, VO_MPEG2_VIDEO_INFO *pVideoHeadDataInfo)
{
	if (ParserReadBits(pBS, 16) != 0 || ParserReadBits(pBS, 16) != 0x100) // start_code
	{
		return -1;
	}
	ParserFlushBits(pBS, 10);
	switch (ParserReadBits(pBS, 3))
	{
	case 1:
		pVideoHeadDataInfo->nFrame_type = VO_VIDEO_FRAME_I;
		pVideoHeadDataInfo->isRefFrame = 1;
		break;
	case 2:
		pVideoHeadDataInfo->nFrame_type = VO_VIDEO_FRAME_P;
		pVideoHeadDataInfo->isRefFrame = 1;
		break;
	case 3:
		pVideoHeadDataInfo->nFrame_type = VO_VIDEO_FRAME_B;
		pVideoHeadDataInfo->isRefFrame = 0;
		break;
	default:
		return -1;
	}

	return 0;
}

VO_S32 voMPEG2Init(VO_HANDLE *pParHandle)
{
	VO_MPEG2_VIDEO_INFO *pParser = (VO_MPEG2_VIDEO_INFO*)malloc(sizeof(VO_MPEG2_VIDEO_INFO));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0,sizeof(VO_MPEG2_VIDEO_INFO));
	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voMPEG2Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_MPEG2_VIDEO_INFO *pParser = (VO_MPEG2_VIDEO_INFO*)pParHandle;	
	VO_PBYTE	pBuffer = pInData->Buffer;		/*!< Buffer pointer */
	VO_U32		Length = pInData->Length;		/*!< Buffer size in byte */
	VO_S32 len;
	VO_U32 sqState = 0;
	//reset state
	pParser->fmState = 0;

	//find & parser sequence header
	len = voSearchMpeg2SHC(pBuffer, Length);
	if (len >= 0)
	{
		PARSER_BITSTREAM bs;
		pBuffer += len;
		Length -= len;
		ParserInitBits(&bs, pBuffer, Length);
		switch (voParserMpeg2SH(&bs, pParser))
		{
		case 0:
			pParser->SQHeader.Length = ParserGetNextBytePos(&bs) - pBuffer;
			if (pParser->SQHeader.Buffer)
			{
				free(pParser->SQHeader.Buffer);
			}
			if ((pParser->SQHeader.Buffer = (VO_U8*)malloc(pParser->SQHeader.Length)) == NULL)
			{
				return VO_ERR_VIDEOPARSER_MEMORY;
			}
			memcpy(pParser->SQHeader.Buffer, pBuffer, pParser->SQHeader.Length);
			pParser->sqState = 1;
			sqState = 1;
			break;
		case -1:
			return VO_ERR_VIDEOPARSER_SQHEAD;
		default:
			;
		}
	}

	//find & parser frame header
	len = voSearchMpeg2VOPSC(pBuffer, Length);
	if (len >= 0)
	{
		PARSER_BITSTREAM bs;
		pBuffer += len;
		Length -= len;
		ParserInitBits(&bs, pBuffer, Length);
		if (voParserMpeg2VOP(&bs, pParser) < 0)
		{
			return VO_ERR_VIDEOPARSER_FMHEAD;
		}
		pParser->fmState = 1;
	}

	if (sqState && pParser->fmState)
	{
		return VO_RETURN_SQFMHEADER;
	}
	if (sqState)
	{
		return VO_RETURN_SQHEADER;
	}
	if (pParser->fmState)
	{
		return VO_RETURN_FMHEADER;
	}

	return VO_ERR_VIDEOPARSER_INPUTDAT;
}

VO_S32 voMPEG2GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_MPEG2_VIDEO_INFO *pParser = (VO_MPEG2_VIDEO_INFO*)pParHandle;
	switch(nID)
	{
	case VO_PID_VIDEOPARSER_PROFILE:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nProfile;
		break;
	case VO_PID_VIDEOPARSER_LEVEL:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nLevel;
		break;
	case VO_PID_VIDEOPARSER_FRAMETYPE:
		if (!pParser->fmState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nFrame_type;
		break;
	case VO_PID_VIDEOPARSER_WIDTH:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nWidth;
		break;
	case VO_PID_VIDEOPARSER_HEIGHT:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nHeight;
		break;
	case VO_PID_VIDEOPARSER_INTERLANCE:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->isInterlace;
		break;
	case VO_PID_VIDEOPARSER_REFFRAME:
		if (!pParser->fmState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->isRefFrame;
		break;
	case VO_PID_VIDEOPARSER_HEADERDATA:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_CODECBUFFER*)pValue) = pParser->SQHeader;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_S32 voMPEG2Uninit(VO_HANDLE pParHandle)
{
	VO_MPEG2_VIDEO_INFO *pParser = (VO_MPEG2_VIDEO_INFO*)pParHandle;
	if(pParser)
	{
		if (pParser->SQHeader.Buffer)
		{
			free(pParser->SQHeader.Buffer);
		}
		free(pParser);
		pParser = NULL;
	}
	return VO_ERR_NONE;
}
