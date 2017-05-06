#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "voYYDef_VPSR.h"

#include "voMPEG4Parser.h"
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
	VO_S32 nTimeIncBits;
	VO_U32 nSprite;
	VO_U32 isInterlace;
	VO_U32 isDataPartition;
	VO_U32 isRefFrame;
	VO_CODECBUFFER SQHeader;
	VO_USERDATA_Params userData;

	//private state
	VO_U32 pfState;		//0-no profile&level; 1-only profile; 2-only level; 3-both profile&level
	VO_U32 udState;
	VO_U32 sqState;
	VO_U32 fmState;
} VO_MPEG4_VIDEO_INFO;



static VO_S32 volog2(VO_U32 nData)
{
	VO_S32 i;

	if (!nData) ++nData;
	for (i = 0;nData; ++i)
		nData >>= 1;

	return i;
}




#define MPEG4_SC_LEN 4
static int voSearchMpeg4NextHeader(const unsigned char *input, int length)
{
	/*find 0x000001 */
	const unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	while (head < end)
	{
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
		if (head[2] == 0x01)
		{
			/* find 000001xx*/
			return (int)(head - input);
		}
		else
		{
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
		}
	}
	/* not finding start code */
	return -1;
}

static int voSearchMpeg4ProFile(const unsigned char *input, int length)
{
	/*find 0x000001b0 */
	const unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	while (head < end)
	{
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
		case 0xB0:
			/* find pf*/
			return (int)(head - input);
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	}
	/* not finding start code */
	return -1;
}

static int voSearchMpeg4UserData(const unsigned char *input, int length)
{
	/*find 0x000001b2 */
	const unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	while (head < end)
	{
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
		case 0xB2:
			/* find pf*/
			return (int)(head - input);
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	}
	/* not finding start code */
	return -1;
}

static int voSearchMpeg4VOLSC(const unsigned char *input, int length)
{
	/*find 0x0000012X */
	const unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	while (head < end)
	{
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
		switch (head[-1] & 0xf0){
		case 0x20:
			/* find voL*/
			return (int)(head - input) - 4;
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	}
	/* not finding start code */
	return -1;
}

static int voSearchMpeg4VOPSC(const unsigned char *input, int length)
{
	/*find 0x000001b6 */
	const unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

	//printf("input buffer address = 0x%x, length = %d\n", input, length);

	while (head < end)
	{
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
		case 0xB6:
			/* find vop*/
			return (int)(head - input) - 4;
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	}
	/* not finding start code */
	return -1;
}

static VO_S32 voParserMpeg4ProfileLevel(VO_S32 iIndication, VO_MPEG4_VIDEO_INFO *pVideoProfileLevel)
{
	switch (iIndication)
	{
	case 31:
		{
			pVideoProfileLevel->nProfile	= VO_VIDEO_MPEG4ProfileSimpleFace;
			pVideoProfileLevel->nLevel	= VO_VIDEO_MPEG4Level1;

			return VO_ERR_NONE;
		}
	case 32:
		{
			pVideoProfileLevel->nProfile	= VO_VIDEO_MPEG4ProfileSimpleFace;
			pVideoProfileLevel->nLevel	= VO_VIDEO_MPEG4Level2;

			return VO_ERR_NONE;
		}
	case 33:
		{
			pVideoProfileLevel->nProfile	= VO_VIDEO_MPEG4ProfileSimpleFBA;
			pVideoProfileLevel->nLevel	= VO_VIDEO_MPEG4Level1;

			return VO_ERR_NONE;
		}
	case 34:
		{
			pVideoProfileLevel->nProfile	= VO_VIDEO_MPEG4ProfileSimpleFBA;
			pVideoProfileLevel->nLevel	= VO_VIDEO_MPEG4Level2;

			return VO_ERR_NONE;
		}
	}

	switch (iIndication & 0xf0)
	{
	case 0://Simple Profile
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileSimple;
			break;
		}
	case 0x10:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileSimpleScalable;
			break;
		}
	case 0x20:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileCore;
			break;
		}
	case 0x30:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileMain;
			break;
		}
	case 0x40:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileNbit;
			break;
		}
	case 0x50:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileScalableTexture;
			break;
		}
// 	case 0x60:
// 		break;
	case 0x70:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileBasicAnimated;
			break;
		}
	case 0x80:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileHybrid;
			break;
		}
	case 0x90:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileAdvancedRealTime;
			break;
		}
	case 0xA0:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileCoreScalable;
			break;
		}
	case 0xB0:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileAdvancedCoding;
			break;
		}
	case 0xC0:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileAdvancedCore;
			break;
		}
	case 0xD0:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileAdvancedScalable;
			break;
		}
	case 0xF0:
		{
			pVideoProfileLevel->nProfile = VO_VIDEO_MPEG4ProfileAdvancedSimple;
			break;
		}
	default:
		pVideoProfileLevel->pfState &= ~1;	//no profile
		break;
	}

	switch (iIndication & 0x0f)
	{
	case 0:
	case 8:
		{
			pVideoProfileLevel->nLevel = VO_VIDEO_MPEG4Level0;
			break;
		}
	case 1:
		{
			pVideoProfileLevel->nLevel = VO_VIDEO_MPEG4Level1;
			break;
		}
	case 2:
		{
			pVideoProfileLevel->nLevel = VO_VIDEO_MPEG4Level2;
			break;
		}
	case 3:
		{
			pVideoProfileLevel->nLevel = VO_VIDEO_MPEG4Level3;
			break;
		}
	case 4:
		{
			pVideoProfileLevel->nLevel = VO_VIDEO_MPEG4Level4;
			break;
		}
	case 5:
		{
			pVideoProfileLevel->nLevel = VO_VIDEO_MPEG4Level5;
			break;
		}
	default:
		pVideoProfileLevel->pfState &= ~2;	//no level
		break;
	}

	return VO_ERR_NONE;
}

static int voParserMpeg4VOL(PARSER_BITSTREAM *pBS, VO_MPEG4_VIDEO_INFO *pInfo)
{
	VO_S32 nTimeIncResolution;
	VO_S32 visual_object_layer_verid;
// 	VO_S32 nSprite;

	if (ParserReadBits(pBS, 16) != 0 || (ParserReadBits(pBS, 16)&0xFFF0) != 0x120) // start_code
	{
		return -1;
	}
	ParserFlushBits(pBS, 1); // random_accessible_vol

	if (pInfo->pfState&1)
	{
		ParserFlushBits(pBS, 8);
	}
	else
	{
		pInfo->pfState |= 1;
		switch (ParserReadBits(pBS, 8))	//video_object_type_indication
		{
		case 1:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileSimple;
			break;
		case 2:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileSimpleScalable;
			break;
		case 3:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileCore;
			break;
		case 4:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileMain;
			break;
		case 5:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileNbit;
			break;
		case 6:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileBasicAnimated;
			break;
		case 7:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileHybrid;
			break;
		case 8:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileSimpleFace;
			break;
		case 9:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileScalableTexture;
			break;
		case 10:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileAdvancedRealTime;
			break;
		case 11:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileCoreScalable;
			break;
		case 12:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileAdvancedCoding;
			break;
		case 13:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileAdvancedScalable;
			break;
		case 14:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileSimpleFBA;
			break;
			// 	case 15:
			// 		break;
		case 16:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileAdvancedCore;
			break;
		case 17:
			pInfo->nProfile = VO_VIDEO_MPEG4ProfileAdvancedSimple;
			break;
			// 	case 18:
			// 		break;
		default:
			pInfo->pfState &= ~1;
		}
	}

	if (ParserReadBits(pBS, 1)) {//is_object_layer_identifier
		visual_object_layer_verid = ParserReadBits(pBS, 4);
		ParserFlushBits(pBS, 3);//video_object_layer_priority
	}
	else{
		visual_object_layer_verid = 1;
	}

	if (15 == ParserReadBits(pBS, 4)){// aspect ratio
		ParserFlushBits(pBS, 8); //aspect_width
		ParserFlushBits(pBS, 8); //aspect_height
	}

	if (ParserReadBits(pBS, 1)){ // vol control parameters
		ParserFlushBits(pBS, 2); // chroma_format
		ParserFlushBits(pBS, 1); // b-frames

		if (ParserReadBits(pBS, 1)){ // vbv parameters
			ParserFlushBits(pBS,15);
			if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			ParserFlushBits(pBS,15);
			if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			ParserFlushBits(pBS,15);
			if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			ParserFlushBits(pBS,14);
			if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			ParserFlushBits(pBS,15);
			if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
			{
				return -1;
			}
		}
	}

	ParserFlushBits(pBS, 2); // shape
	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}

	nTimeIncResolution = ParserReadBits(pBS, 16);
	if (nTimeIncResolution <= 0)
		nTimeIncResolution = 1;
	pInfo->nTimeIncBits = volog2(nTimeIncResolution - 1);

	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	if (ParserReadBits(pBS, 1)) //fixed_vop_rate
		ParserFlushBits(pBS, pInfo->nTimeIncBits);

	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	pInfo->nWidth = ParserReadBits(pBS,13); //nWidth

	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	pInfo->nHeight = ParserReadBits(pBS,13); //nHeight
	if (pInfo->nWidth < 16 || pInfo->nHeight < 16)	// too small
	{
		return -1;
	}

	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	pInfo->isInterlace = ParserReadBits(pBS,1);//interlace
	ParserFlushBits(pBS,1); // obmc_disable

	pInfo->nSprite = ParserReadBits(pBS,(visual_object_layer_verid==1)?1:2);
	if (pInfo->nSprite == 1)	//static sprite
	{
		ParserFlushBits(pBS,13);	//nWidth
		if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
		{
			return -1;
		}
		ParserFlushBits(pBS,13);	//nHeight
		if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
		{
			return -1;
		}
		ParserFlushBits(pBS,13);	//left
		if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
		{
			return -1;
		}
		ParserFlushBits(pBS,13);	//top
		if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
		{
			return -1;
		}

		ParserFlushBits(pBS, 10);	//WarpPoints(6) + Warpgmc_accuracy(2) + brightness_change(1) + low_latency_sprite_enable(1)
	}
	else if (pInfo->nSprite == 2) //gmc sprite
	{
		ParserFlushBits(pBS, 9);	//WarpPoints(6) + Warpgmc_accuracy(2) + brightness_change(1)
	}

	if (ParserReadBits(pBS, 1)) //not 8 bit
	{
		ParserFlushBits(pBS, 8);
	}

	if (ParserReadBits(pBS, 1)) // quant type
	{
		VO_S32 i;
		if (ParserReadBits(pBS, 1))	// load intra
		{
			i = 0;
			do 
			{
				if (ParserReadBits(pBS,8))
				{
					++i;
				}
				else
				{
					break;
				}
			} while (i<64);
		}
		if (ParserReadBits(pBS, 1))	// load inter
		{
			i = 0;
			do 
			{
				if (ParserReadBits(pBS,8))
				{
					++i;
				}
				else
				{
					break;
				}
			} while (i<64);
		}
	}

	if (visual_object_layer_verid != 1)
	{
		ParserFlushBits(pBS, 1); //qpel
	}

	ParserFlushBits(pBS, 2);	//complexity estimation + Resync marker disabled

	if ((pInfo->isDataPartition = ParserReadBits(pBS, 1)) == 1)	//data partitioned
	{
		ParserFlushBits(pBS, 1);	//rvlc
	}

	if (visual_object_layer_verid != 1)
	{
		if (ParserReadBits(pBS, 1))	//newpred_enable
		{
			ParserFlushBits(pBS, 3);	// req msg type, seg type
		}
		ParserFlushBits(pBS, 1);	//reduced_resolution_vop_enable
	}

	if (ParserEndOfBits(pBS))
	{
		return -1;
	}

	if (ParserReadBits(pBS, 1)) //scalability
	{
		ParserFlushBits(pBS, 16);
		ParserFlushBits(pBS, 10);
		if (ParserEndOfBits(pBS))
		{
			return -1;
		}
		ParserFlushBits(pBS, 1);
	}

	return 0;
}

static int voParserMpeg4VOP(PARSER_BITSTREAM *pBS, VO_MPEG4_VIDEO_INFO *pInfo)
{
	VO_S32 time_increment;

	if (ParserReadBits(pBS, 16) != 0 || ParserReadBits(pBS, 16) != 0x1B6) // start_code
	{
		return -1;
	}
	pInfo->nFrame_type = ParserReadBits(pBS,2);
	pInfo->isRefFrame = pInfo->nFrame_type == VO_VIDEO_FRAME_I || pInfo->nFrame_type == VO_VIDEO_FRAME_P;

	while (ParserReadBits(pBS, 1)) ;

	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}

	if (pInfo->sqState == 0)
	{
		//no sq head
		//try to guess nTimeIncBits
		pInfo->nTimeIncBits = 0;
	}

	if(pInfo->nTimeIncBits==0 || !(ParserShowBits(pBS, pInfo->nTimeIncBits+1)&1)){
// 		printf("try to guess nTimeIncBits\n");

		for(pInfo->nTimeIncBits=1 ;pInfo->nTimeIncBits<16; pInfo->nTimeIncBits++){
			if (    pInfo->nFrame_type == VO_VIDEO_FRAME_P
				|| (pInfo->nFrame_type == VO_VIDEO_FRAME_S && pInfo->nSprite == 2)) {
					if((ParserShowBits(pBS, pInfo->nTimeIncBits+6)&0x37) == 0x30) 
						break;
			}else
				if((ParserShowBits(pBS, pInfo->nTimeIncBits+5)&0x1F) == 0x18) 
					break;
		}

// 		printf("guess nTimeIncBits is %d\n",pInfo->nTimeIncBits);
	}

	time_increment = ParserReadBits(pBS,pInfo->nTimeIncBits);

	if (ParserEndOfBits(pBS) || !ParserReadBits(pBS, 1))	// marker
	{
		return -1;
	}

	if (!ParserReadBits(pBS, 1)) 
	{
		pInfo->nFrame_type = VO_VIDEO_FRAME_NULL;
		pInfo->isRefFrame = 0;
		return 0;
	}  

	if (pInfo->nFrame_type == VO_VIDEO_FRAME_P || pInfo->nFrame_type == VO_VIDEO_FRAME_S)
		ParserFlushBits(pBS,1);	//rounding

	ParserFlushBits(pBS, 3);	//nIntraDCThreshold index

// 	if (pInfo->isInterlace){
// 		ParserFlushBits(pBS, 2);//top_field_first & alternate_scan
// 	}

// 	if (pDec->nVOPType == VO_VIDEO_FRAME_S && (pDec->nSprite == SPRITE_STATIC || pDec->nSprite == SPRITE_GMC)){
// 		VO_S32 i;
// 
// 		for (i = 0 ; i < pDec->nSpriteWarpPoints; i++)
// 		{
// 			VO_S32 length;
// 			VO_S32 x = 0, y = 0;
// 
// 			/* nSprite code borowed from ffmpeg; thx Michael Niedermayer <michaelni@gmx.at> */
// 			length = GetSpritetrajectory(pDec);
// 			if(length){
// 				UPDATE_CACHE(pDec);
// 				x= GetBits(pDec,length);
// 				if ((x >> (length - 1)) == 0) /* if MSB not set it is negative*/
// 					x = - (x ^ ((1 << length) - 1));
// 			}
// 			FLUSH_BITS(pDec,1); //MARKER();
// 
// 			length = GetSpritetrajectory(pDec);
// 			if(length){
// 				UPDATE_CACHE(pDec);
// 				y = GetBits(pDec,length);
// 				if ((y >> (length - 1)) == 0) /* if MSB not set it is negative*/
// 					y = - (y ^ ((1 << length) - 1));
// 			}
// 			FLUSH_BITS(pDec,1); //MARKER();
// 			pDec->warp_point[i] = MAKEMV(x,y);
// 		}
// 	}
// 
// 	pDec->nQuant = GetBits(pDec,pDec->nQuantPrecision); // vop quant
// 
// 	if(pDec->nQuant==0){
// 		pDec->nQuant = 1;
// 		pDec->nMBError |= ERR_QUANT;
// 	}
// 
// 	UPDATE_CACHE(pDec);
// 	if (pDec->nVOPType != VO_VIDEO_FRAME_I)
// 	{
// 		pDec->nFcode = GetBits(pDec,3);
// 
// 		if(pDec->nFcode==0){
// 			pDec->nFcode = 1;
// 			pDec->nMBError |= ERR_FCODE_FOR;
// 		}
// 	}
// 
// 	if (pDec->nVOPType == VO_VIDEO_FRAME_B) 
// 	{
// 		pDec->nBcode = GetBits(pDec,3);
// 
// 		if(pDec->nBcode==0){
// 			pDec->nBcode = 1;
// 			pDec->nMBError |= ERR_FCODE_BAK;
// 		}
// 	}

	return 0;
}

VO_S32 voMPEG4Init(VO_HANDLE *pParHandle)
{
	VO_MPEG4_VIDEO_INFO *pParser = (VO_MPEG4_VIDEO_INFO*)malloc(sizeof(VO_MPEG4_VIDEO_INFO));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0,sizeof(VO_MPEG4_VIDEO_INFO));
	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voMPEG4Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_MPEG4_VIDEO_INFO *pParser = (VO_MPEG4_VIDEO_INFO*)pParHandle;	
	VO_PBYTE	pBuffer = pInData->Buffer;		/*!< Buffer pointer */
	VO_U32		Length = pInData->Length;		/*!< Buffer size in byte */
	VO_S32 len;
	VO_U32 sqState = 0;

	//reset frame state
	pParser->fmState = 0;

	//find & parser profile & level
	len = voSearchMpeg4ProFile(pInData->Buffer, pInData->Length);
	if (len >= 0)
	{
		pParser->pfState = 3;
		voParserMpeg4ProfileLevel(pInData->Buffer[len], pParser);
		pBuffer += len;
		Length -= len;
	}
	//find & parser sequence header
	len = voSearchMpeg4VOLSC(pBuffer, Length);
	if (len >= 0)
	{
		PARSER_BITSTREAM bs;
		pBuffer += len;
		Length -= len;
		ParserInitBits(&bs, pBuffer, Length);
		if (voParserMpeg4VOL(&bs, pParser) < 0)
		{
			return VO_ERR_VIDEOPARSER_SQHEAD;
		}
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
	}

	//find & parser user data
	len = voSearchMpeg4UserData(pBuffer, Length);
	if (len >= 0)
	{
		VO_PBYTE pUserdata[255];
		VO_U32 sizeUserData[255];
		VO_U32 nUserData = 0;

		do 
		{
			pBuffer += len;
			Length -= len;
			len = voSearchMpeg4NextHeader(pBuffer, Length);
			if (len != 0)
			{
				if (len < 0)
				{
					len = Length;
				}
				sizeUserData[nUserData] = len;
				pUserdata[nUserData] = pBuffer;
				++nUserData;
				pBuffer += len;
				Length -= len;
			}
			len = voSearchMpeg4UserData(pBuffer, Length);
		} while (len == 4);		//for continue user data again

		if (nUserData > 0)
		{
			VO_U32 i;
			VO_PBYTE pData;

			for (len = 0, i = 0; i < nUserData; ++i)
			{
				len += sizeUserData[i];
			}
			if (pParser->userData.buffer)
			{
				free(pParser->userData.buffer);
			}
			if ((pParser->userData.buffer = (VO_U8*)malloc(len)) == NULL)
			{
				return VO_ERR_VIDEOPARSER_MEMORY;
			}

			pParser->udState = 1;	
			pParser->userData.count = nUserData;
			for (pData = pParser->userData.buffer, i = 0; i < nUserData; ++i)
			{
				memcpy(pData, pUserdata[i], sizeUserData[i]);
				pParser->userData.size[i] = sizeUserData[i];
				pData += sizeUserData[i];
			}

		}
	}

	//find & parser frame header
	len = voSearchMpeg4VOPSC(pBuffer, Length);
	if (len >= 0)
	{
		PARSER_BITSTREAM bs;
		pBuffer += len;
		Length -= len;
		ParserInitBits(&bs, pBuffer, Length);
		if (voParserMpeg4VOP(&bs, pParser) < 0)
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

VO_S32 voMPEG4GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_MPEG4_VIDEO_INFO *pParser = (VO_MPEG4_VIDEO_INFO*)pParHandle;
	switch(nID)
	{
	case VO_PID_VIDEOPARSER_PROFILE:
		if (!(pParser->pfState&1))
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->nProfile;
		break;
	case VO_PID_VIDEOPARSER_LEVEL:
		if (!(pParser->pfState&2))
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
	case VO_PID_VIDEOPARSER_DATA_PARTITION:
		if (!pParser->sqState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_U32*)pValue) = pParser->isDataPartition;
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
	case VO_PID_VIDEOPARSER_USERDATA:
		if (!pParser->udState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		*((VO_USERDATA_Params*)pValue) = pParser->userData;
		break;
	case VO_PID_VIDEOPARSER_GENERATE_HEAD:
		if (pParser->sqState || !pParser->fmState)
		{
			return VO_ERR_WRONG_STATUS;
		}
		{
			VO_PARSER_MPEG4_GENERATE_HEAD *pHeadInfo = (VO_PARSER_MPEG4_GENERATE_HEAD*)pValue;
			VO_BYTE Mpeg4SeqHeader[]={0x00,0x00 ,0x01 ,0xb0 ,0x01 ,0x00 ,0x00 ,0x01 ,0xb5 ,0x89 ,0x13 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00
				,0x00 ,0x01 ,0x20 ,0x00 ,0xc4 ,0x8d ,0x88 ,0x00 ,0x05 ,0x00 ,0x04 ,0x00 ,0x14 ,0x63 ,0x00 ,0x00
				,0x01 ,0xb2 ,0x4c ,0x61 ,0x76 ,0x63 ,0x35 ,0x32 ,0x2e ,0x32 ,0x37 ,0x2e ,0x30};
			VO_U32 w_h = (pHeadInfo->width << 19) | (pHeadInfo->height << 5);//27 bits
			VO_S32 nFPS;
			if (pHeadInfo->nDuration == 0)
			{
				nFPS = 24;	//default value, need check
			}
			else
			{
				double fps;
				fps = ((double)1000000000/ ((VO_U64)pHeadInfo->nDuration));

				//add by Harry, issue 16603
				//ISO 14496-2, Table 6-15
				if (fabs(fps - 7.5) < 0.01)
				{
					nFPS = 15;
				}
				else if (fabs(fps - 29.97) < 0.001)
				{
					nFPS = 30000;
				}
				else if (fabs(fps - 59.94) < 0.001)
				{
					nFPS = 60000;
				}
				else
				{
					nFPS = (VO_S32)(fps+0.5);
				}
			}

			//check the nFPS
			assert(pParser->nTimeIncBits > 0);
			if (nFPS > (1 << pParser->nTimeIncBits) || nFPS <= (1 << (pParser->nTimeIncBits - 1)))
			{
				//manually adjust
				nFPS = 1<<pParser->nTimeIncBits;
			}

			Mpeg4SeqHeader[22] |= ((VO_U16)nFPS) >> 13;//3 bits
			Mpeg4SeqHeader[23] |= (VO_U8)(nFPS >> 5); //8 bits
			Mpeg4SeqHeader[24] |= (VO_U8)(nFPS << 3); //5 bits
			Mpeg4SeqHeader[25] |= (VO_U8)(w_h>>24);//8bits
			Mpeg4SeqHeader[26] |= (VO_U8)(w_h>>16);//8btis
			Mpeg4SeqHeader[27] |= (VO_U8)(w_h>>8);//8btis
			Mpeg4SeqHeader[28] |= (VO_U8)(w_h);//3btis

			pParser->SQHeader.Length = sizeof(Mpeg4SeqHeader);
			if (pParser->SQHeader.Buffer)
			{
				free(pParser->SQHeader.Buffer);
			}
			if ((pParser->SQHeader.Buffer = (VO_U8*)malloc(pParser->SQHeader.Length)) == NULL)
			{
				return VO_ERR_VIDEOPARSER_MEMORY;
			}
			memcpy(pParser->SQHeader.Buffer, Mpeg4SeqHeader, pParser->SQHeader.Length);

			pHeadInfo->Header = pParser->SQHeader;
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_S32 voMPEG4Uninit(VO_HANDLE pParHandle)
{
	VO_MPEG4_VIDEO_INFO *pParser = (VO_MPEG4_VIDEO_INFO*)pParHandle;
	if(pParser)
	{
		if (pParser->SQHeader.Buffer)
		{
			free(pParser->SQHeader.Buffer);
		}
		if (pParser->userData.buffer)
		{
			free(pParser->userData.buffer);
		}
		free(pParser);
		pParser = NULL;
	}
	return VO_ERR_NONE;
}
