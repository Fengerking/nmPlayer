#include "voVP8Common.h"
#include "voVP8Decoder.h"
#include "voVP8DecFrame.h"
#include "voVP8DecMBlock.h"
#include "voVP8Memory.h"
#include "voVP8DecRecon.h"
#include "voVP8DecInterPolationFilter.h"

//#include "stdio.h"

extern int decoder_mode_mvs_mbrow(VP8D_COMP *pbi,int mb_row);

const int vp8_default_zig_zag1d[16] =
{
    0,  1,  4,  8,
    5,  2,  3,  6,
    9, 12, 13, 10,
    7, 11, 14, 15,
};
static const int dc_qlookup[QINDEX_RANGE] =
{
    4,    5,    6,    7,    8,    9,   10,   10,   11,   12,   13,   14,   15,   16,   17,   17,
    18,   19,   20,   20,   21,   21,   22,   22,   23,   23,   24,   25,   25,   26,   27,   28,
    29,   30,   31,   32,   33,   34,   35,   36,   37,   37,   38,   39,   40,   41,   42,   43,
    44,   45,   46,   46,   47,   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,
    59,   60,   61,   62,   63,   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,
    75,   76,   76,   77,   78,   79,   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,
    91,   93,   95,   96,   98,  100,  101,  102,  104,  106,  108,  110,  112,  114,  116,  118,
    122,  124,  126,  128,  130,  132,  134,  136,  138,  140,  143,  145,  148,  151,  154,  157,
};

static const int ac_qlookup[QINDEX_RANGE] =
{
    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,
    36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51,
    52,   53,   54,   55,   56,   57,   58,   60,   62,   64,   66,   68,   70,   72,   74,   76,
    78,   80,   82,   84,   86,   88,   90,   92,   94,   96,   98,  100,  102,  104,  106,  108,
    110,  112,  114,  116,  119,  122,  125,  128,  131,  134,  137,  140,  143,  146,  149,  152,
    155,  158,  161,  164,  167,  170,  173,  177,  181,  185,  189,  193,  197,  201,  205,  209,
    213,  217,  221,  225,  229,  234,  239,  245,  249,  254,  259,  264,  269,  274,  279,  284,
};

#ifdef STABILITY
void get_scaleparam(int scale,int *scaleX0,int *scaleX1)
{
	switch(scale)
	{
	case 0:
		*scaleX0 =1; *scaleX1 =1; break;
	case 1:
		*scaleX0 =5; *scaleX1 =4; break;
	case 2:
		*scaleX0 =5; *scaleX1 =3; break;
	case 3:
		*scaleX0 =2; *scaleX1 =1; break;
	default:		
		*scaleX0 =1; *scaleX1 =1; break;
	}
	return;
}
#endif

static int get_delta_q(vp8_reader *bc, int prev, int *q_update)
{
    int ret_val = 0;

    if (vp8_read_bit(bc))
    {
        ret_val = vp8_read_literal(bc, 4);

        if (vp8_read_bit(bc))
            ret_val = -ret_val;
    }

    /* Trigger a quantizer update if the delta-q value has changed */
    if (ret_val != prev)
        *q_update = 1;

    return ret_val;

}
int vp8_dc_quant(int QIndex, int Delta)
{
    int retval;

    QIndex = QIndex + Delta;

    if (QIndex > 127)
        QIndex = 127;
    else if (QIndex < 0)
        QIndex = 0;

    retval = dc_qlookup[ QIndex ];
    return retval;
}

int vp8_dc2quant(int QIndex, int Delta)
{
    int retval;

    QIndex = QIndex + Delta;

    if (QIndex > 127)
        QIndex = 127;
    else if (QIndex < 0)
        QIndex = 0;

    retval = dc_qlookup[ QIndex ] * 2;
    return retval;

}
int vp8_dc_uv_quant(int QIndex, int Delta)
{
    int retval;

    QIndex = QIndex + Delta;

    if (QIndex > 127)
        QIndex = 127;
    else if (QIndex < 0)
        QIndex = 0;

    retval = dc_qlookup[ QIndex ];

    if (retval > 132)
        retval = 132;

    return retval;
}

int vp8_ac_yquant(int QIndex)
{
    int retval;

    if (QIndex > 127)
        QIndex = 127;
    else if (QIndex < 0)
        QIndex = 0;

    retval = ac_qlookup[ QIndex ];
    return retval;
}

int vp8_ac2quant(int QIndex, int Delta)
{
    int retval;

    QIndex = QIndex + Delta;

    if (QIndex > 127)
        QIndex = 127;
    else if (QIndex < 0)
        QIndex = 0;

    retval = (ac_qlookup[ QIndex ] * 155) / 100;

    if (retval < 8)
        retval = 8;

    return retval;
}
int vp8_ac_uv_quant(int QIndex, int Delta)
{
    int retval;

    QIndex = QIndex + Delta;

    if (QIndex > 127)
        QIndex = 127;
    else if (QIndex < 0)
        QIndex = 0;

    retval = ac_qlookup[ QIndex ];
    return retval;
}

void vp8cx_init_de_quantizer(VP8D_COMP *pbi)
{
    int i;
    int Q;
    VP8_COMMON *const pc = & pbi->common;

    for (Q = 0; Q < QINDEX_RANGE; Q++)
    {
        pc->Y1dequant[Q][0] = (short)vp8_dc_quant(Q, pc->y1dc_delta_q);
        pc->Y2dequant[Q][0] = (short)vp8_dc2quant(Q, pc->y2dc_delta_q);
        pc->UVdequant[Q][0] = (short)vp8_dc_uv_quant(Q, pc->uvdc_delta_q);

        for (i = 1; i < 16; i++)
        {
            int rc = vp8_default_zig_zag1d[i];
           /* r = (rc >> 2);
            c = (rc & 3);*/

            pc->Y1dequant[Q][rc] = (short)vp8_ac_yquant(Q);
            pc->Y2dequant[Q][rc] = (short)vp8_ac2quant(Q, pc->y2ac_delta_q);
            pc->UVdequant[Q][rc] = (short)vp8_ac_uv_quant(Q, pc->uvac_delta_q);
        }
    }
}

void mb_init_dequantizer(VP8D_COMP *pbi, MACROBLOCKD *xd,MODE_INFO *pModeInfo)
{
    int QIndex;
    MB_MODE_INFO *mbmi = &pModeInfo->mbmi;
    VP8_COMMON *const pc = & pbi->common;

    // Decide whether to use the default or alternate baseline Q value.
    if (xd->segmentation_enabled)
    {
        // Abs Value
        if (xd->mb_segement_abs_delta == SEGMENT_ABSDATA)
            QIndex = xd->segment_feature_data[MB_LVL_ALT_Q][mbmi->segment_id];

        // Delta Value
        else
        {
            QIndex = pc->base_qindex + xd->segment_feature_data[MB_LVL_ALT_Q][mbmi->segment_id];
            QIndex = (QIndex >= 0) ? ((QIndex <= MAXQ) ? QIndex : MAXQ) : 0;    // Clamp to valid range
        }
    }
    else
        QIndex = pc->base_qindex;

	xd->dq_y = (short*)pc->Y1dequant[QIndex];
	xd->dq_uv = (short*)pc->UVdequant[QIndex];
    xd->dq_y2 = (short*)pc->Y2dequant[QIndex];
}
static void init_key_frame(VP8D_COMP *pbi)
{
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;

    //if (pc->frame_type == KEY_FRAME)
    {
        // Various keyframe initializations
		CopyMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8*)(pc->fc.mvc), (VO_U8*)(vp8_default_mv_context),  sizeof(vp8_default_mv_context));

        vp8_init_mbmode_probs(pc);

        vp8_default_coef_probs(pc);

        vp8_kf_default_bmode_probs(pc->kf_bmode_prob);

        // reset the segment feature data to 0 with delta coding (Default state).
		SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->segment_feature_data, 0, sizeof(xd->segment_feature_data));

        xd->mb_segement_abs_delta = SEGMENT_DELTADATA;

       // reset the mode ref deltasa for loop filter
		SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->ref_lf_deltas, 0, sizeof(xd->ref_lf_deltas));
		SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->mode_lf_deltas, 0, sizeof(xd->mode_lf_deltas));


        // All buffers are implicitly updated on key frames.
        pc->refresh_golden_frame = 1;
        pc->refresh_alt_ref_frame = 1;
        pc->copy_buffer_to_gf = 0;
        pc->copy_buffer_to_arf = 0;

        // Note that Golden and Altref modes cannot be used on a key frame so
        // ref_frame_sign_bias[] is undefined and meaningless
        pc->ref_frame_sign_bias[GOLDEN_FRAME] = 0;
        pc->ref_frame_sign_bias[ALTREF_FRAME] = 0;
    }
    xd->left_context = &pc->left_context;
    xd->mode_info_context = pc->mi;
    xd->frame_type = pc->frame_type;
    xd->mode_info_context->mbmi.mode = DC_PRED;
    xd->mode_info_stride = pc->mode_info_stride;
}
static void init_inter_frame(VP8D_COMP *pbi)
{
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;
    {
		if (!pc->use_bilinear_mc_filter)
		{
		    xd->subpixel_predict      = vp8_sixtap_predict;
            xd->subpixel_predict8x4   = vp8_sixtap_predict8x4;
            xd->subpixel_predict8x8   = vp8_sixtap_predict8x8;
            xd->subpixel_predict16x16 = vp8_sixtap_predict16x16;
		}
		else
		{
			xd->subpixel_predict      = vp8_bilinear_predict4x4;
            xd->subpixel_predict8x4   =  vp8_bilinear_predict8x4;
            xd->subpixel_predict8x8   =  vp8_bilinear_predict8x8;
            xd->subpixel_predict16x16 =  vp8_bilinear_predict16x16;
		}
    }

    xd->left_context = &pc->left_context;
    xd->mode_info_context = pc->mi;
    xd->frame_type = pc->frame_type;
    xd->mode_info_context->mbmi.mode = DC_PRED;
    xd->mode_info_stride = pc->mode_info_stride;
}

void vp8_extend_mb_row(YV12_BUFFER_CONFIG *ybf, unsigned char *YPtr, unsigned char *UPtr, unsigned char *VPtr)
{
    int i;

    YPtr += ybf->y_stride * 14;
    UPtr += ybf->uv_stride * 6;
    VPtr += ybf->uv_stride * 6;

    for (i = 0; i < 4; i++)
    {
        YPtr[i] = YPtr[-1];
        UPtr[i] = UPtr[-1];
        VPtr[i] = VPtr[-1];
    }

    YPtr += ybf->y_stride;
    UPtr += ybf->uv_stride;
    VPtr += ybf->uv_stride;

    for (i = 0; i < 4; i++)
    {
        YPtr[i] = YPtr[-1];
        UPtr[i] = UPtr[-1];
        VPtr[i] = VPtr[-1];
    }
}

int vp8_decode_mb_row(VP8D_COMP *pbi,
                       VP8_COMMON *pc,
                       int mb_row,
                       MACROBLOCKD *xd)
{
    int recon_yoffset, recon_uvoffset;
    int mb_col;
    int ref_fb_idx = pc->lst_fb_idx;
    int dst_fb_idx = pc->new_fb_idx;
    int recon_y_stride = pc->yv12_fb[ref_fb_idx].y_stride;
    int recon_uv_stride = pc->yv12_fb[ref_fb_idx].uv_stride;
	MODE_INFO *pModeInfo = xd->mode_info_context + mb_row*(pc->mb_cols+1);

	SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)(&pc->left_context), 0, sizeof(pc->left_context));
    recon_yoffset = mb_row * recon_y_stride * 16;
    recon_uvoffset = mb_row * recon_uv_stride * 8;
    // reset above block coeffs

	xd->above_context = pc->above_context;
    xd->up_available = (mb_row != 0);

    for (mb_col = 0; mb_col < pc->mb_cols; mb_col++)
    {
        xd->dst.y_buffer = pc->yv12_fb[dst_fb_idx].y_buffer + recon_yoffset;
        xd->dst.u_buffer = pc->yv12_fb[dst_fb_idx].u_buffer + recon_uvoffset;
        xd->dst.v_buffer = pc->yv12_fb[dst_fb_idx].v_buffer + recon_uvoffset;

        xd->left_available = (mb_col != 0);

        // Select the appropriate reference frame for this MB
		if (pModeInfo->mbmi.ref_frame == LAST_FRAME)
            ref_fb_idx = pc->lst_fb_idx;
        else if (pModeInfo->mbmi.ref_frame == GOLDEN_FRAME)
            ref_fb_idx = pc->gld_fb_idx;
        else
            ref_fb_idx = pc->alt_fb_idx;

        xd->pre.y_buffer = pc->yv12_fb[ref_fb_idx].y_buffer + recon_yoffset;
        xd->pre.u_buffer = pc->yv12_fb[ref_fb_idx].u_buffer + recon_uvoffset;
        xd->pre.v_buffer = pc->yv12_fb[ref_fb_idx].v_buffer + recon_uvoffset;

		if (xd->frame_type == KEY_FRAME  ||  pModeInfo->mbmi.ref_frame== INTRA_FRAME)
		{
			if(vp8_decode_intra_macroblock(pbi, xd,pModeInfo)!=0)
				return -1;
		}
		else
		{
			pbi->common.build_uvmvs(xd,pModeInfo);

			if(vp8_decode_inter_macroblock(pbi, xd,pModeInfo)!=0)
				return -1;
		}

        recon_yoffset += 16;
        recon_uvoffset += 8;

       // ++xd->mode_info_context;  /* next mb */
		++pModeInfo;
		xd->above_context++;
#if 1
		if(mb_col == pc->mb_cols-1)
		{
			vp8_extend_mb_row( &pc->yv12_fb[dst_fb_idx], xd->dst.y_buffer + 16, xd->dst.u_buffer + 8, xd->dst.v_buffer + 8 );
		}
#endif

    }

	return 0;
}

int vp8_decode_mb_row_parser(VP8D_COMP *pbi,
                       VP8_COMMON *pc,
                       int mb_row,
                       MACROBLOCKD *xd,int thread,BOOL_DECODER *bc)
{
    int mb_col;
	MACROBLOCKD* xd_row;
	MODE_INFO *pModeInfo = xd->mode_info_context + mb_row*(pc->mb_cols+1);

	if(thread==0)
		xd_row = pbi->mbrow;
	else
		xd_row = pbi->mbrow2;

	SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)(&pc->left_context), 0, sizeof(pc->left_context));
    // reset above block coeffs

	xd->above_context = pc->above_context;
    xd->up_available = (mb_row != 0);

	//parser
	for (mb_col = 0; mb_col < pc->mb_cols; mb_col++)
    {
		MACROBLOCKD* xdcol = &xd_row[mb_col];
		vp8_setup_block_dptrs(xdcol);

		//printf("mode = %d\n",xd->mode_info_context->mbmi.mode);

		if (xd->frame_type == KEY_FRAME  ||  pModeInfo->mbmi.ref_frame== INTRA_FRAME)
		{
			vp8_decode_intra_macroblock_parser(pbi, xd, xdcol, bc,pModeInfo);
		}
		else
		{
			vp8_decode_inter_macroblock_parser(pbi, xd, xdcol, bc,pModeInfo);
		}
		 ++pModeInfo;  /* next mb */
		xd->above_context++;
	}

	/*if(pbi->max_threads == 2)
		sem_post(pbi->h_event_start_dec_row[thread]);*/

	return 0;
}


int vp8_decode_mb_row_idctmc(VP8D_COMP *pbi,
                       VP8_COMMON *pc,
                       int mb_row,
                       MACROBLOCKD *xd,int thread)
{
    int recon_yoffset, recon_uvoffset;
    int mb_col,i;
    int ref_fb_idx = pc->lst_fb_idx;
    int dst_fb_idx = pc->new_fb_idx;
    int recon_y_stride = pc->yv12_fb[ref_fb_idx].y_stride;
    int recon_uv_stride = pc->yv12_fb[ref_fb_idx].uv_stride;
	MACROBLOCKD* xd_row = NULL;

	MODE_INFO *pModeInfo = xd->mode_info_context +  mb_row*(pc->mb_cols+1);

	if(thread ==0)
		 xd_row = pbi->mbrow;
	else
		xd_row = pbi->mbrow2;

	//SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)(&pc->left_context), 0, sizeof(pc->left_context));
    recon_yoffset = mb_row * recon_y_stride * 16;
    recon_uvoffset = mb_row * recon_uv_stride * 8;
    // reset above block coeffs

	xd->above_context = pc->above_context;
    xd->up_available = (mb_row != 0);

	//xd->mode_info_context -= pc->mb_cols;

	pbi->thread_mb_row[thread] = mb_row;
	for (mb_col = 0; mb_col < pc->mb_cols; mb_col++)
	{
		MACROBLOCKD* xdcol = &xd_row[mb_col];

		if(pbi->max_threads == 2)
		{
			volatile int * mby = &pbi->thread_mb_row[thread^1];
			volatile int * mbx = &pbi->thread_mb_col[thread^1];	
			while((mb_col >= *mbx )&& mb_row > *mby && mb_row>0)
			{
			} 
		}

		xd->dst.y_buffer = pc->yv12_fb[dst_fb_idx].y_buffer + recon_yoffset;
        xd->dst.u_buffer = pc->yv12_fb[dst_fb_idx].u_buffer + recon_uvoffset;
        xd->dst.v_buffer = pc->yv12_fb[dst_fb_idx].v_buffer + recon_uvoffset;

		xd->left_available = (mb_col != 0);
		if (pModeInfo->mbmi.ref_frame == LAST_FRAME)
            ref_fb_idx = pc->lst_fb_idx;
        else if (pModeInfo->mbmi.ref_frame == GOLDEN_FRAME)
            ref_fb_idx = pc->gld_fb_idx;
        else
            ref_fb_idx = pc->alt_fb_idx;

		xd->pre.y_buffer = pc->yv12_fb[ref_fb_idx].y_buffer + recon_yoffset;
        xd->pre.u_buffer = pc->yv12_fb[ref_fb_idx].u_buffer + recon_uvoffset;
        xd->pre.v_buffer = pc->yv12_fb[ref_fb_idx].v_buffer + recon_uvoffset;

		for (i = 0; i < 25; i++)
		{
			BLOCKD *b = &xd->block[i];
			BLOCKD *b2 = &xdcol->block[i];
			b->qcoeff = b2->qcoeff;
			xd->eob[i] = xdcol->eob[i];
		}

		if(xd->frame_type == KEY_FRAME  && mb_row == 0 && mb_col ==11)
			mb_row = 0;

		if (xd->frame_type == KEY_FRAME  ||  pModeInfo->mbmi.ref_frame== INTRA_FRAME)
		{
			vp8_decode_intra_macroblock_idctmc(pbi, xd, xdcol, pModeInfo);
		}
		else
		{						
			pbi->common.build_uvmvs(xd,pModeInfo);	
			vp8_decode_inter_macroblock_idctmc(pbi, xd, xdcol, pModeInfo);
		}

		recon_yoffset += 16;
        recon_uvoffset += 8;

        //++xd->mode_info_context;  /* next mb */
		++pModeInfo;

		if(mb_col == pc->mb_cols-1)
			vp8_extend_mb_row( &pc->yv12_fb[dst_fb_idx], xd->dst.y_buffer + 16, xd->dst.u_buffer + 8, xd->dst.v_buffer + 8 );

		pbi->thread_mb_col[thread]  = mb_col;
#if 1
		if(pbi->max_threads == 2 && mb_col == 0 ) 
			pbi->thread_mbrow_parser_ready[thread] = mb_row;//sem_post(pbi->h_event_start_dec_row[thread]);
#endif
	}
	//++xd->mode_info_context;
	//xd->mode_info_context += pc->mb_cols+1;
	pbi->thread_mb_col[thread] += 1;   
	return 0;
}

vpx_codec_err_t vp8_decode_key_frame(VP8D_COMP *pbi)
{
	vp8_reader *const bc = & pbi->bc;
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;

	MODE_INFO *mi ;

    const unsigned char *data = (const unsigned char *)pbi->Source;
    const unsigned char *const data_end = data + pbi->source_sz;
    int first_partition_length_in_bytes;

	const int Width = pc->Width;
    const int Height = pc->Height;

#ifdef STABILITY
	const int hs =  pc->horiz_scale;
	const int vs =  pc->vert_scale;
	int scaleX0, scaleX1,scaleY0, scaleY1;
	int PrescaleX0, PrescaleX1,PrescaleY0, PrescaleY1;
#endif

    int mb_row;
    int i, j, k, l;
    const int *const mb_feature_data_bits = vp8_mb_feature_data_bits;

	//int mb_row=-1;

	vpx_codec_err_t res;

	pc->version = (data[0] >> 1) & 7;
    pc->show_frame = (data[0] >> 4) & 1;
    first_partition_length_in_bytes = (data[0] | (data[1] << 8) | (data[2] << 16)) >> 5;
    data += 3;

#ifdef STABILITY
    if (data + first_partition_length_in_bytes > data_end)
	{
		return VPX_CODEC_CORRUPT_FRAME;
	}
#endif

	vp8_setup_version(pc);

	 if (data[0] != 0x9d || data[1] != 0x01 || data[2] != 0x2a)
			return VPX_CODEC_UNSUP_BITSTREAM;

    pc->Width = (data[3] | (data[4] << 8)) & 0x3fff;
    pc->horiz_scale = data[4] >> 6;
    pc->Height = (data[5] | (data[6] << 8)) & 0x3fff;
    pc->vert_scale = data[6] >> 6;
    data += 7;

#ifdef STABILITY	
	get_scaleparam(hs, &PrescaleX0, &PrescaleX1);
	get_scaleparam(vs, &PrescaleY0, &PrescaleY1);

	get_scaleparam( pc->horiz_scale,&scaleX0, &scaleX1);
	get_scaleparam( pc->vert_scale, &scaleY0, &scaleY1);

	if( pc->Width ==1072)
		 pc->Width= pc->Width;


	if(pc->Width>1920 || pc->Height>1080)
	{
		return VPX_CODEC_CORRUPT_FRAME;
	}
#endif

#ifdef STABILITY
	if(( ( ABS(pc->PreKF_Width - pc->Width*scaleX0/scaleX1) > 2) || ABS(pc->PreKF_Height - pc->Height*scaleY0/scaleY1) > 2 )		
		&& pc->PreKF_Width !=0 && pc->PreKF_Height !=0/*&& pbi->KfFlag  ==1*/  )
	{
		pc->PreKF_Width = pc->Width =0;
		pc->PreKF_Height = pc->Height =0;
		return VPX_CODEC_CORRUPT_FRAME;
	} 
#endif

	if (Width  != pc->Width  ||  Height != pc->Height)
    {
		int i,j;
#ifdef STABILITY
		if(ABS(Width*PrescaleX0/PrescaleX1 -pc->Width*scaleX0/scaleX1) >2 || ABS(Height*PrescaleY0/PrescaleY1 - pc->Height*scaleY0/scaleY1) > 2)
		{
			if(Width !=0 && Height!=0)
			{
				pc->Width =0;
				pc->Height = 0;
				return VPX_CODEC_CORRUPT_FRAME;  //stability
			}
		}
#endif

        if (pc->Width <= 0)
        {
            pc->Width = Width;
			return VPX_CODEC_CORRUPT_FRAME;
        }

        if (pc->Height <= 0)
        {
            pc->Height = Height;
			return VPX_CODEC_CORRUPT_FRAME;
        }

		pc->nCodecIdx = pbi->nCodecIdx;
	    pc->pUserData = pbi->pUserData;

        if (vp8_alloc_frame_buffers(pc, pc->Width, pc->Height))
			return VPX_CODEC_MEM_ERROR;

		mi = pc->mi;

		// Distance of Mb to the various image edges.
		 // These specified to 8th pel as they are always compared to values that are in 1/8th pel units
		for(i=0;i<pbi->common.mb_rows;i++)
		{ 			
			for(j=0;j<pbi->common.mb_cols;j++)
			{
				MB_MODE_INFO *mbmi = &mi->mbmi;
				mbmi->mb_to_left_edge = (-((j * 16) << 3))-(16 << 3);
				mbmi->mb_to_right_edge = (((pbi->common.mb_cols - 1 - j) * 16) << 3 )+(16 << 3);
				mbmi->mb_to_top_edge = (-((i * 16)) << 3 )-(16 << 3);
				mbmi->mb_to_bottom_edge = (((pbi->common.mb_rows - 1 - i) * 16) << 3)+(16 << 3);
				mi++;
			}
			mi++;
		}

#ifdef NEWTHREAD
		FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->mbrow);
		FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->mbrow2);

		pbi->mbrow = (MACROBLOCKD*)MallocMem(pbi->pUserData, pbi->nCodecIdx, ((pc->Width + 16 - (pc->Width & 0xf))>>4)*sizeof(MACROBLOCKD), CACHE_LINE);
		pbi->mbrow2 = (MACROBLOCKD*)MallocMem(pbi->pUserData, pbi->nCodecIdx, ((pc->Width + 16 - (pc->Width & 0xf))>>4)*sizeof(MACROBLOCKD), CACHE_LINE);

		for(i=0;i< ((pc->Width + 16 - (pc->Width & 0xf))>>4);i++)
		{
			memset(pbi->mbrow[i].qcoeff,0,sizeof(short)*400);
			memset(pbi->mbrow2[i].qcoeff,0,sizeof(short)*400);
		}
	
#endif

    }
	if (pc->Width == 0 || pc->Height == 0)
		return -1;
	init_key_frame(pbi);

	if (vp8dx_start_decode(bc, data, data_end - data))
		return VPX_CODEC_MEM_ERROR;

	pc->clr_type    = (YUV_TYPE)vp8_read_bit(bc);
	pc->clamp_type  = (CLAMP_TYPE)vp8_read_bit(bc);
	xd->segmentation_enabled = (unsigned char)vp8_read_bit(bc);

    if (xd->segmentation_enabled)
    {
        // Signal whether or not the segmentation map is being explicitly updated this frame.
        xd->update_mb_segmentation_map = (unsigned char)vp8_read_bit(bc);
        xd->update_mb_segmentation_data = (unsigned char)vp8_read_bit(bc);

        if (xd->update_mb_segmentation_data)
        {
            xd->mb_segement_abs_delta = (unsigned char)vp8_read_bit(bc);

			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)(xd->segment_feature_data), 0, sizeof(xd->segment_feature_data));

            // For each segmentation feature (Quant and loop filter level)
            for (i = 0; i < MB_LVL_MAX; i++)
            {
                for (j = 0; j < MAX_MB_SEGMENTS; j++)
                {
                    // Frame level data
                    if (vp8_read_bit(bc))
                    {
                        xd->segment_feature_data[i][j] = (signed char)vp8_read_literal(bc, mb_feature_data_bits[i]);

                        if (vp8_read_bit(bc))
                            xd->segment_feature_data[i][j] = -xd->segment_feature_data[i][j];
                    }
                    else
                        xd->segment_feature_data[i][j] = 0;
                }
            }
        }

        if (xd->update_mb_segmentation_map)
        {
            // Which macro block level features are enabled
			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)(xd->mb_segment_tree_probs),255, sizeof(xd->mb_segment_tree_probs));

            // Read the probs used to decode the segment id for each macro block.
            for (i = 0; i < MB_FEATURE_TREE_PROBS; i++)
            {
                // If not explicitly set value is defaulted to 255 by memset above
                if (vp8_read_bit(bc))
                    xd->mb_segment_tree_probs[i] = (unsigned char)vp8_read_literal(bc, 8);
            }
        }
    }

	pc->filter_type = (LOOPFILTERTYPE) vp8_read_bit(bc);
    pc->filter_level = vp8_read_literal(bc, 6);
    pc->sharpness_level = vp8_read_literal(bc, 3);

    // Read in loop filter deltas applied at the MB level based on mode or ref frame.
    xd->mode_ref_lf_delta_update = 0;
    xd->mode_ref_lf_delta_enabled = (unsigned char)vp8_read_bit(bc);

    if (xd->mode_ref_lf_delta_enabled)
    {
        // Do the deltas need to be updated
        xd->mode_ref_lf_delta_update = (unsigned char)vp8_read_bit(bc);

        if (xd->mode_ref_lf_delta_update)
        {
            // Send update
            for (i = 0; i < MAX_REF_LF_DELTAS; i++)
            {
                if (vp8_read_bit(bc))
                {
                    //sign = vp8_read_bit( bc );
                    xd->ref_lf_deltas[i] = (signed char)vp8_read_literal(bc, 6);
                    if (vp8_read_bit(bc))        // Apply sign
                        xd->ref_lf_deltas[i] = xd->ref_lf_deltas[i] * -1;
                }
            }

            // Send update
            for (i = 0; i < MAX_MODE_LF_DELTAS; i++)
            {
                if (vp8_read_bit(bc))
                {
                    //sign = vp8_read_bit( bc );
                    xd->mode_lf_deltas[i] = (signed char)vp8_read_literal(bc, 6);
                    if (vp8_read_bit(bc))        // Apply sign
                        xd->mode_lf_deltas[i] = xd->mode_lf_deltas[i] * -1;
                }
            }
        }
    }

	if((res = setup_token_decoder(pbi, data + first_partition_length_in_bytes))) 
	{		 
#ifdef STABILITY
		stop_token_decoder(pbi);
#endif
		return res;
	}
    xd->current_bc = &pbi->bc2;

    // Read the default quantizers.
    {
        int Q, q_update;

        Q = vp8_read_literal(bc, 7);  // AC 1st order Q = default
        pc->base_qindex = Q;
        q_update = 0;
        pc->y1dc_delta_q = get_delta_q(bc, pc->y1dc_delta_q, &q_update);
        pc->y2dc_delta_q = get_delta_q(bc, pc->y2dc_delta_q, &q_update);
        pc->y2ac_delta_q = get_delta_q(bc, pc->y2ac_delta_q, &q_update);
        pc->uvdc_delta_q = get_delta_q(bc, pc->uvdc_delta_q, &q_update);
        pc->uvac_delta_q = get_delta_q(bc, pc->uvac_delta_q, &q_update);

        if (q_update)
		{
            vp8cx_init_de_quantizer(pbi);
		}
        // MB level dequantizer setup
        mb_init_dequantizer(pbi, &pbi->mb,xd->mode_info_context);
    }

	pc->refresh_entropy_probs = vp8_read_bit(bc);
    if (pc->refresh_entropy_probs == 0)
    {
		CopyMem(pc->pUserData, pc->nCodecIdx,(VO_U8*)(&pc->lfc), (VO_U8*)(&pc->fc),  sizeof(pc->fc));
    }

    pc->refresh_last_frame = pc->frame_type == KEY_FRAME  ||  vp8_read_bit(bc);

    {
        // read coef probability tree
        for (i = 0; i < BLOCK_TYPES; i++)
            for (j = 0; j < COEF_BANDS; j++)
                for (k = 0; k < PREV_COEF_CONTEXTS; k++)
                    for (l = 0; l < MAX_ENTROPY_TOKENS - 1; l++)
                    {
                        unsigned char *const p = pc->fc.coef_probs [i][j][k] + l;

                        if (vp8_read(bc, vp8_coef_update_probs [i][j][k][l]))
                        {
                            *p = (unsigned char)vp8_read_literal(bc, 8);
                        }
                    }
    }

	xd->pre   =  pc->yv12_fb[pc->lst_fb_idx];
	xd->dst   =  pc->yv12_fb[pc->new_fb_idx];//zou 1230

    // set up frame new frame for intra coded blocks
    vp8_setup_intra_recon(&pc->yv12_fb[pc->new_fb_idx]);

    vp8_setup_block_dptrs(xd);
	vp8_build_block_doffsets(xd);

    // Read the mb_no_coeff_skip flag
    pc->mb_no_coeff_skip = (int)vp8_read_bit(bc);

    vp8_kfread_modes(pbi);

	SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)pc->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES) * pc->mb_cols);

	if(pbi->max_threads == 2)
	{
		MACROBLOCKD*xd2 = &pbi->mb2;
		memcpy(xd2,xd,sizeof(MACROBLOCKD));
		//xd2->mode_info_context

		vp8_setup_block_dptrs(xd2);
		vp8_build_block_doffsets(xd2);
		pbi->thread_mb_row[0] = 0;
        pbi->thread_mb_col[0] = 0;
		pbi->thread_mb_row[1] = 0;
        pbi->thread_mb_col[1] = 0;
		pbi->thread_mbrow_parser_ready[0]= -2;
		pbi->thread_mbrow_parser_ready[1] = -1;
		pbi->isloopfilter = 0;
		sem_post(pbi->h_event_startframe);
		//sem_post(pbi->h_event_start_dec_row[1]);
	}

	if(pbi->max_threads  == 2)
	{
        int num_part = 1 << pc->multi_token_partition;
		BOOL_DECODER *bc = xd->current_bc;

        for (mb_row = 0; mb_row < pc->mb_rows; mb_row+=2)
        { 
			volatile int *mby = &pbi->thread_mbrow_parser_ready[1];

			while( mb_row-1 != *mby  )
			{
				;
			} 

			//if (sem_wait(pbi->h_event_start_dec_row[1]) == 0)
			{	
				if(num_part > 1)
					bc = & pbi->mbc[mb_row%num_part];
				if(vp8_decode_mb_row_parser(pbi, pc, mb_row, xd, 0,bc)!=VPX_CODEC_OK) //key
				{
					stop_token_decoder(pbi);
					return VPX_CODEC_CORRUPT_FRAME;
				}

				if(vp8_decode_mb_row_idctmc(pbi, pc, mb_row, xd, 0)!=VPX_CODEC_OK)
				{
					return VPX_CODEC_CORRUPT_FRAME;
				}
			}
        }

		if(pbi->max_threads == 2)
			sem_wait(pbi->h_event_main);

		while(!sem_trywait(pbi->h_event_main));
		while(!sem_trywait(pbi->h_event_startframe));
	}
	else
	{
#if 1
		int ibc = 0;
        int num_part = 1 << pc->multi_token_partition;
		BOOL_DECODER *bc = NULL;

        // Decode the individual macro block
        for (mb_row = 0; mb_row < pc->mb_rows; mb_row++)
        {
            if (num_part > 1)
            {
                bc = xd->current_bc = & pbi->mbc[ibc];
                ibc++;		

                if (ibc == num_part)
                    ibc = 0;
            }

            if(vp8_decode_mb_row(pbi, pc, mb_row, xd)!=VPX_CODEC_OK)
			{
				stop_token_decoder(pbi);
				return VPX_CODEC_CORRUPT_FRAME;
			}
        }	
#endif
	}

    stop_token_decoder(pbi);

    if (pc->refresh_entropy_probs == 0)
		CopyMem(pc->pUserData, pc->nCodecIdx,(VO_U8*)&pc->fc, (VO_U8*)&pc->lfc,  sizeof(pc->fc));

#ifdef STABILITY
	pbi->KfFlag  =1;
	pc->PreKF_Width = pc->Width*scaleX0/scaleX1 ;
    pc->PreKF_Height = pc->Height*scaleY0/scaleY1;
#endif
    return VPX_CODEC_OK;
}

vpx_codec_err_t vp8_decode_inter_frame(VP8D_COMP *pbi)
{
    vp8_reader *const bc = & pbi->bc;
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;
    const unsigned char *data = (const unsigned char *)pbi->Source;
    const unsigned char *const data_end = data + pbi->source_sz;
    int first_partition_length_in_bytes;

    int mb_row;
    int i, j, k, l;
    const int *const mb_feature_data_bits = vp8_mb_feature_data_bits;
	vpx_codec_err_t res;

    pc->version = (data[0] >> 1) & 7;
    pc->show_frame = (data[0] >> 4) & 1;
    first_partition_length_in_bytes = (data[0] | (data[1] << 8) | (data[2] << 16)) >> 5;
    data += 3;

#ifdef STABILITY
    if (data + first_partition_length_in_bytes > data_end)
	{
		return VPX_CODEC_CORRUPT_FRAME;
	}
#endif

    vp8_setup_version(pc);

#ifdef STABILITY
    if (pc->Width == 0 || pc->Height == 0)
	{
        return VPX_CODEC_CORRUPT_FRAME; //stability
	}

	 if (pc->Width >1920 || pc->Height >1080)
	 {
        return VPX_CODEC_CORRUPT_FRAME; //stability
	 }
#endif

    init_inter_frame(pbi);

    if (vp8dx_start_decode(bc, data, data_end - data))
		return VPX_CODEC_MEM_ERROR;

    // Is segmentation enabled
    xd->segmentation_enabled = (unsigned char)vp8_read_bit(bc);

    if (xd->segmentation_enabled)
    {
        // Signal whether or not the segmentation map is being explicitly updated this frame.
        xd->update_mb_segmentation_map = (unsigned char)vp8_read_bit(bc);
        xd->update_mb_segmentation_data = (unsigned char)vp8_read_bit(bc);

        if (xd->update_mb_segmentation_data)
        {
            xd->mb_segement_abs_delta = (unsigned char)vp8_read_bit(bc);
            // For each segmentation feature (Quant and loop filter level)
            for (i = 0; i < MB_LVL_MAX; i++)
            {
                for (j = 0; j < MAX_MB_SEGMENTS; j++)
                {                    
                    if (vp8_read_bit(bc))// Frame level data
                    {
                        xd->segment_feature_data[i][j] = (signed char)vp8_read_literal(bc, mb_feature_data_bits[i]);
                        if (vp8_read_bit(bc))
                            xd->segment_feature_data[i][j] = -xd->segment_feature_data[i][j];
                    }
                    else
                        xd->segment_feature_data[i][j] = 0;
                }
            }
        }

        if (xd->update_mb_segmentation_map)
        {
            // Which macro block level features are enabled
			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->mb_segment_tree_probs,255,  sizeof(xd->mb_segment_tree_probs));
            // Read the probs used to decode the segment id for each macro block.
            for (i = 0; i < MB_FEATURE_TREE_PROBS; i++)
            {
                // If not explicitly set value is defaulted to 255 by memset above
                if (vp8_read_bit(bc))
                    xd->mb_segment_tree_probs[i] = (unsigned char)vp8_read_literal(bc, 8);
            }
        }
    }

    // Read the loop filter level and type
    pc->filter_type = (LOOPFILTERTYPE) vp8_read_bit(bc);
    pc->filter_level = vp8_read_literal(bc, 6);

    pc->sharpness_level = vp8_read_literal(bc, 3);

    // Read in loop filter deltas applied at the MB level based on mode or ref frame.
    xd->mode_ref_lf_delta_update = 0;
    xd->mode_ref_lf_delta_enabled = (unsigned char)vp8_read_bit(bc);

    if (xd->mode_ref_lf_delta_enabled)
    {       
        xd->mode_ref_lf_delta_update = (unsigned char)vp8_read_bit(bc); // Do the deltas need to be updated
        if (xd->mode_ref_lf_delta_update)
        {
            for (i = 0; i < MAX_REF_LF_DELTAS; i++) // Send update
            {
                if (vp8_read_bit(bc))
                {
                    //sign = vp8_read_bit( bc );
                    xd->ref_lf_deltas[i] = (signed char)vp8_read_literal(bc, 6);
                    if (vp8_read_bit(bc))        // Apply sign
                        xd->ref_lf_deltas[i] = xd->ref_lf_deltas[i] * -1;
                }
            }

            // Send update
            for (i = 0; i < MAX_MODE_LF_DELTAS; i++)
            {
                if (vp8_read_bit(bc))
                {
                    //sign = vp8_read_bit( bc );
                    xd->mode_lf_deltas[i] = (signed char)vp8_read_literal(bc, 6);
                    if (vp8_read_bit(bc))        // Apply sign
                        xd->mode_lf_deltas[i] = xd->mode_lf_deltas[i] * -1;
                }
            }
        }
    }

	if((res = setup_token_decoder(pbi, data + first_partition_length_in_bytes))) 
	{
#ifdef STABILITY
		 stop_token_decoder(pbi);
#endif
		return res;
	}
    xd->current_bc = &pbi->bc2;

    // Read the default quantizers.
    {
        int Q, q_update;

        Q = vp8_read_literal(bc, 7);  // AC 1st order Q = default
        pc->base_qindex = Q;
        q_update = 0;
        pc->y1dc_delta_q = get_delta_q(bc, pc->y1dc_delta_q, &q_update);
        pc->y2dc_delta_q = get_delta_q(bc, pc->y2dc_delta_q, &q_update);
        pc->y2ac_delta_q = get_delta_q(bc, pc->y2ac_delta_q, &q_update);
        pc->uvdc_delta_q = get_delta_q(bc, pc->uvdc_delta_q, &q_update);
        pc->uvac_delta_q = get_delta_q(bc, pc->uvac_delta_q, &q_update);

        if (q_update)
            vp8cx_init_de_quantizer(pbi);
        // MB level dequantizer setup
        mb_init_dequantizer(pbi, &pbi->mb,xd->mode_info_context);
    }

    // Determine if the golden frame or ARF buffer should be updated and how.
    // For all non key frames the GF and ARF refresh flags and sign bias
    // flags must be set explicitly.
    //if (pc->frame_type != KEY_FRAME)
    {
        // Should the GF or ARF be updated from the current frame
        pc->refresh_golden_frame = vp8_read_bit(bc);
        pc->refresh_alt_ref_frame = vp8_read_bit(bc);

        // Buffer to buffer copy flags.
        pc->copy_buffer_to_gf = 0;

        if (!pc->refresh_golden_frame)
            pc->copy_buffer_to_gf = vp8_read_literal(bc, 2);

        pc->copy_buffer_to_arf = 0;

        if (!pc->refresh_alt_ref_frame)
            pc->copy_buffer_to_arf = vp8_read_literal(bc, 2);

        pc->ref_frame_sign_bias[GOLDEN_FRAME] = vp8_read_bit(bc);
        pc->ref_frame_sign_bias[ALTREF_FRAME] = vp8_read_bit(bc);
    }

    pc->refresh_entropy_probs = vp8_read_bit(bc);

    if (pc->refresh_entropy_probs == 0)
		CopyMem(pc->pUserData, pc->nCodecIdx,(VO_U8*)(&pc->lfc), (VO_U8*)(&pc->fc),  sizeof(pc->fc));

    pc->refresh_last_frame = pc->frame_type == KEY_FRAME  ||  vp8_read_bit(bc);

	//printf("%d %d %d \n", pc->refresh_golden_frame,pc->refresh_alt_ref_frame,pc->refresh_last_frame);
    {
        // read coef probability tree
        for (i = 0; i < BLOCK_TYPES; i++)
            for (j = 0; j < COEF_BANDS; j++)
                for (k = 0; k < PREV_COEF_CONTEXTS; k++)
                    for (l = 0; l < MAX_ENTROPY_TOKENS - 1; l++)
                    {
                        unsigned char *const p = pc->fc.coef_probs [i][j][k] + l;

                        if (vp8_read(bc, vp8_coef_update_probs [i][j][k][l]))
                        {
                            *p = (unsigned char)vp8_read_literal(bc, 8);
                        }
                    }
    }

	//if ( pc->multi_token_partition == ONE_PARTITION || !(pc->filter_level)) //zou 1230 error
		vp8_setup_intra_recon(&pc->yv12_fb[pc->new_fb_idx]);
    // Read the mb_no_coeff_skip flag
    pc->mb_no_coeff_skip = (int)vp8_read_bit(bc);

#if 1
	{
		vp8_decode_mode_mvs_new(pbi);
#if 0
		mb_row =-1;
		while (++mb_row < pc->mb_rows)
			decoder_mode_mvs_mbrow(pbi,mb_row);
#endif
	}
#endif
	
	SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)pc->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES) * pc->mb_cols);

	if(pbi->max_threads == 2 )
	{
		MACROBLOCKD*xd2 = &pbi->mb2;
		memcpy(xd2,xd,sizeof(MACROBLOCKD));

		vp8_setup_block_dptrs(xd2);
		vp8_build_block_doffsets(xd2);
		pbi->thread_mb_row[0] = 0;
        pbi->thread_mb_col[0] = 0;
		pbi->thread_mb_row[1] = 0;
        pbi->thread_mb_col[1] = 0;
		pbi->thread_mbrow_parser_ready[0] = -2;
		pbi->thread_mbrow_parser_ready[1] = -1;
		pbi->isloopfilter = 0;
		sem_post(pbi->h_event_startframe);
		//sem_post(pbi->h_event_start_dec_row[1]);
	}

	if(pbi->max_threads == 2)
	{
		int num_part = 1 << pc->multi_token_partition;
		BOOL_DECODER *bc = xd->current_bc;

		for (mb_row = 0; mb_row < pc->mb_rows; mb_row+=2)
		{
			volatile int *mby = &pbi->thread_mbrow_parser_ready[1];
			while( mb_row-1 != *mby  )
			{
				//thread_sleep(0);
			} 

			//if (sem_wait(pbi->h_event_start_dec_row[1]) == 0)
			{
				if(num_part>1)
					bc = & pbi->mbc[mb_row%num_part];		

				decoder_mode_mvs_mbrow(pbi,mb_row);

				if(vp8_decode_mb_row_parser(pbi, pc, mb_row, xd, 0,bc)!=VPX_CODEC_OK) //INTER
				{
					stop_token_decoder(pbi);
					return VPX_CODEC_CORRUPT_FRAME;
				}
				//pbi->thread_mbrow_parser_ready[0] = mb_row;				

				if(vp8_decode_mb_row_idctmc(pbi, pc, mb_row, xd, 0)!=VPX_CODEC_OK)
				{
					return VPX_CODEC_CORRUPT_FRAME;
				}
			}
		}
		if(pbi->max_threads == 2)
			sem_wait(pbi->h_event_main);

		while(!sem_trywait(pbi->h_event_main)) ;
		while(!sem_trywait(pbi->h_event_startframe));
        //while(!sem_trywait(pbi->h_event_start_dec_row[0]));
        //while(!sem_trywait(pbi->h_event_start_dec_row[1]));
	}
	else
    {
#if 1
		int ibc = 0;
        int num_part = 1 << pc->multi_token_partition;
        // Decode the individual macro block
        for (mb_row = 0; mb_row < pc->mb_rows; mb_row++)
        {
            if (num_part > 1)
            {
                xd->current_bc = & pbi->mbc[ibc];
                ibc++;
                if (ibc == num_part)
                    ibc = 0;
            }
			decoder_mode_mvs_mbrow(pbi,mb_row);

            if(vp8_decode_mb_row(pbi, pc, mb_row, xd)!=VPX_CODEC_OK)
			{
				stop_token_decoder(pbi);
				return VPX_CODEC_CORRUPT_FRAME;
			}
        }
#endif
    }

    stop_token_decoder(pbi);
    if (pc->refresh_entropy_probs == 0)
		CopyMem(pc->pUserData, pc->nCodecIdx,(VO_U8*)(&pc->fc), (VO_U8*)(&pc->lfc),  sizeof(pc->fc));

    return VPX_CODEC_OK;
}   

vpx_codec_err_t vp8_decode_frame(VP8D_COMP *pbi)
{
    //vp8_reader *const bc = & pbi->bc;
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;
    const unsigned char *data = (const unsigned char *)pbi->Source;
    //const unsigned char *const data_end = data + pbi->source_sz;
    //const int *const mb_feature_data_bits = vp8_mb_feature_data_bits;
	//static int z=0,z1=0;

	vpx_codec_err_t res;

    pc->frame_type = (FRAME_TYPE)(data[0] & 1);

	pbi->mb.pUserData = pbi->pUserData;
	pbi->mb.nCodecIdx = pbi->nCodecIdx;

	SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->qcoeff, 0, sizeof(xd->qcoeff));

	if (pc->frame_type == KEY_FRAME)
	{
		res =vp8_decode_key_frame(pbi);	

#ifdef STABILITY
		if(res != VPX_CODEC_OK)
			pbi->KfFlag =0;
#endif
	}
	else
	{
#ifdef STABILITY
		if(pbi->KfFlag==0)
		{
			return VPX_CODEC_CORRUPT_FRAME;
		}
#endif	

		res =vp8_decode_inter_frame(pbi);


	}

	return res;
}