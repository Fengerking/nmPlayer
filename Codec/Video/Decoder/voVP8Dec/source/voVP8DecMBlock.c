#include "voVP8DecMBlock.h"
#include "voVP8DecFrame.h"
#include "voVP8DecRecon.h"
#include "voVP8Memory.h"

extern int vp8_decode_mb_tokens(VP8D_COMP *dx, MACROBLOCKD *x,MODE_INFO* pModeInfo);
extern int vp8_decode_mb_tokens_new(VP8D_COMP *dx, MACROBLOCKD *x,MACROBLOCKD* xdcol,BOOL_DECODER *bc,MODE_INFO *pModeInfo);

const int vp8_mb_feature_data_bits[MB_LVL_MAX] = {7, 6};

typedef enum
{
    PRED = 0,
    DEST = 1,
} BLOCKSET;

void vp8_setup_block
(
    BLOCKD *b,
    int mv_stride,
    unsigned char **base,
    int Stride,
    int offset,
    BLOCKSET bs
)
{

    if (bs == DEST)
    {
        b->dst_stride = Stride;
        b->dst = offset;
        b->base_dst = base;
    }
    else
    {
        b->pre_stride = Stride;
        b->pre = offset;
        b->base_pre = base;
    }

}

void vp8_setup_macroblock(MACROBLOCKD *x, BLOCKSET bs)
{
    int block;

    unsigned char **y, **u, **v;

    if (bs == DEST)
    {
        y = &x->dst.y_buffer;
        u = &x->dst.u_buffer;
        v = &x->dst.v_buffer;
    }
    else
    {
        y = &x->pre.y_buffer;
        u = &x->pre.u_buffer;
        v = &x->pre.v_buffer;
    }

    for (block = 0; block < 16; block++) // y blocks
    {
        vp8_setup_block(&x->block[block], x->dst.y_stride, y, x->dst.y_stride,
                        (block >> 2) * 4 * x->dst.y_stride + (block & 3) * 4, bs);
    }

    for (block = 16; block < 20; block++) // U and V blocks
    {
        vp8_setup_block(&x->block[block], x->dst.uv_stride, u, x->dst.uv_stride,
                        ((block - 16) >> 1) * 4 * x->dst.uv_stride + (block & 1) * 4, bs);

        vp8_setup_block(&x->block[block+4], x->dst.uv_stride, v, x->dst.uv_stride,
                        ((block - 16) >> 1) * 4 * x->dst.uv_stride + (block & 1) * 4, bs);
    }
}

void vp8_setup_block_dptrs(MACROBLOCKD *x)
{
    int r, c;

    for (r = 0; r < 4; r++)
    {
        for (c = 0; c < 4; c++)
        {
           // x->block[r*4+c].diff      = &x->diff[r * 4 * 16 + c * 4];
            x->block[r*4+c].predictor = x->predictor + r * 4 * 16 + c * 4;
        }
    }

    for (r = 0; r < 2; r++)
    {
        for (c = 0; c < 2; c++)
        {
            //x->block[16+r*2+c].diff      = &x->diff[256 + r * 4 * 8 + c * 4];
            x->block[16+r*2+c].predictor = x->predictor + 256 + r * 4 * 8 + c * 4;

        }
    }

    for (r = 0; r < 2; r++)
    {
        for (c = 0; c < 2; c++)
        {
            //x->block[20+r*2+c].diff      = &x->diff[320+ r * 4 * 8 + c * 4];
            x->block[20+r*2+c].predictor = x->predictor + 320 + r * 4 * 8 + c * 4;

        }
    }

    //x->block[24].diff = &x->diff[384];

    for (r = 0; r < 25; r++)
    {
        x->block[r].qcoeff  = x->qcoeff  + r * 16;
    }
}

void vp8_build_block_doffsets(MACROBLOCKD *x)
{
    // handle the destination pitch features
    vp8_setup_macroblock(x, DEST);
    vp8_setup_macroblock(x, PRED);
}
int vp8_decode_intra_macroblock(VP8D_COMP *pbi, MACROBLOCKD *xd,MODE_INFO *pModeInfo)
{
	int eobtotal = 0,i;
	if (pModeInfo->mbmi.mb_skip_coeff)
	{
		if((pModeInfo-1)->mbmi.dc_diff)
		{
		 if (pModeInfo->mbmi.mode != B_PRED )
			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
		else
			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}

		if((pModeInfo-xd->mode_info_stride)->mbmi.dc_diff)
		{
			if (pModeInfo->mbmi.mode != B_PRED )
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
			else
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}		
	}
    else
	{		
        eobtotal = vp8_decode_mb_tokens(pbi, xd,pModeInfo);
		if(eobtotal<0)
			return -1;
	}
	pModeInfo->mbmi.dc_diff = 1;

	if (pModeInfo->mbmi.mode != B_PRED && eobtotal == 0)
    {
		pModeInfo->mbmi.dc_diff = 0;		
        vp8_build_intra_predictors_mbuv(xd,xd->dst.u_buffer,xd->dst.v_buffer,xd->dst.uv_stride,pModeInfo); 
        vp8_build_intra_predictors_mby_s(xd,xd->dst.y_buffer,xd->dst.y_stride,pModeInfo);
        return 0;
	}

	if (xd->segmentation_enabled)
        mb_init_dequantizer(pbi, xd,pModeInfo);	

	// do prediction  
	vp8_build_intra_predictors_mbuv(xd,&xd->predictor[256],&xd->predictor[320],8,pModeInfo); 

	if (pModeInfo->mbmi.mode != B_PRED)
    {
        BLOCKD *b = &xd->block[24];

		vp8_build_intra_predictors_mby(xd,xd->predictor,16,pModeInfo);

		vp8_dequantize_b(b,xd->dq_y2);
		if(xd->eob[24]>1)
		{
			vp8_short_inv_walsh4x4(b->qcoeff, b->qcoeff);
		}
        else
		{
			vp8_short_inv_walsh4x4_1(b->qcoeff, b->qcoeff);
		}

		vp8_dc_idct_add_y_block(xd,xd->dq_y);
    }
    else 
    {
		vp8_intra_prediction_down_copy(xd);  //Y
        for (i = 0; i < 16; i++)
        {
            BLOCKD *b = &xd->block[i];
            vp8_predict_intra4x4(b, pModeInfo->bmi[i].mode, b->predictor);		
			if(xd->eob[i]>1)
				vp8_dequant_idct_add(b->qcoeff, xd->dq_y,  b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
            else
			{
				vp8_dc_only_idct_add(b->qcoeff[0] * (*xd->dq_y), b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
				((int *)b->qcoeff)[0] = 0;
			}
        }
    }
	vp8_idct_add_uv_block(xd,xd->dq_uv);

	return 0;
}

int vp8_decode_intra_macroblock_parser(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD* xdcol,BOOL_DECODER *bc,MODE_INFO *pModeInfo)
{
	//pbi->mbrow[mb_col].eobtotal  = 0;
	xdcol->eobtotal = 0;
	if (pModeInfo->mbmi.mb_skip_coeff)
	{
#if 1
		if((pModeInfo-1)->mbmi.dc_diff)
		{
		 if (pModeInfo->mbmi.mode != B_PRED )
			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
		else
			SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}

		if((pModeInfo-xd->mode_info_stride)->mbmi.dc_diff)
		{
			if (pModeInfo->mbmi.mode != B_PRED )
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
			else
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}
#endif
	}
    else
	{		
        xdcol->eobtotal = vp8_decode_mb_tokens_new(pbi, xd,xdcol,bc,pModeInfo);//vp8_decode_mb_tokens_new(pbi, xd);
		if(xdcol->eobtotal<0)
			return -1;
	}
	pModeInfo->mbmi.dc_diff = 1;
	return 0;
}

int vp8_decode_intra_macroblock_idctmc(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD *xdcol,MODE_INFO *pModeInfo)
{
	int i=0;
	int eobtotal = xdcol->eobtotal;//pbi->mbrow[mb_col].eobtotal;

	if (pModeInfo->mbmi.mode != B_PRED && eobtotal == 0)
    {
		pModeInfo->mbmi.dc_diff = 0;		
        vp8_build_intra_predictors_mbuv(xd,xd->dst.u_buffer,xd->dst.v_buffer,xd->dst.uv_stride,pModeInfo); 
        vp8_build_intra_predictors_mby_s(xd,xd->dst.y_buffer,xd->dst.y_stride,pModeInfo);
        return 0;
	}

	if (xd->segmentation_enabled)
        mb_init_dequantizer(pbi, xd,pModeInfo);	

	// do prediction   
	vp8_build_intra_predictors_mbuv(xd,&xd->predictor[256],&xd->predictor[320],8,pModeInfo); 


	if (pModeInfo->mbmi.mode != B_PRED)
    {
        BLOCKD *b = &xd->block[24];

		vp8_build_intra_predictors_mby(xd,xd->predictor,16,pModeInfo);

		vp8_dequantize_b(b,xd->dq_y2);
		if(xd->eob[24]>1)
		{
			vp8_short_inv_walsh4x4(b->qcoeff, b->qcoeff);
		}
        else
		{
			vp8_short_inv_walsh4x4_1(b->qcoeff, b->qcoeff);
		}

		vp8_dc_idct_add_y_block(xd,xd->dq_y);
    }
    else 
    {
		vp8_intra_prediction_down_copy(xd);  //Y
        for (i = 0; i < 16; i++)
        {
            BLOCKD *b = &xd->block[i];

            vp8_predict_intra4x4(b, pModeInfo->bmi[i].mode, b->predictor);		
			if(xd->eob[i]>1)
				vp8_dequant_idct_add(b->qcoeff, xd->dq_y,  b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
            else
			{
				vp8_dc_only_idct_add(b->qcoeff[0] * (*xd->dq_y), b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
				((int *)b->qcoeff)[0] = 0;
			}
        }
    }
	
	vp8_idct_add_uv_block(xd,xd->dq_uv);

	return 0;
}

int vp8_decode_inter_macroblock(VP8D_COMP *pbi, MACROBLOCKD *xd,MODE_INFO *pModeInfo)
{
	int eobtotal = 0;
	BLOCKD *b;
    pModeInfo->mbmi.dc_diff = 1;

	// set idct coeff to the default vaule : 0
	if (pModeInfo->mbmi.mb_skip_coeff)
	{
		if((pModeInfo-1)->mbmi.dc_diff)
		{
			 if (pModeInfo->mbmi.mode != SPLITMV)
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
			else
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}

		if((pModeInfo-xd->mode_info_stride)->mbmi.dc_diff)
		{
			if ( pModeInfo->mbmi.mode != SPLITMV)
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
			else
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}
	}
	else
	{
		eobtotal = vp8_decode_mb_tokens(pbi, xd,pModeInfo);	
		if(eobtotal<0)
			return -1;
	}

	if (xd->segmentation_enabled && eobtotal!=0) // zou 1230 ?
		mb_init_dequantizer(pbi, xd,pModeInfo);

	if (pModeInfo->mbmi.mode != SPLITMV)
	{	
		if ( eobtotal == 0)
		{
			pModeInfo->mbmi.dc_diff = 0;
			vp8_build_inter_predictors_nosplitmv_mb(xd,xd->dst.y_buffer,xd->dst.u_buffer,xd->dst.v_buffer,xd->dst.y_stride,xd->dst.uv_stride,pModeInfo);
			return 0;
		}
		// do prediction 
		 vp8_build_inter_predictors_nosplitmv_mb(xd,xd->predictor,&xd->predictor[256],&xd->predictor[320],16,8,pModeInfo);		 
		 if(xd->eob[24]>0)
		 {
			 b = &xd->block[24];
			 vp8_dequantize_b(b,xd->dq_y2);
			 // do 2nd order transform on the dc block
			 if(xd->eob[24]>1)
			 {
				 vp8_short_inv_walsh4x4(b->qcoeff, b->qcoeff);
			 }
			 else
			 {
				 vp8_short_inv_walsh4x4_1(b->qcoeff, b->qcoeff);
			 }
		 }
		 vp8_dc_idct_add_y_block	(xd,xd->dq_y);
		 vp8_idct_add_uv_block	(xd,xd->dq_uv);
	}
	else  //10k
	{
		if(eobtotal==0)
		{			
			vp8_build_inter_predictors_splitmv_mb_s(xd,pModeInfo);
			return 0;
		}
		// do prediction 
		vp8_build_inter_predictors_splitmv_mb(xd,pModeInfo);
		vp8_idct_add_y_block(xd,xd->dq_y);
		vp8_idct_add_uv_block(xd,xd->dq_uv);
	}	

	return 0;
}
int vp8_decode_inter_macroblock_parser(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD* xdcol,BOOL_DECODER *bc,MODE_INFO *pModeInfo)
{
	//pbi->mbrow[mb_col].eobtotal = 0;
	xdcol->eobtotal = 0;

	// set idct coeff to the default vaule : 0
	if (pModeInfo->mbmi.mb_skip_coeff)
	{
#if 1
		if((pModeInfo-1)->mbmi.dc_diff)
		{
			 if (pModeInfo->mbmi.mode != SPLITMV)
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
			else
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}

		if((pModeInfo-xd->mode_info_stride)->mbmi.dc_diff)
		{
			if ( pModeInfo->mbmi.mode != SPLITMV)
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));
			else
				SetMem(pbi->pUserData, pbi->nCodecIdx, (VO_U8 *)xd->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES)-1);
		}
#endif
	}
	else
	{
		xdcol->eobtotal = vp8_decode_mb_tokens_new(pbi, xd,xdcol,bc,pModeInfo);	
		if(xdcol->eobtotal<0)
			return -1;
	}
	pModeInfo->mbmi.dc_diff = 1;

	return 0;
}
int vp8_decode_inter_macroblock_idctmc(VP8D_COMP *pbi, MACROBLOCKD *xd,MACROBLOCKD *xdcol,MODE_INFO *pModeInfo)
{
	BLOCKD *b;    
	int eobtotal = xdcol->eobtotal;//pbi->mbrow[mb_col].eobtotal;

	if (xd->segmentation_enabled && eobtotal!=0) // zou 1230 ?
		mb_init_dequantizer(pbi, xd,pModeInfo);

	if (pModeInfo->mbmi.mode != SPLITMV)
	{	
		if ( eobtotal == 0)
		{
			pModeInfo->mbmi.dc_diff = 0;
			vp8_build_inter_predictors_nosplitmv_mb(xd,xd->dst.y_buffer,xd->dst.u_buffer,xd->dst.v_buffer,xd->dst.y_stride,xd->dst.uv_stride,pModeInfo);
			return 0;
		}
		// do prediction 
		 vp8_build_inter_predictors_nosplitmv_mb(xd,xd->predictor,&xd->predictor[256],&xd->predictor[320],16,8,pModeInfo);		 
		 if(xd->eob[24]>0)
		 {
			 b = &xd->block[24];
			 vp8_dequantize_b(b,xd->dq_y2);
			 // do 2nd order transform on the dc block
			 if(xd->eob[24]>1)
			 {
				 vp8_short_inv_walsh4x4(b->qcoeff, b->qcoeff);
			 }
			 else
			 {
				 vp8_short_inv_walsh4x4_1(b->qcoeff, b->qcoeff);
			 }
		 }
		 vp8_dc_idct_add_y_block	(xd,xd->dq_y);
		 vp8_idct_add_uv_block	(xd,xd->dq_uv);
	}
	else  //10k
	{
		if(eobtotal==0)
		{			
			vp8_build_inter_predictors_splitmv_mb_s(xd,pModeInfo);
			return 0;
		}
		// do prediction 
		vp8_build_inter_predictors_splitmv_mb(xd,pModeInfo);
		vp8_idct_add_y_block(xd,xd->dq_y);
		vp8_idct_add_uv_block(xd,xd->dq_uv);
	}	

	return 0;
}
//#endif



