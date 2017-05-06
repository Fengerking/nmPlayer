/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include "voMpeg4Parser.h"
#include "voMpeg4Frame.h"
#include "voMpeg4Haff.h"
#include "voMpegReadbits.h"
#include "voMpegBuf.h"
#include "voMpeg4Rvlc.h"
#include "voMpeg4Gmc.h"
#include "voMpeg4Dec.h"
#include "voMpeg4MB.h"
#include "voMpeg4Decoder.h"

VOCONST VO_U8 nIntraDCThreshold[] = 
{
	32,
	13,
	15,
	17,
	19,
	21,
	23,
	1,
};

VOCONST VO_U8 def_quant_intra[64] = 
{
	 8, 17, 18, 19, 21, 23, 25, 27,
	17, 18, 19, 21, 23, 25, 27, 28,
	20, 21, 22, 23, 24, 26, 28, 30,
	21, 22, 23, 24, 26, 28, 30, 32,
	22, 23, 24, 26, 28, 30, 32, 35,
	23, 24, 26, 28, 30, 32, 35, 38,
	25, 26, 28, 30, 32, 35, 38, 41,
	27, 28, 30, 32, 35, 38, 41, 45
};

VOCONST VO_U8 def_quant_inter[64] = 
{
	16, 17, 18, 19, 20, 21, 22, 23,
	17, 18, 19, 20, 21, 22, 23, 24,
	18, 19, 20, 21, 22, 23, 24, 25,
	19, 20, 21, 22, 23, 24, 26, 27,
	20, 21, 22, 23, 25, 26, 27, 28,
	21, 22, 23, 24, 26, 27, 28, 30,
	22, 23, 24, 26, 27, 28, 30, 31,
	23, 24, 25, 27, 28, 30, 31, 33
};

VOCONST VO_U8 default_h263_chroma_qscale_table[32]={
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
};

VOCONST VO_U8 h263_chroma_qscale_table[32]={
    0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 9,10,10,11,11,12,12,12,13,13,13,14,14,14,14,14,15,15,15,15,15
};

void getmatrix( VO_MPEG4_DEC* pDec, VO_U8* m )
{
	VO_S32 i = 0;
	VO_S32 last = 0,value;

	do 
	{
		UPDATE_CACHE(pDec);

		if ((value = GetBits(pDec,8)))
		{
			last = value;
			m[scanTab[0][i++]] = (VO_U8)value;
		}
		else
		{
			break;
		}
	}
	while (i<64);

//     i--;
	while (i < 64) 
		m[scanTab[0][i++]] = (VO_U8)last;
}


VO_U32 GetVOLHeader( VO_MPEG4_DEC* pDec )
{
	VO_S32 nWidth,nHeight,aspect;
	VO_S32 visual_object_layer_verid;
	VO_S32 is_object_layer_identifier;
	VO_U32 voRC = VO_ERR_NONE;
#if ENABLE_HEADINFO
	VO_CHAR tmpInfo[128];
#endif

	FLUSH_BITS_LARGE(pDec,32+1+8); // start_code + random_accessible_vol + video_object_type_indication

	is_object_layer_identifier = GetBits(pDec,1);

	if (is_object_layer_identifier) {
		visual_object_layer_verid = GetBits(pDec,4);
		FLUSH_BITS(pDec,3); // video_object_layer_priority
	} 
	else {
		visual_object_layer_verid = 1;
	}

	aspect = GetBits(pDec,4); // aspect ratio
	switch (aspect)
	{
	case ASPECT_SQUARE:
		pDec->nAspectRadio = VO_RATIO_11;
		break;
	case ASPECT_625TYPE_43:
	case ASPECT_525TYPE_43:
		pDec->nAspectRadio = VO_RATIO_43;
		break;
	case ASPECT_625TYPE_169:
	case ASPECT_525TYPE_169:
		pDec->nAspectRadio = VO_RATIO_169;
		break;
	case ASPECT_CUSTOM:
		{
		UPDATE_CACHE(pDec);
			pDec->nAspectRadio = GetBits(pDec,8) << 16; //aspect_width
			pDec->nAspectRadio |= GetBits(pDec,8); //aspect_height
			break;
		}
	default:
		pDec->nAspectRadio = 0;		
	}

	if (GetBits(pDec, 1)){ // vol control parameters
		FLUSH_BITS(pDec,2); // chroma_format
		pDec->bLowDelay = (VO_S8)GetBits(pDec,1); // b-frames
		if (GetBits(pDec, 1)) // vbv parameters
			FLUSH_BITS_LARGE(pDec,15+1+15+1+15+1+3+11+1+15+1);
	}

#if ENABLE_HEADINFO
	if(pDec->pHeadInfo){
		sprintf(tmpInfo, "\r\n B frame = %d", pDec->bLowDelay);
		strcpy(pDec->pHeadInfo->Description, tmpInfo);
	}
#endif

	FLUSH_BITS(pDec,2+1); // shape + marker
	UPDATE_CACHE(pDec);

	pDec->nTimeIncResolution = GetBits(pDec,16);
	if (pDec->nTimeIncResolution <= 0)
		pDec->nTimeIncResolution = 1;
	pDec->nTimeIncBits = _log2(pDec->nTimeIncResolution-1);

	FLUSH_BITS(pDec,1); // marker
	if (GetBits(pDec, 1)) //fixed_vop_rate
		FLUSH_BITS_LARGE(pDec,pDec->nTimeIncBits);

	FLUSH_BITS(pDec,1); // marker
	UPDATE_CACHE_LARGE(pDec);
	nWidth = GetBits(pDec,13); //nWidth

	FLUSH_BITS(pDec,1);
	UPDATE_CACHE(pDec);
	nHeight = GetBits(pDec,13); //nHeight
#if ENABLE_HEADINFO
	if(pDec->pHeadInfo){
		sprintf(tmpInfo, "\r\n width = %d, height = %d", nWidth, nHeight);
		strcat(pDec->pHeadInfo->Description, tmpInfo);
	}
#endif
	FLUSH_BITS(pDec,1); // marker
	pDec->bInterlaced = (VO_S8)GetBits(pDec,1);
	FLUSH_BITS(pDec,1); // obmc_disable

	if (!nWidth || !nHeight || \
		(nWidth >= (MB_X*16)) || (nHeight >= (MB_Y*16))){
			return  VO_ERR_DEC_MPEG4_HEADER;
	}

	UPDATE_CACHE(pDec);
	pDec->nSprite = GetBits(pDec,(visual_object_layer_verid==1)?1:2);

	if (pDec->nSprite == SPRITE_STATIC || pDec->nSprite == SPRITE_GMC){

#if ENABLE_HEADINFO
		if(pDec->pHeadInfo){
			sprintf(tmpInfo, "\r\n Sprite coding!");
			strcat(pDec->pHeadInfo->Description, tmpInfo);
		}
#endif
		if (pDec->nSprite != SPRITE_GMC){
			FLUSH_BITS_LARGE(pDec,13+1+13+1+13+1+13+1); //nWidth+marker+nHeight+marker+left+marker+top+marker
		}
		pDec->nSpriteWarpPoints = GetBits(pDec,6);	
		if (pDec->nSpriteWarpPoints > 3)
		{
			pDec->nSpriteWarpPoints = 0;
			return  VO_ERR_DEC_MPEG4_HEADER;
		}
		pDec->nSpriteWarpgmc_accuracy = GetBits(pDec,2);
		FLUSH_BITS(pDec,1); // brightness change not supported

		if (pDec->nSprite != SPRITE_GMC)
			FLUSH_BITS(pDec,1); // low_latency_sprite_enable
	}

	UPDATE_CACHE(pDec);
	if (GetBits(pDec, 1)){  // not 8 bit
		pDec->nQuantPrecision = GetBits(pDec,4);
		FLUSH_BITS(pDec,4); // bit per pixel
	}else
		pDec->nQuantPrecision = 5;

	pDec->nQuantType = GetBits(pDec,1);
	if (pDec->nQuantType){ // quant type
		if (GetBits(pDec, 1))	// load intra
			getmatrix(pDec,pDec->quant[0]);
		else
			CopyMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->quant[0], (VO_U8*)def_quant_intra,sizeof(def_quant_intra));

		if (GetBits(pDec, 1))	// load inter
			getmatrix(pDec,pDec->quant[1]);
		else
			CopyMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->quant[1], (VO_U8*)def_quant_inter,sizeof(def_quant_inter));
		pDec->vld_block = MpegGetBlockVld;
#if ENABLE_HEADINFO
		if(pDec->pHeadInfo){
			sprintf(tmpInfo, "\r\n mpeg quant!");
			strcat(pDec->pHeadInfo->Description, tmpInfo);
		}
#endif

	}else{
		pDec->vld_block = Mpeg4GetBlockVld;
#if ENABLE_HEADINFO
		if(pDec->pHeadInfo){
			sprintf(tmpInfo, "\r\n mpeg4 quant!");
			strcat(pDec->pHeadInfo->Description, tmpInfo);
		}
#endif
	}


	UPDATE_CACHE(pDec);
	if (visual_object_layer_verid != 1){ 
		/* qpel */
		pDec->qpel = GetBits(pDec, 1);
	}else{
		pDec->qpel = 0;
	}

#if ENABLE_HEADINFO
	if(pDec->pHeadInfo){
		sprintf(tmpInfo, "\r\n Qpel = %d", pDec->qpel);
		strcat(pDec->pHeadInfo->Description, tmpInfo);
	}
#endif

	FLUSH_BITS(pDec,1); // complexity estimation
	FLUSH_BITS(pDec,1); // Resync marker disabled (?)

	if (GetBits(pDec, 1)){
		pDec->nMemConfig |= INNER_MEM_DATA_PARTITION;
#if ENABLE_HEADINFO
		if(pDec->pHeadInfo){
			sprintf(tmpInfo, "\r\n Data partition!");
			strcat(pDec->pHeadInfo->Description, tmpInfo);
		}
#endif

		if(GetBits(pDec, 1)){//rvlc
			pDec->vld_block = Mpeg4GetBlockRvld;
#if ENABLE_HEADINFO
			if(pDec->pHeadInfo){
				sprintf(tmpInfo, "\r\n RVLC!");
				strcat(pDec->pHeadInfo->Description, tmpInfo);
			}
#endif
		}
		pDec->fpDecIVOP = MPEG4DecDPIVOP;
		pDec->fpDecPVOP = MPEG4DecDPPVOP;
		pDec->fpDecBVOP = MPEG4DecDPBVOP;

	}

	if (visual_object_layer_verid != 1){ 
		if (GetBits(pDec, 1)) //new pred not supported
			FLUSH_BITS(pDec,2+1); // req msg type, seg type
		FLUSH_BITS(pDec,1); //reduced res not supported
	}

	if (GetBits(pDec, 1)) // scalability
		FLUSH_BITS_LARGE(pDec,1+4+1+5+5+5+5+1); // not supported


	pDec->nTrueWidth = nWidth;
	pDec->nTrueHeight = nHeight;
	voRC = ConfigDecoder(pDec, nWidth, nHeight);
	if(voRC)
		return voRC;

	pDec->validvol = 1;
	return VO_ERR_NONE;
}

typedef struct
{
	VO_U32 code;
	VO_U8 len;
}
GMC_VLC;
GMC_VLC sprite_trajectory_len[15] = {
	{ 0x00 , 2},
	{ 0x02 , 3}, { 0x03, 3}, { 0x04, 3}, { 0x05, 3}, { 0x06, 3},
	{ 0x0E , 4}, { 0x1E, 5}, { 0x3E, 6}, { 0x7E, 7}, { 0xFE, 8},
	{ 0x1FE, 9}, {0x3FE,10}, {0x7FE,11}, {0xFFE,12}
};



VO_S32 GetSpritetrajectory(VO_MPEG4_DEC *pDec)
{
	VO_S32 i;
	for (i = 0; i < 12; i++){
		if (SHOW_BITS(pDec, sprite_trajectory_len[i].len) == sprite_trajectory_len[i].code){
			UPDATE_CACHE(pDec);
			FLUSH_BITS_LARGE(pDec,sprite_trajectory_len[i].len)
				return i;
		}
	}
	return -1;
}

VO_U32 MPEG4GetHdr(struct VO_MPEG4_DEC *pDec, const VO_S32 nHeaderFlag)
{
	VO_S32 code, result;
// 	VO_S32 time_cnt = 0;
	for (;;)
	{
		bytealign(pDec);
		UPDATE_CACHE(pDec);
		if (EofBits(pDec))
			return VO_ERR_INPUT_BUFFER_SMALL;

		code = SHOW_BITS(pDec,32);

		if ((code & ~VOL_START_CODE_MASK) == VOL_START_CODE){
			result = GetVOLHeader( pDec );
			CHECK_ERROR(result);
			if(nHeaderFlag == DEC_VOLHR) 
			{
				return VO_ERR_NONE;
			}
		}else if (code == VOP_START_CODE) {

			VO_S32 time_increment;
			VO_S32 rounding;


			FLUSH_BITS_LARGE(pDec,32);
			if(!pDec->validvol)
				continue;
			pDec->nVOPType = GetBits(pDec,2);

#if ENABLE_VT
			if((pDec->nVOPType == B_VOP)&&pDec->bLowDelay)
				return VO_ERR_DEC_MPEG4_HEADER;
#endif
			while (GetBits(pDec, 1)) 
			{
// 				++time_cnt;
				UPDATE_CACHE(pDec);
			}

			FLUSH_BITS(pDec,1); //marker
			UPDATE_CACHE(pDec);
			time_increment = GetBits(pDec,pDec->nTimeIncBits);

			FLUSH_BITS(pDec,1); //marker

			UPDATE_CACHE(pDec);

			if (!GetBits(pDec, 1)) 
			{
				pDec->nVOPType = VO_VIDEO_FRAME_NULL;
				return VO_ERR_NONE;
			}  

			if(nHeaderFlag == DEC_FRAMETYPE)
				return VO_ERR_NONE;

			if (!pDec->bLowDelay && pDec->nTimeIncResolution)
			{
				if (pDec->nVOPType != VO_VIDEO_FRAME_B) {
					pDec->time_pp = (pDec->nTimeIncResolution + 
						time_increment - pDec->last_reftime) % pDec->nTimeIncResolution;
					pDec->last_reftime = time_increment;
				} else {
					pDec->time_bp = (pDec->nTimeIncResolution + 
						pDec->last_reftime - time_increment) % pDec->nTimeIncResolution;
				}
			}


			rounding = 0;
			if (pDec->nVOPType == VO_VIDEO_FRAME_P || pDec->nVOPType == VO_VIDEO_FRAME_S)
				rounding = GetBits(pDec,1);
			pDec->rounding = rounding;

			pDec->nIntraDCThreshold = nIntraDCThreshold[GetBits(pDec,3)];

			pDec->alter_ver_scan = 0;
			if (pDec->bInterlaced){
				FLUSH_BITS(pDec, 1);//top_field_first
				pDec->alter_ver_scan = (VO_S8)GetBits(pDec, 1); //alternate_scan
			}

			if (pDec->nVOPType == VO_VIDEO_FRAME_S && (pDec->nSprite == SPRITE_STATIC || pDec->nSprite == SPRITE_GMC)){
				VO_S32 i;

				for (i = 0 ; i < pDec->nSpriteWarpPoints; i++)
				{
					VO_S32 length;
					VO_S32 x = 0, y = 0;

					/* nSprite code borowed from ffmpeg; thx Michael Niedermayer <michaelni@gmx.at> */
					length = GetSpritetrajectory(pDec);
					if(length){
						UPDATE_CACHE(pDec);
						x= GetBits(pDec,length);
						if ((x >> (length - 1)) == 0) /* if MSB not set it is negative*/
							x = - (x ^ ((1 << length) - 1));
					}
					FLUSH_BITS(pDec,1); //MARKER();

					length = GetSpritetrajectory(pDec);
					if(length){
						UPDATE_CACHE(pDec);
						y = GetBits(pDec,length);
						if ((y >> (length - 1)) == 0) /* if MSB not set it is negative*/
							y = - (y ^ ((1 << length) - 1));
					}
					FLUSH_BITS(pDec,1); //MARKER();
					pDec->warp_point[i] = MAKEMV(x,y);
				}
			}

			pDec->nQuant = GetBits(pDec,pDec->nQuantPrecision); // vop quant

			if(pDec->nQuant==0){
				pDec->nQuant = 1;
				pDec->nMBError |= ERR_QUANT;
			}
			
			UPDATE_CACHE(pDec);
			if (pDec->nVOPType != VO_VIDEO_FRAME_I)
			{
				pDec->nFcode = GetBits(pDec,3);

				if(pDec->nFcode==0){
					pDec->nFcode = 1;
					pDec->nMBError |= ERR_FCODE_FOR;
				}
			}

			if (pDec->nVOPType == VO_VIDEO_FRAME_B) 
			{
				pDec->nBcode = GetBits(pDec,3);

				if(pDec->nBcode==0){
					pDec->nBcode = 1;
					pDec->nMBError |= ERR_FCODE_BAK;
				}
			}
		
		
			return VO_ERR_NONE;
		}
		else
			FLUSH_BITS(pDec,8);
	}
	return VO_ERR_DEC_MPEG4_HEADER;
}

const VO_U16 mba_max[6]={
     47,  98, 395,1583,6335,9215
};
const VO_U8 mba_length[6]={
      6,   7,   9,  11,  13,  14
};

VO_S32 H263GetMBA(VO_MPEG4_DEC* pDec, VO_U32 nWidth, VO_U32 nHeight)
{
    VO_S32 i, nMBA;
	VO_U32 nMBWidth = ((nWidth + 15)&0xFFFFFFF0)>>4;
	VO_U32 nMBHeight = ((nHeight + 15)&0xFFFFFFF0)>>4;

	UPDATE_CACHE(pDec);

    for(i=0; i<6; i++){
        if((nMBWidth*nMBHeight)-1 <= mba_max[i]) break;
    }
    nMBA= GetBits(pDec, mba_length[i]);
//   s->mb_x= mb_pos % s->nMBWidth;
//   s->mb_y= mb_pos / s->nMBWidth;
//	pDec->nResyncPos = mb_pos;
    return nMBA;
}

VO_S32 Resync(VO_MPEG4_DEC* pDec, VO_S32 *pMBPos)
{
	VO_S32 i, nMBX, nMBY;
	VO_S32 bMpeg4 = (pDec->nCodecId == MPEG4_ID);

	if (!bMpeg4){
		FLUSH_BITS(pDec,16);
		UPDATE_CACHE(pDec);

		while (!GetBits(pDec, 1))
		{
			UPDATE_CACHE(pDec);
			if(EofBits(pDec)) 
				return VO_ERR_FAILED;
		}

		if(pDec->bSliceStructuredH263){
			VO_S32 nMBNum = 0;

			if(GetBits(pDec, 1)==0)
				return VO_ERR_FAILED;


			nMBNum = H263GetMBA(pDec, pDec->nTrueWidth, pDec->nTrueHeight);
			nMBX = nMBNum % pDec->nMBWidth;
			nMBY = nMBNum / pDec->nMBWidth;
			if((pDec->nMBWidth * pDec->nMBHeight) > 1583){
				if(GetBits(pDec, 1)==0)
					return VO_ERR_FAILED;
			}

			pDec->nQuant = GetBits(pDec, 5); /* SQUANT */
			if(GetBits(pDec, 1)==0)
				return VO_ERR_FAILED;

			GetBits(pDec,2); /* GFID */
		}else{
			nMBX = 0;
			nMBY = GetBits(pDec,5); /*gob number*/

			if (pDec->nMBWidth > 25)
				nMBY <<= 1;
			if (pDec->nMBWidth > 50)
				nMBY <<= 1;
			
			GetBits(pDec, 2); /*gfid*/
			i = GetBits(pDec, 5);
			if (i)
				pDec->nQuant = i;
		}
	}else{
		do{
			UPDATE_CACHE(pDec);
			i = BitsToNextByte(pDec);
			FLUSH_BITS(pDec, i);
			UPDATE_CACHE(pDec);

			for (i = 0; i < 32; ++i){
				if (GetBits(pDec, 1))
					break;
				if (i == 16) 
					UPDATE_CACHE(pDec);
			}
			if(EofBits(pDec)) 
				return VO_ERR_FAILED;

		}while(i+1 != pDec->nResyncMarkerBits);
		UPDATE_CACHE(pDec);

		nMBX = GetBits(pDec, pDec->nMBBits);
		nMBY = nMBX / pDec->nMBWidth;
		nMBX -= nMBY * pDec->nMBWidth;

		UPDATE_CACHE(pDec);
		i = GetBits(pDec,pDec->nQuantPrecision);

		if (i)
			pDec->nQuant = i;
		if (GetBits(pDec, 1)){
			VO_S32 fcode=0, bcode=0;
			VO_S32 nTimeIncrement;

			UPDATE_CACHE(pDec);
			/*modulo_time_bse*/
			while (GetBits(pDec, 1)) 
				UPDATE_CACHE(pDec);

			/*marker_bit*/
			FLUSH_BITS(pDec,1); 
			/*vop_time_increment*/
			UPDATE_CACHE(pDec);

			nTimeIncrement = GetBits(pDec,pDec->nTimeIncBits);
			if (!pDec->bLowDelay && pDec->nTimeIncResolution){
				if (pDec->nVOPType != VO_VIDEO_FRAME_B) {
					pDec->time_pp = (pDec->nTimeIncResolution + 
						nTimeIncrement - pDec->last_reftime) % pDec->nTimeIncResolution;
					pDec->last_reftime = nTimeIncrement;
				} else {
					pDec->time_bp = (pDec->nTimeIncResolution + 
						pDec->last_reftime - nTimeIncrement) % pDec->nTimeIncResolution;
				}
			}
			/*marker_bit*/
			FLUSH_BITS(pDec,1); 
			/*vop_coding_type*/
			UPDATE_CACHE(pDec);
			FLUSH_BITS(pDec,2);
			//if(pDec->nVOPType != GetBits(pDec,2))
			//	return -1;
			//intra_dc_vlc_thr
			//pDec->nIntraDCThreshold = nIntraDCThreshold[GetBits(pDec,3)];
			FLUSH_BITS(pDec,3);
			//vop_fcode_forward
			UPDATE_CACHE(pDec);

			if (pDec->nVOPType != VO_VIDEO_FRAME_I){
				fcode  = GetBits(pDec,3);

				if(fcode==0){
					fcode = 1;
					pDec->nMBError |= ERR_FCODE_FOR;
				}
			}

			if (pDec->nVOPType == VO_VIDEO_FRAME_B){ 
				bcode = GetBits(pDec,3);
				if(bcode==0){
					bcode = 1;
					pDec->nMBError |= ERR_FCODE_BAK;
				}
			}
		}
	}

	for (i = 0; i < 4*MB_X*2; i++)
		pDec->dc_lum[i] = 1024;

	for (i = 0; i < 2*MB_X; i++){ 
		pDec->dc_chr[0][i] = 1024;
		pDec->dc_chr[1][i] = 1024;
	}

	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->ac_left_lum, 0, sizeof(pDec->ac_left_lum));
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->ac_top_lum, 0, sizeof(pDec->ac_top_lum));
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->ac_left_chr, 0, sizeof(pDec->ac_left_chr));
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, (VO_U8*)pDec->ac_top_chr, 0, sizeof(pDec->ac_top_chr));

	i = *pMBPos = pDec->nResyncPos = nMBX + nMBY * MB_X;

	if((i < 0)||(i >= pDec->nPosEnd))
		return VO_ERR_FAILED;

    return VO_ERR_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////////
//H.263 related

VO_U32 H263GetHdr( VO_MPEG4_DEC *pDec , const VO_S32 nHeaderFlag)
{
	VO_S32 code, result, UFEP, source_format = 0xf;
	VO_S32  nWidth = pDec->nWidth , nHeight = pDec->nHeight;
	static const VO_S32 size[8][2] = {
		{ 0,0 },
		{ 128,96 },
		{ 176,144 },
		{ 352,288 },
		{ 704,576 },
		{ 1408,1152 }};
#if ENABLE_HEADINFO
	VO_CHAR tmpInfo[128];
#endif

	bytealign(pDec);
	UPDATE_CACHE(pDec);

    code = GetBits(pDec,22-8);
	while (!EofBits(pDec))
	{
		UPDATE_CACHE(pDec);
		code = ((code << 8) + GetBits(pDec,8)) & 0x3FFFFF;
		if (code == 32)
			break;
	}

	if (code != 32)
		return VO_ERR_DEC_MPEG4_HEADER;

	FLUSH_BITS(pDec,8); // picture timestamp

	UPDATE_CACHE(pDec);
	if (!GetBits(pDec, 1)) // marker
		return VO_ERR_DEC_MPEG4_HEADER;
	if (GetBits(pDec, 1)) // h263 id
		return VO_ERR_DEC_MPEG4_HEADER;

	FLUSH_BITS(pDec,3);

	code = GetBits(pDec, 3); // format

	if (code == 7){
		VO_S32 custom_pcf = 0;
		
		UFEP = GetBits (pDec, 3);
		
		if (UFEP == 1){                           /* OPPTYPE */
			
			source_format = GetBits (pDec, 3);		
			/* optional custom setting */
			custom_pcf = GetBits(pDec, 1);

			if((code = GetBits (pDec, 3))){/* UMV + SAC + OBMC*/
#if ENABLE_HEADINFO
				if(pDec->pHeadInfo){
					sprintf(tmpInfo, "\r\n Unsupport UMV, SAC, OBMC !");
					strcat(pDec->pHeadInfo->Description, tmpInfo);
				}
#endif
				return VO_ERR_NOT_IMPLEMENT;
			}

			pDec->aic = (VO_S8)GetBits(pDec, 1); /* AIC */
			pDec->loop_filter = (VO_S8)GetBits(pDec, 1);/*loop_filter*/
			pDec->bSliceStructuredH263 = (VO_S8)GetBits(pDec, 1);/*bSliceStructuredH263*/
			code |= GetBits(pDec, 1);/*Reference Picture Selection*/
			code |= GetBits(pDec, 1);/*Independent Segment Decoding*/
			code |= GetBits(pDec, 1);/*alt_inter_vlc*/
			if(code){
#if ENABLE_HEADINFO
				if(pDec->pHeadInfo){
					sprintf(tmpInfo, "\r\n Unsupport RPS, ISD, AIV !");
					strcat(pDec->pHeadInfo->Description, tmpInfo);
				}
#endif
				return VO_ERR_NOT_IMPLEMENT;
			}
			if((pDec->h263_modified_quant = (VO_S8)GetBits(pDec, 1))){/*modified_quant*/
				pDec->chroma_qscale_table = (VO_U8*)h263_chroma_qscale_table;
			}

			code = GetBits (pDec, 4);				
			if (code != 8){   /* OPPTYPE : bit15=1, bit16,bit17,bit18=0 */				
				return VO_ERR_DEC_MPEG4_HEADER;				
			}				
		}

		if ((UFEP == 1) || (UFEP == 0))	{
			UPDATE_CACHE(pDec);
			/* MMPTYPE */
			pDec->nVOPType = GetBits (pDec, 3);
			if(nHeaderFlag == DEC_FRAMETYPE)
				return VO_ERR_NONE;
			
			if (!(pDec->nVOPType == VO_VIDEO_FRAME_I || pDec->nVOPType == VO_VIDEO_FRAME_P)){
				return VO_ERR_NOT_IMPLEMENT;
			}
			
			code = GetBits (pDec, 2);
			
			if(code)
				return VO_ERR_NOT_IMPLEMENT;

			pDec->rounding = GetBits(pDec, 1);      /* rounding type */
			
			code = GetBits (pDec, 3);
			
			if (code != 1){                         /* MPPTYPE : bit7,bit8=0  bit9=1 */
				return VO_ERR_DEC_MPEG4_HEADER;
			}
		} else {
			/* UFEP is neither 001 nor 000 */
			return VO_ERR_DEC_MPEG4_HEADER;
		}
		
		if((code = GetBits (pDec, 1))){		
			return VO_ERR_DEC_MPEG4_HEADER;
		}
		
		if (UFEP){
			if(source_format == 6){
				/* Read custom picture format */
				code = GetBits (pDec, 4);
				
				//if (code != 2){
				//	return VO_ERR_DEC_MPEG4_HEADER;
				//}
				
				UPDATE_CACHE(pDec);
				code = GetBits (pDec, 9);
				nWidth = (code + 1 ) * 4;	
				if(!(code = GetBits (pDec, 1)))
					return VO_ERR_DEC_MPEG4_HEADER;
				
				code = GetBits (pDec, 9);
				nHeight = code * 4;
				//			if ((nWidth&0xf) || (heigh&0xf)){
				//				return VO_ERR_DEC_MPEG4_HEADER;
				//			}
			}else{ 
				nWidth = size[source_format][0]; 
				nHeight = size[source_format][1];
				
			}

			if(custom_pcf){
				UPDATE_CACHE(pDec);
				FLUSH_BITS(pDec, 8);
            }
		}

		if (!nWidth || !nHeight || \
			(nWidth >= (MB_X*16)) || (nHeight >= (MB_Y*16))){
				return  VO_ERR_DEC_MPEG4_HEADER;
		}

		if(custom_pcf){
			FLUSH_BITS(pDec, 2); //extended Temporal reference
		}

        if (UFEP) {
            //if(umvplus){
            //   if(get_bits1(&s->gb)==0) /* Unlimited Unrestricted Motion Vectors Indicator (UUI) */
			//      skip_bits1(&s->gb); 
			//}

            if(pDec->bSliceStructuredH263){
				if (GetBits(pDec, 1) != 0) {
					return VO_ERR_DEC_MPEG4_HEADER;
				}
				if (GetBits(pDec, 1) != 0) {
					return VO_ERR_DEC_MPEG4_HEADER;
				}
            }
        }

		pDec->nQuant = GetBits (pDec, 5);

	}else {

		nWidth = size[code][0];
        nHeight = size[code][1];

		if (!nWidth || !nHeight || \
			(nWidth >= (MB_X*16)) || (nHeight >= (MB_Y*16))){
				return VO_ERR_DEC_MPEG4_HEADER;
		}

		UPDATE_CACHE(pDec);
        pDec->nVOPType = GetBits(pDec,1);

		if(nHeaderFlag == DEC_FRAMETYPE)
			return VO_ERR_NONE;

		pDec->long_vectors = (VO_S8)GetBits(pDec,1);

		if (GetBits(pDec, 1)){ // sac not supported
#if ENABLE_HEADINFO
			if(pDec->pHeadInfo){
				sprintf(tmpInfo, "\r\n Unsupport SAC !");
				strcat(pDec->pHeadInfo->Description, tmpInfo);
			}
#endif
			return VO_ERR_NOT_IMPLEMENT;
		}

		if (GetBits(pDec,1)){ // obmc
#if ENABLE_HEADINFO
			if(pDec->pHeadInfo){
				sprintf(tmpInfo, "\r\n Unsupport OBMC !");
				strcat(pDec->pHeadInfo->Description, tmpInfo);
			}
#endif
			return VO_ERR_NOT_IMPLEMENT;
		}
        
		if (GetBits(pDec, 1)){ // pb frame not supported
#if ENABLE_HEADINFO
			if(pDec->pHeadInfo){
				sprintf(tmpInfo, "\r\n Unsupport PB Frame !");
				strcat(pDec->pHeadInfo->Description, tmpInfo);
			}
#endif
			return VO_ERR_NOT_IMPLEMENT;
		}

		pDec->nQuant = GetBits(pDec,5);
		FLUSH_BITS(pDec,1);

		pDec->rounding = 0;
	}

	while (GetBits(pDec, 1)) // pei
	{
		FLUSH_BITS(pDec,8);
		UPDATE_CACHE(pDec);
	}

	if(pDec->bSliceStructuredH263){
		if (GetBits(pDec, 1) != 1) {
			return VO_ERR_DEC_MPEG4_HEADER;
		}

		H263GetMBA(pDec, nWidth, nHeight);

		if (GetBits(pDec, 1) != 1) {
			return VO_ERR_DEC_MPEG4_HEADER;
		}
	}

	if(pDec->validvol&& \
		(pDec->nTrueWidth != nWidth || pDec->nTrueHeight != nHeight))
		return VO_ERR_DEC_MPEG4_HEADER;

	pDec->nTrueWidth = nWidth;
	pDec->nTrueHeight = nHeight;
	
	result = ConfigDecoder(pDec, nWidth,nHeight);
	if(result)
		return result;

	pDec->nFcode = 1;
	pDec->validvol = 1;
	return VO_ERR_NONE;
}


VO_U32 S263GetHdr( VO_MPEG4_DEC *pDec , const VO_S32 nHeaderFlag)
{
	VO_S32 code, result;

	//TBD check the start code like h.263
	for (;;){
		bytealign(pDec);
		UPDATE_CACHE(pDec);
		if (EofBits(pDec))
			return VO_ERR_INPUT_BUFFER_SMALL;
	
		code = SHOW_BITS(pDec, 17);//0x00008(17bits)
		if (code == 1) {
			VO_S32 h263_flv, nWidth, nHeight, format;
			
			FLUSH_BITS(pDec, 17);	
			h263_flv = GetBits(pDec, 5);
			if (h263_flv != 0 && h263_flv != 1) {
				return VO_ERR_DEC_MPEG4_HEADER;
			}
			pDec->nCodecId |= h263_flv;

			//h263_flv += 1;

			UPDATE_CACHE(pDec);
			//s->picture_number = FLUSH_BITS(pDec, 8); /* picture timestamp */
			FLUSH_BITS(pDec, 8);
			format = GetBits(pDec, 3);
			UPDATE_CACHE(pDec);
			switch (format) {
			case 0:
				nWidth = GetBits(pDec, 8);
				nHeight = GetBits(pDec, 8);
				break;
			case 1:
				nWidth = GetBits(pDec, 16);
				UPDATE_CACHE(pDec);
				nHeight = GetBits(pDec, 16);
				break;
			case 2:
				nWidth = 352;
				nHeight = 288;
				break;
			case 3:
				nWidth = 176;
				nHeight = 144;
				break;
			case 4:
				nWidth = 128;
				nHeight = 96;
				break;
			case 5:
				nWidth = 320;
				nHeight = 240;
				break;
			case 6:
				nWidth = 160;
				nHeight = 120;
				break;
			default:
				nWidth = nHeight = 0;
				break;
			}
			if (!nWidth || !nHeight || \
				(nWidth >= (MB_X*16)) || (nHeight >= (MB_Y*16))){
					return VO_ERR_DEC_MPEG4_HEADER;
			}

			pDec->nVOPType = GetBits(pDec, 2);
			if(nHeaderFlag == DEC_FRAMETYPE)
				return VO_ERR_NONE;

			pDec->dropable= pDec->nVOPType > VO_VIDEO_FRAME_P;
			if (pDec->dropable)
				pDec->nVOPType = VO_VIDEO_FRAME_P;
//			printf("S263 type: %d\n", pDec->nVOPType);
			UPDATE_CACHE(pDec);
			FLUSH_BITS(pDec, 1);	/* deblocking flag */
			// s->chroma_qscale= s->qscale = get_bits(&s->gb, 5);
			pDec->nQuant = GetBits(pDec, 5);

			//s->h263_plus = 0;

			pDec->aic = 0; /* AIC */
			//pDec->unrestricted_mv = 1;
			pDec->long_vectors = 0;


			/* PEI */
			while (GetBits(pDec, 1)) {
				UPDATE_CACHE(pDec);
				FLUSH_BITS(pDec, 8);
			}

			if(pDec->validvol&& \
				(pDec->nTrueWidth != nWidth || pDec->nTrueHeight != nHeight))
				return VO_ERR_DEC_MPEG4_HEADER;

			pDec->nTrueWidth = nWidth;
			pDec->nTrueHeight = nHeight;

			result = ConfigDecoder(pDec, nWidth, nHeight);
			if(result)
				return result;

			pDec->nFcode = 1;
			pDec->validvol = 1;    	
			return VO_ERR_NONE;
		}
		else
			FLUSH_BITS(pDec,8);
	}
	return VO_ERR_DEC_MPEG4_HEADER;
}

