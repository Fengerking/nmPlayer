#include <string.h>
#include <stdlib.h>

#include "voH263Parser.h"
#include "voVideoParser.h"
#include "voReadBits.h"


typedef struct
{
	VO_VOID* pCodecUser;
// 	VO_VIDEO_PROFILETYPE nProfile;
// 	VO_VIDEO_LEVELTYPE nLevel;
	VO_VIDEO_FRAMETYPE nFrame_type;
	VO_U32 nWidth;
	VO_U32 nHeight;
// 	VO_S32 nTimeIncBits;
	VO_U32 isInterlace;
	VO_U32 isRefFrame;

	//private state
// 	VO_U32 pfState;
// 	VO_U32 sqState;
	VO_U32 fmState;
} VO_H263_VIDEO_INFO;

/* vop start code is 0000 0000 0000 0000 1000 00 22bits*/
#define H263_PSC_LEN 3		
static int voSearchH263PSC(const unsigned char *input, int length, unsigned int bIsS263)
{
	/*find 0000 0000 0000 0000 1000 00 22bits*/
	const unsigned char *head = input, *end = input + length - H263_PSC_LEN;
	unsigned char scFlag = 0xFC;
	if (bIsS263)
	{
		scFlag = 0x80;
	}
	do {
		if (head[0]) {/* faster if most of bytes are not zero*/
			head++;
			continue;
		}
		/* find 0x00xx */
		if (head[1]){
			head += 2; /* skip 2 bytes */
			continue;
		}
		/* find 0x0000xx */
		if ((head[2]&scFlag) != 0x80){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find PSC */
		head += 3;
		return (int)(head - input)-H263_PSC_LEN;
	} while (head < end);
	/* not finding start code */
	return -1;
}

static VO_U32 voParserH263Header(PARSER_BITSTREAM *pBS, VO_H263_VIDEO_INFO *pInfo)
{
	VO_S32 code, UFEP, source_format = 0xf;
	static const VO_S32 size[8][2] = {{ 0,0 },{ 128,96 },{ 176,144 },
	{ 352,288 },{ 704,576 },{ 1408,1152 }};

	code = ParserReadBits(pBS ,22-8);

	while (!ParserEndOfBits(pBS)&&pBS->nLen>=8){
		code = ((code << 8) + ParserReadBits(pBS, 8)) & 0x3FFFFF;
		if (code == 32)
			break;
	}

	if (code != 32)
		return -1;

	ParserFlushBits(pBS, 8); // picture timestamp

	if (!ParserReadBits(pBS, 1)) // marker
		return -1;
	if (ParserReadBits(pBS, 1)) // h263 id
		return -1;

	ParserFlushBits(pBS, 3);

	code = ParserReadBits(pBS, 3); // format

	if (code == 7){
		VO_S32 custom_pcf = 0;

		UFEP = ParserReadBits (pBS, 3);

		if (UFEP == 1){                           /* OPPTYPE */

			source_format = ParserReadBits (pBS, 3);		
			/* optional custom setting */
			custom_pcf = ParserReadBits(pBS, 1);

			if((code = ParserReadBits(pBS, 3))){/* UMV + SAC + OBMC*/
				return -1;
			}

			ParserFlushBits(pBS, 7); /* AIC */ /*loop_filter*//*bSliceStructuredH263*/
			/*Reference Picture Selection*/
			/*Independent Segment Decoding*//*alt_inter_vlc*/
			//modified_quan 

			ParserFlushBits(pBS, 4);				
			if (8 != ParserReadBits (pBS, 4)){   // OPPTYPE 		
				return -1;				
			}				
		}

		if ((UFEP == 1) || (UFEP == 0))	{
			/* MMPTYPE */
			pInfo->nFrame_type = ParserReadBits(pBS, 3);
			if(ParserReadBits (pBS, 2))
				return -1;

			ParserFlushBits(pBS, 1);      /* rounding type */

			if (1 != ParserReadBits(pBS, 3)){
				return -1;
			}
		} else {
			/* UFEP is neither 001 nor 000 */
			return -1;
		}

		if(ParserReadBits(pBS, 1))
			return -1;

		if (UFEP)
		{
			if(source_format == 6)
			{
				/* Read custom picture format */
				ParserFlushBits(pBS, 4);

				code = ParserReadBits(pBS, 9);
				pInfo->nWidth = (code + 1 ) * 4;	

				if(!(code = ParserReadBits (pBS, 1)))
					return -1;

				code = ParserReadBits(pBS, 9);
				pInfo->nHeight = code * 4;
			}
			else
			{ 
				pInfo->nWidth = size[source_format][0]; 
				pInfo->nHeight = size[source_format][1];
			}
		}
	}
	else
	{
		pInfo->nFrame_type = ParserReadBits(pBS, 1);
		pInfo->nWidth = size[code][0];
		pInfo->nHeight = size[code][1];
	}
	pInfo->isRefFrame = pInfo->nFrame_type == VO_VIDEO_FRAME_I || pInfo->nFrame_type == VO_VIDEO_FRAME_P;

	return 0;
}

VO_S32 voH263Init(VO_HANDLE *pParHandle)
{
	VO_H263_VIDEO_INFO *pParser = (VO_H263_VIDEO_INFO*)malloc(sizeof(VO_H263_VIDEO_INFO));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0,sizeof(VO_H263_VIDEO_INFO));
	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voH263Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_H263_VIDEO_INFO *pParser = (VO_H263_VIDEO_INFO*)pParHandle;	
	VO_PBYTE	pBuffer = pInData->Buffer;		/*!< Buffer pointer */
	VO_U32		Length = pInData->Length;		/*!< Buffer size in byte */
	VO_S32 len;

	//reset state
	pParser->fmState = 0;

	//find & parser profile & level
	len = voSearchH263PSC(pInData->Buffer, pInData->Length, 0);
	if (len >= 0)
	{
		PARSER_BITSTREAM bs;
		pBuffer += len;
		Length -= len;
		ParserInitBits(&bs, pBuffer, Length);
		if (voParserH263Header(&bs, pParser) < 0)
		{
			return VO_ERR_VIDEOPARSER_SQHEAD;
		}
		pParser->fmState = 1;
		return VO_RETURN_FMHEADER;
	}

	return VO_ERR_VIDEOPARSER_INPUTDAT;
}

VO_S32 voH263GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_H263_VIDEO_INFO *pParser = (VO_H263_VIDEO_INFO*)pParHandle;
	switch(nID)
	{
// 	case VO_PID_VIDEOPARSER_PROFILE:
// 		if (!pParser->pfState)
// 		{
// 			return VO_ERR_WRONG_STATUS;
// 		}
// 		*((VO_U32*)pValue) = pParser->nProfile;
// 		break;
// 	case VO_PID_VIDEOPARSER_LEVEL:
// 		if (!pParser->pfState)
// 		{
// 			return VO_ERR_WRONG_STATUS;
// 		}
// 		*((VO_U32*)pValue) = pParser->nLevel;
// 		break;
	case VO_PID_VIDEOPARSER_FRAMETYPE:
		if (!pParser->fmState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nFrame_type;
		break;
	case VO_PID_VIDEOPARSER_WIDTH:
		if (!pParser->fmState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nWidth;
		break;
	case VO_PID_VIDEOPARSER_HEIGHT:
		if (!pParser->fmState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nHeight;
		break;
	case VO_PID_VIDEOPARSER_INTERLANCE:
		if (!pParser->fmState)
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
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_S32 voH263Uninit(VO_HANDLE pParHandle)
{
	VO_H263_VIDEO_INFO *pParser = (VO_H263_VIDEO_INFO*)pParHandle;
	if(pParser)
	{
		free(pParser);
		pParser = NULL;
	}
	return VO_ERR_NONE;
}
