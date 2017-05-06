#include "voVP8Memory.h"
#include "voVP8DecRecon.h"

void vp8_setup_intra_recon(YV12_BUFFER_CONFIG *ybf)
{
    int i;
    // set up frame new frame for intra coded blocks
	SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)(ybf->y_buffer - 1 - 2 * ybf->y_stride), 127, ybf->y_width + 5);
	SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)(ybf->y_buffer - 1 - ybf->y_stride), 127, ybf->y_width + 5);

    for (i = 0; i < ybf->y_height; i++)
        ybf->y_buffer[ybf->y_stride *i - 1] = (unsigned char) 129;

	SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)(ybf->u_buffer - 1 - 2 * ybf->uv_stride), 127, ybf->uv_width + 5);
	SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)(ybf->u_buffer - 1 - ybf->uv_stride), 127, ybf->uv_width + 5);

    for (i = 0; i < ybf->uv_height; i++)
        ybf->u_buffer[ybf->uv_stride *i - 1] = (unsigned char) 129;

	SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)(ybf->v_buffer - 1 - 2 * ybf->uv_stride), 127, ybf->uv_width + 5);
	SetMem(ybf->pUserData, ybf->nCodecIdx, (VO_U8 *)(ybf->v_buffer - 1 - ybf->uv_stride), 127, ybf->uv_width + 5);

    for (i = 0; i < ybf->uv_height; i++)
        ybf->v_buffer[ybf->uv_stride *i - 1] = (unsigned char) 129;

}

void vp8_predict_intra4x4(BLOCKD *x,
                          int b_mode,
                          unsigned char *predictor)
{
#ifndef VOARMV7
    int i, r, c;
#endif

    unsigned char *Above = *(x->base_dst) + x->dst - x->dst_stride;
    unsigned char Left[4];
    unsigned char top_left = Above[-1];

    Left[0] = (*(x->base_dst))[x->dst - 1];
    Left[1] = (*(x->base_dst))[x->dst - 1 + x->dst_stride];
    Left[2] = (*(x->base_dst))[x->dst - 1 + 2 * x->dst_stride];
    Left[3] = (*(x->base_dst))[x->dst - 1 + 3 * x->dst_stride];

    switch (b_mode)
    {
    case B_DC_PRED:
    {
#ifdef VOARMV7
		vo_B_DC_PRED_neon(Above,Left,predictor);
#else
        int expected_dc = 0;

        for (i = 0; i < 4; i++)
        {
            expected_dc += Above[i];
            expected_dc += Left[i];
        }

        expected_dc = (expected_dc + 4) >> 3;

        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                predictor[c] = expected_dc;
            }

            predictor += 16;
        }
#endif
    }
    break;
    case B_TM_PRED:
    {
        // prediction similar to true_motion prediction
#ifdef VOARMV7
		vo_B_TM_PRED_neon(Above,Left,top_left,predictor);
#else
        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                int pred = Above[c] - top_left + Left[r];

                if (pred < 0)
                    pred = 0;

                if (pred > 255)
                    pred = 255;

                predictor[c] = pred;
            }

            predictor += 16;
        }
#endif
    }
    break;

    case B_VE_PRED:
    {
#ifdef VOARMV7
		vo_B_VE_PRED_neon(Above,top_left,predictor);
#else
        unsigned char ap[4];  //zou 1-5

        ap[0] = (top_left  + 2 * Above[0] + Above[1] + 2) >> 2;
        ap[1] = (Above[0] + 2 * Above[1] + Above[2] + 2) >> 2;
        ap[2] = (Above[1] + 2 * Above[2] + Above[3] + 2) >> 2;
        ap[3] = (Above[2] + 2 * Above[3] + Above[4] + 2) >> 2;
		for (r = 0; r < 4; r++)
		{
			predictor[0] = ap[0];
			predictor[1] = ap[1];
			predictor[2] = ap[2];
			predictor[3] = ap[3];
			predictor += 16;
		}
#endif
    }
    break;
    case B_HE_PRED:
    {
#ifdef VOARMV7
		vo_B_HE_PRED_neon(Left,top_left,predictor);
#else
        unsigned char lp[4];

        lp[0] = (top_left + 2 * Left[0] + Left[1] + 2) >> 2;
        lp[1] = (Left[0] + 2 * Left[1] + Left[2] + 2) >> 2;
        lp[2] = (Left[1] + 2 * Left[2] + Left[3] + 2) >> 2;
        lp[3] = (Left[2] + 2 * Left[3] + Left[3] + 2) >> 2;		

        for (r = 0; r < 4; r++)
        {
            for (c = 0; c < 4; c++)
            {
                predictor[c] = lp[r];
            }
            predictor += 16;
        }
#endif
    }
    break;
    case B_LD_PRED:
    {
#ifdef VOARMV7
		vo_B_LD_PRED_neon(Above,predictor); //vo_B_LD_PRED_neon(unsigned char*Above,unsigned char* predictor);
#else
        unsigned char *ptr = Above;

        predictor[0 * 16 + 0] = (ptr[0] + ptr[1] * 2 + ptr[2] + 2) >> 2;
        predictor[0 * 16 + 1] =
            predictor[1 * 16 + 0] = (ptr[1] + ptr[2] * 2 + ptr[3] + 2) >> 2;
        predictor[0 * 16 + 2] =
            predictor[1 * 16 + 1] =
                predictor[2 * 16 + 0] = (ptr[2] + ptr[3] * 2 + ptr[4] + 2) >> 2;
        predictor[0 * 16 + 3] =
            predictor[1 * 16 + 2] =
                predictor[2 * 16 + 1] =
                    predictor[3 * 16 + 0] = (ptr[3] + ptr[4] * 2 + ptr[5] + 2) >> 2;
        predictor[1 * 16 + 3] =
            predictor[2 * 16 + 2] =
                predictor[3 * 16 + 1] = (ptr[4] + ptr[5] * 2 + ptr[6] + 2) >> 2;
        predictor[2 * 16 + 3] =
            predictor[3 * 16 + 2] = (ptr[5] + ptr[6] * 2 + ptr[7] + 2) >> 2;
        predictor[3 * 16 + 3] = (ptr[6] + ptr[7] * 2 + ptr[7] + 2) >> 2;
#endif
    }
    break;
    case B_RD_PRED:
    {
        unsigned char pp[9];
        pp[0] = Left[3];        pp[1] = Left[2];
        pp[2] = Left[1];        pp[3] = Left[0];
        pp[4] = top_left;
        pp[5] = Above[0];        pp[6] = Above[1];
        pp[7] = Above[2];        pp[8] = Above[3];
#ifdef VOARMV7
		vo_B_RD_PRED_neon(pp,predictor);
#else
        predictor[3 * 16 + 0] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        predictor[3 * 16 + 1] =
            predictor[2 * 16 + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        predictor[3 * 16 + 2] =
            predictor[2 * 16 + 1] =
                predictor[1 * 16 + 0] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        predictor[3 * 16 + 3] =
            predictor[2 * 16 + 2] =
                predictor[1 * 16 + 1] =
                    predictor[0 * 16 + 0] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        predictor[2 * 16 + 3] =
            predictor[1 * 16 + 2] =
                predictor[0 * 16 + 1] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        predictor[1 * 16 + 3] =
            predictor[0 * 16 + 2] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
        predictor[0 * 16 + 3] = (pp[6] + pp[7] * 2 + pp[8] + 2) >> 2;
#endif
    }
    break;
    case B_VR_PRED:
    {

        unsigned char pp[9];

        pp[0] = Left[3];
        pp[1] = Left[2];
        pp[2] = Left[1];
        pp[3] = Left[0];
        pp[4] = top_left;
        pp[5] = Above[0];
        pp[6] = Above[1];
        pp[7] = Above[2];
        pp[8] = Above[3];


        predictor[3 * 16 + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        predictor[2 * 16 + 0] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        predictor[3 * 16 + 1] =
            predictor[1 * 16 + 0] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        predictor[2 * 16 + 1] =
            predictor[0 * 16 + 0] = (pp[4] + pp[5] + 1) >> 1;
        predictor[3 * 16 + 2] =
            predictor[1 * 16 + 1] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        predictor[2 * 16 + 2] =
            predictor[0 * 16 + 1] = (pp[5] + pp[6] + 1) >> 1;
        predictor[3 * 16 + 3] =
            predictor[1 * 16 + 2] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
        predictor[2 * 16 + 3] =
            predictor[0 * 16 + 2] = (pp[6] + pp[7] + 1) >> 1;
        predictor[1 * 16 + 3] = (pp[6] + pp[7] * 2 + pp[8] + 2) >> 2;
        predictor[0 * 16 + 3] = (pp[7] + pp[8] + 1) >> 1;

    }
    break;
    case B_VL_PRED:
    {

        unsigned char *pp = Above;

        predictor[0 * 16 + 0] = (pp[0] + pp[1] + 1) >> 1;
        predictor[1 * 16 + 0] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        predictor[2 * 16 + 0] =
            predictor[0 * 16 + 1] = (pp[1] + pp[2] + 1) >> 1;
        predictor[1 * 16 + 1] =
            predictor[3 * 16 + 0] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        predictor[2 * 16 + 1] =
            predictor[0 * 16 + 2] = (pp[2] + pp[3] + 1) >> 1;
        predictor[3 * 16 + 1] =
            predictor[1 * 16 + 2] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        predictor[0 * 16 + 3] =
            predictor[2 * 16 + 2] = (pp[3] + pp[4] + 1) >> 1;
        predictor[1 * 16 + 3] =
            predictor[3 * 16 + 2] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        predictor[2 * 16 + 3] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        predictor[3 * 16 + 3] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
    }
    break;

    case B_HD_PRED:
    {
        unsigned char pp[9];

        pp[0] = Left[3];
        pp[1] = Left[2];
        pp[2] = Left[1];
        pp[3] = Left[0];
        pp[4] = top_left;
        pp[5] = Above[0];
        pp[6] = Above[1];
        pp[7] = Above[2];
        pp[8] = Above[3];		


        predictor[3 * 16 + 0] = (pp[0] + pp[1] + 1) >> 1;
        predictor[3 * 16 + 1] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        predictor[2 * 16 + 0] =
            predictor[3 * 16 + 2] = (pp[1] + pp[2] + 1) >> 1;
        predictor[2 * 16 + 1] =
            predictor[3 * 16 + 3] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        predictor[2 * 16 + 2] =
            predictor[1 * 16 + 0] = (pp[2] + pp[3] + 1) >> 1;
        predictor[2 * 16 + 3] =
            predictor[1 * 16 + 1] = (pp[2] + pp[3] * 2 + pp[4] + 2) >> 2;
        predictor[1 * 16 + 2] =
            predictor[0 * 16 + 0] = (pp[3] + pp[4] + 1) >> 1;
        predictor[1 * 16 + 3] =
            predictor[0 * 16 + 1] = (pp[3] + pp[4] * 2 + pp[5] + 2) >> 2;
        predictor[0 * 16 + 2] = (pp[4] + pp[5] * 2 + pp[6] + 2) >> 2;
        predictor[0 * 16 + 3] = (pp[5] + pp[6] * 2 + pp[7] + 2) >> 2;
    }
    break;


    case B_HU_PRED:
    {
        unsigned char *pp = Left;
        predictor[0 * 16 + 0] = (pp[0] + pp[1] + 1) >> 1;
        predictor[0 * 16 + 1] = (pp[0] + pp[1] * 2 + pp[2] + 2) >> 2;
        predictor[0 * 16 + 2] =
            predictor[1 * 16 + 0] = (pp[1] + pp[2] + 1) >> 1;
        predictor[0 * 16 + 3] =
            predictor[1 * 16 + 1] = (pp[1] + pp[2] * 2 + pp[3] + 2) >> 2;
        predictor[1 * 16 + 2] =
            predictor[2 * 16 + 0] = (pp[2] + pp[3] + 1) >> 1;
        predictor[1 * 16 + 3] =
            predictor[2 * 16 + 1] = (pp[2] + pp[3] * 2 + pp[3] + 2) >> 2;
        predictor[2 * 16 + 2] =
            predictor[2 * 16 + 3] =
                predictor[3 * 16 + 0] =
                    predictor[3 * 16 + 1] =
                        predictor[3 * 16 + 2] =
                            predictor[3 * 16 + 3] = pp[3];
    }
    break;
    }
}

void vp8_intra_prediction_down_copy(MACROBLOCKD *x)
{
    unsigned char *above_right = *(x->block[0].base_dst) + x->block[0].dst - x->block[0].dst_stride + 16;

    unsigned int *src_ptr = (unsigned int *)above_right;
    unsigned int *dst_ptr0 = (unsigned int *)(above_right + 4 * x->block[0].dst_stride);
    unsigned int *dst_ptr1 = (unsigned int *)(above_right + 8 * x->block[0].dst_stride);
    unsigned int *dst_ptr2 = (unsigned int *)(above_right + 12 * x->block[0].dst_stride);

    *dst_ptr0 = *src_ptr;
    *dst_ptr1 = *src_ptr;
    *dst_ptr2 = *src_ptr;
}

#ifdef  VOARMV7

extern void vp8_build_intra_predictors_mby_neon_func(
    unsigned char *y_buffer,
    unsigned char *ypred_ptr,
    int y_stride,
    int mode,
    int Up,
    int Left);

extern void vp8_build_intra_predictors_mby_s_neon_func(
    unsigned char *y_buffer,
    unsigned char *ypred_ptr,
    int y_stride,
    int mode,
    int Up,
    int Left);

void vp8_build_intra_predictors_mby_neon(MACROBLOCKD *x,unsigned char* y,unsigned long stride,MODE_INFO *pModeInfo)
{
    unsigned char *y_buffer = x->dst.y_buffer;
    unsigned char *ypred_ptr = x->predictor;
    int y_stride = x->dst.y_stride;
    int mode = pModeInfo->mbmi.mode;
    int Up = x->up_available;
    int Left = x->left_available;

    vp8_build_intra_predictors_mby_neon_func(y_buffer, ypred_ptr, y_stride, mode, Up, Left);
}

void vp8_build_intra_predictors_mby_s_neon(MACROBLOCKD *x,unsigned char* y,unsigned long stride,MODE_INFO *pModeInfo)
{
    unsigned char *y_buffer = x->dst.y_buffer;
    unsigned char *ypred_ptr = x->predictor;
    int y_stride = x->dst.y_stride;
    int mode = pModeInfo->mbmi.mode;
    int Up = x->up_available;
    int Left = x->left_available;

    vp8_build_intra_predictors_mby_s_neon_func(y_buffer, ypred_ptr, y_stride, mode, Up, Left);
}
#elif VOARMV6
void vp8_build_intra_predictors_mby_c (MACROBLOCKD *x,unsigned char* y,unsigned long stride)
{

    unsigned char *yabove_row = x->dst.y_buffer - x->dst.y_stride;
    unsigned char yleft_col[16];
    unsigned char ytop_left = yabove_row[-1];
    unsigned char *ypred_ptr = x->predictor;
    int r, c, i;

    int y_stride = stride;//x->dst.y_stride;
    ypred_ptr = y;//x->dst.y_buffer; //x->predictor;

	if(x->mode_info_context->mbmi.mode!=V_PRED) //zou add
	{
		for (i = 0; i < 16; i++)
		{
			yleft_col[i] = x->dst.y_buffer [i* x->dst.y_stride -1];
		}
	}

    // for Y
    switch (x->mode_info_context->mbmi.mode)
    {
    case DC_PRED:
    {
        int expected_dc;
        int i;
        int shift;
        int average = 0;
        if (x->up_available || x->left_available)
        {
            if (x->up_available)
            {
                for (i = 0; i < 16; i++)
                {
                    average += yabove_row[i];
                }
            }

            if (x->left_available)
            {

                for (i = 0; i < 16; i++)
                {
                    average += yleft_col[i];
                }

            }
            shift = 3 + x->up_available + x->left_available;
            expected_dc = (average + (1 << (shift - 1))) >> shift;
        }
        else
        {
            expected_dc = 128;
        }
        for (r = 0; r < 16; r++)
        {
			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)ypred_ptr, expected_dc, 16);
            ypred_ptr += y_stride; //16;
        }
    }
    break;
    case V_PRED:
    {

        for (r = 0; r < 16; r++)
        {

            ((int *)ypred_ptr)[0] = ((int *)yabove_row)[0];
            ((int *)ypred_ptr)[1] = ((int *)yabove_row)[1];
            ((int *)ypred_ptr)[2] = ((int *)yabove_row)[2];
            ((int *)ypred_ptr)[3] = ((int *)yabove_row)[3];
            ypred_ptr += y_stride; //16;
        }
    }
    break;
    case H_PRED:
    {

        for (r = 0; r < 16; r++)
        {

			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)ypred_ptr, yleft_col[r], 16);
            ypred_ptr += y_stride;  //16;
        }

    }
    break;
    case TM_PRED:
    {
        for (r = 0; r < 16; r++)
        {
            for (c = 0; c < 16; c++)
            {
                int pred =  yleft_col[r] + yabove_row[ c] - ytop_left;

                if (pred < 0)
                    pred = 0;

                if (pred > 255)
                    pred = 255;

                ypred_ptr[c] = pred;
            }

            ypred_ptr += y_stride;  //16;
        }
    }
    break;
    case B_PRED:
    case NEARESTMV:
    case NEARMV:
    case ZEROMV:
    case NEWMV:
    case SPLITMV:
    case MB_MODE_COUNT:
        break;
    }
}
#else
void vp8_build_intra_predictors_mby_c (MACROBLOCKD *x,unsigned char* y,unsigned long stride,MODE_INFO *pModeInfo)
{

    unsigned char *yabove_row = x->dst.y_buffer - x->dst.y_stride;
    unsigned char yleft_col[16];
    unsigned char ytop_left = yabove_row[-1];
    unsigned char *ypred_ptr = x->predictor;
    int r, c, i;

    int y_stride = stride;//x->dst.y_stride;
    ypred_ptr = y;//x->dst.y_buffer; //x->predictor;

	if(pModeInfo->mbmi.mode!=V_PRED) //zou add
	{
		for (i = 0; i < 16; i++)
		{
			yleft_col[i] = x->dst.y_buffer [i* x->dst.y_stride -1];
		}
	}

    // for Y
    switch (pModeInfo->mbmi.mode)
    {
    case DC_PRED:
    {
        int expected_dc;
        int i;
        int shift;
        int average = 0;
        if (x->up_available || x->left_available)
        {
            if (x->up_available)
            {
                for (i = 0; i < 16; i++)
                {
                    average += yabove_row[i];
                }
            }

            if (x->left_available)
            {

                for (i = 0; i < 16; i++)
                {
                    average += yleft_col[i];
                }

            }
            shift = 3 + x->up_available + x->left_available;
            expected_dc = (average + (1 << (shift - 1))) >> shift;
        }
        else
        {
            expected_dc = 128;
        }
        for (r = 0; r < 16; r++)
        {
			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)ypred_ptr, expected_dc, 16);
            ypred_ptr += y_stride; //16;
        }
    }
    break;
    case V_PRED:
    {

        for (r = 0; r < 16; r++)
        {

            ((int *)ypred_ptr)[0] = ((int *)yabove_row)[0];
            ((int *)ypred_ptr)[1] = ((int *)yabove_row)[1];
            ((int *)ypred_ptr)[2] = ((int *)yabove_row)[2];
            ((int *)ypred_ptr)[3] = ((int *)yabove_row)[3];
            ypred_ptr += y_stride; //16;
        }
    }
    break;
    case H_PRED:
    {

        for (r = 0; r < 16; r++)
        {

			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)ypred_ptr, yleft_col[r], 16);
            ypred_ptr += y_stride;  //16;
        }

    }
    break;
    case TM_PRED:
    {
        for (r = 0; r < 16; r++)
        {
            for (c = 0; c < 16; c++)
            {
                int pred =  yleft_col[r] + yabove_row[ c] - ytop_left;

                if (pred < 0)
                    pred = 0;

                if (pred > 255)
                    pred = 255;

                ypred_ptr[c] = pred;
            }

            ypred_ptr += y_stride;  //16;
        }
    }
    break;
    case B_PRED:
    case NEARESTMV:
    case NEARMV:
    case ZEROMV:
    case NEWMV:
    case SPLITMV:
    case MB_MODE_COUNT:
        break;
    }
}
#endif



void vp8_build_intra_predictors_mbuv(MACROBLOCKD *x,unsigned char* u,unsigned char* v,unsigned long stride,MODE_INFO *pModeInfo)
{
    unsigned char *uabove_row = x->dst.u_buffer - x->dst.uv_stride;
    unsigned char uleft_col[16];
    unsigned char utop_left = uabove_row[-1];
    unsigned char *vabove_row = x->dst.v_buffer - x->dst.uv_stride;
    unsigned char vleft_col[20];
    unsigned char vtop_left = vabove_row[-1];
    unsigned char *upred_ptr = u;//x->dst.u_buffer; //&x->predictor[256];
    unsigned char *vpred_ptr = v;//x->dst.v_buffer; //&x->predictor[320];
    unsigned long uv_stride = stride;//x->dst.uv_stride;

    int i, j;

	if(pModeInfo->mbmi.uv_mode!=V_PRED)
	{
		for (i = 0; i < 8; i++)
		{
			uleft_col[i] = x->dst.u_buffer [i* x->dst.uv_stride -1];
			vleft_col[i] = x->dst.v_buffer [i* x->dst.uv_stride -1];
		}
	}

    switch (pModeInfo->mbmi.uv_mode)
    {
    case DC_PRED:
    {
        int expected_udc;
        int expected_vdc;
        int i;
        int shift;
        int Uaverage = 0;
        int Vaverage = 0;

        if (x->up_available)
        {
            for (i = 0; i < 8; i++)
            {
                Uaverage += uabove_row[i];
                Vaverage += vabove_row[i];
            }
        }

        if (x->left_available)
        {
            for (i = 0; i < 8; i++)
            {
                Uaverage += uleft_col[i];
                Vaverage += vleft_col[i];
            }
        }

        if (!x->up_available && !x->left_available)
        {
            expected_udc = 128;
            expected_vdc = 128;
        }
        else
        {
            shift = 2 + x->up_available + x->left_available;
            expected_udc = (Uaverage + (1 << (shift - 1))) >> shift;
            expected_vdc = (Vaverage + (1 << (shift - 1))) >> shift;
        }

        for (i = 0; i < 8; i++)
        {
			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)upred_ptr, expected_udc, 8);
			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)vpred_ptr, expected_vdc, 8);
            upred_ptr += uv_stride; //8;
            vpred_ptr += uv_stride; //8;
        }
    }
    break;
    case V_PRED:
    {
        int i;

        for (i = 0; i < 8; i++)
        {
			CopyMem(x->pUserData, x->nCodecIdx,upred_ptr, uabove_row, 8);
			CopyMem(x->pUserData, x->nCodecIdx,vpred_ptr, vabove_row, 8);
            upred_ptr += uv_stride; //8;
            vpred_ptr += uv_stride; //8;
        }

    }
    break;
    case H_PRED:
    {
        int i;

        for (i = 0; i < 8; i++)
        {
			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)upred_ptr, uleft_col[i], 8);
			SetMem(x->pUserData, x->nCodecIdx, (VO_U8 *)vpred_ptr, vleft_col[i], 8);
            upred_ptr += uv_stride; //8;
            vpred_ptr += uv_stride; //8;
        }
    }

    break;
    case TM_PRED:
    {
        int i;
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                int predu = uleft_col[i] + uabove_row[j] - utop_left;
                int predv = vleft_col[i] + vabove_row[j] - vtop_left;

                if (predu < 0)
                    predu = 0;

                if (predu > 255)
                    predu = 255;

                if (predv < 0)
                    predv = 0;

                if (predv > 255)
                    predv = 255;

                upred_ptr[j] = predu;
                vpred_ptr[j] = predv;
            }

            upred_ptr += uv_stride; //8;
            vpred_ptr += uv_stride; //8;
        }

    }
    break;
    case B_PRED:
    case NEARESTMV:
    case NEARMV:
    case ZEROMV:
    case NEWMV:
    case SPLITMV:
    case MB_MODE_COUNT:
        break;
    }
}