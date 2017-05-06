 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265_mem.c
    \brief    memory operation functions
    \author   Renjie Yu
	\change
*/

#include "h265dec_mem.h"
#include "stdlib.h"
#include "stdio.h"
#include <string.h>


void * AlignMalloc(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_U32 size, VO_U32 align)
{
	VO_U8 *p_ptr, *p_ptr_ori;

	if (align == 0) {
		align = 1;// just store offset
	}

	//Allocate memory size + align
	if (!p_user_op) {		//default memory
		p_ptr_ori = (VO_U8*) malloc(size + align);
	} else {
		if (p_user_op->memflag == VO_IMF_USERMEMOPERATOR) {  
			VO_MEM_OPERATOR *p_mem_op = (VO_MEM_OPERATOR *)p_user_op->memData;
			VO_MEM_INFO mem_info;

			if (!p_mem_op || !p_mem_op->Alloc) {
				return NULL;
			}

			mem_info.VBuffer = NULL;
			mem_info.Size = size + align;
			p_mem_op->Alloc(codec_id, &mem_info); 
			p_ptr_ori = (VO_U8*)mem_info.VBuffer;

		} else if (p_user_op->memflag == VO_IMF_PREALLOCATEDBUFFER) { //not support
			//VIDEO_INNER_MEM * p_inner_mem = (VIDEO_INNER_MEM *)p_user_op->memData;
            //
			//if (!p_inner_mem || !p_inner_mem->private_mem_size || !p_inner_mem->p_priv_mem) {
			//	return NULL;
			//}

			//p_ptr_ori = (VO_U8*)p_inner_mem->p_priv_mem;
			//p_inner_mem->p_priv_mem += (size + align);
			p_ptr_ori = NULL; 
		} else {
			return NULL;
		}
	}

	if (p_ptr_ori == NULL) {
		return NULL;
	}

	//Align  
	p_ptr = (VO_U8 *) ((VO_U32) (p_ptr_ori + align) & (~(align - 1)));
		
	//(mem_ptr - 1) is stored offset of real address
	*(p_ptr - 1) = (VO_U8) (p_ptr - p_ptr_ori);

	//Return the aligned pointer 
	return ((void *)p_ptr);


	return NULL;
}

void AlignFree(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_PTR p_dst)
{
	VO_U8 *p_ptr;

	if (p_dst == NULL)
		return;

	//Aligned pointer 
	p_ptr = (VO_U8 *)p_dst;

	// *(p_ptr - 1) stored the offset of real address
	p_ptr -= *(p_ptr - 1);

	//Free the memory 
	if (!p_user_op) {		//default memory
		free(p_ptr);
	} else {
		if (p_user_op->memflag == VO_IMF_USERMEMOPERATOR) {  
			VO_MEM_OPERATOR *p_mem_op = (VO_MEM_OPERATOR *)p_user_op->memData;

			if (p_mem_op && p_mem_op->Free) {
				p_mem_op->Free(codec_id, p_ptr);//TBD

			}
		}
	}
}


void SetMem(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_PTR p_dst, VO_U8 value, VO_U32 size)
{
	if (!p_user_op) {/* default memory*/
		memset(p_dst, value, size);
	} else {
		if (p_user_op->memflag == VO_IMF_USERMEMOPERATOR) { 
			VO_MEM_OPERATOR *p_mem_op = (VO_MEM_OPERATOR *)p_user_op->memData;

			if (p_mem_op&&p_mem_op->Set) {
				p_mem_op->Set(codec_id, p_dst, value, size); 
			}
		} else {//TBD
			memset(p_dst, value, size);
		}
	}
}
VOINLINE void CopyMem(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_PTR p_dst,VO_CPTR p_src, VO_U32 size)
{
	if (!p_user_op) {/* default memory*/
#if defined(VOARMV7)&&USE_INLINE_ASM
        VO_U8 * pSrcU8 = (VO_U8 *)p_src;
        VO_U8 * pDstU8 = (VO_U8 *)p_dst;
        VO_S32 length = size;
            __asm__ volatile
                (
                "   subs      %2, %2, #16                   \n\t"
                "   blt      2f                             \n\t"
                "1: vld1.u8 {d0, d1}, [%0]!                 \n\t"               
                "   vst1.u8 {d0, d1}, [%1]!                 \n\t"
                "   subs      %2, %2, #16                   \n\t"
                "   bge       1b                            \n\t"
                "2: adds      %2,  %2, #16                   \n\t"
                "   beq       4f                            \n\t"
                "3: ldrb     r0, [%0],#1                      \n\t"
                "   strb     r0, [%1],#1                      \n\t"
                "   subs      %2, %2, #1                    \n\t"
                "   bgt       3b                            \n\t"
                "4:                                         \n\t"
                :"+r"(pSrcU8),"+r"(pDstU8),"+r"(length)
                : 
            :"memory","r0","r1", "q0"
                );
#else
    memcpy(p_dst, p_src, size);
#endif
	} else {
		if (p_user_op->memflag == VO_IMF_USERMEMOPERATOR) {  
			VO_MEM_OPERATOR *p_mem_op = (VO_MEM_OPERATOR *)p_user_op->memData;

			if (p_mem_op && p_mem_op->Copy) {
				p_mem_op->Copy(codec_id, p_dst, p_src, size); 
			}
		} else {//TBD
			memcpy(p_dst, p_src, size);
		}
	}
}

#if 0
void
SwapImage(VO_IMGYUV ** iMgDst,VO_IMGYUV ** iMgSrc)
{
	VO_IMGYUV *tmp;
	
	tmp = *iMgDst;
	*iMgDst = *iMgSrc;
	*iMgSrc = tmp;
}


void
ColoneImage(VO_CODEC_INIT_USERDATA * p_user_op,
		  VO_U32 codec_id,
		  VO_IMGYUV * iMgDst,
		   VO_IMGYUV * iMgSrc,
		   VO_U32 ExWidth,
		   VO_U32 pic_height)
{
	CopyMem(p_user_op, codec_id, iMgDst->y, iMgSrc->y, ExWidth * pic_height);
	CopyMem(p_user_op, codec_id, iMgDst->u, iMgSrc->u, ExWidth * pic_height / 4);
	CopyMem(p_user_op, codec_id, iMgDst->v, iMgSrc->v, ExWidth * pic_height / 4);
}

#if defined(VOARMV7) || defined(VOARMV6)
extern void fill_edge_y_armv6(VO_U8 *src_in, const VO_U32 ExWidth, VO_U32 width, VO_U32 height, VO_U32 edge);
extern void fill_edge_uv_armv6(VO_U8 *src_in, const VO_U32 ExWidth, VO_U32 width, VO_U32 height, VO_U32 edge);
#endif
void
SetImageEdge(VO_CODEC_INIT_USERDATA * p_user_op,
			 VO_U32 codec_id,
			 VO_IMGYUV * image,
			   const VO_U32 ExWidthY,
			   const VO_U32 ExWidthUV,
			   const VO_U32 ExHeight,
			   VO_U32 pic_width,
			   VO_U32 pic_height)
{
#if defined(VOARMV7) || defined(VOARMV6)
	fill_edge_y_armv6(image->y, ExWidthY, pic_width, pic_height, EDGE_SIZE);
	fill_edge_uv_armv6(image->u, ExWidthUV, pic_width/2, pic_height/2, EDGE_SIZE2);
	fill_edge_uv_armv6(image->v, ExWidthUV, pic_width/2, pic_height/2, EDGE_SIZE2);		
#else
	const VO_U32 edged_width2 = ExWidthUV;
	VO_U32 width2, height2;
	VO_U32 i;
	VO_U8 *dst;
	VO_U8 *src;

	dst = image->y - (EDGE_SIZE + EDGE_SIZE * ExWidthY);
	src = image->y;

	pic_width  = (pic_width+15)&~15;
	pic_height = (pic_height+15)&~15;


	width2 = pic_width>>1;
	height2 = pic_height>>1;

	for (i = 0; i < EDGE_SIZE; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE);
		CopyMem(p_user_op, codec_id, dst + EDGE_SIZE, src, pic_width);
		SetMem(p_user_op, codec_id, dst + EDGE_SIZE + pic_width, *(src + pic_width - 1),
			   EDGE_SIZE);
		dst += ExWidthY;
	}

	for (i = 0; i < pic_height; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE);
		SetMem(p_user_op, codec_id, dst + pic_width + EDGE_SIZE, src[pic_width - 1], EDGE_SIZE);
		dst += ExWidthY;
		src += ExWidthY;
	}

	src -= ExWidthY;
	for (i = 0; i < EDGE_SIZE; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE);
		CopyMem(p_user_op, codec_id, dst + EDGE_SIZE, src, pic_width);
		SetMem(p_user_op, codec_id, dst + pic_width + EDGE_SIZE, *(src + pic_width - 1),
				   EDGE_SIZE);
		dst += ExWidthY;
	}


	/* U */
	dst = image->u - (EDGE_SIZE2 + EDGE_SIZE2 * edged_width2);
	src = image->u;
	
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE2);
		CopyMem(p_user_op, codec_id, dst + EDGE_SIZE2, src, width2);
		SetMem(p_user_op, codec_id, dst + width2 + EDGE_SIZE2, *(src + width2 - 1),
			EDGE_SIZE2);
		dst += edged_width2;
	}
	
	for (i = 0; i < height2; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE2);
		SetMem(p_user_op, codec_id, dst + width2 + EDGE_SIZE2, src[width2 - 1], EDGE_SIZE2);
		dst += edged_width2;
		src += edged_width2;
	}
	src -= edged_width2;
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE2);
		CopyMem(p_user_op, codec_id, dst + EDGE_SIZE2, src, width2);
		SetMem(p_user_op, codec_id, dst + width2 + EDGE_SIZE2, *(src + width2 - 1),
			EDGE_SIZE2);
		dst += edged_width2;
	}
	
	
	/* V */
	dst = image->v - (EDGE_SIZE2 + EDGE_SIZE2 * edged_width2);
	src = image->v;
	
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE2);
		CopyMem(p_user_op, codec_id, dst + EDGE_SIZE2, src, width2);
		SetMem(p_user_op, codec_id, dst + width2 + EDGE_SIZE2, *(src + width2 - 1),
			EDGE_SIZE2);
		dst += edged_width2;
	}
	
	for (i = 0; i < height2; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE2);
		SetMem(p_user_op, codec_id, dst + width2 + EDGE_SIZE2, src[width2 - 1], EDGE_SIZE2);
		dst += edged_width2;
		src += edged_width2;
	}
	src -= edged_width2;
	for (i = 0; i < EDGE_SIZE2; i++) {
		SetMem(p_user_op, codec_id, dst, *src, EDGE_SIZE2);
		CopyMem(p_user_op, codec_id, dst + EDGE_SIZE2, src, width2);
		SetMem(p_user_op, codec_id, dst + width2 + EDGE_SIZE2, *(src + width2 - 1),
			EDGE_SIZE2);
		dst += edged_width2;
	}
#endif
}

VO_S32 GetFrameBufIdx(VO_IMGYUV* img, VO_IMGYUV* img_seq)
{	
	VO_S32 i;

	for(i = 0; i < MAXFRAMES; i++){
		if(img == (img_seq + i)){
			return i;
		}
	}
	return 0;
}


VO_IMGYUV*  FrameBufCtl(FIFOTYPE *priv, VO_IMGYUV* img , const VO_U32 flag)
{
    VO_IMGYUV *temp = NULL;
    VO_U32 w_idx = 0;

    w_idx = priv->w_idx;    /* Save the old index before proceeding */
    if (flag == FIFO_WRITE){ /*write data*/

        /* Save it to buffer */
        if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
            /* Adjust read index since buffer is full */
            /* Keep the latest one and drop the oldest one */
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
        }
        priv->img_seq[priv->w_idx] = img;
        priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;

    } else {/*read data*/
		if ((priv->r_idx == w_idx)){
            return NULL;/*there is no free buffer*/
		} else {
            temp = priv->img_seq[priv->r_idx];
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
        }
    }
    return temp;

}

#endif