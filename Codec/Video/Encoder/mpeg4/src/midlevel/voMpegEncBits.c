/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include <string.h>
#include <stdio.h>
#include "../lowlevel/voMpegEncWriteBits.h"
#include "../lowlevel/voMpegEncTable.h"
#include "../lowlevel/voMpegVlcTable.h"
#include "voMpegEncBits.h"
#include "../common/src/voMpegIdct.h"
#include "../voUtilits.h"

#define LEVELOFFSET 32

#define DMV_CALCULATE(x_vec,y_vec,pMB) \
    x_vec = pMB->mvs[0].x - pMB->pmvs[0].x;\
	y_vec = pMB->mvs[0].y - pMB->pmvs[0].y;\
	if (x_vec < -32)   x_vec += 64;\
	else if (x_vec > 31)  x_vec -= 64;\
	if (y_vec < -32)   y_vec += 64;\
	else if (y_vec > 31)  y_vec -= 64;\
    if (x_vec < 0)  x_vec += 64;\
    if (y_vec < 0)  y_vec += 64;

#define IDCT_BLOCK(qdata,dst,len) \
    if (len == 1){ \
        IDCT_Block1x1((qdata), (dst), stride, (dst), stride);\
    }else if ( len < 15 && ((qdata)[32]==0)){\
        IDCT_Block4x4((qdata), (dst), stride, (dst), stride);\
    }else if ((len<26 && (!(((VO_U32*)(qdata))[2] | ((VO_U32*)(qdata))[6])))){\
        IDCT_Block4x8((qdata), (dst), stride, (dst), stride);\
    }else{\
        IDCT_Block8x8((qdata), (dst), stride, (dst), stride);\
    }
#define DEQUANT_INTER(qcoeff,prev_level) \
    if((prev_level)>=0){\
		(qcoeff)[zigzag[i-1]] = (VO_S16)((prev_level) * twofold_quant + quant_add);\
        VOCLAMP_UP((qcoeff)[zigzag[i-1]],2047);\
	}\
     else {\
         (qcoeff)[zigzag[i-1]] = (VO_S16)((prev_level) * twofold_quant - quant_add);\
         VOCLAMP_DOWN((qcoeff)[zigzag[i-1]],-2048);\
    }

#define  CLAMP(value,cmp,cmp_max,cmp_min,parm) \
    if (value >= cmp_max)\
        value -= parm * cmp;\
    else if (value < cmp_min)\
        value += parm * cmp;	

static const VO_S32 DeQuant_Table[5] = {1, 0, -1 , 2, 3};

//write vol header Video Object Layer
VO_VOID Mpeg4WriteVolHeader(ENCHND * const enc_hnd, BitStream * const bs)
{
    const VO_U32 vo_id = 0;
    const VO_U32 vol_id = 0;
    //VO_S32 vol_ver_id = 1;
    VO_S32 vol_type_ind = VIDOBJLAY_TYPE_SIMPLE;
    VO_S32 vol_profile = 0x08; // Simple level 3;
    VO_S32 bits;

    bits = GetBitsPos(bs);

    //VOS header
    PutBits(bs, VISOBJSEQ_START_CODE, 32);
    PutBits(bs, vol_profile, 8); 	// profile_and_level_indication		

    // VISOBJ_START_CODE
    PadBits(bs);
    PutBits(bs, VISOBJ_START_CODE, 32);
    PutBits(bs, 0, 1);		// is_visual_object_identifier	

    // Video type
    PutBits(bs, (VISOBJ_TYPE_VIDEO<<1)|0, 5);		// visual_object_type  video_signal_type

    // video object_start_code & vo_id 
    PadBitsAlways(bs, 1); //next_start_code()
    PutBits(bs, VIDOBJ_START_CODE|(vo_id&0x5), 32);	

	// video_object_layer_start_code & vol_id
    PadBits(bs);
    PutBits(bs, VIDOBJLAY_START_CODE|(vol_id&0x4), 32);	
    //random_accessible_vol  video_object_type_indication  is_object_layer_identified (0=not given)
    PutBits(bs,0<<9 | vol_type_ind<<1 |0,10 );

    // Aspect ratio 
    //aspect_ratio_info (1=1:1) ;vol_control_parameters;
    //video_object_layer_shape (0=rectangular)	
    PutBits(bs,(((1<<4)|(0<<3)) |(0<1) )|1, 8);	

#ifndef VT
    PutBits(bs, enc_hnd->framerate_base, 16);
#else
    PutBits(bs, 30, 16);
#endif
    PutBits(bs,(0x5<<28)|((enc_hnd->img_width<<1)|1)<<14|(enc_hnd->img_height<<1)|1,31);

    //0x88 = 0100 01000  // interlace obmc_disable,sprite_enable==off not_8_bit 
    //quant_type resync_marker_disable data_partitioned  scalability
    PutBits(bs,0x88,9);
    PadBitsAlways(bs, 1); // next_start_code();
    enc_hnd->sStat.pic_hr_bits += (GetBitsPos(bs) - bits);
}

//write vop header
VO_VOID Mpeg4WriteVopHeader(ENCLOCAL * const enc_local, VO_U32 framerate_base,
                                     VO_U32	 pic_format, VO_U32 img_width, 
                                     VO_U32 img_height, BitStream * const bs, Statistics *sStat)
{
    VO_U32 i,num=0;
#ifdef P_DOMAIN_RC 
    VO_U32 bits;
    bits = GetBitsPos(bs);
#endif
    PutBits(bs, VOP_START_CODE, 32);	
    PutBits(bs, enc_local->coding_type, 2);

    for (i = 0; i < enc_local->seconds; i++) {
        PutBit(bs, 1);
	}    

#ifndef VT
    num = MAX(log2bin(framerate_base-1), 1);
    PutBits(bs,1,2);
    PutBits(bs, enc_local->ticks, num);
#else
    num = MAX(log2bin(30-1), 1);
    PutBits(bs,1,2);
    PutBits(bs, enc_local->ticks, num);
#endif   

    if ( (enc_local->coding_type == P_VOP) || (enc_local->coding_type == S_VOP) ) {
        PutBits(bs,(3<<12) | (enc_local->rounding <<11) | ( enc_local->quant<<3) |enc_local->fcode_for, 14);
    }
    else {
        PutBits(bs,(3<<8)|(0<<5)|enc_local->quant,10);//
    }

#ifdef P_DOMAIN_RC
    sStat->pic_hr_bits += (GetBitsPos(bs) - bits);
#endif
}

VO_S32 voMpeg4WriteVIH(VO_VOID* p_hnd, VO_U8 *output)
{
    BitStream tmp_bs, *bs;
    static const VO_U32 vo_id = 0,vol_id = 0,vol_profile = 0x08; // Simple level 0;
    VO_S32 /*vol_ver_id = 1,*/vol_type_ind = VIDOBJLAY_TYPE_SIMPLE;
    ENCHND* enc_hnd = (ENCHND*)(p_hnd);	
    bs = &tmp_bs;	
 
    InitBits(bs, output);//, 0);
    // Write the VOS header
    PutBits(bs, VISOBJSEQ_START_CODE, 32);
    PutBits(bs, vol_profile, 8); 	//profile_and_level_indication	

    // visual_object_start_code
    PadBits(bs);  // no needed
    PutBits(bs, VISOBJ_START_CODE, 32);
    PutBits(bs, 0, 1);		// is_visual_object_identifier/	

    // Video type: visual_object_type & video_signal_type =0
    PutBits(bs, VISOBJ_TYPE_VIDEO<<1|0 , 5);

    // video object_start_code & vo_id
    PadBitsAlways(bs, 1); // next_start_code()
    PutBits(bs, VIDOBJ_START_CODE|(vo_id&0x5), 32);	

	//video_object_layer_start_code & vol_id/
    PadBits(bs);
    PutBits(bs, VIDOBJLAY_START_CODE|(vol_id&0x4), 32);	
    //random_accessible_vol  
    //video_object_type_indication 
    //is_object_layer_identified (0=not given)
    PutBits(bs, 0<<8|vol_type_ind<<1|0, 10);

    // Aspect ratio 
    //aspect_ratio_info (1=1:1)
    //vol_control_parameters =0
    //video_object_layer_shape (0=rectangular)	
    PutBits(bs, 1<<4 | 0<<1 |0<<2 |1,8);

#ifndef VT
    // fixed vop rate = 0
    PutBits(bs, enc_hnd->framerate_base<<2 | 1<<1|0, 18);
#else
    // fixed_vop_rate = 0
    PutBits(bs, 30<<2 |1<<1|0, 18);	
#endif
     
    PutBits(bs, 1<<28|enc_hnd->img_width<<15 | 1<<14 | enc_hnd->img_height<<1|1, 29);

    PutBits(bs,0x88,9);	

    PadBitsAlways(bs, 1);// next_start_code()

    return FlushBits(bs);
}

static __inline VO_VOID CodeMotionVector(BitStream * bs, VO_S32 value, VO_S32 f_code)
{	
    const VO_S32 scale_factor = 1 << (f_code - 1);
    VO_U16 value_length =0;
    const VO_S32 cmp = scale_factor << 5;  

    CLAMP(value,scale_factor,cmp-1,-cmp,64);

    if (value == 0) {
        PutBits(bs, 1, 1);
    } else {
        VO_U16 length, code, mv_res, sign;
        length = 16 << f_code;
        f_code--;
        sign = (value < 0);
        CLAMP(value,length,length,-length,2);
        if(sign)
            value = -value;
        value--;
        
        if (!f_code) {
            code = (VO_U16)(value + 1);
            if(sign)
                code =-code;
            code = code + 32;
            value_length = mb_motion_table[code];
            PutBits(bs, value_length>>8, value_length&0xff);            
        }  
        else {
            mv_res = (VO_U16)(value & ((1 << f_code) - 1));
            code = (VO_U16)(((value - mv_res) >> f_code) + 1);
            if(sign)
                code =-code;
            code = code + 32;
            value_length = mb_motion_table[code];            
            PutBits(bs, value_length>>8, value_length&0xff);
            PutBits(bs, mv_res, f_code);
        }
    }
}

static __inline VO_S32 CodeCoeffInter(BitStream * bs, VO_S16 *qcoeff, const VO_U16 * zigzag, VO_S32 quant)
{
    VO_U32 i, run, prev_run, code, len, length;
    VO_S32 level, prev_level, level_shifted;
    const VO_S32 twofold_quant = quant << 1;
    const VO_S32 quant_add = quant - 1 + (quant & 1);
    VO_U32 len_code = 0;

    i = 0;
    run = 0;

    while (!(level = qcoeff[zigzag[i++]]));
    prev_level = level;
    prev_run   = i-1;
    length = i;

    //dequant
    DEQUANT_INTER((qcoeff),(prev_level));

    for(;i<64;)
    {
        level = qcoeff[zigzag[i++]];
        if(level)
        {
            level_shifted = prev_level + 32;
            if (!(level_shifted & -64)) { //[0,63]
                len_code = mpeg4_vlc_table[VOMPEG4POS(0,0,level_shifted,prev_run)];
                code = len_code &0x01ffffff;
                len   = len_code>>25;
            }
            else {
                code = (ESCAPE3 << 21) | (prev_run << 14) | (1 << 13) | ((prev_level & 0xfff) << 1) | 1;
                len  = 30;
            }
            PutBits(bs, code, len);

            prev_run   = run;
            prev_level = level;			
            length += (run+1); //zou
            run = 0;
            DEQUANT_INTER((qcoeff),(prev_level));
        }
        else{
            run++;
        }
    }

    level_shifted = prev_level + 32;
    if (!(level_shifted & -64)){
        len_code = mpeg4_vlc_table[VOMPEG4POS(0,1,level_shifted,prev_run)];
        code = len_code &0x01ffffff;
        len   = len_code>>25;
    }else{
        code = (ESCAPE3 << 21) | (1 << 20) | (prev_run << 14) | (1 << 13) | ((prev_level & 0xfff) << 1) | 1;
        len  = 30;
    }
    PutBits(bs, code, len);

    return length;
}

static __inline VO_VOID CodeCoeffIntra(BitStream * bs, const VO_S16 *qcoeff, const VO_U16 * zigzag)
{
	VO_U32 i, abs_level, run, prev_run, code, len;
	VO_S32 level, prev_level;
    int pos=0;
    VO_U32 len_code = 0;

	i = 1;
	run = 0;

	while (i<64 && !(level = qcoeff[zigzag[i++]])) ;
	prev_level = level;
	prev_run   = i-2;
    for(;i<64;i++)
	{
        level = qcoeff[zigzag[i]];
        if(level)
		{
			abs_level = VOGETABS(prev_level);
			abs_level = abs_level < 64 ? abs_level : 0;

            len_code = mpeg4_vlc_table[VOMPEG4POS(1,0,abs_level,prev_run)];      
            len   = len_code>>25;

            if (len != 127) {
                code = len_code &0x01ffffff;
                code |= (prev_level < 0);
            }
            else {
                code = (ESCAPE3 << 21) | (prev_run << 14) | (1 << 13) | ((prev_level & 0xfff) << 1) | 1;
				len = 30;
            }

			PutBits(bs, code, len);
			prev_level = level;
			prev_run = run;
			run = 0;
		}
		else
			run++;
	}

	abs_level = VOGETABS(prev_level);
	abs_level = abs_level < 64 ? abs_level : 0;

    pos = VOMPEG4POS(1,1,abs_level,prev_run);
    len_code = mpeg4_vlc_table[pos];
    len   = len_code>>25;
    if (len != 127) {
        code = len_code &0x01ffffff;
        code |= (prev_level < 0);
    }
    else {
        code = (ESCAPE3 << 21) | (1 << 20) | (prev_run << 14) | (1 << 13) | ((prev_level & 0xfff) << 1) | 1;
		len  = 30;
    }

	PutBits(bs, code, len);
}

// returns the number of bits required to encode qcoeff 
VO_S32 CalcIntraCoeffBits(const VO_S16 qcoeff[64], const VO_S32 direction)
{
	const VO_U16 *zigzag = scan_tables[direction];
	VO_S32 bits = 0;
	VO_U32 i = 1, abs_level, run = 0, prev_run, len;
	VO_S32 level =0, prev_level;
    //VO_U32 len_code = 0;

    while (i<64 && !(level))
        level = qcoeff[zigzag[i++]];

	if (i > 63) 
        return 0;	
	prev_level = level;
	prev_run   = i-2;

    for(;i<64;i++)
	{
        if( (level = qcoeff[zigzag[i]]) == 0 ) {
            run++;
        }
        else {
            abs_level = VOGETABS(prev_level);
            prev_level = level;
            if(abs_level < 64)
                 len = mpeg4_vlc_table[VOMPEG4POS(1,0,abs_level,prev_run)]>>25;
            else 
                 len = mpeg4_vlc_table[VOMPEG4POS(1,0,0,prev_run)]>>25;
			bits      += len!= 127 ? len : 30;
            prev_run   = run;
			run = 0;	
        }
	}

	abs_level = VOGETABS(prev_level);
    if(abs_level < 64) 
        len = mpeg4_vlc_table[VOMPEG4POS(1,1,abs_level,prev_run)]>>25;
    else 
         len = mpeg4_vlc_table[VOMPEG4POS(1,1,0,prev_run)]>>25;

	bits += len!= 127 ? len : 30;

	return bits;
}


VO_VOID EncodeMpeg4IntraMB(ENCLOCAL * const enc_local, 
                                    MACROBLOCK * pMB, 
				                    VO_S16 *qcoeff, 
                                    BitStream * bs, 
                                    Statistics * pStat)
{

	VO_U32 i, mcbpc, cbpy;
	const VO_U32 coding_type = enc_local->coding_type;
    VO_U16 value_length = 0;

#ifdef P_DOMAIN_RC
    VO_U32 bits0, bits1;
	bits0 = GetBitsPos(bs);
#endif

    //pMB->cbp  6bit:
    //0xb 123456   -->  1234 ---Y: 
    //                  -->   56   ---uv
    //pMB->cbp = CalcCBP(qcoeff);

	cbpy = pMB->cbp >> 2;
	// write mcbpc
	if (coding_type == I_VOP) {
		mcbpc = ((pMB->mode >> 1) & 3) | ((pMB->cbp & 3) << 2);
        value_length = mcbpc_intra_table[mcbpc];
		PutBits(bs, value_length>>8, value_length&0xff);
	} else {
		PutBit(bs, 0);
		mcbpc = (pMB->mode & 7) | ((pMB->cbp & 3) << 3);
        value_length = mcbpc_inter_table[mcbpc];
		PutBits(bs, value_length>>8, value_length&0xff);
	}

    value_length = cbp_lunina_table[cbpy];
    // ac prediction flag and cbpy
    if (pMB->ac_dir & ACDIR_MSK)
        PutBits(bs, 1<<(value_length&0xff) | (value_length>>8), 1+(value_length&0xff));
    else
        PutBits(bs,  (value_length>>8), 1+(value_length&0xff));

	// write dquant 
	if (pMB->mode == MODE_INTRA_Q)
		PutBits(bs, DeQuant_Table[(pMB->dquant)+2], 2);

#ifdef P_DOMAIN_RC
	bits1 = GetBitsPos(bs);
	pStat->mb_hr_bits += (bits1 - bits0);
#endif

	// code block coeffs
    // Y
    for (i = 0; i < 4; i++) {
        value_length = dc_lumina_table[qcoeff[i * 64 + 0] + 255];
		PutBits(bs, value_length>>4, value_length&0xf);
		if (pMB->cbp & (1 << (5 - i))) {
			const VO_U16 *scan_table = scan_tables[(pMB->ac_dir & (ACDIR_MSK << (i*2))) >> (i*2)];
			CodeCoeffIntra(bs, &qcoeff[i * 64], scan_table);
		}
	}
   // U V
    value_length = dc_chroma_table[qcoeff[4 * 64 + 0] + 255];
    PutBits(bs, value_length>>4, (value_length&0xf)+1);
    if (pMB->cbp & (1 << (5 - 4))) {
        const VO_U16 *scan_table = scan_tables[(pMB->ac_dir & (ACDIR_MSK << (4*2))) >> (4*2)];
        CodeCoeffIntra(bs, &qcoeff[4 * 64], scan_table);
    }
    
    value_length = dc_chroma_table[qcoeff[5 * 64 + 0] + 255];
    PutBits(bs, value_length>>4, (value_length&0xf)+1);
    if (pMB->cbp & (1 << (5 - 5))) {
        const VO_U16 *scan_table = scan_tables[(pMB->ac_dir & (ACDIR_MSK << (5*2))) >> (5*2)];
        CodeCoeffIntra(bs, &qcoeff[5 * 64], scan_table);
    }

#ifdef P_DOMAIN_RC
	bits0 = GetBitsPos(bs);
	pStat->text_bits += (bits0 - bits1);
#endif
}

VO_VOID EncodeMpeg4InterMB(ENCLOCAL * const enc_local, 
                                MACROBLOCK * pMB, 
				                VO_S16 *qcoeff, 
                                BitStream * bs, 
                                Statistics * pStat,
                                Mpeg4Frame *img_cur, 
                                VO_S32 stride)
{

	VO_S32 i,len;
	VO_U32 mcbpc, cbpy;
	const VO_U32 fcode_for = enc_local->fcode_for;
    VO_U16 value_length =0,value_length2 =0;
    VO_S16 * data;
    VO_U8* dst;
#ifdef P_DOMAIN_RC
    VO_S32 bits0, bits1;
	bits0 = GetBitsPos(bs);
#endif

	PutBit(bs, 0);
	mcbpc = (pMB->mode & 7) | ((pMB->cbp & 3) << 3);
	cbpy = 15 - (pMB->cbp >> 2);

    value_length = mcbpc_inter_table[mcbpc];
    value_length2 = cbp_lunina_table[cbpy];
	PutBits(bs, (value_length>>8)<<(value_length2&0xff)|value_length2>>8, (value_length&0xff) + (value_length2&0xff));

	// write dquant
	if (pMB->mode == MODE_INTER_Q)
		PutBits(bs, DeQuant_Table[(pMB->dquant)+2], 2);

#ifdef P_DOMAIN_RC	
	bits1 = GetBitsPos(bs);
	pStat->mb_hr_bits += (bits1 - bits0);
#endif

    CodeMotionVector(bs, pMB->mvs[0].x - pMB->pmvs[0].x, fcode_for);
    CodeMotionVector(bs, pMB->mvs[0].y - pMB->pmvs[0].y, fcode_for);
    if(pMB->mode == MODE_INTER4V) {
        CodeMotionVector(bs, pMB->mvs[1].x - pMB->pmvs[1].x, fcode_for);
        CodeMotionVector(bs, pMB->mvs[1].y - pMB->pmvs[1].y, fcode_for);
        CodeMotionVector(bs, pMB->mvs[2].x - pMB->pmvs[2].x, fcode_for);
        CodeMotionVector(bs, pMB->mvs[2].y - pMB->pmvs[2].y, fcode_for);
        CodeMotionVector(bs, pMB->mvs[3].x - pMB->pmvs[3].x, fcode_for);
        CodeMotionVector(bs, pMB->mvs[3].y - pMB->pmvs[3].y, fcode_for);
    }
#ifdef P_DOMAIN_RC	
	bits0 = GetBitsPos(bs);	
	pStat->mv_bits += (bits0 - bits1);
#endif

    //bs->buf64 = (VO_U64)bs->buf<<32;

    for (i = 0; i < 4; i++){		
		if (pMB->cbp & (1 << (5 - i))) {
			data = qcoeff + (i << 6);
            dst = img_cur->y+(i>>1)* (stride<<3)+((i&1)<<3);	
			len = CodeCoeffInter(bs, data, scan_tables[0],pMB->quant);
            IDCT_BLOCK(data,dst,len);
		}
	}

    stride>>=1;
    dst = img_cur->uv[0];
    if (pMB->cbp & (1 << (1))) {
        data = qcoeff + (4 << 6);
        len = CodeCoeffInter(bs, data, scan_tables[0],pMB->quant);
        IDCT_BLOCK(data,dst,len);
    }

    dst = img_cur->uv[1];
    if (pMB->cbp & (1 << (0))) {
        data = qcoeff + (5 << 6);
        len = CodeCoeffInter(bs, data, scan_tables[0],pMB->quant);
        IDCT_BLOCK(data,dst,len);
    }
#ifdef P_DOMAIN_RC
	bits1 = GetBitsPos(bs);
	pStat->text_bits += (bits1 - bits0);
#endif
}
#ifdef MPEG4_RESYNC
static const VECTOR zeroMV = { 0, 0 };
VO_VOID VOMpeg4Resync(ENCHND * const enc_hnd, 
                            BitStream * bs, 
                            const VO_S32 mb_x, 
                            const VO_S32 mb_y, 
                            const VO_S32 mb_width, 
				            MACROBLOCK *pMB, 
                            const VO_S32 bits)
{
	VO_S32 i;
	VO_S32 width = enc_hnd->img_width<<1;
	VO_U16 * const acdc_row = enc_hnd->pred_acdc_row;
	VO_U16 * const acdc_col = enc_hnd->pred_acdc_col;
	
//	PadBits(bs);
	PadBitsAlways(bs, 1);// TBD check stuff_idx for h.263
	PutBits(bs, 1, bits);
	PutBits(bs, mb_y*mb_width+mb_x, enc_hnd->mb_num_len);
	PutBits(bs, pMB->quant, 5);	// quantizer
	PutBits(bs, 0, 1);
	
	//reset ACDC buffer
	for (i = 0; i <= width; i+=32){
		acdc_row[i] = 1024;
		acdc_row[i+1] = acdc_row[i+2] = acdc_row[i+3] = acdc_row[i+4] = \
			acdc_row[i+5] = acdc_row[i+6] = acdc_row[i+7] = 0;
		acdc_row[i+8] = 1024;
		acdc_row[i+9] = acdc_row[i+10] = acdc_row[i+11] = acdc_row[i+12] =\
			acdc_row[i+13] = acdc_row[i+14] = acdc_row[i+15] = 0;
		acdc_row[i+16] = 1024;
		acdc_row[i+17] = acdc_row[i+18] = acdc_row[i+19] = acdc_row[i+20] =\
			acdc_row[i+21] = acdc_row[i+22] = acdc_row[i+23] =	0;
		acdc_row[i+24] = 1024;
		acdc_row[i+25] = acdc_row[i+26] = acdc_row[i+27] = acdc_row[i+28] =\
			acdc_row[i+29] = acdc_row[i+30] = acdc_row[i+31]=	0;		
	}

	acdc_col[0] = 1024;
	acdc_col[1] = acdc_col[2] = acdc_col[3] = acdc_col[4] = \
		acdc_col[5] = acdc_col[6] = acdc_col[7] = 0;
	acdc_col[8] = 1024;
	acdc_col[9] = acdc_col[10] = acdc_col[11] = acdc_col[12] = \
		acdc_col[13] = acdc_col[14] = acdc_col[15] = 0;
	acdc_col[16] = 1024;
	acdc_col[17] = acdc_col[18] = acdc_col[19] = acdc_col[20] = \
		acdc_col[21] = acdc_col[22] = acdc_col[23] = 0;
	acdc_col[24] = 1024;
	acdc_col[25] = acdc_col[26] = acdc_col[27] = acdc_col[28] = \
		acdc_col[29] = acdc_col[30] = acdc_col[31] = 0;
}
#endif//MPEG4_RESYNC


//VO_VIDEO_CodingH263 Bitstreams
VO_VOID H263WritePicHeader(ENCLOCAL * const enc_local, 
                                VO_U32 framerate_base,
                                VO_U32	pic_format, 
                                VO_U32 img_width,
                                VO_U32 img_height,
                                BitStream * const bs,
                                Statistics *sStat)
{
	VO_U32 coding_IVOP= (enc_local->coding_type == I_VOP);
	
#ifdef P_DOMAIN_RC
    VO_S32 bits;
	bits = GetBitsPos(bs);
#endif

    PutBits(bs, 1, 17);
    PutBits(bs, 0x1f0, 18);

	if(pic_format == 6){
        if(coding_IVOP == 1) {
            PutBits(bs, (7<<3)|coding_IVOP, 6);		
            PutBits(bs, (pic_format<<15)|0x8, 18);
            PutBits(bs, (enc_local->coding_type<<7)|(enc_local->rounding<<4)|0x2, 10);
            PutBits(bs, ((2<<9)|((img_width/4)-1))<<10 | (1<<9)|(img_height/4) , 23); 
            PutBits(bs, enc_local->quant, 5);
        }
        else {
            PutBits(bs, (7<<3)|coding_IVOP, 6);		
            PutBits(bs, (enc_local->coding_type<<7)|(enc_local->rounding<<4)|0x2, 10);
            PutBits(bs, enc_local->quant, 5);
        }	
	}else{
        PutBits(bs, pic_format<<11 | enc_local->coding_type<<10 | enc_local->quant<<1 , 14);
	}		
	PutBits(bs, 0, 1);	

#ifdef P_DOMAIN_RC
	sStat->pic_hr_bits += (GetBitsPos(bs) - bits);	
#endif
}

static VO_S32 EncodeH263Coeff(BitStream * const bs, VO_S32 mode, VO_S16 *qcoeff, VO_S32 quant)
{
	VO_S32 j, ind, len;
	VO_S32 prev_run, run, prev_level, level, first, level_shifted;
	VO_U32 code;
	const VO_U16 * zigzag = scan_tables[0];
	const VO_S32 twofold_quant = quant << 1;
	const VO_S32 quant_add = quant;		
	
	run = 0;
	first = 1;
	prev_run = prev_level = level = 0;

	if(mode == MODE_INTRA || mode == MODE_INTRA_Q){
		// DC coeff
		if (qcoeff[0] != 128)
			PutBits(bs, qcoeff[0], 8);
		else
			PutBits(bs, 255, 8);
		j = len = 1;
	}else{
		j = len = 0;
	}
	
	while(j < 64) {
		// AC coeff
		ind = zigzag[j++];
        level = qcoeff[ind];

        if(!level) {
            run++; 
        }
        else {
            len += (run + 1);
            if (level < 0) {
				qcoeff[ind] = (VO_S16)(level * twofold_quant - quant_add);
                VOCLAMP_DOWN(qcoeff[ind],-2048);
			} else {
				qcoeff[ind] = (VO_S16)(level * twofold_quant + quant_add);
                VOCLAMP_UP(qcoeff[ind],2047);
			}
            if (!first) { // encode the previous coeff			
				level_shifted = prev_level + 32;
				if (!(level_shifted & -64)){
					code = h263_coeff_vlc_table[VOH263POS(level_shifted,prev_run)];
					PutBits(bs, code>>8, code&0xff);
				}else{
					code = 0x18000 | (prev_run << 8) | (prev_level &0xff);
					PutBits(bs, code, 22);
				}
			}
            prev_run = run;
			prev_level = level; 
			run = first = 0;	
        }
	}
	// Encode the last coeff
	if (!first) {
		level_shifted = prev_level + 32;
		if (!(level_shifted & -64)){
            code = h263_coeff_vlc_last_table[VOH263POS(level_shifted,prev_run)];
			PutBits(bs, code>>8, code&0xff);
		}else{
			code = 0x18000 | (prev_run << 8) | (prev_level &0xff);
			PutBits(bs, code, 22);
		}
	}
	return len;
}


VO_VOID EncodeH263InterMB(ENCLOCAL * const enc_local, 
                                 MACROBLOCK * pMB, 
                                 VO_S16 *qcoeff, 
					             BitStream * bs, 
                                 Statistics * pStat, 
                                 Mpeg4Frame *img_cur, 
                                 VO_S32 stride)
{
	VO_S32 cbpy, mcbpc, x_vec, y_vec, i;
	const VO_S32 mode = pMB->mode, cbp = pMB->cbp;
    VO_U16 value_lenght_x = 0,value_lenght_y=0;
    VO_S32 len;
#ifdef P_DOMAIN_RC
    VO_U32 bits0, bits1;	
	bits0 = GetBitsPos(bs);
#endif
	// COD 
	PutBits(bs, 0, 1);
	
	// CBPCM
	mcbpc = (mode & 7) | ((cbp & 3) << 3);
	cbpy = 15 - (cbp >> 2);

	// write mcbpc
    value_lenght_x = mcbpc_inter_table[mcbpc];
	PutBits(bs, value_lenght_x>>8, value_lenght_x&0xff);

	// write cbpy
    value_lenght_x = cbp_lunina_table[cbpy];
    PutBits(bs,value_lenght_x>>8,value_lenght_x&0xff);

	// DQUANT
	if (mode == MODE_INTER_Q)
		PutBits(bs, DeQuant_Table[(pMB->dquant)+2], 2);

#ifdef P_DOMAIN_RC	
	bits1 = GetBitsPos(bs);
	pStat->mb_hr_bits += (bits1 - bits0);
#endif
	
	DMV_CALCULATE(x_vec,y_vec,pMB);

    value_lenght_x = h263_motion_table[x_vec];
    value_lenght_y = h263_motion_table[y_vec];
    PutBits(bs, value_lenght_x>>8,value_lenght_x&0xff);
    PutBits(bs, value_lenght_y>>8,value_lenght_y&0xff);

#ifdef P_DOMAIN_RC
	bits0 = GetBitsPos(bs);
	pStat->mv_bits += (bits0 - bits1);
#endif

	if (cbp){
        VO_S16 * qdata;
        VO_U8* dst;

        for (i = 0; i < 4; i++) {
			if(cbp&(0x20>>i)){
				qdata = qcoeff + (i << 6);
				dst = img_cur->y+(i>>1)* (stride<<3)+((i&1)<<3);
				len = EncodeH263Coeff(bs, mode, qdata, pMB->quant);
				IDCT_BLOCK(qdata,dst,len);
			}
		}

        stride>>=1;
        if(cbp&(0x20>>4)){
            qdata = qcoeff + (4<< 6);
            dst = img_cur->uv[0];
            len = EncodeH263Coeff(bs, mode, qdata, pMB->quant);
            IDCT_BLOCK(qdata,dst,len);				
        }

        if(cbp&(0x20>>5)){
            qdata = qcoeff + (5<< 6);
            dst = img_cur->uv[1];
            len = EncodeH263Coeff(bs, mode, qdata, pMB->quant);
            IDCT_BLOCK(qdata,dst,len);				
        }
	}
#ifdef P_DOMAIN_RC
	bits1 = GetBitsPos(bs);
	pStat->text_bits += (bits1 - bits0);
#endif
}

VO_VOID EncodeH263IntraMB(ENCLOCAL * const enc_local,
                                MACROBLOCK * pMB, 
                                VO_S16 *qcoeff, 
                                BitStream * bs, 
                                Statistics * pStat)
{
	VO_S32 cbpy, mcbpc;
	const VO_S32 mode = pMB->mode;
	const VO_U32 coding_type = enc_local->coding_type;
    VO_U16 value_length =0;

#ifdef P_DOMAIN_RC	
    VO_U32 bits0, bits1;
	bits0 = GetBitsPos(bs);
#endif

    if (coding_type == I_VOP) {
        mcbpc = ((pMB->mode >> 1) & 3) | ((pMB->cbp & 3) << 2);
		value_length = mcbpc_intra_table[mcbpc];
		PutBits(bs, value_length>>8, value_length&0xff);
    }
    else {
        if(coding_type == P_VOP)
            PutBits(bs, 0, 1);
        mcbpc = (pMB->mode & 7) | ((pMB->cbp & 3) << 3);
        value_length = mcbpc_inter_table[mcbpc];
		PutBits(bs, value_length>>8, value_length&0xff);
    }

	//pMB->cbp = CalcCBP(qcoeff);	

	cbpy = pMB->cbp >> 2;
	// write cbpy
    value_length = cbp_lunina_table[cbpy];
    PutBits(bs,value_length>>8,value_length&0xff);

	// write DQUANT
	if (mode == MODE_INTRA_Q)
		PutBits(bs, DeQuant_Table[(pMB->dquant)+2], 2);

#ifdef P_DOMAIN_RC
	bits1 = GetBitsPos(bs);
	pStat->mb_hr_bits += (bits1 - bits0);
#endif

    EncodeH263Coeff(bs, mode, qcoeff + (0<<6), 0);
    EncodeH263Coeff(bs, mode, qcoeff + (1<<6), 0);
    EncodeH263Coeff(bs, mode, qcoeff + (2<<6), 0);
    EncodeH263Coeff(bs, mode, qcoeff + (3<<6), 0);
    EncodeH263Coeff(bs, mode, qcoeff + (4<<6), 0);
    EncodeH263Coeff(bs, mode, qcoeff + (5<<6), 0);

#ifdef P_DOMAIN_RC
	bits0 = GetBitsPos(bs);
	pStat->text_bits += (bits0 - bits1);
#endif

}

