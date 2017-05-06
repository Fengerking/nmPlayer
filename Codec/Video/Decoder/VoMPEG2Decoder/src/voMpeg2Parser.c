/************************************************************************
*									                                                      *
*	VisualOn, Inc. Confidential and Proprietary, 2007		          *
*								 	                                                      *
************************************************************************/
#include "voMpeg2Parser.h"
#include "voMpegReadbits.h"
#include "voMpegMem.h"
#include "voMpegBuf.h"

VOCONST int FrameRate[16] = 
{
        0,
    24000,
    24024,
    25025,
    30000,
    30030,
    50050,
    60000,
    60060,
    15015,
     5005,
    10010,
    12012,
    15015,
    25025,
    25025,
};


VOCONST VO_S8 MPEG1_IntraMatrix[64] = 
{
	8,  16, 19, 22, 26, 27, 29, 34,
	16, 16, 22, 24, 27, 29, 34, 37,
	19, 22, 26, 27, 29, 34, 34, 38,
	22, 22, 26, 27, 29, 34, 37, 40,
	22, 26, 27, 29, 32, 35, 40, 48,
	26, 27, 29, 32, 35, 40, 48, 58,
	26, 27, 29, 34, 38, 46, 56, 69,
	27, 29, 35, 38, 46, 56, 69, 83
};


#define ZIG_ZAG 0
static VO_U8 scan[2][64] = {
	{
		 0,  1,  8, 16,  9,  2,  3, 10, 
		17,	24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34, 
		27,	20, 13,  6,  7, 14, 21, 28, 
		35, 42,	49, 56, 57, 50, 43, 36, 
		29, 22,	15, 23, 30, 37, 44, 51, 
		58, 59, 52, 45, 38, 31, 39, 46, 
		53, 60, 61,	54, 47, 55, 62, 63
	},
	{
		0,  8,  16, 24,  1,  9,  2, 10, 
		17, 25, 32, 40, 48, 56, 57, 49,
		41, 33, 26, 18,  3, 11,  4, 12, 
		19, 27, 34, 42, 50, 58, 35, 43,
		51, 59, 20, 28,  5, 13,  6, 14, 
		21, 29, 36, 44, 52, 60, 37, 45,
		53, 61, 22, 30,  7, 15, 23, 31, 
		38, 46, 54, 62, 39, 47, 55, 63,
	}
};

// static VO_VOID voFreeImgBuf(MpegDecode* dec)
// {
// 	VO_S32 i;
// 
// 	for(i = 0; i < MAXFRAMES; i++){
// 		image_destroy(dec->decReferenceFrames[i], dec->img_stride, dec->img_height, dec);
// 		FreeMem(dec->decReferenceFrames[i], &dec->memoryOperator);		
// 		dec->decReferenceFrames[i] = NULL;
// 		dec->privFIFO.img_seq[i] = NULL;
// 	}
// 
// 	dec->current_frame = NULL;
// 	dec->reference_frame = NULL;
// 	dec->Bframe = NULL;
// }

static VOMPEG2DECRETURNCODE dec_buf_ini(MpegDecode* const dec, VO_S32 img_width, VO_S32 img_height)
{
	VO_S32 i, Result = ERR_NONE;
	const VO_S32 block_size = 8 >> DOWNSAMPLE_FLAG(dec);
	VO_MEM_VIDEO_INFO VideoMem;
	int zero_flage = 0;

	if((dec->img_width!= 0)||(dec->img_height != 0)){
		zero_flage = 1;
	}

	if (dec->img_width != img_width || dec->img_height != img_height){

		img_width = max(16, img_width);
		img_width = min(1920,img_width);
		img_height = max(16,img_height);
		img_height = min(1200,img_height);

		dec->out_img_width= img_width;
		dec->out_img_height = img_height;

		dec->mb_w = ((img_width + 15) >> 4); 
		dec->mb_h = ((img_height + 15) >> 4);

		dec->img_width = dec->mb_w << 4;
		dec->img_height = dec->mb_h << 4;
		dec->img_stride = (dec->img_width + EDGE_SIZE)>>DOWNSAMPLE_FLAG(dec);//EDGE_SIZE = 0 in mpeg2 decoder
		dec->img_stride_uv = (dec->uv_interlace_flage)?(dec->img_stride):(dec->img_stride>>1);

		if(zero_flage){
			dec->img_width     = dec->max_img_width;
			dec->img_height    = dec->max_img_height;
			dec->img_stride    = dec->max_img_stride;
			dec->img_stride_uv = dec->max_img_stride_uv;
			dec->out_img_width = dec->max_out_img_width;
			dec->out_img_height= dec->max_out_img_height;

			dec->mb_w = ((dec->img_width  + 15) >> 4); 
			dec->mb_h = ((dec->img_height + 15) >> 4);

			return Result;
		}else{
			dec->max_img_width     = dec->img_width;
			dec->max_img_height    = dec->img_height;
			dec->max_img_stride    = dec->img_stride;
			dec->max_img_stride_uv = dec->img_stride_uv;
			dec->max_out_img_width = dec->out_img_width;
			dec->max_out_img_height= dec->out_img_height;
		}

		/*TBD error check and free*/
		if(!dec->blockptr){

			i = 64*sizeof(VO_S16) + 8*block_size*6*sizeof(VO_U8) 
				+ 256*sizeof(VO_U8);

			dec->blockptr = (VO_S16 *)(MallocMem(i + SAFETY, CACHE_LINE, &dec->memoryOperator));

			dec->mc_dst[0] = (VO_U8*)dec->blockptr + 64*sizeof(VO_S16);
			dec->numLeadingZeros = (VO_U8*)dec->mc_dst[0] + 8*block_size*6*sizeof(VO_U8);
			dec->mc_dst[1] = dec->mc_dst[0] + 8*block_size*4;

			if (dec->uv_interlace_flage){
				dec->mc_dst[2] = dec->mc_dst[1] +(8 >> DOWNSAMPLE_FLAG(dec));
			}else{
				dec->mc_dst[2] = dec->mc_dst[1] + 8*block_size;
			}

			SetMem((VO_U8 *)dec->blockptr, 0, 64 * sizeof(VO_S16), &dec->memoryOperator);

#ifdef IPP_EDIT
			dec->pBuff = (VO_S16 *)(MallocMem(128 + SAFETY, CACHE_LINE, &dec->memoryOperator))
#endif //IPP_EDIT

			for (i = 0; i < 256; i++){
				int k, j = i;
				for (k = 0; k < 8; k++){
					if (j & 0x80)
						break;
					j <<= 1;
				}
				dec->numLeadingZeros[i] = k;
			}
		}

#ifdef SKIP_IDENTICAL_COPY
		if (!dec->frBCopyFlag){
			int copyFlagSize = dec->mb_w * dec->mb_h;
			// B mb uses 2 bits, P mb use 1 bit;
			copyFlagSize = (copyFlagSize>>2) + ((copyFlagSize&3) ? 2 : 0);
			// make it CACHE_LINE aligned;
			copyFlagSize += (CACHE_LINE - (copyFlagSize&(CACHE_LINE-1)));
			dec->frBCopyFlag = mpeg_malloc((copyFlagSize + (copyFlagSize>>1))
				* sizeof(VO_U8) + SAFETY, CACHE_LINE);
			dec->frPCopyFlag = dec->frBCopyFlag + copyFlagSize;
			dec->frPCopySize32 = (copyFlagSize>>3);
			// only reset B frame, P frame will be reset at the end of each I frame;
			// But be careful about random access, we need to reset it for random jump;
			memset (dec->frBCopyFlag, 0, copyFlagSize);
		}
#endif //SKIP_IDENTICAL_COPY
		if(dec->memoryShare.Init){
			VideoMem.Stride = dec->img_stride;
			VideoMem.Height = dec->img_height;
			VideoMem.FrameCount = MAXFRAMES;
			VideoMem.ColorType = VO_COLOR_YUV_PLANAR420;
			dec->memoryShare.Init(VO_INDEX_DEC_MPEG2,&VideoMem);
			dec->img_stride = VideoMem.VBuffer[0].Stride[0];
			dec->img_stride_uv = VideoMem.VBuffer[0].Stride[1];
		}

		for(i = 0; i < MAXFRAMES; i++){
			if(!dec->decReferenceFrames[i]){
				dec->decReferenceFrames[i] = (Image *)MallocMem(sizeof(Image) + SAFETY, CACHE_LINE, &dec->memoryOperator);
				if(!dec->memoryShare.Init){
					image_create(dec->decReferenceFrames[i], dec->img_stride, dec->img_height, dec);
				}else{
					dec->decReferenceFrames[i]->y = (VO_U8 *)(VideoMem.VBuffer[i].Buffer[0]);
					dec->decReferenceFrames[i]->u = (VO_U8 *)(VideoMem.VBuffer[i].Buffer[1]);
					dec->decReferenceFrames[i]->v = (VO_U8 *)(VideoMem.VBuffer[i].Buffer[2]);
				}
			}
		}

#ifdef LICENSEFILE
		dec->pDisplayFrame = (Image*)(MallocMem(sizeof(Image) + SAFETY, CACHE_LINE, &dec->memoryOperator));	
#endif
		dec->privFIFO.r_idx = 0;
		dec->privFIFO.w_idx = MAXFRAMES;

		for(i = 0; i < MAXFRAMES; i++){
			dec->privFIFO.img_seq[i] = dec->decReferenceFrames[i];
		}

		dec->current_frame   = dec->decReferenceFrames[0];
		dec->reference_frame = dec->decReferenceFrames[1];
		dec->Bframe          = dec->decReferenceFrames[2];

#ifdef SKIP_IDENTICAL_COPY
		dec->Bframe->Id = 0;
		dec->current_frame->Id = 1;
		dec->reference_frame->Id = 2;
		dec->lastRefId = 2;
#endif //SKIP_IDENTICAL_COPY
	}

	return Result;
}

static VOMPEG2DECRETURNCODE sequence_extension(MpegDecode* dec)
{
    VO_S32 horiz_size_ext, vert_size_ext;
	VO_U32 data, mpeg2_flag = dec->mpeg2_flag;

	// still 28 bits left;
	data = GetBits(dec, 28);

	mpeg2_flag &= SEQ_EXTE_MASK;
	// data has 20 bits left;
	dec->progressive_sequence = ((data>>19)&1);
    mpeg2_flag |= ((data>>19)&1); //GetBits(dec, 1); /* progressive_sequence */
    mpeg2_flag |= (((data>>17)&0x3) << CHROMA_FORMAT); /* chroma_format 1=420, 2=422, 3=444 */
    horiz_size_ext = ((data>>15)&0x3);
    vert_size_ext = ((data>>13)&0x3);
    //dec->img_width |= (horiz_size_ext << 12);//huwei 20110613 don't support 2047 * 2047 image
    //dec->img_height |= (vert_size_ext << 12);
    // skip 13 bits, FLUSH_BITS(dec, 13);  /* bit_rate_ext bit_rate += (bit_rate_ext << 18) * 400; 12 + marker 1*/
	UPDATE_CACHE(dec);
	data = GetBits(dec, 16);
	// skip 8 bits;
    //FLUSH_BITS(dec, 8); /*rc_buffer_size +=GetBits(dec, 8)*1024*16<<10; rc_buffer_size*/

    mpeg2_flag |= (((data>>7)&1)<<LOW_DELAY);
//    FLUSH_BITS(dec, 5);
#ifndef MPEG2_DEC
	  dec->codec_id = MPEG2_D;
#endif
//    s->avctx->sub_id = 2; /* indicates mpeg2 found */
	dec->mpeg2_flag = mpeg2_flag;
	return VO_ERR_NONE;
}

static VOMPEG2DECRETURNCODE sequence_display_extension(MpegDecode* dec)
{
	return VO_ERR_NONE;
}

static VOMPEG2DECRETURNCODE quant_matrix_extension(MpegDecode* dec)
{
	int i;
	VO_U32 mpeg2_flag = dec->mpeg2_flag;
	VO_U8 *scan0;

	scan0 = &scan[0][0];
	if (GetBits(dec, 1)){ 
		for (i=0;i<64;i++){
			UPDATE_CACHE(dec);
			dec->DefaultIntraMatrix[scan0[i]] = dec->IntraMatrix[i] = (VO_U8)GetBits(dec,8);
 			dec->zigzag[i] = scan0[i];
		}
	}

	if (GetBits(dec, 1)){ 
		for (i=0;i<64;i++){ 
			UPDATE_CACHE(dec);
			dec->DefaultInterMatrix[scan0[i]] = dec->InterMatrix[i] 
			= (VO_U8)GetBits(dec, 8);
		}
	}

	if(GET_ALTER_SCAN(mpeg2_flag)){
		VO_S32 i;
		for (i=0;i<64;i++){ 
			VO_S32 pos = dec->zigzag[i] = scan[1][i];
			dec->IntraMatrix[i] = dec->DefaultIntraMatrix[pos];
			dec->InterMatrix[i] = dec->DefaultInterMatrix[pos];
		}
	}

	return VO_ERR_NONE;
}
// static VOMPEG2DECRETURNCODE sequence_scalable_extension(MpegDecode* dec)
// {
// 	return VO_ERR_NONE;
// }
static VOMPEG2DECRETURNCODE picture_display_extension(MpegDecode* dec)
{
	return VO_ERR_NONE;
}
static VOMPEG2DECRETURNCODE picture_coding_extension(MpegDecode* dec)
{
	VO_S32 alternate_scan;
	VO_U32 data, mpeg2_flag = dec->mpeg2_flag;

	if (dec->frame_state != FRAME_DECODING)
	{
		return VO_ERR_WRONG_STATUS;
	}
	mpeg2_flag &= PIC_EXTE_MASK;
	// still 28 bits left;
	data = GetBits(dec, 28);
    dec->mpeg_f_code[0][0] = (data>>24); //GetBits(dec, 4);
    dec->mpeg_f_code[0][1] = ((data>>20)&0xf); //GetBits(dec, 4);
    dec->mpeg_f_code[1][0] = ((data>>16)&0xf); //GetBits(dec, 4);
    dec->mpeg_f_code[1][1] = ((data>>12)&0xf); //GetBits(dec, 4);
    dec->intra_dc_precision = ((data>>10)&0x3); //GetBits(dec, 2);
    dec->picture_structure = ((data>>8)&0x3); //GetBits(dec, 2);
    mpeg2_flag |= (((data>>7)&0x1)<<TOP_FIELD_FIRST);
    mpeg2_flag |= (((data>>6)&0x1)<<FRAME_PRED_FRAME_DCT);
    mpeg2_flag |= (((data>>5)&0x1)<<CON_MOTION_VEC);
    mpeg2_flag |= (((data>>4)&0x1)<<Q_SCALE_TYPE);
    mpeg2_flag |= (((data>>3)&0x1)<<INTRA_VLC_FORT);
    alternate_scan = ((data>>2)&0x1);
    mpeg2_flag |= (((data>>1)&0x1)<<REPEAT_FIRST_FIELD);
    mpeg2_flag |= (((data>>0)&0x1)<<CHROMA_420);
	UPDATE_CACHE(dec);
    mpeg2_flag |= (GetBits(dec, 1)<<PROGRESSIVE_FRAME);
	dec->cur_progressive_frame = (mpeg2_flag & (1<< PROGRESSIVE_FRAME))>>PROGRESSIVE_FRAME;

    if(IsFramePicture(dec))
        CLR_FIRST_FIELD(mpeg2_flag);
    else{
        SET_FIRST_FIELD(mpeg2_flag);
    }

    //TBD
    if(alternate_scan){
		if(!GET_ALTER_SCAN(mpeg2_flag)){
			VO_S32 i;
			for (i=0;i<64;i++){ 
				VO_S32 pos = dec->zigzag[i] = scan[1][i];
				//dec->IntraMatrix[i] = MPEG1_IntraMatrix[pos];
				dec->IntraMatrix[i] = dec->DefaultIntraMatrix[pos];
				dec->InterMatrix[i] = dec->DefaultInterMatrix[pos];
			}
		}
		SET_ALTER_SCAN(mpeg2_flag);
    }else{
		if(GET_ALTER_SCAN(mpeg2_flag)){
			VO_S32 i;
			for (i=0;i<64;i++){ 
				VO_S32 pos = dec->zigzag[i] = scan[0][i];
				//dec->IntraMatrix[i] = MPEG1_IntraMatrix[pos];
				dec->IntraMatrix[i] = dec->DefaultIntraMatrix[pos];
				dec->InterMatrix[i] = dec->DefaultInterMatrix[pos];
			}
		}
		CLR_ALTER_SCAN(mpeg2_flag);
	}

	dec->mpeg2_flag = mpeg2_flag;
	return VO_ERR_NONE;
}
// static VOMPEG2DECRETURNCODE picture_spatial_scalable_extension(MpegDecode* dec)
// {
// 	return VO_ERR_NONE;
// }
// static VOMPEG2DECRETURNCODE picture_temporal_scalable_extension(MpegDecode* dec)
// {
// 	return VO_ERR_NONE;
// }
// static VOMPEG2DECRETURNCODE copyright_extension(MpegDecode* dec)
// {
// 	return VO_ERR_NONE;
// }
VOMPEG2DECRETURNCODE extension_and_user_data(MpegDecode* dec)
{
	return VO_ERR_NONE;
}

VOMPEG2DECRETURNCODE mpeg_decode_extension(MpegDecode* dec)
{
 //   Mpeg1Context *s1 = avctx->priv_data;
//    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int ext_type;
	VOMPEG2DECRETURNCODE rt;
//    init_get_bits(&s->gb, buf, buf_size*8);
    UPDATE_CACHE(dec);
    ext_type = GetBits(dec, 4);
    switch(ext_type) {
    case 0x1:
        rt = sequence_extension(dec);
        break;
    case 0x2:
        rt = sequence_display_extension(dec);
        break;
    case 0x3:
        rt = quant_matrix_extension(dec);
        break;
    case 0x7:
        rt = picture_display_extension(dec);
        break;
    case 0x8:
        rt = picture_coding_extension(dec);
        break;
	default:
		rt = VO_ERR_DEC_MPEG2_HEADER_ERR;
    }
	return rt;
}
//#endif
VOMPEG2DECRETURNCODE sequence_header( MpegDecode* dec )
{
	int i;
	int img_width,img_height;
	VO_U32 data;
	VO_U8 *scan0;
	UPDATE_CACHE(dec);
	// 32 bits available;
	data = GetBits32(dec);
	img_width  = (data>>20); //GetBits(dec,12);
	img_height = ((data>>8)&0x0fff); 

	if(!(dec->amd_malloc_flag)){
		dec_buf_ini(dec, img_width, img_height);
	}else{
		if (dec->img_width != img_width || dec->img_height != img_height){
			
			img_width = max(16, img_width);
			img_width = min(dec->decPictureWidth,img_width);
			img_height = max(16,img_height);
			img_height = min(dec->decPictureHeight,img_height);

			dec->out_img_width = img_width;
			dec->out_img_height = img_height;

			dec->mb_w = ((img_width + 15)>>4); 
			dec->mb_h = ((img_height + 15)>>4);

			dec->img_width = dec->mb_w<<4;
			dec->img_height = dec->mb_h<<4;
			dec->img_stride = (dec->img_width + EDGE_SIZE)>>DOWNSAMPLE_FLAG(dec);//EDGE_SIZE = 0 in mpeg2 decoder

			if (dec->uv_interlace_flage)
			{
				dec->img_stride_uv = dec->img_stride;
			} 
			else
			{
				dec->img_stride_uv = (dec->img_stride>>1);
			}
		}
	
	}

	// skip 4 bits for aspect;
	dec->aspect_ratio = ((data>>4)&0xf);// aspect


	//TBD
	//i= FrameRate[data&0xf];
	dec->frame_rate = data&0xf;
	// 0 bit available, need to UPDATE_CACHE(dec) if we need to read the value;
	// UPDATE_CACHE(dec);
	// skipped 30 bits;
	FLUSH_BITS(dec,30);
	//i= GetBits(dec,18) * 50;
    //FLUSH_BITS(dec,1); // marker
    //FLUSH_BITS(dec,10); // vbv_buffer_size
    //FLUSH_BITS(dec,1);

	UPDATE_CACHE(dec);
	// 26 bits available;
	//default is zigzag in encoded bitstream
	scan0 = &scan[0][0];
	CLR_ALTER_SCAN(dec->mpeg2_flag);
    if (GetBits(dec, 1)){ 
        for (i=0;i<64;i++){
			UPDATE_CACHE(dec);
			//dec->IntraMatrix[i] = (VO_U8)GetBits(dec,8);
			dec->DefaultIntraMatrix[scan0[i]] = dec->IntraMatrix[i] = (VO_U8)GetBits(dec,8);
			dec->zigzag[i] = scan0[i];
        }
    }else{  
		i = 63;
		do {
            //dec->IntraMatrix[i] = MPEG1_IntraMatrix[scan0[i]];
			dec->DefaultIntraMatrix[scan0[i]] = dec->IntraMatrix[i] = MPEG1_IntraMatrix[scan0[i]];
			dec->zigzag[i] = scan0[i];
		} while (--i >= 0);
    }

    if (GetBits(dec, 1)){ 
        for (i=0;i<64;i++){ 
			UPDATE_CACHE(dec);
            dec->DefaultInterMatrix[scan0[i]] = dec->InterMatrix[i] 
				= (VO_U8)GetBits(dec, 8);
        }
    }else{  
		i = 63;
		do {
            dec->DefaultInterMatrix[scan0[i]] = dec->InterMatrix[i] = 16;
		} while (--i >= 0);
    }

	SET_VALID_SEQ(dec->mpeg2_flag);
	return VO_ERR_NONE;
}

VOMPEG2DECRETURNCODE group_header( MpegDecode* dec )
{
	VO_U32 data;

	UPDATE_CACHE(dec);
	data = GetBits(dec, 27);
	dec->group_time_code = data >> 2;

	return VO_ERR_NONE;
}
