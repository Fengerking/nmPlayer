#include "fVideoHeadDataInfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "voYYDef_common.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef struct {
	VO_U32 nBuf;
	VO_S32 nLen;
	VO_U8 *pEnd;
	VO_S32 nPos;
} BITSTREAM;

typedef enum {
	FREXT_CAVLC444 = 44,       //!< YUV 4:4:4/14 "CAVLC 4:4:4"
	BASELINE       = 66,       //!< YUV 4:2:0/8  "Baseline"
	MAIN           = 77,       //!< YUV 4:2:0/8  "Main"
	EXTENDED       = 88,       //!< YUV 4:2:0/8  "Extended"
	FREXT_HP       = 100,      //!< YUV 4:2:0/8  "High"
	FREXT_Hi10P    = 110,      //!< YUV 4:2:0/10 "High 10"
	FREXT_Hi422    = 122,      //!< YUV 4:2:2/10 "High 4:2:2"
	FREXT_Hi444    = 244,      //!< YUV 4:4:4/14 "High 4:4:4"
	MVC_HIGH       = 118,      //!< YUV 4:2:0/8  "Multiview High"
	STEREO_HIGH    = 128       //!< YUV 4:2:0/8  "Stereo High"
} H264ProfileIDC;

VO_S32 vhdilog2(VO_U32 nData)
{
	VO_S32 i;

	if (!nData) ++nData;
	for (i = 0;nData; ++i)
		nData >>= 1;

	return i;
}

//modified by Harry, 2012.1.18, for issue 10335
void InitBits(BITSTREAM *pBS, VO_U8 *pBuf, VO_U32 nLen)
{
	if (nLen >= 4)
	{
		pBS->nLen = nLen-4;

		pBS->nBuf = ((VO_U32)(*pBuf++)) << 24;
		pBS->nBuf |= ((VO_U32)(*pBuf++)) << 16;
		pBS->nBuf |= ((VO_U32)(*pBuf++)) << 8;
		pBS->nBuf |= (*pBuf++);

		pBS->pEnd = pBuf;
		pBS->nPos = 0;
	}
	else
	{
		pBS->nPos = 32;
		pBS->nBuf = 0;
		while (nLen--)
		{
			pBS->nPos -= 8;
			pBS->nBuf |= ((VO_U32)(*pBuf++)) << pBS->nPos;
		}
		pBS->pEnd = pBuf;
		pBS->nLen = 0;
	}
}

void  FlushBits(BITSTREAM *pBS, VO_U32 nBits)  
{
	pBS->nBuf <<= nBits;
	pBS->nPos += nBits;

	if((pBS->nPos - 16) > 0){
		if(pBS->nLen >= 2){
			VO_U32 nPos = pBS->nPos;

			pBS->nLen -= 2;

			pBS->nBuf |= (*pBS->pEnd++) << (nPos - 8);
			pBS->nBuf |= (*pBS->pEnd++) << (nPos - 16);

			pBS->nPos -= 16;
		}else if (pBS->nLen > 0){
			VO_U32 nPos = pBS->nPos;

			pBS->nBuf |= (*pBS->pEnd++) << (nPos - 8);
			pBS->nLen = 0;
			pBS->nPos -= 8;
		}else{	
			return;
		}
	}
}

VO_U32 ReadBits(BITSTREAM *pBS, VO_U32 nBits)
{
	VO_U32 nBuf;

	nBuf = pBS->nBuf >> (32 - nBits);

	FlushBits(pBS, nBits);

	return nBuf;
}

VO_U32 EndOfBits(BITSTREAM *pBS)
{
	return (pBS->nLen <= 0 && pBS->nPos >= 32);
}

VO_U8 *GetNextBytePos(BITSTREAM *pBS) 
{
	if (pBS->nPos >= 32)
	{
		return pBS->pEnd;
	}
	return pBS->pEnd - ((32 - pBS->nPos)>>3);
}

//return -1 when VOL is not valid
int ParserVOL(BITSTREAM *pBS, VO_VIDEO_HEADDATAINFO *pInfo)
{
	VO_S32 nTimeIncResolution;
	VO_S32 nTimeIncBits;
	VO_S32 visual_object_layer_verid;
	VO_S32 nSprite;

	if (ReadBits(pBS, 16) != 0 || (ReadBits(pBS, 16)&0xFFF0) != 0x120) // start_code
	{
		return -1;
	}
	FlushBits(pBS, 1+8); // random_accessible_vol + video_object_type_indication

	if (ReadBits(pBS, 1)) {//is_object_layer_identifier
		visual_object_layer_verid = ReadBits(pBS, 4);
		FlushBits(pBS, 3);//video_object_layer_priority
	}
	else{
		visual_object_layer_verid = 1;
	}

	if (15 == ReadBits(pBS, 4)){// aspect ratio
		FlushBits(pBS, 8); //aspect_width
		FlushBits(pBS, 8); //aspect_height
	}

	if (ReadBits(pBS, 1)){ // vol control parameters
		FlushBits(pBS, 2); // chroma_format
		FlushBits(pBS, 1); // b-frames

		if (ReadBits(pBS, 1)){ // vbv parameters
			FlushBits(pBS,15);
			if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			FlushBits(pBS,15);
			if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			FlushBits(pBS,15);
			if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			FlushBits(pBS,14);
			if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
			{
				return -1;
			}
			FlushBits(pBS,15);
			if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
			{
				return -1;
			}
		}
	}

	FlushBits(pBS, 2); // shape
	if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
	{
		return -1;
	}

	nTimeIncResolution = ReadBits(pBS, 16);
	if (nTimeIncResolution <= 0)
		nTimeIncResolution = 1;
	nTimeIncBits = vhdilog2(nTimeIncResolution - 1);

	if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	if (ReadBits(pBS, 1)) //fixed_vop_rate
		FlushBits(pBS, nTimeIncBits);

	if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	pInfo->Width = ReadBits(pBS,13); //nWidth

	if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	pInfo->Height = ReadBits(pBS,13); //nHeight
	if (pInfo->Width < 16 || pInfo->Height < 16)	// too small
	{
		return -1;
	}

	if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
	{
		return -1;
	}
	pInfo->Reserved[0] = ReadBits(pBS,1);//interlace
	FlushBits(pBS,1); // obmc_disable

	nSprite = ReadBits(pBS,(visual_object_layer_verid==1)?1:2);
	if (nSprite == 1)	//static sprite
	{
		FlushBits(pBS,13);	//nWidth
		if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
		{
			return -1;
		}
		FlushBits(pBS,13);	//nHeight
		if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
		{
			return -1;
		}
		FlushBits(pBS,13);	//left
		if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
		{
			return -1;
		}
		FlushBits(pBS,13);	//top
		if (EndOfBits(pBS) || !ReadBits(pBS, 1))	// marker
		{
			return -1;
		}

		FlushBits(pBS, 10);	//WarpPoints(6) + Warpgmc_accuracy(2) + brightness_change(1) + low_latency_sprite_enable(1)
	}
	else if (nSprite == 2) //gmc sprite
	{
		FlushBits(pBS, 9);	//WarpPoints(6) + Warpgmc_accuracy(2) + brightness_change(1)
	}

	if (ReadBits(pBS, 1)) //not 8 bit
	{
		FlushBits(pBS, 8);
	}

	if (ReadBits(pBS, 1)) // quant type
	{
		VO_S32 i;
		if (ReadBits(pBS, 1))	// load intra
		{
			i = 0;
			do 
			{
				if (ReadBits(pBS,8))
				{
					++i;
				}
				else
				{
					break;
				}
			} while (i<64);
		}
		if (ReadBits(pBS, 1))	// load inter
		{
			i = 0;
			do 
			{
				if (ReadBits(pBS,8))
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
		FlushBits(pBS, 1); //qpel
	}

	FlushBits(pBS, 2);	//complexity estimation + Resync marker disabled

	if (ReadBits(pBS, 1))	//data partitioned
	{
		FlushBits(pBS, 1);	//rvlc
	}

	if (visual_object_layer_verid != 1)
	{
		if (ReadBits(pBS, 1))	//newpred_enable
		{
			FlushBits(pBS, 3);	// req msg type, seg type
		}
		FlushBits(pBS, 1);	//reduced_resolution_vop_enable
	}

	if (EndOfBits(pBS))
	{
		return -1;
	}

	if (ReadBits(pBS, 1)) //scalability
	{
		FlushBits(pBS, 16);
		FlushBits(pBS, 10);
		if (EndOfBits(pBS))
		{
			return -1;
		}
		FlushBits(pBS, 1);
	}

	return 0;
}


VO_U32 ParserH263Header(BITSTREAM *pBS, VO_VIDEO_HEADDATAINFO *pInfo)
{
	VO_S32 code, UFEP, source_format = 0xf;  //ACW_UNUSED <result>: VO_S32 code, result, UFEP, source_format = 0xf;
	static const VO_S32 size[8][2] = {{ 0,0 },{ 128,96 },{ 176,144 },
	{ 352,288 },{ 704,576 },{ 1408,1152 }};

	code = ReadBits(pBS ,22-8);

	while (!EndOfBits(pBS)&&pBS->nLen>=8){
		code = ((code << 8) + ReadBits(pBS, 8)) & 0x3FFFFF;
		if (code == 32)
			break;
	}

	if (code != 32)
		return -1;

	FlushBits(pBS, 8); // picture timestamp

	if (!ReadBits(pBS, 1)) // marker
		return -1;
	if (ReadBits(pBS, 1)) // h263 id
		return -1;

	FlushBits(pBS, 3);

	code = ReadBits(pBS, 3); // format

	if (code == 7){
		//VO_S32 custom_pcf = 0;

		UFEP = ReadBits (pBS, 3);

		if (UFEP == 1){                           /* OPPTYPE */

			source_format = ReadBits (pBS, 3);		
			/* optional custom setting */
			//custom_pcf = ReadBits(pBS, 1);
			ReadBits(pBS, 1);

			if ((code = ReadBits(pBS, 3))) {/* UMV + SAC + OBMC*/
				return -1;
			}

			FlushBits(pBS, 7); /* AIC */ /*loop_filter*//*bSliceStructuredH263*/
			/*Reference Picture Selection*/
			/*Independent Segment Decoding*//*alt_inter_vlc*/
			//modified_quan 

			FlushBits(pBS, 4);				
			if (8 != ReadBits (pBS, 4)){   // OPPTYPE 		
				return -1;				
			}				
		}

		if ((UFEP == 1) || (UFEP == 0))	{
			/* MMPTYPE */
			FlushBits(pBS, 3);
			if(ReadBits (pBS, 2))
				return -1;

			FlushBits(pBS, 1);      /* rounding type */

			if (1 != ReadBits(pBS, 3)){
				return -1;
			}
		} else {
			/* UFEP is neither 001 nor 000 */
			return -1;
		}

		if(ReadBits(pBS, 1))
			return -1;

		if (UFEP)
		{
			if(source_format == 6)
			{
				/* Read custom picture format */
				FlushBits(pBS, 4);

				code = ReadBits(pBS, 9);
				pInfo->Width = (code + 1 ) * 4;	

				if(!(code = ReadBits (pBS, 1)))
					return -1;

				code = ReadBits(pBS, 9);
				pInfo->Height = code * 4;
			}
			else
			{ 
				pInfo->Width = size[source_format][0]; 
				pInfo->Height = size[source_format][1];
			}
		}
	}
	else
	{
		pInfo->Width = size[code][0];
		pInfo->Height = size[code][1];
	}

	return 0;
}

VO_S32 Mpeg4ProfileLevel(VO_S32 iIndication, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel)
{
	switch (iIndication)
	{
	case 31:
		{
			pVideoProfileLevel->Profile	= VO_VIDEO_MPEG4ProfileSimpleFace;
			pVideoProfileLevel->Level	= VO_VIDEO_MPEG4Level1;

			return VO_ERR_NONE;
		}
	case 32:
		{
			pVideoProfileLevel->Profile	= VO_VIDEO_MPEG4ProfileSimpleFace;
			pVideoProfileLevel->Level	= VO_VIDEO_MPEG4Level2;

			return VO_ERR_NONE;
		}
	case 33:
		{
			pVideoProfileLevel->Profile	= VO_VIDEO_MPEG4ProfileSimpleFBA;
			pVideoProfileLevel->Level	= VO_VIDEO_MPEG4Level1;

			return VO_ERR_NONE;
		}
	case 34:
		{
			pVideoProfileLevel->Profile	= VO_VIDEO_MPEG4ProfileSimpleFBA;
			pVideoProfileLevel->Level	= VO_VIDEO_MPEG4Level2;

			return VO_ERR_NONE;
		}
	}

	switch (iIndication & 0xf0)
	{
	case 0://Simple Profile
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileSimple;
			break;
		}
	case 0x10:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileSimpleScalable;
			break;
		}
	case 0x20:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileCore;
			break;
		}
	case 0x30:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileMain;
			break;
		}
	case 0x40:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileNbit;
			break;
		}
	case 0x50:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileScalableTexture;
			break;
		}
	case 0x60:
		break;
	case 0x70:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileBasicAnimated;
			break;
		}
	case 0x80:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileHybrid;
			break;
		}
	case 0x90:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileAdvancedRealTime;
			break;
		}
	case 0xA0:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileCoreScalable;
			break;
		}
	case 0xB0:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileAdvancedCoding;
			break;
		}
	case 0xC0:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileAdvancedCore;
			break;
		}
	case 0xD0:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileAdvancedScalable;
			break;
		}
	case 0xF0:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_MPEG4ProfileAdvancedSimple;
			break;
		}
	default:
		break;
	}

	switch (iIndication & 0x0f)
	{
	case 0:
	case 8:
		{
			pVideoProfileLevel->Level = VO_VIDEO_MPEG4Level0;
			break;
		}
	case 1:
		{
			pVideoProfileLevel->Level = VO_VIDEO_MPEG4Level1;
			break;
		}
	case 2:
		{
			pVideoProfileLevel->Level = VO_VIDEO_MPEG4Level2;
			break;
		}
	case 3:
		{
			pVideoProfileLevel->Level = VO_VIDEO_MPEG4Level3;
			break;
		}
	case 4:
		{
			pVideoProfileLevel->Level = VO_VIDEO_MPEG4Level4;
			break;
		}
	case 5:
		{
			pVideoProfileLevel->Level = VO_VIDEO_MPEG4Level5;
			break;
		}
	default:
		break;
	}

	return VO_ERR_NONE;
}



/* vop start code is 0x000001b6  4byte*/
#define MPEG4_SC_LEN 4
int voSearchMpeg4VOLSC(unsigned char *input, int length)
{
	/*find 0x000001b6 */
	unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

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
		switch (head[-1] & 0xf0){
		case 0x20:
			/* find voL*/
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


int voSearchMpeg4ProFile(unsigned char *input, int length)
{
	/*find 0x000001b0 */
	unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

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
		case 0xB0:
			/* find voL*/
			return (int)(head - input);
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	} while (head < end);
	/* not finding start code */
	return -1;
}

//add by Harry :issue 10340
//To generate a valid mpeg4 header
//[out]codec_buf: new header buffer
//[in]width,height,nDuration: resolution and (1/fps) are needed
//note: 
//     if codec_buf->Buffer is NULL, 
//     the header size will be set into codec_buf->Length and function will return ignore other input
void GenerateMPEG4Header(VO_CODECBUFFER* codec_buf, VO_U32 width, VO_U32 height, VO_S64 nDuration)
{
	VO_BYTE Mpeg4SeqHeader[]={0x00,0x00 ,0x01 ,0xb0 ,0x01 ,0x00 ,0x00 ,0x01 ,0xb5 ,0x89 ,0x13 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00
		,0x00 ,0x01 ,0x20 ,0x00 ,0xc4 ,0x8d ,0x88 ,0x00 ,0x05 ,0x00 ,0x04 ,0x00 ,0x14 ,0x63 ,0x00 ,0x00
		,0x01 ,0xb2 ,0x4c ,0x61 ,0x76 ,0x63 ,0x35 ,0x32 ,0x2e ,0x32 ,0x37 ,0x2e ,0x30};
	if (codec_buf->Buffer == NULL)
	{
		codec_buf->Length = sizeof(Mpeg4SeqHeader);
		return;
	}
	VO_U32 w_h = (width << 19) | (height << 5);//27 bits
	VO_U32 nFPS;
	if (nDuration == 0)
	{
		nFPS = 24;	//default value, need check
	}
	else
	{
		double fps;
		fps = ((double)1000000000/ ((VO_U64)nDuration));

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
			nFPS = (VO_U32)(fps+0.5);
		}
	}
	Mpeg4SeqHeader[22] |= ((VO_U16)nFPS) >> 13;//3 bits
	Mpeg4SeqHeader[23] |= (VO_U8)(nFPS >> 5); //8 bits
	Mpeg4SeqHeader[24] |= (VO_U8)(nFPS << 3); //5 bits
	Mpeg4SeqHeader[25] |= (VO_U8)(w_h>>24);//8bits
	Mpeg4SeqHeader[26] |= (VO_U8)(w_h>>16);//8btis
	Mpeg4SeqHeader[27] |= (VO_U8)(w_h>>8);//8btis
	Mpeg4SeqHeader[28] |= (VO_U8)(w_h);//3btis

	memcpy(codec_buf->Buffer, Mpeg4SeqHeader, sizeof(Mpeg4SeqHeader));
	codec_buf->Length = sizeof(Mpeg4SeqHeader);
}

int voSearchH264SEI(unsigned char *input, int length)
{
	/*find 0x000001x6 */
	unsigned char *head = input, *end = input + length - MPEG4_SC_LEN;

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
		switch (head[-1] & 0x0f){
		case 0x6:
			/* find voL*/
			return (int)(head - input) - 1;
		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	} while (head < end);
	/* not finding start code */
	return -1;
}

/* vop start code is 0000 0000 0000 0000 1000 00 22bits*/
#define H263_PSC_LEN 3		
int voSearchH263PSC(unsigned char *input, int length)
{
	/*find 0000 0000 0000 0000 1000 00 22bits*/
	unsigned char *head = input, *end = input + length - H263_PSC_LEN;

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
		if ((head[2]&0xfc) != 0x80){
			if(head[2] == 0){
				head += 1;
			}else{
				head += 3; /* skip 3 bytes */
			}
			continue;
		}
		/* find PSC */
		head += 3;
		return (VO_S32)(head - input)-H263_PSC_LEN;
	} while (head < end);
	/* not finding start code */
	return -1;
}

/* sequence header code is 0x000001b3  4byte*/
#define MPEG2_SC_LEN 4
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

typedef struct  
{
	VO_S32 bit_offset;
	VO_BYTE* pos;
}BitStream;

typedef struct 
{
	VO_S32 len;
	VO_S32 info;
}CodeWord;

void AdjustBitstream(BitStream *bs,VO_S32 len)
{
	bs->pos+=(len+bs->bit_offset)/8;
	bs->bit_offset = (len+bs->bit_offset)%8;
}

CodeWord GetVLCSymbol(BitStream* bs)
{
	VO_BYTE *buf = bs->pos;// = GetBitStreamPos();
	VO_S32 bit_offset = 7-bs->bit_offset;
	VO_S32 ctr_bit = (buf[0] &(0x01<<bit_offset));
	VO_S32 len = 0;
	//VO_S32 bit_counter = 0;
	VO_S32 byte_offset = 0;
	VO_S32 inf,info_bit;
	while (0 == ctr_bit) 
	{
		len++;
		bit_offset--;           
		//bit_counter++;
		if(bit_offset<0)
		{						// finish with current VO_BYTE ?
			bit_offset+=8;
			byte_offset++;
		}
		ctr_bit=buf[byte_offset] & (0x01<<(bit_offset));

	}
	//len--;
	// make infoword
	inf=0;                          // shortest possible code is 1, then info is always 0
	for(info_bit=0;info_bit<len; info_bit++)
	{

		//bit_counter++;
		bit_offset--;
		if (bit_offset<0)
		{                 // finished with current VO_BYTE ?
			bit_offset+=8;
			byte_offset++;
		}


		inf <<=1;
		if(buf[byte_offset] & (0x01<<(bit_offset)))
			inf |=1;


	}

	CodeWord code;
	code.len = len;
	code.info = inf;

	//adjust the bitstream position

	AdjustBitstream(bs,2*len+1);
	return code;
}


VO_S32 GetBits(BitStream* bs,VO_S32 len)
{
	VO_S32 forward = len;
	VO_BYTE* buf = bs->pos;
	VO_S32 bit_offset = 7-bs->bit_offset;
	VO_S32 value = 0;
	VO_S32 byte_offset = 0;
	while(len>0)
	{

		value<<=1;
		if(bit_offset < 0)
		{
			bit_offset+=8;
			byte_offset++;
		}
		if(buf[byte_offset] & (0x01<<bit_offset))
			value |=1;

		len--;
		bit_offset--;

	}
	AdjustBitstream(bs,forward);
	return value;
}

VO_S32 u(VO_S32 len, BitStream *pBS)
{
	return GetBits(pBS,len);
}

VO_S32 ue(BitStream *pBS)
{
	CodeWord code = GetVLCSymbol(pBS);
	//return (VO_S32)pow(2,code.len)+code.info-1; 
	return (1<<code.len)+code.info-1; 
}

VO_S32 se(BitStream *pBS)
{
	CodeWord code = GetVLCSymbol(pBS);

	VO_S32 code_num = (1<<code.len)+code.info-1;
	VO_S32 value = (code_num+1)/2;
	if((code_num & 0x01)==0)         // lsb is signed bit
		value = -value;

	return value;
}


VO_S32 CleanTheBitStream(VO_BYTE*	head, VO_S32 size)
{
	VO_BYTE *end = head + size;

	do {
		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx

		if(head[2]!=3)
		{
			head+=3;
		}
		else
		{
			memmove(head+2,head+3,end-(head+3));
			end--;
			head+=2;
		}
	}
	while(head+3<end);

	return 0;
}
void interpret_stereo_video_info_info( unsigned char* payload, int size, VO_S3D_params *S3D )
{
  int field_views_flags;
  int top_field_is_left_view_flag, current_frame_is_left_view_flag;//, next_frame_is_second_view_flag;
  //int left_view_self_contained_flag;
  //int right_view_self_contained_flag;

  BitStream bs = {0};
  bs.pos = payload;

  field_views_flags = u(1, &bs);//"SEI: field_views_flags"
  if (field_views_flags)
  {
    top_field_is_left_view_flag         = u(1, &bs);//"SEI: top_field_is_left_view_flag"
    if(top_field_is_left_view_flag)
      S3D->order = S3D_ORDER_LF;
    else
      S3D->order = S3D_ORDER_RF;
  }
  else
  {
    current_frame_is_left_view_flag     = u(1, &bs);//"SEI: current_frame_is_left_view_flag"
    if(current_frame_is_left_view_flag)
      S3D->order = S3D_ORDER_LF;
    else
      S3D->order = S3D_ORDER_RF;
    //next_frame_is_second_view_flag      = u(1, &bs);//"SEI: next_frame_is_second_view_flag"
    u(1, &bs);//"SEI: next_frame_is_second_view_flag"
  }

  //left_view_self_contained_flag         = u(1, &bs);//"SEI: left_view_self_contained_flag"
  u(1, &bs);//"SEI: left_view_self_contained_flag"
  //right_view_self_contained_flag        = u(1, &bs);//"SEI: right_view_self_contained_flag"
  u(1, &bs);//"SEI: right_view_self_contained_flag"
}
typedef enum {
   checkerboard = 0,
   column = 1,
   row = 2,
   side_by_side = 3,
   top_bottom = 4,
   temporal = 5,
}sframe_packing_arrangement_type;

void interpret_frame_packing_arrangement_info( unsigned char* payload, int size, VO_S3D_params *S3D  )
{
  frame_packing_arrangement_information_struct seiFramePackingArrangement;
  //Bitstream* buf;

  BitStream bs = {0};
  bs.pos = payload;

  S3D->active = true;
  S3D->mode = S3D_MODE_ON;
  S3D->subsampling = S3D_SS_NONE;
  seiFramePackingArrangement.frame_packing_arrangement_id = (unsigned int)ue(&bs );//"SEI: frame_packing_arrangement_id"
  seiFramePackingArrangement.frame_packing_arrangement_cancel_flag = u(1, &bs);//"SEI: frame_packing_arrangement_cancel_flag"
  if ( seiFramePackingArrangement.frame_packing_arrangement_cancel_flag == 0 )
  {
    seiFramePackingArrangement.frame_packing_arrangement_type = (unsigned char)u(7, &bs);//"SEI: frame_packing_arrangement_type"
    switch(seiFramePackingArrangement.frame_packing_arrangement_type)
    {
        case checkerboard:
        {
            S3D->fmt = S3D_FORMAT_CHECKB;
            break;
        }
        case column:
        {
            S3D->fmt = S3D_FORMAT_COL_IL;
            break;
        }
        case row:
        {
            S3D->fmt = S3D_FORMAT_ROW_IL;
            break;
        }
        case side_by_side:
        {
            S3D->fmt = S3D_FORMAT_SIDEBYSIDE;
            S3D->subsampling = S3D_SS_HOR;
            break;
        }
        case top_bottom:
        {
            S3D->fmt = S3D_FORMAT_OVERUNDER;
            S3D->subsampling = S3D_SS_VERT;
            break;
        }
        case temporal:
        {
            S3D->fmt = S3D_FORMAT_FRM_SEQ;
            break;
        }
        default:
        {
            S3D->fmt = S3D_FORMAT_OVERUNDER;
            break;
        }
    }
    seiFramePackingArrangement.quincunx_sampling_flag         = u(1, &bs);//"SEI: quincunx_sampling_flag"
    seiFramePackingArrangement.content_interpretation_type    = (unsigned char)u(6, &bs);//"SEI: content_interpretation_type"
    if(seiFramePackingArrangement.content_interpretation_type)
      S3D->order = S3D_ORDER_LF;
    else
      S3D->order = S3D_ORDER_RF;
    seiFramePackingArrangement.spatial_flipping_flag          = u(1, &bs);//"SEI: spatial_flipping_flag"
    seiFramePackingArrangement.frame0_flipped_flag            = u(1, &bs);//"SEI: frame0_flipped_flag"
    seiFramePackingArrangement.field_views_flag               = u(1, &bs);//"SEI: field_views_flag"
    seiFramePackingArrangement.current_frame_is_frame0_flag   = u(1, &bs);//"SEI: current_frame_is_frame0_flag"
    seiFramePackingArrangement.frame0_self_contained_flag     = u(1, &bs);//"SEI: frame0_self_contained_flag"
    seiFramePackingArrangement.frame1_self_contained_flag     = u(1, &bs);//"SEI: frame1_self_contained_flag"
    if ( seiFramePackingArrangement.quincunx_sampling_flag == 0 && seiFramePackingArrangement.frame_packing_arrangement_type != 5 )
    {
      seiFramePackingArrangement.frame0_grid_position_x = (unsigned char)u(4, &bs);//"SEI: frame0_grid_position_x"
      seiFramePackingArrangement.frame0_grid_position_y = (unsigned char)u(4, &bs);//"SEI: frame0_grid_position_y"
      seiFramePackingArrangement.frame1_grid_position_x = (unsigned char)u(4, &bs);//"SEI: frame1_grid_position_x"
      seiFramePackingArrangement.frame1_grid_position_y = (unsigned char)u(4, &bs);//"SEI: frame1_grid_position_y"
    }
    seiFramePackingArrangement.frame_packing_arrangement_reserved_byte = (unsigned char)u(8, &bs);//"SEI: frame_packing_arrangement_reserved_byte"
    seiFramePackingArrangement.frame_packing_arrangement_repetition_period = (unsigned int)ue(&bs );//"SEI: frame_packing_arrangement_repetition_period"
  }
  seiFramePackingArrangement.frame_packing_arrangement_extension_flag = u(1, &bs);//"SEI: frame_packing_arrangement_extension_flag"

}

VO_S32 ProcessS3D(VO_BYTE* buf,VO_S32 size, VO_S3D_params *S3D)
{
	int payload_type = 0;
  int payload_size = 0;
  int offset = 1;
  unsigned char tmp_byte;
  VO_U8 has_3D = 1;
  unsigned char *pCopy = new unsigned char[size];
  memcpy(pCopy, buf, size);
  CleanTheBitStream(pCopy,size);
  
  do
  {
  	payload_type = 0;
    tmp_byte = pCopy[offset++];
    while (tmp_byte == 0xFF)
    {
      payload_type += 255;
      tmp_byte = pCopy[offset++];
    }
    payload_type += tmp_byte;   // this is the last byte

    payload_size = 0;
    tmp_byte = pCopy[offset++];
    while (tmp_byte == 0xFF)
    {
      payload_size += 255;
      tmp_byte = pCopy[offset++];
    }
    payload_size += tmp_byte;   // thi
    switch ( payload_type )     // sei_payload( type, size );
    {
    case  SEI_STEREO_VIDEO_INFO:
      interpret_stereo_video_info_info( pCopy+offset, payload_size, S3D );
      has_3D = 0;
      break;
    case  SEI_FRAME_PACKING_ARRANGEMENT:
      interpret_frame_packing_arrangement_info( pCopy+offset, payload_size, S3D );
      has_3D = 0;
      break;
    default:
      break;   
    }
    offset += payload_size;
  }while( pCopy[offset] != 0x80 );
  delete []pCopy;
  return has_3D;
}

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

static unsigned char* GetNextNal(unsigned char* currPos,VO_U32* size)
{
	unsigned char* p = currPos;  
	unsigned char* endPos = currPos+(*size)-8;
	for (; p < endPos; p++)
	{
		if (XRAW_IS_ANNEXB2(p))
		{
			p+=4;
			break;
		}
		else if (XRAW_IS_ANNEXB(p))
		{
			p+=3;
			break;
		}
	}
	if(p>=endPos)
		p = NULL;
	else
		*size -= p-currPos;

	return p;
}

VO_S32 GetFrameType (VO_U8* pInData, VO_S32 nInSize)
{
	VO_U32 nal_unit_type     = *(pInData) & 0x1f;
	if (nal_unit_type == 5)
		return 1;
	else if (nal_unit_type == 1)
	{
	   BitStream bs = {0}; 
	   VO_U32 tmp;
	   bs.pos = pInData+1;
	   if(!ue(&bs))
		   return 3;
	   tmp = ue(&bs);
	   if (tmp > 4)
		   tmp -= 5;
       if (tmp == 2)
		   return 2;
	   else
		   return 3;
	}
	else
		return 0;
}

VO_BOOL IsKeyFrame_H264(VO_PBYTE pData, VO_U32 cData, VO_U32 IDR_flag)
{
	VO_PBYTE p = pData;
	VO_S32 frame_type = 0;
	if (XRAW_IS_ANNEXB2(p))
	{
		p+=4;
		cData-=4;
	}
	else if(XRAW_IS_ANNEXB(p))
	{
		p+=3;
		cData-=3;
	}
    while (p!=NULL)
    {
		frame_type = GetFrameType(p,cData);
		if (frame_type)
			break;
		p = GetNextNal(p,&cData);
    }

    if(!frame_type)
		return VO_FALSE;
	if(frame_type == 1)
		return VO_TRUE;
	if(!IDR_flag && frame_type ==2)
		return VO_TRUE;

	return VO_FALSE;

}

static VO_U8 ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

static VO_U8 ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

void Scaling_List_Parser(int *scalingList, int sizeOfScalingList, BitStream *bs)
{
	int j, scanj;
	int delta_scale, lastScale, nextScale;

	lastScale      = 8;
	nextScale      = 8;

	for(j=0; j<sizeOfScalingList; j++)
	{
		scanj = (sizeOfScalingList==16) ? ZZ_SCAN[j]:ZZ_SCAN8[j];

		if(nextScale!=0)
		{
			delta_scale = se (bs);
			nextScale = (lastScale + delta_scale + 256) % 256;
		}

		scalingList[scanj] = (nextScale==0) ? lastScale:nextScale;
		lastScale = scalingList[scanj];
	}
}






VO_S32 ProcessSPS(VO_BYTE* buf,VO_S32 size, seq_parameter_set_rbsp_t *sps)
{
	int return_val = 0;
	BitStream bs = {0};
	
	if(size <= 0)
		return 1;

  unsigned char *pCopy = new unsigned char[size];
  memcpy(pCopy, buf, size);
	CleanTheBitStream(pCopy,size);

	bs.pos = pCopy;

	sps->profile_idc							= u(8, &bs);
	if ((sps->profile_idc!=BASELINE       ) &&
		(sps->profile_idc!=MAIN           ) &&
		(sps->profile_idc!=EXTENDED       ) &&
		(sps->profile_idc!=FREXT_HP       ) &&
		(sps->profile_idc!=FREXT_Hi10P    ) &&
		(sps->profile_idc!=FREXT_Hi422    ) &&
		(sps->profile_idc!=FREXT_Hi444    ) &&
		(sps->profile_idc!=FREXT_CAVLC444 ) && 
		(sps->profile_idc!=MVC_HIGH       ) && 
		(sps->profile_idc!=STEREO_HIGH)
		)
	{
		delete []pCopy;
		return 1;
	}
	sps->constrained_set0_flag					= u(1, &bs);
	sps->constrained_set1_flag					= u(1, &bs);
	sps->constrained_set2_flag					= u(1, &bs);
	sps->constrained_set3_flag					= u(1, &bs);
	VO_S32 reserved_zero						= u(4, &bs);
	if (reserved_zero)
	{
		delete []pCopy;
		return 1;
	}

	sps->level_idc								= u(8, &bs);
	if( sps->level_idc != 10 &&
		sps->level_idc != 11 &&
		sps->level_idc != 12 &&
		sps->level_idc != 13 &&
		sps->level_idc != 20 &&
		sps->level_idc != 21 &&
		sps->level_idc != 22 &&
		sps->level_idc != 30 &&
		sps->level_idc != 31 &&
		sps->level_idc != 32 &&
		sps->level_idc != 40 &&
		sps->level_idc != 41 &&
		sps->level_idc != 42 &&
		sps->level_idc != 50 &&
		sps->level_idc != 51)
	{
		delete []pCopy;
		return 1;
	}

	sps->seq_parameter_set_id					= ue(&bs);
	if (sps->seq_parameter_set_id & 0xffffffe0)
	{
		delete []pCopy;
		return 1;
	}
	sps->chroma_format_idc = 1;
	if((sps->profile_idc==FREXT_HP   ) ||
		(sps->profile_idc==FREXT_Hi10P) ||
		(sps->profile_idc==FREXT_Hi422) ||
		(sps->profile_idc==FREXT_Hi444) ||
		(sps->profile_idc==FREXT_CAVLC444)
		)
	{
		sps->chroma_format_idc = ue(&bs);
		if (sps->chroma_format_idc & 0xfffffffc)
		{
		  delete []pCopy;
		  return 1;
	  }

		if(sps->chroma_format_idc==3)
		{
			/* MCW_UNUSED <separate_colour_plane_flag>: VO_S32 separate_colour_plane_flag =  */ u(1, &bs);
		}

		/* MCW_UNUSED <bit_depth_luma_minus8>: VO_S32 bit_depth_luma_minus8 = */ ue(&bs);
		/* MCW_UNUSED <bit_depth_chroma_minus8>: VO_S32 bit_depth_chroma_minus8 =  */ ue(&bs);
		/* MCW_UNUSED <qpprime_y_zero_transform_bypass_flag> VO_S32 qpprime_y_zero_transform_bypass_flag = */ u(1, &bs);
		VO_S32 seq_scaling_matrix_present_flag = u(1, &bs);

		if(seq_scaling_matrix_present_flag)
		{
			VO_S32 n_ScalingList = (sps->chroma_format_idc != 3) ? 8 : 12;
			VO_S32 i;
			for( i=0; i<n_ScalingList; i++)
			{
				sps->seq_scaling_list_present_flag[i]=u(1, &bs);
				if(sps->seq_scaling_list_present_flag[i])
				{
					if(i<6)
						Scaling_List_Parser(sps->ScalingList4x4[i], 16, &bs);
					else
						Scaling_List_Parser(sps->ScalingList8x8[i-6], 64, &bs);
				}
			}
		}
	}

	sps->log2_max_frame_num_minus4				= ue(&bs);// ("SPS: log2_max_frame_num_minus4"                , s);
	if (sps->log2_max_frame_num_minus4 < 0 || sps->log2_max_frame_num_minus4 > 12)
	{
		delete []pCopy;
		return 1;
	}
	sps->pic_order_cnt_type						= ue(&bs);//"SPS: pic_order_cnt_type"                       , s);
	if (sps->pic_order_cnt_type < 0 || sps->pic_order_cnt_type > 2)
	{
		delete []pCopy;
		return 1;
	}

	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4	= ue(&bs);//"SPS: log2_max_pic_order_cnt_lsb_minus4"           , s)
	else if (sps->pic_order_cnt_type == 1)
	{
		//VO_S32 tmp,count;
		VO_S32 count;
		u(1, &bs);
		se(&bs);
		se(&bs);
		count = ue(&bs);

		for(VO_S32 i=0; i<count; i++)
			se(&bs);
	}											

	sps->num_ref_frames                        = ue(&bs);// ("SPS: num_ref_frames"                         , s);
	sps->gaps_in_frame_num_value_allowed_flag  = u(1, &bs);// ("SPS: gaps_in_frame_num_value_allowed_flag"   , s);
	sps->pic_width_in_mbs_minus1               = ue(&bs);//("SPS: pic_width_in_mbs_minus1"                , s);
	sps->pic_height_in_map_units_minus1        = ue(&bs);// ("SPS: pic_height_in_map_units_minus1"         , s);
	sps->frame_mbs_only_flag                   = u(1, &bs);//("SPS: frame_mbs_only_flag"                    , s);
	if(sps->frame_mbs_only_flag==0)
		sps->mb_adaptive_frame_field_flag		= u(1, &bs);//  ("SPS: mb_adaptive_frame_field_flag"           , s);
	sps->direct_8x8_inference_flag             = u(1, &bs); // (s);
	sps->frame_cropping_flag                   = u(1, &bs); // (s);
	if(sps->frame_cropping_flag)
	{
		sps->frame_cropping_rect_left_offset      = ue (&bs);
		sps->frame_cropping_rect_right_offset     = ue (&bs);
		sps->frame_cropping_rect_top_offset       = ue (&bs);//*(4-2*sps->frame_mbs_only_flag);//TODO:
		sps->frame_cropping_rect_bottom_offset    = ue (&bs);//*(4-2*sps->frame_mbs_only_flag);
	}
  delete []pCopy;
	return return_val;	
}


VO_S32 ProcessSPS_AVC(VO_BYTE* buf, VO_S32 size, seq_parameter_set_rbsp_t *sps)
{
	unsigned char *pCopy = new unsigned char[size];
	memcpy(pCopy, buf, size);

	unsigned char *head = pCopy, *end = pCopy + size - 12;

	while (head < end){//AVCDecoderConfigurationRecord
		if ( 1 == head[0] && (head[5] & 0x1F) )
		{
			unsigned char *pSPS = NULL;
			if ( 0x7 == (head[8] & 0x7) )
				pSPS = head + 9;
			else if ( 0 == head[8] && 0 == head[9] && 0 == head[10] && 1 == head[11] && 0x7 == (head[12] & 0x7) )
				pSPS = head + 13;
			else
				break;

			int i = pSPS - pCopy;
			if (0  == ProcessSPS(pSPS, size - i, sps))
			{
				delete []pCopy;
				return 0;
			}
		}

		head++;
	};

	delete []pCopy;
	return -1;
}

VO_S32 ProcessSPS_H264(VO_BYTE* input, VO_S32 length, seq_parameter_set_rbsp_t *sps)
{
	unsigned char *pCopy = new unsigned char[length];
	memcpy(pCopy, input, length);

	/*find 0x000001x7 */
	unsigned char *head = pCopy, *end = pCopy + length - 3;

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
		if (head[2] != 0x01)
		{
			if(head[2] == 0)
				head += 1;
			else
				head += 3; /* skip 3 bytes */

			continue;
		}
		/* find 000001xx*/
		head += 4; /* update pointer first, since it will be used in all cases below*/
		
		if(head >= end)
		{
			delete []pCopy;

			return -1;
		}
			
		switch (head[-1] & 0x0f){
		case 0x7:
			/* find voL*/
			{
				int i = head - pCopy;
				if (0  == ProcessSPS(head, length - i, sps))
				{
					delete []pCopy;
					return 0;
				}
			}

		default:
			/* find 0000xx, xx > 1*/
			/* do nothing */
			break;
		}
	} while (head < end);

	delete []pCopy;

	return -1;
}


VO_U32 IsInterlace_Mpeg4(VO_CODECBUFFER *pCodecBuf, VO_BOOL *b)
{
	BITSTREAM bs;
	VO_S32 rt = 0;
	VO_S32 nUsedLen = 0;
	VO_VIDEO_HEADDATAINFO VideoHeadDataInfo = {0};

	VO_BYTE *pBuf = new VO_BYTE[pCodecBuf->Length];
	memcpy(pBuf, pCodecBuf->Buffer, pCodecBuf->Length);

	do{
		VO_S32 len = voSearchMpeg4VOLSC(pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		if(len<0)
		{
			delete []pBuf;

			return VO_ERR_BASE;
		}

		nUsedLen += len;

		InitBits(&bs, pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		rt = ParserVOL(&bs, &VideoHeadDataInfo);
		nUsedLen += 4;
	}while(rt);

	delete []pBuf;

	if (VideoHeadDataInfo.Reserved[0])
		*b = VO_TRUE;
	else
		*b = VO_FALSE;

	return VO_ERR_NONE;
}

VO_U32 getResolution_H264(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo)
{
	VO_PBYTE p = new VO_BYTE[pCodecBuf->Length];
	memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);

	seq_parameter_set_rbsp_t seq_params = {0};
	if (ProcessSPS_AVC(p, pCodecBuf->Length, &seq_params) != 0)
	{
		memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);
		if (ProcessSPS_H264(p, pCodecBuf->Length, &seq_params) != 0)
		{
			delete []p;
			return VO_ERR_BASE;
		}
	}

	delete []p;

	static const VO_S32 SubWidthC  [4]= { 1, 2, 2, 1};
	static const VO_S32 SubHeightC [4]= { 1, 2, 1, 1};

	pVideoHeadDataInfo->Width	= (seq_params.pic_width_in_mbs_minus1 + 1)*16;
	pVideoHeadDataInfo->Height = (seq_params.pic_height_in_map_units_minus1 + 1)*16*(2 - seq_params.frame_mbs_only_flag);

	if(seq_params.frame_cropping_flag)
	{
		VO_S32 crop_left   = SubWidthC[seq_params.chroma_format_idc] * seq_params.frame_cropping_rect_left_offset;
		VO_S32 crop_right  = SubWidthC[seq_params.chroma_format_idc] * seq_params.frame_cropping_rect_right_offset;
		VO_S32 crop_top    = SubHeightC[seq_params.chroma_format_idc]*( 2 - seq_params.frame_mbs_only_flag ) * seq_params.frame_cropping_rect_top_offset;
		VO_S32 crop_bottom = SubHeightC[seq_params.chroma_format_idc]*( 2 - seq_params.frame_mbs_only_flag ) * seq_params.frame_cropping_rect_bottom_offset;
		pVideoHeadDataInfo->Width = pVideoHeadDataInfo->Width - crop_left - crop_right;
		pVideoHeadDataInfo->Height = pVideoHeadDataInfo->Height - crop_top - crop_bottom;
	}

	if (!pVideoHeadDataInfo->Width || !pVideoHeadDataInfo->Height ||
		pVideoHeadDataInfo->Width > 3840 || pVideoHeadDataInfo->Height > 2160)
		return  VO_ERR_BASE;

	return VO_ERR_NONE;
}

VO_U32 getProfileLevel_H264(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel)
{
	VO_PBYTE p = new VO_BYTE[pCodecBuf->Length];
	memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);

	seq_parameter_set_rbsp_t seq_params = {0};
	if (ProcessSPS_AVC(p, pCodecBuf->Length, &seq_params) != 0)
	{
		memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);
		if (ProcessSPS_H264(p, pCodecBuf->Length, &seq_params) != 0)
		{
			delete []p;
			return VO_ERR_BASE;
		}
	}

	delete []p;

	switch (seq_params.profile_idc)
	{
	case BASELINE:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileBaseline;
			break;
		}
	case MAIN:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileMain;
			break;
		}
	case EXTENDED:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileExtended;
			break;
		}
	case FREXT_HP:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileHigh;
			break;
		}
	case FREXT_Hi10P:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileHigh10;
			break;
		}
	case FREXT_Hi422:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileHigh422;
			break;
		}
	case FREXT_Hi444:
		{
			pVideoProfileLevel->Profile = VO_VIDEO_AVCProfileHigh444;
			break;
		}
	default:
		{
			return VO_ERR_BASE;
		}
	}

	switch (seq_params.level_idc)
	{
	case 10:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel1;
			break;
		}
	case 11:
		{
			if (seq_params.constrained_set3_flag)
				pVideoProfileLevel->Level = VO_VIDEO_AVCLevel1b;
			else
				pVideoProfileLevel->Level = VO_VIDEO_AVCLevel11;
			break;
		}
	case 12:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel12;
			break;
		}
	case 13:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel13;
			break;
		}
	case 20:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel2;
			break;
		}
	case 21:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel21;
			break;
		}
	case 22:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel22;
			break;
		}
	case 30:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel3;
			break;
		}
	case 31:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel31;
			break;
		}
	case 32:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel32;
			break;
		}
	case 40:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel4;
			break;
		}
	case 41:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel41;
			break;
		}
	case 42:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel42;
			break;
		}
	case 50:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel5;
			break;
		}
	case 51:
		{
			pVideoProfileLevel->Level = VO_VIDEO_AVCLevel51;
			break;
		}
	}

	return VO_ERR_NONE;
}

VO_U32 getResolution_H263(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo)
{
	VO_BYTE *pBuf = new VO_BYTE[pCodecBuf->Length];
	memcpy(pBuf, pCodecBuf->Buffer, pCodecBuf->Length);

	BITSTREAM bs;

	InitBits(&bs, pBuf, pCodecBuf->Length);

	VO_U32 iRet = ParserH263Header(&bs, pVideoHeadDataInfo);

	delete []pBuf;

	if (!pVideoHeadDataInfo->Width || !pVideoHeadDataInfo->Height ||
		pVideoHeadDataInfo->Width > 3840 || pVideoHeadDataInfo->Height > 2160)
		return  VO_ERR_BASE;

	return iRet;
}

VO_U32 getResolution_Mpeg4(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo)
{
	BITSTREAM bs;
	VO_S32 rt = 0;
	VO_S32 nUsedLen = 0;

	VO_BYTE *pBuf = new VO_BYTE[pCodecBuf->Length];
	memcpy(pBuf, pCodecBuf->Buffer, pCodecBuf->Length);

	do{
		VO_S32 len = voSearchMpeg4VOLSC(pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		if(len<0)
		{
			delete []pBuf;

			return VO_ERR_BASE;
		}

		nUsedLen += len;

		InitBits(&bs, pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		rt = ParserVOL(&bs, pVideoHeadDataInfo);
		nUsedLen += 4;
	}while(rt);

	delete []pBuf;

	if (!pVideoHeadDataInfo->Width || !pVideoHeadDataInfo->Height ||
		pVideoHeadDataInfo->Width > 3840 || pVideoHeadDataInfo->Height > 2160)
		return  VO_ERR_BASE;

	return VO_ERR_NONE;
}

//add by leon :issue 7396
VO_U32 Mpeg4VideoInfo(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo)
{
	BITSTREAM bs;
	VO_U32 Init = 0;

	VO_S32 len = voSearchMpeg4ProFile(pCodecBuf->Buffer, pCodecBuf->Length);

	if (len >= 0)
	{
		VO_VIDEO_PROFILELEVEL VideoProfileLevel;
		if ( Mpeg4ProfileLevel(pCodecBuf->Buffer[len], &VideoProfileLevel) )
			return VO_ERR_BASE;

		Init = 1;
	}
	else
	{
		VO_U32 ShortMark;

		ShortMark = ((pCodecBuf->Buffer[0] << 16) | (pCodecBuf->Buffer[1] << 8) | pCodecBuf->Buffer[2]) >> 2;
		if(ShortMark == 0x20)
			return VO_ERR_DEC_MPEG4_HEADER;

	}

	VO_S32 rt = 0;
	VO_S32 nUsedLen = 0;

	VO_BYTE *pBuf = new VO_BYTE[pCodecBuf->Length];
	memcpy(pBuf, pCodecBuf->Buffer, pCodecBuf->Length);

	do{
		len = voSearchMpeg4VOLSC(pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		if(len<0)
		{
			delete []pBuf;

			if(Init)
				return VO_ERR_DEC_MPEG4_HEADER;// No good VOL header
			else
				return VO_ERR_BASE;
		}

		nUsedLen += len;

		InitBits(&bs, pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		rt = ParserVOL(&bs, pVideoHeadDataInfo);
		nUsedLen += 4;
	}while(rt);

	delete []pBuf;

	return VO_ERR_NONE;	
}
//add by leon :issue 7396

/************************************************************************/
// Add by Harry, 2012,1,18, for issue 10335
// Check the MPEG4 sequence header
/************************************************************************/
VO_BOOL IsCorrectSequenceHeader(VO_BYTE* buf, VO_S32 size, VO_U32 *pReserve)
{
	BITSTREAM bs;
	VO_VIDEO_HEADDATAINFO vHeadInfo;
// 	VO_U32 head;

// 	head = ((VO_U32)(*buf))<<24;
// 	head |= ((VO_U32)(*(buf+1)))<<16;
// 	head |= ((VO_U32)(*(buf+2)))<<8;
// 	head |= *(buf+3);
// 	if (head == 0x1B0)
// 	{
		VO_S32 offset = voSearchMpeg4VOLSC(buf, size);
		if (offset < 0)
		{
			return VO_FALSE;
		}
		buf += offset;
		size -= offset;
// 	}
	InitBits(&bs, buf, size);
	if (ParserVOL(&bs, &vHeadInfo) < 0)
	{
		return VO_FALSE;
	}

	//here you can check width, height or interlace which stored in vHeadInfo if necessary

	return VO_TRUE;	
}

/*****************************************************************************************************************
//add by Harry :issue 9305
//get information for next good "Video Sequence Header" coming from pCodecBuf[in]
//the width, height and interlace will output in pVideoHeadDataInfo[out]
//notice: 
//1. no information output about "Profile" & "Level"
//2. the information about "is interlace" will be stored in pVideoHeadDataInfo->Reserved[0]
*****************************************************************************************************************/
VO_U32 Mpeg2VideoInfo(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_HEADDATAINFO *pVideoHeadDataInfo)
{
	BITSTREAM bs;
	BITSTREAM *pBS = &bs;

	//ACW_UNUSED <rt>: VO_S32 rt = 0;
	VO_S32 nUsedLen = 0;

	VO_BYTE *pBuf = new VO_BYTE[pCodecBuf->Length];
	memcpy(pBuf, pCodecBuf->Buffer, pCodecBuf->Length);

	do 
	{
		pVideoHeadDataInfo->Reserved[1] = pCodecBuf->Length;
		VO_S32 len = voSearchMpeg2SHC(pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);

		if (len < 0)
		{
			delete []pBuf;

			return VO_ERR_BASE;
		}

		pVideoHeadDataInfo->Reserved[1] = nUsedLen+len;	//header start pos
		nUsedLen += len + 4;	//skip start code
		InitBits(pBS, pBuf + nUsedLen, pCodecBuf->Length - nUsedLen);
		pVideoHeadDataInfo->Width = ReadBits(pBS, 12);
		pVideoHeadDataInfo->Height = ReadBits(pBS, 12);

		//skip 38 bits
		FlushBits(pBS, 16);
		FlushBits(pBS, 16);
		FlushBits(pBS, 6);

		if (EndOfBits(pBS))
		{
			delete []pBuf;

			return VO_ERR_BASE;
		}
		if (1 == ReadBits(pBS, 1))	//intra_quantiser_matrix
		{
			//skip 8*64 bits
			for (VO_U32 i = 32; i != 0; --i)
			{
				if (EndOfBits(pBS))
				{
					delete []pBuf;

					return VO_ERR_BASE;
				}
				FlushBits(pBS, 16);
			}
		}

		if (EndOfBits(pBS))
		{
			delete []pBuf;

			return VO_ERR_BASE;
		}
		if (1 == ReadBits(pBS, 1))	//non_intra_quantiser_matrix
		{
			//skip 8*64 bits
			for (VO_U32 i = 32; i != 0; --i)
			{
				if (EndOfBits(pBS))
				{
					delete []pBuf;

					return VO_ERR_BASE;
				}
				FlushBits(pBS, 16);
			}
		}
		pVideoHeadDataInfo->Reserved[2] = GetNextBytePos(pBS) - pBuf;
		if (EndOfBits(pBS))
		{
			pVideoHeadDataInfo->Reserved[3] = 0;
			break;	//mpeg1, no Sequence extension
		}

		//Sequence extension code is 0x000001B5, ID is 0001
		if (0 == ReadBits(pBS, 16)
			&& 0x01B5 == ReadBits(pBS, 16)
			&& 0x1 == ReadBits(pBS, 4))
		{
			FlushBits(pBS, 8);
			if (0 == ReadBits(pBS, 1))
			{
				pVideoHeadDataInfo->Reserved[0] = 1;	//interlace flag
			}
			FlushBits(pBS, 16);
			FlushBits(pBS, 2);
			if (EndOfBits(pBS))
			{
				delete []pBuf;

				return VO_ERR_BASE;
			}

			if (!ReadBits(pBS, 1))	// marker
			{
				continue;
			}
			FlushBits(pBS, 15);
			if (EndOfBits(pBS))
			{
				delete []pBuf;

				return VO_ERR_BASE;
			}
			FlushBits(pBS, 1);

			pVideoHeadDataInfo->Reserved[3] = GetNextBytePos(pBS) - pBuf;
			break;
		}
		else
		{
			pVideoHeadDataInfo->Reserved[3] = 0;
			break;	//mpeg1, no Sequence extension
		}
	} while (!EndOfBits(pBS));

	delete []pBuf;
	return VO_ERR_NONE;	
}

//add by Harry :issue 11104
//get a mpeg2 sequence header according to input buf
//must call "ReleaseMpeg2SequenceHead" to release it after used
VO_U32 GetMpeg2SequenceHead(VO_PBYTE pInputBuf,VO_U32 nInputSize,VO_PBYTE* pOutputBuf,VO_U32* nOutputSize)
{
	//ACW_UNUSED <bFind>: VO_U32 bFind = 0;
	VO_PBYTE start;

	VO_CODECBUFFER inBuf = {pInputBuf, nInputSize, 0, NULL};
	VO_VIDEO_HEADDATAINFO info;

	VO_U32 ret = Mpeg2VideoInfo(&inBuf, &info);
	if (ret != VO_ERR_NONE)
	{
		if (info.Reserved[1] >= nInputSize)
		{
			return VO_ERR_FAILED;
		}
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	start = pInputBuf + info.Reserved[1];
	if (info.Reserved[3] == 0)
	{
		//no Sequence extension
		*nOutputSize = info.Reserved[2] - info.Reserved[1];
	}
	else
	{
		*nOutputSize = info.Reserved[3] - info.Reserved[1];
	}

	if (pOutputBuf == NULL)
	{
		return VO_ERR_NONE;
	}

	*pOutputBuf = new VO_BYTE[*nOutputSize];
	if (*pOutputBuf == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	memcpy(*pOutputBuf, start, *nOutputSize);
	return VO_ERR_NONE;
}
//release the header what "GetMpeg2SequenceHead" output
VO_U32 ReleaseMpeg2SequenceHead(VO_PBYTE pHeadBuf)
{
	if (pHeadBuf != NULL)
	{
		delete [] pHeadBuf;
	}
	return VO_ERR_NONE;
}

VO_U32 getProfileLevel_Mpeg4(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel)
{
	VO_S32 len = voSearchMpeg4ProFile(pCodecBuf->Buffer, pCodecBuf->Length);
	if (len >= 0)
	{
		if (0 == Mpeg4ProfileLevel(pCodecBuf->Buffer[len], pVideoProfileLevel) )
			return VO_ERR_NONE;
	}
	else
	{
		VO_U32 ShortMark;

		ShortMark = ((pCodecBuf->Buffer[0] << 16) | (pCodecBuf->Buffer[1] << 8) | pCodecBuf->Buffer[2]) >> 2;
		if(ShortMark == 0x20)
			return VO_ERR_DEC_MPEG4_HEADER;
	}

	return VO_ERR_DEC_MPEG4_BASE;
}

VO_U32 getVIDEO_PARAM_AVCTYPE_H264(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PARAM_AVCTYPE* pAVC)
{
	VO_PBYTE p = new VO_BYTE[pCodecBuf->Length];
	memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);

	seq_parameter_set_rbsp_t seq_params = {0};
	if (ProcessSPS_AVC(p, pCodecBuf->Length, &seq_params) != 0)
	{
		memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);
		if (ProcessSPS_H264(p, pCodecBuf->Length, &seq_params) != 0)
		{
			delete []p;
			return VO_ERR_BASE;
		}
	}

	delete []p;

	pAVC->nRefFrames	= seq_params.num_ref_frames;
	switch (seq_params.profile_idc)
	{
	case BASELINE:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileBaseline;
			break;
		}
	case MAIN:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileMain;
			break;
		}
	case EXTENDED:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileExtended;
			break;
		}
	case FREXT_HP:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileHigh;
			break;
		}
	case FREXT_Hi10P:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileHigh10;
			break;
		}
	case FREXT_Hi422:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileHigh422;
			break;
		}
	case FREXT_Hi444:
		{
			pAVC->eProfile = VO_VIDEO_AVCProfileHigh444;
			break;
		}
	default:
		{
			return VO_ERR_BASE;
		}
	}

	switch (seq_params.level_idc)
	{
	case 10:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel1;
			break;
		}
	case 11:
		{
			if (seq_params.constrained_set3_flag)
				pAVC->eLevel = VO_VIDEO_AVCLevel1b;
			else
				pAVC->eLevel = VO_VIDEO_AVCLevel11;
			break;
		}
	case 12:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel12;
			break;
		}
	case 13:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel13;
			break;
		}
	case 20:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel2;
			break;
		}
	case 21:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel21;
			break;
		}
	case 22:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel22;
			break;
		}
	case 30:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel3;
			break;
		}
	case 31:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel31;
			break;
		}
	case 32:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel32;
			break;
		}
	case 40:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel4;
			break;
		}
	case 41:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel41;
			break;
		}
	case 42:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel42;
			break;
		}
	case 50:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel5;
			break;
		}
	case 51:
		{
			pAVC->eLevel = VO_VIDEO_AVCLevel51;
			break;
		}
	}

	return VO_ERR_NONE;
}

VO_U32 IsInterlace_H264(VO_CODECBUFFER *pCodecBuf, VO_BOOL *b)
{
	VO_PBYTE p = new VO_BYTE[pCodecBuf->Length];
	memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);

	seq_parameter_set_rbsp_t seq_params = {0};
	if (ProcessSPS_AVC(p, pCodecBuf->Length, &seq_params) != 0)
	{
		memcpy(p, pCodecBuf->Buffer, pCodecBuf->Length);
		if (ProcessSPS_H264(p, pCodecBuf->Length, &seq_params) != 0)
		{
			delete []p;
			return VO_ERR_BASE;
		}
	}

	delete []p;

	if (seq_params.frame_mbs_only_flag == 0)
		*b = VO_TRUE;
	else
		*b = VO_FALSE;

	return VO_ERR_NONE;
}

VO_U32 FindPESHeaderInBuffer(VO_PBYTE pBuffer, VO_U32 dwSize)
{
	if(dwSize < 3)
		return -1;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwSize - 2;
	while(pHead < pTail)
	{
		if(pHead[0])
		{
			pHead += 2;
			continue;
		}

		if(pHead == pBuffer || *(pHead - 1))	//previous byte is not 0x00
		{
			//0x00 found
			if(pHead[1])
			{
				pHead += 2;
				continue;
			}

			//0x0000 found
			pHead++;	//point to second 0x00
		}

		while(!pHead[1] && pHead < pTail)
			pHead++;

		if(1 != pHead[1])
		{
			pHead += 2;
			continue;
		}

		return (pHead - pBuffer - 1);
	}

	if(pHead > pTail)
		return -1;

	if(*(pHead - 1))
		return -1;

	if(pHead[0])
		return -1;

	if(1 != pHead[1])
		return -1;

	return (pHead - pBuffer - 1);
}

VO_S32 getS3D_H264(VO_CODECBUFFER *pCodecBuf, VO_S3D_params *S3D)
{
	int iRet = voSearchH264SEI(pCodecBuf->Buffer, pCodecBuf->Length);
	if (-1 == iRet)
		return VO_ERR_BASE;

	pCodecBuf->Buffer += iRet;
	pCodecBuf->Length -= iRet;

	int iLen = FindPESHeaderInBuffer(pCodecBuf->Buffer, pCodecBuf->Length);
	if (-1 == iLen)
	{
		iLen = pCodecBuf->Length;
	}
	else
	{
		iLen--;
	}

	return ProcessS3D(pCodecBuf->Buffer, iLen, S3D);
}

//////////////////////////////////////////////////////////////////////////
#define MAKEFOURCC_WMV(ch0, ch1, ch2, ch3) \
	((VO_U32)(VO_U8)(ch0) | ((VO_U32)(VO_U8)(ch1) << 8) |   \
	((VO_U32)(VO_U8)(ch2) << 16) | ((VO_U32)(VO_U8)(ch3) << 24 ))

#define mmioFOURCC_WMV(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMV(ch0, ch1, ch2, ch3)

#define FOURCC_WMVA_WMV     mmioFOURCC_WMV('W','M','V','A')
#define FOURCC_wmva_WMV     mmioFOURCC_WMV('w','m','v','a')
#define FOURCC_WMV3_WMV     mmioFOURCC_WMV('W','M','V','3')
#define FOURCC_wmv3_WMV     mmioFOURCC_WMV('w','m','v','3')
#define FOURCC_WVC1_WMV     mmioFOURCC_WMV('W','V','C','1')
#define FOURCC_wvc1_WMV     mmioFOURCC_WMV('w','v','c','1')

VO_BOOL CheckWVC1WithSartCode (VO_CODECBUFFER *pInput)
{
	//ACW_UNUSED <iH>: VO_S32 iH;  //ACW_UNUSED <iW>: VO_S32 iW,iH;
	VO_S32 iBuffCount = 0;
	VO_S32 uStartCode;
	//ACW_UNUSED <pExt>: VO_U8  *pExt;
	//ACW_UNUSED <iExtSize>: VO_U32 iExtSize;

	iBuffCount ++; //jump the first byte.

	uStartCode = pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	if(uStartCode != 0x0000010F) 
		return VO_FALSE;

	return VO_TRUE;
}
/*
    pInData: sequence header data
    return: VO_TRUE codec type is WMV, otherwise is VC1
*/
VO_BOOL voIsWMVCodec(VO_CODECBUFFER *pInData)
{
	VO_S32 codecFourCC;
	VO_BITMAPINFOHEADER   *pHeader = NULL;

	if( VO_TRUE == CheckWVC1WithSartCode(pInData) ){//for .acv  	
		codecFourCC = FOURCC_WVC1_WMV;
	}
	else { //without start code
		pHeader = (VO_BITMAPINFOHEADER *)pInData->Buffer;
		codecFourCC = pHeader->biCompression ; 
	}

	if(codecFourCC == FOURCC_WMV3_WMV ||
	   codecFourCC == FOURCC_wmv3_WMV ||
	   codecFourCC == FOURCC_WMVA_WMV ||
	   codecFourCC == FOURCC_wmva_WMV ||
	   codecFourCC == FOURCC_WVC1_WMV ||
	   codecFourCC == FOURCC_wvc1_WMV)
	   return VO_FALSE;
	else
		return VO_TRUE;
}
    
#ifdef _VONAMESPACE
}
#endif
