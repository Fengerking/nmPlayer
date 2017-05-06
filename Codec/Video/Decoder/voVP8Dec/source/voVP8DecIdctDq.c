#include <string.h>
#include "voVP8DecIdctDq.h"

extern void vp8_dequantize_b_loop_neon(short *Q,short *DQC,short *DQ);

void vp8_dequantize_b(BLOCKD *d,short* dq)
{
    short *DQ  = d->qcoeff;
    short *Q   = d->qcoeff;
    short *DQC = dq;
#ifdef VOARMV7
	vp8_dequantize_b_loop_neon(Q, DQC, DQ);
#elif VOARMV6
	int i;
	for (i = 0; i < 16; i++)
    {
        DQ[i] = Q[i] * DQC[i];
    }
#else  
	int i;
    for (i = 0; i < 16; i++)
    {
        DQ[i] = Q[i] * DQC[i];
    }
#endif
}

#ifdef VOARMV7

void idct_dequant_dc_full_2x_neon
            (short *input, short *dq, unsigned char *pre, unsigned char *dst,
             int stride, short *dc);
void idct_dequant_dc_0_2x_neon
            (short *dc, unsigned char *pre, unsigned char *dst, int stride);
void idct_dequant_full_2x_neon
            (short *q, short *dq, unsigned char *pre, unsigned char *dst,
             int pitch, int stride);
void idct_dequant_0_2x_neon
            (short *q, short dq, unsigned char *pre, int pitch,
             unsigned char *dst, int stride);

void vp8_dequant_dc_idct_add_y_block_neon(MACROBLOCKD *xd,short *dq)
{
    int i;
	short *q = xd->block[0].qcoeff;//xd->qcoeff;
	unsigned char *pre = xd->predictor;
	unsigned char *dst =  xd->dst.y_buffer;
	short *dc = xd->block[24].qcoeff;
	char *eobs = xd->eob;
	int stride = xd->dst.y_stride;

    for (i = 0; i < 4; i++)
    {
        if (((short *)eobs)[0] & 0xfefe)
            idct_dequant_dc_full_2x_neon (q, dq, pre, dst, stride, dc);
        else
            idct_dequant_dc_0_2x_neon(dc, pre, dst, stride);

        if (((short *)eobs)[1] & 0xfefe)
            idct_dequant_dc_full_2x_neon (q+32, dq, pre+8, dst+8, stride, dc+2);
        else
            idct_dequant_dc_0_2x_neon(dc+2, pre+8, dst+8, stride);

		*((int*)dc) =0;
		*((int*)(dc+2)) =0;

        q    += 64;
        dc   += 4;
        pre  += 64;
        dst  += 4*stride;
        eobs += 4;
    }
}

void vp8_dequant_idct_add_y_block_neon(MACROBLOCKD *xd,short *dq)
{
    int i;
	short *q = xd->block[0].qcoeff;//xd->qcoeff;
	unsigned char *pre = xd->predictor;
	unsigned char *dst =  xd->dst.y_buffer;
	//short *dc = xd->block[24].qcoeff;
	char *eobs = xd->eob;
	int stride = xd->dst.y_stride;

    for (i = 0; i < 4; i++)
    {
        if (((short *)eobs)[0] & 0xfefe)
            idct_dequant_full_2x_neon (q, dq, pre, dst, 16, stride);
        else
            idct_dequant_0_2x_neon (q, dq[0], pre, 16, dst, stride);

        if (((short *)eobs)[1] & 0xfefe)
            idct_dequant_full_2x_neon (q+32, dq, pre+8, dst+8, 16, stride);
        else
            idct_dequant_0_2x_neon (q+32, dq[0], pre+8, 16, dst+8, stride);

        q    += 64;
        pre  += 64;
        dst  += 4*stride;
        eobs += 4;
    }
}

void vp8_dequant_idct_add_uv_block_neon(MACROBLOCKD *xd,short *dq)
{
	//short *q = xd->block[16].qcoeff;//xd->qcoeff+16*16;

	short *q = xd->block[16].qcoeff;
	unsigned char *pre = xd->predictor+16*16;
	unsigned char *dstu =  xd->dst.u_buffer;
	unsigned char *dstv =  xd->dst.v_buffer;
	//char *eobs = xd->eob[16];//xd->eob+16;
	char *eobs = xd->eob+16;
	int stride = xd->dst.uv_stride;

    if (((short *)eobs)[0] & 0xfefe)
        idct_dequant_full_2x_neon (q, dq, pre, dstu, 8, stride);
    else
        idct_dequant_0_2x_neon (q, dq[0], pre, 8, dstu, stride);

    q    += 32;
    pre  += 32;
    dstu += 4*stride;

    if (((short *)eobs)[1] & 0xfefe)
        idct_dequant_full_2x_neon (q, dq, pre, dstu, 8, stride);
    else
        idct_dequant_0_2x_neon (q, dq[0], pre, 8, dstu, stride);

    q += 32;
    pre += 32;

    if (((short *)eobs)[2] & 0xfefe)
        idct_dequant_full_2x_neon (q, dq, pre, dstv, 8, stride);
    else
        idct_dequant_0_2x_neon (q, dq[0], pre, 8, dstv, stride);

    q    += 32;
    pre  += 32;
    dstv += 4*stride;

    if (((short *)eobs)[3] & 0xfefe)
        idct_dequant_full_2x_neon (q, dq, pre, dstv, 8, stride);
    else
        idct_dequant_0_2x_neon (q, dq[0], pre, 8, dstv, stride);
}
#elif VOARMV6
void vp8_dequant_dc_idct_add_y_block_v6(MACROBLOCKD *xd,short *dq)
{
	int i;
	BLOCKD *b;
	for (i = 0; i < 16; i++)
	{
		b = &xd->block[i];
		if(xd->eob[i]>1)
			vp8_dequant_dc_idct_add(b->qcoeff, dq, b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride, xd->block[24].qcoeff[i]);
		else
		{
			vp8_dc_only_idct_add(xd->block[24].qcoeff[i], b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
			((int *)b->qcoeff)[0] = 0;
		}
		xd->block[24].qcoeff[i] = 0;
	}
}
void vp8_dequant_idct_add_y_block_v6(MACROBLOCKD *xd,short *dq)
{
	int i=0;
	BLOCKD *b;
	for (i = 0; i < 16; i++)
	{
		b = &xd->block[i];
		if(xd->eob[i]>1)
			vp8_dequant_idct_add(b->qcoeff, dq,  b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
		else
		{
			vp8_dc_only_idct_add(b->qcoeff[0] * (*dq), b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
			((int *)b->qcoeff)[0] = 0;
		}
	}
}
void vp8_dequant_idct_add_uv_block_v6(MACROBLOCKD *xd,short *dq)
{
	int i=0;
	BLOCKD *b;
	 for (i = 16; i < 24; i++)
	 {
		 b = &xd->block[i];
		 if(xd->eob[i]>1)
		 {
			 vp8_dequant_idct_add(b->qcoeff, dq,  b->predictor, *(b->base_dst) + b->dst, 8, b->dst_stride);
		 }
		 else
		 {
			 vp8_dc_only_idct_add(b->qcoeff[0] * (*dq), b->predictor, *(b->base_dst) + b->dst, 8, b->dst_stride);
			 ((int *)b->qcoeff)[0] = 0;
		 }
	}
}
//void vp8_short_inv_walsh4x4_c(short *input, short *output)
//{
//    int i;
//    int a1, b1, c1, d1;
//    int a2, b2, c2, d2;
//    short *ip = input;
//    short *op = output;
//
//    for (i = 0; i < 4; i++)
//    {
//        a1 = ip[0] + ip[12];
//        b1 = ip[4] + ip[8];
//        c1 = ip[4] - ip[8];
//        d1 = ip[0] - ip[12];
//
//        op[0] = a1 + b1;
//        op[4] = c1 + d1;
//        op[8] = a1 - b1;
//        op[12] = d1 - c1;
//        ip++;
//        op++;
//    }
//
//    ip = output;
//    op = output;
//
//    for (i = 0; i < 4; i++)
//    {
//        a1 = ip[0] + ip[3];
//        b1 = ip[1] + ip[2];
//        c1 = ip[1] - ip[2];
//        d1 = ip[0] - ip[3];
//
//        a2 = a1 + b1;
//        b2 = c1 + d1;
//        c2 = a1 - b1;
//        d2 = d1 - c1;
//
//        op[0] = (a2 + 3) >> 3;
//        op[1] = (b2 + 3) >> 3;
//        op[2] = (c2 + 3) >> 3;
//        op[3] = (d2 + 3) >> 3;
//
//        ip += 4;
//        op += 4;
//    }
//}
//
//void vp8_short_inv_walsh4x4_1_c(short *input, short *output)
//{
//    int i;
//    int a1;
//    short *op = output;
//
//    a1 = ((input[0] + 3) >> 3);
//
//    for (i = 0; i < 4; i++)
//    {
//        op[0] = a1;
//        op[1] = a1;
//        op[2] = a1;
//        op[3] = a1;
//        op += 4;
//    }
//}
#else
static const int cospi8sqrt2minus1 = 20091;
static const int sinpi8sqrt2      = 35468;
static const int rounding = 0;
#define W1 20091
#define W2 35468
void vp8_short_idct4x4llm_c(short *input, short *output, int pitch)
{
    int i;
    int a1, b1, c1, d1;

    short *ip = input;
    short *op = output;
    int temp1, temp2;
    int shortpitch = pitch >> 1;

    for (i = 0; i < 4; i++)
    {
        a1 = ip[0] + ip[8];
        b1 = ip[0] - ip[8];

        temp1 = (ip[4] * W2 ) >> 16;
        temp2 = ip[12] + ((ip[12] * W1) >> 16);
        c1 = temp1 - temp2;

        temp1 = ip[4] + ((ip[4] * W1) >> 16);
        temp2 = (ip[12] * W2) >> 16;
        d1 = temp1 + temp2;

        op[shortpitch*0] = a1 + d1;
        op[shortpitch*3] = a1 - d1;

        op[shortpitch*1] = b1 + c1;
        op[shortpitch*2] = b1 - c1;

        ip++;
        op++;
    }

    ip = output;
    op = output;

    for (i = 0; i < 4; i++)
    {
        a1 = ip[0] + ip[2];
        b1 = ip[0] - ip[2];

        temp1 = (ip[1] * W2) >> 16;
        temp2 = ip[3] + ((ip[3] * W1) >> 16);
        c1 = temp1 - temp2;

        temp1 = ip[1] + ((ip[1] * W1 ) >> 16);
        temp2 = (ip[3] * W2 ) >> 16;
        d1 = temp1 + temp2;

        op[0] = (a1 + d1 + 4) >> 3;
        op[3] = (a1 - d1 + 4) >> 3;

        op[1] = (b1 + c1 + 4) >> 3;
        op[2] = (b1 - c1 + 4) >> 3;
        ip += shortpitch;
        op += shortpitch;
    }
}

void vp8_dequant_dc_idct_add_c(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride, int Dc)
{
    int i;
    short output[16];
    short *diff_ptr = output;
    int r, c;

    input[0] = (short)Dc;

    for (i = 1; i < 16; i++)
    {
        input[i] = dq[i] * input[i];
    }

    // the idct halves ( >> 1) the pitch
    vp8_short_idct4x4llm_c(input, output, 4 << 1);

    memset(input, 0, 32);  //zou 14

    for (r = 0; r < 4; r++)
    {
        for (c = 0; c < 4; c++)
        {
            int a = diff_ptr[c] + pred[c];

            if (a < 0)
                a = 0;

            if (a > 255)
                a = 255;

            dest[c] = (unsigned char) a;
        }

        dest += stride;
        diff_ptr += 4;
        pred += pitch;
    }
}
void vp8_dequant_idct_add_c(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride)
{
    short output[16];
    short *diff_ptr = output;
    int r, c;
    int i;

    for (i = 0; i < 16; i++)
    {
        input[i] = dq[i] * input[i];
    }

    // the idct halves ( >> 1) the pitch
    vp8_short_idct4x4llm_c(input, output, 4 << 1);

    memset(input, 0, 32); //zou 14

    for (r = 0; r < 4; r++)
    {
        for (c = 0; c < 4; c++)
        {
            int a = diff_ptr[c] + pred[c];

            if (a < 0)
                a = 0;

            if (a > 255)
                a = 255;

            dest[c] = (unsigned char) a;
        }

        dest += stride;
        diff_ptr += 4;
        pred += pitch;
    }
}
void vp8_dc_only_idct_add_c(short input_dc, unsigned char *pred_ptr, unsigned char *dst_ptr, int pitch, int stride)
{
    int a1 = ((input_dc + 4) >> 3);
    int r, c;

    for (r = 0; r < 4; r++)
    {
        for (c = 0; c < 4; c++)
        {
            int a = a1 + pred_ptr[c] ;

            if (a < 0)
                a = 0;

            if (a > 255)
                a = 255;

            dst_ptr[c] = (unsigned char) a ;
        }

        dst_ptr += stride;
        pred_ptr += pitch;
    }
}
void vp8_dequant_idct_add_y_block_c(MACROBLOCKD *xd,short *dq)
 //           (short *q, short *dq, unsigned char *pre,
 //            unsigned char *dstu, unsigned char *dstv, int stride, char *eobs)
{
	int i=0;
	BLOCKD *b;
	for (i = 0; i < 16; i++)
	{
		b = &xd->block[i];
		if(xd->eob[i]>1)
			vp8_dequant_idct_add(b->qcoeff, dq,  b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
		else
		{
			vp8_dc_only_idct_add(b->qcoeff[0] * (*dq), b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
			((int *)b->qcoeff)[0] = 0;
		}
	}
}
void vp8_dequant_idct_add_uv_block_c(MACROBLOCKD *xd,short *dq)
{
	int i=0;
	BLOCKD *b;

	for (i = 16; i < 24; i++)
	{
		b = &xd->block[i];
		if(xd->eob[i]>1)
		{
			vp8_dequant_idct_add(b->qcoeff, dq,  b->predictor, *(b->base_dst) + b->dst, 8, b->dst_stride);
		}
		 else
		 {
			 vp8_dc_only_idct_add(b->qcoeff[0] * (*dq), b->predictor, *(b->base_dst) + b->dst, 8, b->dst_stride);
			 ((int *)b->qcoeff)[0] = 0;
		 }
	}
}
void vp8_dequant_dc_idct_add_y_block_c(MACROBLOCKD *xd,short *dq)
 //(xd->qcoeff, dq_y, xd->predictor, xd->dst.y_buffer,
 //                       xd->dst.y_stride, xd->eob, xd->block[24].qcoeff)
{
	int i;
	BLOCKD *b;
	for (i = 0; i < 16; i++)
	{
		b = &xd->block[i];
		if(xd->eob[i]>1)
			vp8_dequant_dc_idct_add(b->qcoeff, dq, b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride, xd->block[24].qcoeff[i]);
		else
		{
			vp8_dc_only_idct_add(xd->block[24].qcoeff[i], b->predictor, *(b->base_dst) + b->dst, 16, b->dst_stride);
			((int *)b->qcoeff)[0] = 0;
		}
		xd->block[24].qcoeff[i] = 0;
	}
}
void vp8_short_inv_walsh4x4_c(short *input, short *output)
{
    int i;
    int a1, b1, c1, d1;
    int a2, b2, c2, d2;
    short *ip = input;
    short *op = output;

    for (i = 0; i < 4; i++)
    {
        a1 = ip[0] + ip[12];
        b1 = ip[4] + ip[8];
        c1 = ip[4] - ip[8];
        d1 = ip[0] - ip[12];

        op[0] = a1 + b1;
        op[4] = c1 + d1;
        op[8] = a1 - b1;
        op[12] = d1 - c1;
        ip++;
        op++;
    }

    ip = output;
    op = output;

    for (i = 0; i < 4; i++)
    {
        a1 = ip[0] + ip[3];
        b1 = ip[1] + ip[2];
        c1 = ip[1] - ip[2];
        d1 = ip[0] - ip[3];

        a2 = a1 + b1;
        b2 = c1 + d1;
        c2 = a1 - b1;
        d2 = d1 - c1;

        op[0] = (a2 + 3) >> 3;
        op[1] = (b2 + 3) >> 3;
        op[2] = (c2 + 3) >> 3;
        op[3] = (d2 + 3) >> 3;

        ip += 4;
        op += 4;
    }
}

void vp8_short_inv_walsh4x4_1_c(short *input, short *output)
{
    int i;
    int a1;
    short *op = output;

    a1 = ((input[0] + 3) >> 3);

    for (i = 0; i < 4; i++)
    {
        op[0] = a1;
        op[1] = a1;
        op[2] = a1;
        op[3] = a1;
        op += 4;
    }
}
#endif