#include "voVP8DecRecon.h"
#include "voVP8DecInterPolationFilter.h"

void vp8_copy_mem16x16_c(
    unsigned char *src,
    int src_stride,
    unsigned char *dst,
    int dst_stride)
{

    int r;
    for (r = 0; r < 16; r++)
    {
        ((int *)dst)[0] = ((int *)src)[0] ;
        ((int *)dst)[1] = ((int *)src)[1] ;
        ((int *)dst)[2] = ((int *)src)[2] ;
        ((int *)dst)[3] = ((int *)src)[3] ;
        src += src_stride;
        dst += dst_stride;
    }
}

void vp8_copy_mem8x8_c(
    unsigned char *src,
    int src_stride,
    unsigned char *dst,
    int dst_stride)
{
    int r;

    for (r = 0; r < 8; r++)
    {
        ((int *)dst)[0] = ((int *)src)[0] ;
        ((int *)dst)[1] = ((int *)src)[1] ;
        src += src_stride;
        dst += dst_stride;
    }
}

void vp8_copy_mem8x4_c(
    unsigned char *src,
    int src_stride,
    unsigned char *dst,
    int dst_stride)
{
    int r;
    for (r = 0; r < 4; r++)
    {
        ((int *)dst)[0] = ((int *)src)[0] ;
        ((int *)dst)[1] = ((int *)src)[1] ;
        src += src_stride;
        dst += dst_stride;
    }
}

void  vp8_build_inter_predictors_b2(MACROBLOCKD *xd, int numb, vp8_subpix_fn_t sppf,MODE_INFO* pModeInfo)
{
    int r;
    unsigned char *ptr_base;
    unsigned char *ptr;
	unsigned char *pred_ptr =  xd->block[numb].predictor;

    ptr_base = *(xd->block[numb].base_pre);

    if (pModeInfo->bmi[numb].mv.as_mv.row & 7 || pModeInfo->bmi[numb].mv.as_mv.col & 7)
    {
#ifdef MV_POSITION
		 ptr = ptr_base + xd->block[numb].pre + pModeInfo->bmi[numb].mv_position;
#else
        ptr = ptr_base + xd->block[numb].pre + (pModeInfo->bmi[numb].mv.as_mv.row >> 3) * xd->block[numb].pre_stride + (pModeInfo->bmi[numb].mv.as_mv.col >> 3);
#endif    
		sppf(ptr, xd->block[numb].pre_stride, pModeInfo->bmi[numb].mv.as_mv.col & 7, pModeInfo->bmi[numb].mv.as_mv.row & 7, pred_ptr, 16);
    }
    else
    {
 #ifdef MV_POSITION
		ptr_base += xd->block[numb].pre + pModeInfo->bmi[numb].mv_position;
#else
        ptr_base += xd->block[numb].pre + (pModeInfo->bmi[numb].mv.as_mv.row >> 3) * xd->block[numb].pre_stride + (pModeInfo->bmi[numb].mv.as_mv.col >> 3);
#endif
		ptr = ptr_base;

        for (r = 0; r < 4; r++)
        {
            *(int *)pred_ptr = *(int *)ptr ;
            pred_ptr     += 16;
            ptr         += xd->block[numb].pre_stride;
        }
    }
}
static  void vp8_build_inter_predictors_b(BLOCKD *d, int pitch, vp8_subpix_fn_t sppf)
{
    int r;
    unsigned char *ptr_base;
    unsigned char *ptr;
    unsigned char *pred_ptr = d->predictor;

    ptr_base = *(d->base_pre);

    if (d->bmi.mv.as_mv.row & 7 || d->bmi.mv.as_mv.col & 7)
    {
#ifdef MV_POSITION
		ptr = ptr_base + d->pre + d->bmi.mv_position;
#else
        ptr = ptr_base + d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
        sppf(ptr, d->pre_stride, d->bmi.mv.as_mv.col & 7, d->bmi.mv.as_mv.row & 7, pred_ptr, pitch);
    }
    else
    {
#ifdef MV_POSITION
		ptr_base += d->pre + d->bmi.mv_position;
#else
        ptr_base += d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
        ptr = ptr_base;

        for (r = 0; r < 4; r++)
        {
            *(int *)pred_ptr = *(int *)ptr ;

            pred_ptr     += pitch;
            ptr         += d->pre_stride;
        }
    }
}

void  vp8_build_inter_predictors2b(MACROBLOCKD *x, BLOCKD *d, int pitch)
{
    unsigned char *ptr_base;
    unsigned char *ptr;
    unsigned char *pred_ptr = d->predictor;

    ptr_base = *(d->base_pre);
#ifdef MV_POSITION
	ptr = ptr_base + d->pre + d->bmi.mv_position;
#else
    ptr = ptr_base + d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif

    if (d->bmi.mv.as_mv.row & 7 || d->bmi.mv.as_mv.col & 7)
    {
        x->subpixel_predict8x4(ptr, d->pre_stride, d->bmi.mv.as_mv.col & 7, d->bmi.mv.as_mv.row & 7, pred_ptr, pitch);
    }
    else
    {
        vp8_copy_mem8x4(ptr, d->pre_stride, pred_ptr, pitch);
    }
}
void vp8_build_inter_predictors_nosplitmv_mb(MACROBLOCKD *x,unsigned char*y,unsigned char*u,unsigned char*v,unsigned long ystride,unsigned long uvstride,MODE_INFO *pModeInfo )
{
    int offset;
    unsigned char *ptr_base;
    unsigned char *ptr;
    unsigned char *uptr, *vptr;
    unsigned char *pred_ptr = y;//x->predictor;
    unsigned char *upred_ptr = u;//&x->predictor[256];
    unsigned char *vpred_ptr = v;//&x->predictor[320];

    int mv_row ;
    int mv_col ;
    int pre_stride = x->block[0].pre_stride;

	ptr_base = x->pre.y_buffer;

	mv_row = pModeInfo->mbmi.mv.as_mv.row;
    mv_col = pModeInfo->mbmi.mv.as_mv.col;
    
#ifdef MV_POSITION
	ptr = ptr_base + pModeInfo->mbmi.mv_position;
#else
    ptr = ptr_base + (mv_row >> 3) * pre_stride + (mv_col >> 3);
#endif
	
    if ((mv_row | mv_col) & 7)
    {		
       x->subpixel_predict16x16(ptr, pre_stride, mv_col & 7, mv_row & 7, pred_ptr, ystride);
    }
    else
    {
        vp8_copy_mem16x16(ptr, pre_stride, pred_ptr, ystride);
    }

	mv_row =  x->block[16].bmi.mv.as_mv.row;
    mv_col =   x->block[16].bmi.mv.as_mv.col;

    pre_stride >>= 1;

#ifdef MV_POSITION
	offset = x->block[16].bmi.mv_position;
#else
    offset = (mv_row >> 3) * pre_stride + (mv_col >> 3);
#endif
    uptr = x->pre.u_buffer + offset;
    vptr = x->pre.v_buffer + offset;

    if ((mv_row | mv_col) & 7)
    {
       x->subpixel_predict8x8(uptr, pre_stride, mv_col & 7, mv_row & 7, upred_ptr, uvstride);
	   x->subpixel_predict8x8(vptr, pre_stride, mv_col & 7, mv_row & 7, vpred_ptr, uvstride);
    }
    else
    {
        vp8_copy_mem8x8(uptr, pre_stride, upred_ptr, uvstride);
        vp8_copy_mem8x8(vptr, pre_stride, vpred_ptr, uvstride);
    }
}

void vp8_build_inter_predictors_splitmv_mb(MACROBLOCKD *x,MODE_INFO* pModeInfo)
{
    int i;
	unsigned char *ptr_base;
    unsigned char *ptr;

    if (pModeInfo->mbmi.partitioning < 3)
    {
		for(i=0;i<=2;i+=2)
		{
			unsigned char *pred_ptr = x->block[i].predictor;
			ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
			ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
			ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride 
				+ (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif

			if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
				x->subpixel_predict8x8(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, 16);
			else
				vp8_copy_mem8x8(ptr, x->block[i].pre_stride, pred_ptr, 16);
		}
		for(i=8;i<=10;i+=2)
		{
			unsigned char *pred_ptr = x->block[i].predictor;
			ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
			ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
			ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride 
				+ (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
			if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
				x->subpixel_predict8x8(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, 16);
			else
				vp8_copy_mem8x8(ptr, x->block[i].pre_stride, pred_ptr, 16);
		}			
    }
    else
    {
        for (i = 0; i < 16; i += 2)
        {
            if (pModeInfo->bmi[i].mv.as_int == pModeInfo->bmi[i+1].mv.as_int)
			{
				unsigned char *pred_ptr = x->block[i].predictor;

				ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
				ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
				ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride + (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
				if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
					x->subpixel_predict8x4(ptr, x->block[i].pre_stride,pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, 16);
				else
					vp8_copy_mem8x4(ptr, x->block[i].pre_stride, pred_ptr, 16);
			}
            else
            {
				vp8_build_inter_predictors_b2(x, i, x->subpixel_predict,pModeInfo);
				vp8_build_inter_predictors_b2(x, i+1, x->subpixel_predict,pModeInfo);
            }
        }
    }

	for (i = 16; i < 24; i += 2)  //U V
    {
        BLOCKD *d0 = &x->block[i];
        BLOCKD *d1 = &x->block[i+1];

        if (d0->bmi.mv.as_int == d1->bmi.mv.as_int)
            vp8_build_inter_predictors2b(x, d0, 8);
        else
        {
            vp8_build_inter_predictors_b(d0, 8, x->subpixel_predict);
            vp8_build_inter_predictors_b(d1, 8, x->subpixel_predict);
        }
    }
}

void vp8_build_inter_predictors_splitmv_mb_s(MACROBLOCKD *x,MODE_INFO* pModeInfo)
{
    int i;
	unsigned char *ptr_base;
    unsigned char *ptr;
	unsigned char *pred_ptr ;
	unsigned long  ystride = x->block[0].dst_stride;
	unsigned long  uvstride = x->block[16].dst_stride;

    if (pModeInfo->mbmi.partitioning < 3)
    {
		i=0;
		pred_ptr = *(x->block[i].base_dst);//y;//x->predictor;//x->block[i].predictor;
		ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
		ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
		ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride 
				+ (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
		if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
			x->subpixel_predict8x8(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, ystride);
		else
			vp8_copy_mem8x8(ptr, x->block[i].pre_stride, pred_ptr, ystride);

		i=2;
		pred_ptr =  *(x->block[i].base_dst)+x->block[i].dst;//y+8;//x->predictor+8;
		ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
		ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
		ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride 
				+ (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
		if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
			x->subpixel_predict8x8(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, ystride);
		else
			vp8_copy_mem8x8(ptr, x->block[i].pre_stride, pred_ptr, ystride);

		i=8;
		pred_ptr = *(x->block[i].base_dst)+x->block[i].dst;//y+8*ystride;//x->predictor+8*16;
		ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
		ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
		ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride 
				+ (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
		if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
			x->subpixel_predict8x8(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, ystride);
		else
			vp8_copy_mem8x8(ptr, x->block[i].pre_stride, pred_ptr, ystride);

		i=10;
		pred_ptr = *(x->block[i].base_dst)+x->block[i].dst;//y+8*ystride+8; //x->predictor+8*16+8;
		ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
		ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
		ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride 
				+ (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
		if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
			x->subpixel_predict8x8(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7,pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, ystride);
		else
			vp8_copy_mem8x8(ptr, x->block[i].pre_stride, pred_ptr, ystride);	
    }
    else
    {
        for (i = 0; i < 16; i += 2)
        {
            if (pModeInfo->bmi[i].mv.as_int == pModeInfo->bmi[i+1].mv.as_int)
			{
				unsigned char *pred_ptr =  *(x->block[i].base_dst)+x->block[i].dst;//y+(i/4)*4*ystride + (i%4)*4;//x->predictor + (i/4)*4*16 + (i%4)*4;//x->block[i].predictor;

				ptr_base = *(x->block[i].base_pre);
#ifdef MV_POSITION
				ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
				ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride + (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
				if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
					x->subpixel_predict8x4(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, ystride);
				else
					vp8_copy_mem8x4(ptr, x->block[i].pre_stride, pred_ptr, ystride);
			}
            else
            {
				int r;
				unsigned char *ptr_base;
				unsigned char *ptr;
				unsigned char *pred_ptr = *(x->block[i].base_dst)+x->block[i].dst;//y+(i/4)*4*ystride + (i%4)*4;//x->predictor + (i/4)*4*16+(i%4)*4;//x->block[i].predictor;

				ptr_base = *(x->block[i].base_pre);
				if (pModeInfo->bmi[i].mv.as_mv.row & 7 || pModeInfo->bmi[i].mv.as_mv.col & 7)
				{
#ifdef MV_POSITION
					ptr = ptr_base + x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
					ptr = ptr_base + x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride + (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif	
					x->subpixel_predict(ptr, x->block[i].pre_stride, pModeInfo->bmi[i].mv.as_mv.col & 7, pModeInfo->bmi[i].mv.as_mv.row & 7, pred_ptr, ystride);
				}
				else
				{
#ifdef MV_POSITION
					ptr_base +=x->block[i].pre + pModeInfo->bmi[i].mv_position;  
#else
					ptr_base += x->block[i].pre + (pModeInfo->bmi[i].mv.as_mv.row >> 3) * x->block[i].pre_stride + (pModeInfo->bmi[i].mv.as_mv.col >> 3);
#endif
					ptr = ptr_base;

					for (r = 0; r < 4; r++)
					{
						*(int *)pred_ptr = *(int *)ptr ;
						pred_ptr     += ystride;
						ptr         += x->block[i].pre_stride;
					}
				}

				pred_ptr = *(x->block[(i+1)].base_dst)+x->block[(i+1)].dst;//y+(i/4)*4*ystride + (i%4)*4;//x->predictor + ((i+1)/4)*4*16+((i+1)%4)*4;//x->block[(i+1)].predictor;
				ptr_base = *(x->block[(i+1)].base_pre);
				if (pModeInfo->bmi[(i+1)].mv.as_mv.row & 7 || pModeInfo->bmi[(i+1)].mv.as_mv.col & 7)
				{
#ifdef MV_POSITION
					ptr = ptr_base + x->block[i+1].pre + pModeInfo->bmi[i+1].mv_position;  
#else
					ptr = ptr_base + x->block[(i+1)].pre + (pModeInfo->bmi[(i+1)].mv.as_mv.row >> 3) * x->block[(i+1)].pre_stride + (pModeInfo->bmi[(i+1)].mv.as_mv.col >> 3);
#endif
					x->subpixel_predict(ptr, x->block[(i+1)].pre_stride, pModeInfo->bmi[(i+1)].mv.as_mv.col & 7, pModeInfo->bmi[(i+1)].mv.as_mv.row & 7, pred_ptr, ystride);
				}
				else
				{
#ifdef MV_POSITION
					ptr_base += x->block[i+1].pre + pModeInfo->bmi[i+1].mv_position;  
#else
					ptr_base += x->block[(i+1)].pre + (pModeInfo->bmi[(i+1)].mv.as_mv.row >> 3) * x->block[(i+1)].pre_stride + (pModeInfo->bmi[(i+1)].mv.as_mv.col >> 3);
#endif
					ptr = ptr_base;

					for (r = 0; r < 4; r++)
					{
						*(int *)pred_ptr = *(int *)ptr ;
						pred_ptr     += ystride;
						ptr         += x->block[(i+1)].pre_stride;
					}
				}
            }
        }
    }

	 for (i = 16; i < 24; i += 2)
    {
        BLOCKD *d0 = &x->block[i];
        BLOCKD *d1 = &x->block[i+1];
		BLOCKD *d = d0;

        if (d0->bmi.mv.as_int == d1->bmi.mv.as_int)
		{
			unsigned char *ptr_base;
			unsigned char *ptr;
			unsigned char *pred_ptr = *(d->base_dst) +d->dst;//x->predictor+i*16;//d->predictor;

			ptr_base = *(d->base_pre);
#ifdef MV_POSITION
			ptr = ptr_base + d->pre + d->bmi.mv_position;
#else
			ptr = ptr_base + d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
			if (d->bmi.mv.as_mv.row & 7 || d->bmi.mv.as_mv.col & 7)
				x->subpixel_predict8x4(ptr, d->pre_stride, d->bmi.mv.as_mv.col & 7, d->bmi.mv.as_mv.row & 7, pred_ptr,uvstride);
			else
				vp8_copy_mem8x4(ptr, d->pre_stride, pred_ptr, uvstride);
		}
        else
        {
			int r;
			unsigned char *ptr_base;
			unsigned char *ptr;
			unsigned char *pred_ptr = *(d->base_dst) +d->dst;//x->predictor+i*16;//d->predictor;

			ptr_base = *(d->base_pre);
			if (d->bmi.mv.as_mv.row & 7 || d->bmi.mv.as_mv.col & 7)
			{
#ifdef MV_POSITION
				ptr = ptr_base + d->pre + d->bmi.mv_position;
#else
				ptr = ptr_base + d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
				x->subpixel_predict(ptr, d->pre_stride, d->bmi.mv.as_mv.col & 7, d->bmi.mv.as_mv.row & 7, pred_ptr, uvstride);
			}
			else
			{
#ifdef MV_POSITION
				ptr_base += d->pre + d->bmi.mv_position;
#else
				ptr_base += d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
				ptr = ptr_base;

				for (r = 0; r < 4; r++)
				{
					*(int *)pred_ptr = *(int *)ptr ;

					pred_ptr     += uvstride;
					ptr         += d->pre_stride;
				}
			}
			
			d = d1;
			pred_ptr = *(d->base_dst) +d->dst;//x->predictor+i*16+4;//d->predictor;
			ptr_base = *(d->base_pre);
			if (d->bmi.mv.as_mv.row & 7 || d->bmi.mv.as_mv.col & 7)
			{
#ifdef MV_POSITION
				ptr = ptr_base + d->pre + d->bmi.mv_position;
#else
				ptr = ptr_base + d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
				x->subpixel_predict(ptr, d->pre_stride, d->bmi.mv.as_mv.col & 7, d->bmi.mv.as_mv.row & 7, pred_ptr, uvstride);
			}
			else
			{
#ifdef MV_POSITION
				ptr_base += d->pre + d->bmi.mv_position;
#else
				ptr_base += d->pre + (d->bmi.mv.as_mv.row >> 3) * d->pre_stride + (d->bmi.mv.as_mv.col >> 3);
#endif
				ptr = ptr_base;

				for (r = 0; r < 4; r++)
				{
					*(int *)pred_ptr = *(int *)ptr ;

					pred_ptr     += uvstride;
					ptr         += d->pre_stride;
				}
			}
            //vp8_build_inter_predictors_b(d0, 8, x->subpixel_predict);
            //vp8_build_inter_predictors_b(d1, 8, x->subpixel_predict);
        }
    }
}
