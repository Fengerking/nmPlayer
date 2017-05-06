/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/
#include <math.h>
#include <stdlib.h>
#include "voH264EncGlobal.h"
#include "voH264EncRC.h"
#include "voH264EncME.h"
#include "voCheck.h"
#include "voH264Frame.h"
#include "voH264EncBitStream.h"


#ifdef CAL_MEM
long total_mem;
#endif


#ifdef CAL_PSNR
#include <math.h>
long sse_y;     /* [out] Y plane's sse */
long sse_u;     /* [out] U plane's sse */
long sse_v;     /* [out] V plane's sse */
#define SSEPSNR(sse, pel_count) ((!(sse))?0.0f : 48.131f - 10*(float)log10((float)(sse)/((float)((pel_count)))))
float total_psnr_y, total_psnr_u,total_psnr_v;

VO_U32
sse8_8bit_c(const VO_U8 * b1,
			const VO_U8 * b2,
			const VO_U32 stride)
{
	VO_S32 i;
	VO_S32 sse = 0;
	
	for (i=0; i<8; i++) {
		sse += (b1[0] - b2[0])*(b1[0] - b2[0]);
		sse += (b1[1] - b2[1])*(b1[1] - b2[1]);
		sse += (b1[2] - b2[2])*(b1[2] - b2[2]);
		sse += (b1[3] - b2[3])*(b1[3] - b2[3]);
		sse += (b1[4] - b2[4])*(b1[4] - b2[4]);
		sse += (b1[5] - b2[5])*(b1[5] - b2[5]);
		sse += (b1[6] - b2[6])*(b1[6] - b2[6]);
		sse += (b1[7] - b2[7])*(b1[7] - b2[7]);
		
		b1 = b1+stride;
		b2 = b2+stride;
	}
	
	return(sse);
}


long vo_sse(VO_U8 *orig,VO_U8 *rec,VO_S32 stride,VO_S32 width,VO_S32 height)
{
	VO_S32 j, iwidth,iheight;
	long sse = 0;
	
	iwidth  = width  & (~0x07);
	iheight = height & (~0x07);
	
	for (j = 0; j<iheight; j += 8) {
		VO_S32 i;
		
		for (i = 0; i<iwidth; i += 8)
			sse += sse8_8bit_c(orig  + i, rec + i, stride);
		
		for (i = iwidth; i < width; i++) {
			VO_S32 val;
			val = *(orig + 0*stride + i) - *(rec + 0*stride + i);
			sse += val * val;
			val = *(orig + 1*stride + i) - *(rec + 1*stride + i);
			sse += val * val;
			val = *(orig + 2*stride + i) - *(rec + 2*stride + i);
			sse += val * val;
			val = *(orig + 3*stride + i) - *(rec + 3*stride + i);
			sse += val * val;
			val = *(orig + 4*stride + i) - *(rec + 4*stride + i);
			sse += val * val;
			val = *(orig + 5*stride + i) - *(rec + 5*stride + i);
			sse += val * val;
			val = *(orig + 6*stride + i) - *(rec + 6*stride + i);
			sse += val * val;
			val = *(orig + 7*stride + i) - *(rec + 7*stride + i);
			sse += val * val;
		}
		
		orig  += 8*stride;
		rec += 8*stride;
	}
	
	/* Compute the down rectangle sse */
	for (j = iheight; j < height; j++) {
		VO_S32 i;
		for (i = 0; i < width; i++) {
			VO_S32 val;
			val = *(orig + i) - *(rec + i);
			sse += val * val;
		}
		orig += stride;
		rec += stride;
	}
	
	return (sse);
}
#endif

#ifdef RECON_FILE
FILE *rec_file;
#endif


VO_HANDLE voH264EnchInst = NULL;


#define NALU_nOverHead 5 // startcode + NAL type costs 5 bytes per frame

//#define BitUE BitUEBig

static VO_S32 voH264EncPicEnd( H264ENC *pEncGlobal);




/****************************************************************************
 * IniInternParam:
 ****************************************************************************/
void    IniInternParam( INTER_PARAM *InternalParam )
{
    /* */
    memset( InternalParam, 0, sizeof( INTER_PARAM ) );


    /* Video properties */

    InternalParam->i_slice_max_size = 0;
    InternalParam->i_slice_max_mbs = 0;
    InternalParam->i_slice_count = 0;

    /* Encoder parameters */
    InternalParam->i_deblocking_filter_alphac0 = 0;
    InternalParam->i_deblocking_filter_beta = 0;

    InternalParam->i_qp_min = 10;
    InternalParam->i_qp_max = 51;
    InternalParam->i_me_range = 16;

    InternalParam->i_mv_range = -1; // set from level_idc
    InternalParam->b_dct_decimate = 1;
    InternalParam->b_annexb = 1;
	
	//add by Really Yang
	InternalParam->i_analyse_flags = 0;
	//end of add

}



VO_S32 EncodeNAL( VO_U8 *dst, VO_S32 b_annexb, VO_NAL *nal )
{
    VO_U8 *src = nal->p_payload;
    VO_U8 *end = nal->p_payload + nal->i_payload;
    VO_U8 *orig_dst = dst;
    VO_S32 i_count = 0, size;

    /* long nal start code (we always use long ones) */
    if( b_annexb )
    {
        *dst++ = 0x00;
        *dst++ = 0x00;
        *dst++ = 0x00;
        *dst++ = 0x01;
    }
    else /* save room for size later */
        dst += 4;

    /* nal header */
    *dst++ = (VO_U8)(( 0x00 << 7 ) | ( nal->i_ref_idc << 5 ) | nal->nal_type);

    while( src < end )
    {
        if( i_count == 2 && *src <= 0x03 )
        {
            *dst++ = 0x03;
            i_count = 0;
        }
        if( *src == 0 )
            i_count++;
        else
            i_count = 0;
        *dst++ = *src++;
    }
    size = (dst - orig_dst) - 4;

    /* Write the size header for mp4/etc */
    if( !b_annexb )
    {
        /* Size doesn't include the size of the header we're writing now. */
        orig_dst[0] = (VO_U8)(size>>24);
        orig_dst[1] = (VO_U8)(size>>16);
        orig_dst[2] = (VO_U8)(size>> 8);
        orig_dst[3] = (VO_U8)(size>> 0);
    }

    return size+4;
}

void *voMalloc( VO_S32 nSize )
{
    VO_U8 *align_buf = NULL;

    VO_U8 *buf = (VO_U8 *)malloc( nSize + 15 + sizeof(void **) + sizeof(VO_S32) );
    if( buf )
    {
        align_buf = buf + 15 + sizeof(void **) + sizeof(VO_S32);
        align_buf -= (int) align_buf & 15;
        *( (void **) ( align_buf - sizeof(void **) ) ) = buf;
        *( (VO_S32 *) ( align_buf - sizeof(void **) - sizeof(VO_S32) ) ) = nSize;
    }

    if( !align_buf )
        return 0;
#ifdef CAL_MEM
	total_mem += nSize;
#endif

    return align_buf;
}

void voFree( void *p )
{
    if( p )
    {
        free( *( ( ( void **) p ) - 1 ) );

    }
}



/****************************************************************************
 * voMalloc:
 ****************************************************************************/
VO_VOID * voH264Malloc(H264ENC *enc_hnd, VO_U32 size, VO_U8 alignment)	//The alignment must be multiple of 4.
{
	VO_U8 *mem_ptr;

#ifdef MAX_RAM_USED_CHECK
	g_RamUsed += size + alignment;
#endif

	if(enc_hnd) {
		if(enc_hnd->memOperater) {
			if (!alignment) {
				voMemAlloc(mem_ptr, enc_hnd->memOperater, enc_hnd->codec_id, size+1);
				if(mem_ptr == NULL) {
					return(NULL);
				}
				*mem_ptr = (VO_U8)1;
				return ((VO_VOID *)(mem_ptr+1));
			} else {
				VO_U8 *tmp;
				voMemAlloc(tmp, enc_hnd->memOperater, enc_hnd->codec_id, size+alignment);
				if(tmp == NULL) {
					return(NULL);
				}
				mem_ptr = (VO_U8 *) ((VO_U32) (tmp + alignment - 1) & (~(VO_U32) (alignment - 1)));
				if (mem_ptr == tmp)
					mem_ptr += alignment;
				*(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);
				return ((VO_VOID *)mem_ptr);
			}
		}
	}

	if (!alignment) {

		/* We have not to satisfy any alignment */
		if ((mem_ptr = (VO_U8 *) malloc(size + 1)) != NULL) {

			/* Store (mem_ptr - "real allocated memory") in *(mem_ptr-1) */
			*mem_ptr = (VO_U8)1;

			/* Return the mem_ptr pointer */
			return ((VO_VOID *)(mem_ptr+1));
		}
	} else {
		VO_U8 *tmp;

		/* Allocate the required size memory + alignment so we
		 * can realign the data if necessary */
		if ((tmp = (VO_U8 *) malloc(size + alignment)) != NULL) {

			/* Align the tmp pointer */
			mem_ptr =
				(VO_U8 *) ((VO_U32) (tmp + alignment - 1) &
							 (~(VO_U32) (alignment - 1)));

			/* Special case where malloc have already satisfied the alignment
			 * We must add alignment to mem_ptr because we must store
			 * (mem_ptr - tmp) in *(mem_ptr-1)
			 * If we do not add alignment to mem_ptr then *(mem_ptr-1) points
			 * to a forbidden memory space */
			if (mem_ptr == tmp)
				mem_ptr += alignment;

			/* (mem_ptr - tmp) is stored in *(mem_ptr-1) so we are able to retrieve
			 * the real malloc block allocated and free it in voMpegFree */
			*(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);

			/* Return the aligned pointer */
			return ((VO_VOID *)mem_ptr);
		}
	}

	return(NULL);
}

/****************************************************************************
 * voFree:
 ****************************************************************************/
VO_VOID voH264Free(H264ENC *enc_hnd, VO_VOID *mem_ptr)
{

	VO_U8 *ptr;

	if (mem_ptr == NULL)
		return;

	if(enc_hnd) {
		if(enc_hnd->memOperater) {
			ptr = mem_ptr;
			ptr -= *(ptr - 1);
			enc_hnd->memOperater->Free(enc_hnd->codec_id, ptr); 
			return;
		}
	}

	/* Aligned pointer */
	ptr = mem_ptr;

	/* *(ptr - 1) holds the offset to the real allocated block
	 * we sub that offset os we free the real pointer */
	ptr -= *(ptr - 1);

	/* Free the memory */
	free(ptr);
}



static void InitSliceHdr( H264ENC *pEncGlobal, H264SLICEHEADER *sh)
{
	OUT_PARAM *OutParam = &pEncGlobal->OutParam;
    INTER_PARAM *InternalParam = &pEncGlobal->InternalParam;
	VO_S32 nQP =  RCMBQp( pEncGlobal ) ;


    sh->nStartMB  = 0;
    sh->nEndMB   = pEncGlobal->mb_pic - 1;

    sh->nQP = nQP;

    /* If effective qp <= 15, deblocking would have no effect anyway */
    if( OutParam->b_deblocking_filter
        && ( 15 < nQP + 2 * AVC_MIN(InternalParam->i_deblocking_filter_alphac0, InternalParam->i_deblocking_filter_beta) ) )
    {
        sh->bDisableDeblockIdc = 0;
    }
    else
    {
        sh->bDisableDeblockIdc = 1;
    }
    sh->nAlphaC0Offset = InternalParam->i_deblocking_filter_alphac0 << 1;
    sh->nBetaOffset = InternalParam->i_deblocking_filter_beta << 1;
}


/*ready*/
static void WriteSliceHdr (H264ENC *pEncGlobal, BS_TYPE *s, H264SLICEHEADER *sh )
{
  LargeUEBits( s, sh->nStartMB );
  UEBits( s, (pEncGlobal->pic_type == P_PIC_TYPE ?0:2) + 5 ); 
  UEBits( s,  0);//pps_id
  PutBits( s, pEncGlobal->log2_max_frame_num, pEncGlobal->nLFrameNum & ((1<<pEncGlobal->log2_max_frame_num)-1) );
  if( pEncGlobal->pic_type == IDR_PIC_TYPE )
    LargeUEBits( s, pEncGlobal->nIDRPicID );

  PutBits( s, pEncGlobal->log2_max_poc_lsb, 2 * pEncGlobal->nLFrameNum  & ((1<<pEncGlobal->log2_max_poc_lsb)-1) );

  if( pEncGlobal->pic_type == P_PIC_TYPE )
	PutBits(s,4,12);//num_ref_idx_override:1 ,i_num_ref_idx_l0_active - 1:0 ,b_ref_pic_list_reordering_l0:0 ,adaptive_ref_pic_marking_mode_flag:0
  else         
	PutBits(s,2,0);//no output of prior pics flag:0 ,  long term reference flag:0
	
  SEBits( s, sh->nQP - 26 );// slice qp delta 

  if( pEncGlobal->OutParam.b_deblocking_filter )
  {
    UEBits( s, sh->bDisableDeblockIdc );
    if( sh->bDisableDeblockIdc != 1 )
	{
      SEBits( s, sh->nAlphaC0Offset >> 1 );
      SEBits( s, sh->nBetaOffset >> 1 );
    }
  }
}


static VO_S32 VerifyParam( INTER_PARAM *InternalParam, OUT_PARAM* OutParam)
{
	VO_S32 max_slices;
	
	/* out parameters*/
    if( OutParam->width <= 0 || OutParam->height <= 0 || OutParam->width % 2 || OutParam->height % 2){
        return -1;//YU_TBD
    }

//	OutParam->max_iframe_interval = 250;

    if( OutParam->max_iframe_interval <= 0 )
        OutParam->max_iframe_interval = 1;

    OutParam->i_keyint_min = AVSClip3( OutParam->i_keyint_min, 1, OutParam->max_iframe_interval/2+1 );

	/* internal parameters*/

    InternalParam->i_qp_max = AVSClip3( InternalParam->i_qp_max, 0, 51 );
    InternalParam->i_qp_min = AVSClip3( InternalParam->i_qp_min, 0, InternalParam->i_qp_max );

    max_slices = (OutParam->height+(16-1))/(16);

	InternalParam->i_slice_count = AVSClip3( InternalParam->i_slice_count, 0, max_slices );
	InternalParam->i_slice_max_size = AVC_MAX( InternalParam->i_slice_max_size, 0 );
	InternalParam->i_slice_max_mbs = AVC_MAX( InternalParam->i_slice_max_mbs, 0 );

	if( InternalParam->i_slice_max_mbs || InternalParam->i_slice_max_size )
		InternalParam->i_slice_count = 0;

    InternalParam->i_deblocking_filter_alphac0 = AVSClip3( InternalParam->i_deblocking_filter_alphac0, -6, 6 );
    InternalParam->i_deblocking_filter_beta    = AVSClip3( InternalParam->i_deblocking_filter_beta, -6, 6 );

    return 0;
}

VO_S32 CalMemSize( H264ENC *pEncGlobal, OUT_PARAM *OutParam )
{
  VO_S32 b_size = 0;
  VO_S32 t_size = 0;
  VO_S32 i;
  VO_S32 i_stride, i_width, height;
  pEncGlobal->buffer_used = 0;
  //quant_table
  b_size += 52*16*sizeof(VO_U16);
  //dequant_table
  b_size += 6*16*sizeof(VO_S32);
  //quant_bias[0]
  b_size += 52*16*sizeof(VO_U16);
  //quant_bias[1]
  b_size += 52*16*sizeof(VO_U16);
  //mv_cost 29 not aligned with 32bit so plus 1
  b_size += (4*4*2048 + 1 + 1) * sizeof(VO_U16)*29;
  //YU_TBD nal buffer, write to pOutBuf directly
  pEncGlobal->out.nNAL = 0;
  pEncGlobal->out.nBSSize = (VO_S32)(AVC_MAX( 1000000, pEncGlobal->OutParam.width * pEncGlobal->OutParam.height * 4
        * ( pow( 0.95, pEncGlobal->InternalParam.i_qp_min ))));
  b_size += pEncGlobal->out.nBSSize;

  t_size = pEncGlobal->mb_width * sizeof(VO_S8);
  CHECK_SIZE(t_size);
  b_size += t_size;
  //mv
  b_size += 2*16 * pEncGlobal->mb_pic * sizeof(VO_S16);
  //ref
  b_size += 4 * pEncGlobal->mb_width * sizeof(VO_S8);
  //qp
  t_size = pEncGlobal->mb_pic * sizeof(VO_S8);
  CHECK_SIZE(t_size);
  b_size += t_size;
  //mzc_mbrow
  b_size += pEncGlobal->mb_width *24 * sizeof(VO_U8);
  //LastMBRowLine y
  b_size += pEncGlobal->mb_width*16+32;
  //LastMBRowLine u
  b_size += (pEncGlobal->mb_width*16+32)>>1;
  //LastMBRowLine v
  b_size += (pEncGlobal->mb_width*16+32)>>1;
  //frame parameter
  i_width  = ALIGN( pEncGlobal->OutParam.width, 16 );
  i_stride = ALIGN( i_width + 2*PADH, 64 );
  height  = ALIGN( pEncGlobal->OutParam.height, 16);
  for( i = 0; i < 3; i++ )
  {
    pEncGlobal->i_stride[i] = ALIGN( i_stride >> !!i, 64 );
    pEncGlobal->i_width[i] = i_width >> !!i;
    pEncGlobal->height[i] = height >> !!i;
  }
  //fdec
  t_size = sizeof(AVC_FRAME);
  CHECK_SIZE(t_size);
  b_size += t_size;
  b_size += pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 64);
  b_size += pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 64);
  b_size += pEncGlobal->i_stride[0] * (pEncGlobal->height[0] + 64);
  //fref0
  b_size += t_size;
  b_size += pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 64);
  b_size += pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 64);
  b_size += pEncGlobal->i_stride[0] * (pEncGlobal->height[0] + 64);
#ifdef CAL_PSNR
  //orig
  b_size += t_size;
  b_size += pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 64);
  b_size += pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 64);
  b_size += pEncGlobal->i_stride[0] * (pEncGlobal->height[0] + 64);
#endif
  //rc
  t_size = sizeof(RateControl);
  CHECK_SIZE(t_size);
  b_size += t_size;
  return b_size;
}


VO_S32 H264EncCreate( H264ENC *pEncGlobal, OUT_PARAM *OutParam )
{
    VO_S32 i, qp,total_size,tmp_size;

#ifdef RECON_FILE
	rec_file = fopen("d:\\recon264.yuv", "w+b");
#endif
	

    /* Create a copy of InternalParam */
	memcpy(&pEncGlobal->OutParam, OutParam, sizeof(OUT_PARAM));
	
	
    if( VerifyParam( &pEncGlobal->InternalParam,  &pEncGlobal->OutParam))
        goto fail;

    /* Init H264ENC */
    pEncGlobal->mb_width = ( OutParam->width + 15 ) / 16;
    pEncGlobal->mb_height= ( OutParam->height + 15 ) / 16;
    pEncGlobal->mb_pic = pEncGlobal->mb_width * pEncGlobal->mb_height;
    pEncGlobal->mb_stride = pEncGlobal->mb_width;

	pEncGlobal->frame_crop_right_offset = ((16-OutParam->width) & 15)>>1;
	pEncGlobal->frame_crop_bottom_offset = ((16-OutParam->height) & 15)>>1;
	if(pEncGlobal->frame_crop_right_offset||pEncGlobal->frame_crop_bottom_offset)
	  pEncGlobal->frame_cropping_flag = 1;


    pEncGlobal->nGFrameNum = -1;
    pEncGlobal->nLFrameNum = pEncGlobal->OutParam.max_iframe_interval;
    pEncGlobal->nIDRPicID = 0;
    pEncGlobal->pChromaQP = QP_chroma + 12 ;
	
	total_size = CalMemSize(pEncGlobal,OutParam);
	//CHECKED_MALLOCZERO( pEncGlobal->buffer_total, total_size);
	pEncGlobal->buffer_total = voH264Malloc (pEncGlobal,total_size,64);
	memset(pEncGlobal->buffer_total,0,total_size);
	/* Init sps*/
	IniSPS(pEncGlobal, &pEncGlobal->OutParam );

	/*YU_TBD*/

	pEncGlobal->InternalParam.i_mv_range = pEncGlobal->level->mv_range;

    if( InitCQM( pEncGlobal ))
        goto fail;


    for( qp = pEncGlobal->InternalParam.i_qp_min; qp <= pEncGlobal->InternalParam.i_qp_max; qp++ )
        if( InitMVCost( pEncGlobal, qp ) )
            goto fail;


    if( pEncGlobal->cost_mv[1][2013] != 24 ){
        goto fail;
    }
	//CHECKED_MALLOCZERO( pEncGlobal->out.pBitstream, pEncGlobal->out.nBSSize );
	MEMORY_ALLOC( pEncGlobal->out.pBitstream, pEncGlobal->buffer_total , pEncGlobal->out.nBSSize, pEncGlobal->buffer_used,VO_U8*);


	pEncGlobal->out.nNALNum = pEncGlobal->InternalParam.i_slice_count + 4;
	tmp_size = pEncGlobal->out.nNALNum*sizeof(VO_NAL);
	//CHECK_SIZE(tmp_size);
	//MEMORY_ALLOC( pEncGlobal->out.nal, pEncGlobal->buffer_total , tmp_size, pEncGlobal->buffer_used);
    pEncGlobal->out.nal = voH264Malloc (pEncGlobal,tmp_size,64);
	//CHECKED_MALLOCZERO( pEncGlobal->nMBType, pEncGlobal->mb_width * sizeof(VO_S8));
	tmp_size = pEncGlobal->mb_width * sizeof(VO_S8);
	CHECK_SIZE(tmp_size);
	MEMORY_ALLOC( pEncGlobal->nMBType, pEncGlobal->buffer_total , tmp_size, pEncGlobal->buffer_used,VO_S8*);
	
	//CHECKED_MALLOCZERO( pEncGlobal->mv, 2*16 * pEncGlobal->mb_pic * sizeof(VO_S16) );
	MEMORY_ALLOC( pEncGlobal->mv, pEncGlobal->buffer_total , 2*16 * pEncGlobal->mb_pic * sizeof(VO_S16), pEncGlobal->buffer_used,VO_S16 (*)[2]);
	//MEMORY_ALLOC( pEncGlobal->mv[1], pEncGlobal->buffer_total , 16 * pEncGlobal->mb_pic * sizeof(VO_S16), pEncGlobal->buffer_used,short*);

	//CHECKED_MALLOCZERO( pEncGlobal->ref, 4 * pEncGlobal->mb_width * sizeof(VO_S8) ); 
	MEMORY_ALLOC( pEncGlobal->ref, pEncGlobal->buffer_total , 4 * pEncGlobal->mb_width * sizeof(VO_S8), pEncGlobal->buffer_used,VO_S8*);

    //CHECKED_MALLOCZERO( pEncGlobal->qp, pEncGlobal->mb_pic * sizeof(VO_S8) );
	tmp_size = pEncGlobal->mb_pic * sizeof(VO_S8);
	CHECK_SIZE(tmp_size);
	MEMORY_ALLOC( pEncGlobal->qp, pEncGlobal->buffer_total , tmp_size, pEncGlobal->buffer_used,VO_S8*);
    /* all coeffs */
    //CHECKED_MALLOCZERO( pEncGlobal->mzc_mbrow, pEncGlobal->mb_width *24 * sizeof(VO_U8) );
	MEMORY_ALLOC( pEncGlobal->mzc_mbrow, pEncGlobal->buffer_total , pEncGlobal->mb_width *24 * sizeof(VO_U8), pEncGlobal->buffer_used,VO_U8(*)[16+4+4]);

	//CHECKED_MALLOCZERO( pEncGlobal->mvr, 2 * pEncGlobal->mb_pic * sizeof(VO_S16) );

	/*backup for intra prediction*/
	for( i=0; i<3; i++ ){
		/* shouldn't really be initialized, just silences a valgrind false-positive in predict_8x8_filter_mmx */
		//CHECKED_MALLOCZERO( pEncGlobal->mb.LastMBRowLine[i], (pEncGlobal->mb_width*16+32)>>!!i );
		MEMORY_ALLOC( pEncGlobal->mb.LastMBRowLine[i], pEncGlobal->buffer_total , (pEncGlobal->mb_width*16+32)>>!!i, pEncGlobal->buffer_used,VO_U8*);
		pEncGlobal->mb.LastMBRowLine[i] += 8;
	}

    /* init with not available (for top right idx=7,15) */
    memset( pEncGlobal->mb.ref, -2, AVC_SCAN8_SIZE * sizeof( VO_S8 ) );


	//CHECKED_MALLOC( pEncGlobal->fenc_buf, 24*FENC_STRIDE * sizeof(VO_U8) );

    pEncGlobal->pCur[0] = pEncGlobal->compress_cache;
    pEncGlobal->pCur[1] = pEncGlobal->compress_cache + 16*FENC_STRIDE;
    pEncGlobal->pCur[2] = pEncGlobal->compress_cache + 16*FENC_STRIDE + 8;

	//CHECKED_MALLOC( pEncGlobal->fdec_buf, 27*FENC_STRIDE * sizeof(VO_U8) );


    pEncGlobal->pDec[0] = pEncGlobal->recon_cache + 2*FDEC_STRIDE;  
    pEncGlobal->pDec[1] = pEncGlobal->recon_cache + 19*FDEC_STRIDE;
    pEncGlobal->pDec[2] = pEncGlobal->recon_cache + 19*FDEC_STRIDE + 16;


	pEncGlobal->fdec = AVCCrateFrame( pEncGlobal);
	if( !pEncGlobal->fdec )
		return -1;

	pEncGlobal->fref0 = AVCCrateFrame( pEncGlobal);
	if( !pEncGlobal->fref0 )
		return -1;
#ifdef CAL_PSNR
	pEncGlobal->orig = AVCCrateFrame( pEncGlobal);
	if( !pEncGlobal->orig )
		return -1;
#endif

    if( pRCreate( pEncGlobal ))
        goto fail;

    return 0;
fail:
    //voFree( pEncGlobal );
    return -1;
}



/* internal usage */
static void InitNAL( H264ENC *pEncGlobal, VO_S32 nal_type, VO_S32 i_ref_idc )
{
    VO_NAL *nal = &pEncGlobal->out.nal[pEncGlobal->out.nNAL];

    nal->i_ref_idc = i_ref_idc;
    nal->nal_type    = nal_type;

    nal->i_payload= 0;
    nal->p_payload= &pEncGlobal->out.pBitstream[GetBitsPos( &pEncGlobal->out.bs ) / 8];
}
/* if number of allocated nals is not enough, re-allocate a larger one. */
static VO_S32 CheckOutBuffer( H264ENC *pEncGlobal )
{
    if( pEncGlobal->out.nNAL >= pEncGlobal->out.nNALNum )
    {
		VO_NAL *new_out = voH264Malloc (pEncGlobal,sizeof(VO_NAL) * (pEncGlobal->out.nNALNum*2),64);
        if( !new_out )
            return -1;
        memcpy( new_out, pEncGlobal->out.nal, sizeof(VO_NAL) * (pEncGlobal->out.nNALNum) );
		//if(pEncGlobal->out.nNALNum != 4)//we don't free first nal buffer because it belongs to the buffer_total
          voH264Free(pEncGlobal, pEncGlobal->out.nal );
        pEncGlobal->out.nal = new_out;
        pEncGlobal->out.nNALNum *= 2;
    }
    return 0;
}
static VO_S32 NALEnd( H264ENC *pEncGlobal )
{
    VO_NAL *nal = &pEncGlobal->out.nal[pEncGlobal->out.nNAL];
    nal->i_payload = &pEncGlobal->out.pBitstream[GetBitsPos( &pEncGlobal->out.bs ) / 8] - nal->p_payload;
    pEncGlobal->out.nNAL++;

    return CheckOutBuffer( pEncGlobal );
}

static VO_S32 EncoderEncapsulateNALS( H264ENC *pEncGlobal )
{
    VO_S32 nal_size = 0, i;
	VO_U8 *pOutBuf;

    for( i = 0; i < pEncGlobal->out.nNAL; i++ )
        nal_size += pEncGlobal->out.nal[i].i_payload;


	//YU_TBD
    /* Worst-case NAL unit escaping: reallocate the buffer if it's too small. */
   // if( pEncGlobal->nNALBufSize < nal_size * 3/2 + pEncGlobal->out.nNAL * 4 )


    pOutBuf = pEncGlobal->pOutBuf;

    for( i = 0; i < pEncGlobal->out.nNAL; i++ )
    {
        VO_S32 size = EncodeNAL( pOutBuf, pEncGlobal->InternalParam.b_annexb, &pEncGlobal->out.nal[i] );
        //pEncGlobal->out.nal[i].i_payload = size;
        //pEncGlobal->out.nal[i].p_payload = pOutBuf;
        pOutBuf += size;
    }

    return pOutBuf - pEncGlobal->pOutBuf;
}


static VOINLINE void StartMBRow( H264ENC *pEncGlobal )
{
	M32(pEncGlobal->mb.top_left_pix) = 0;//YU_TBD the first mb, plane module is avaiable?

}

static VOINLINE void SwitchPic( H264ENC *pEncGlobal )
{
	AVC_FRAME    *tmp;

	tmp = pEncGlobal->fref0;
	pEncGlobal->fref0 = pEncGlobal->fdec;
	pEncGlobal->fdec = tmp;
}

const VO_S32 th_table[52] = 
{
  20, 22, 26, 28, 32, 36, 40, 44,  
  52, 56, 64, 72, 80, 88, 104, 112, 
  128, 144, 160, 176, 208, 224, 256, 288, 
  320, 352, 416, 448, 512, 576, 640, 704,
  832, 896, 1024, 1152, 1280, 1408, 1664, 1792,
  2048, 2304, 2560, 2816, 3328, 3584, 4096, 4608,
  5120, 5632, 6656, 7168  
};

static VO_S32 voH264ENCSlice( H264ENC *pEncGlobal )
{
    VO_S32 skip;
    VO_S32 nMBX, nMBY;
	const VO_S32 mb_with = pEncGlobal->mb_width;
	const VO_S32 nEndMB = pEncGlobal->sh.nEndMB;

	H264ENC_L pEncLocal;

    RealignBits( &pEncGlobal->out.bs );//YU_TBD

	InitNAL( pEncGlobal, pEncGlobal->pic_type == IDR_PIC_TYPE ? VO_NAL_SLICE_IDR : VO_NAL_SLICE, pEncGlobal->pic_type == IDR_PIC_TYPE ? VO_NAL_PRIORITY_HIGHEST: VO_NAL_PRIORITY_HIGH );//YU_TBD remove it

    WriteSliceHdr (pEncGlobal, &pEncGlobal->out.bs, &pEncGlobal->sh);

    pEncLocal.nLastQP = pEncGlobal->sh.nQP;

    nMBY = pEncGlobal->sh.nStartMB / mb_with;
    nMBX = pEncGlobal->sh.nStartMB % mb_with;
    skip = 0;
	pEncLocal.skip_th = th_table[pEncLocal.nLastQP];


    while( (nMBX + nMBY * mb_with) <= nEndMB ){
        if( nMBX == 0 )
            StartMBRow( pEncGlobal); 

        /* load cache */

        EncodeMBStart( pEncGlobal, nMBX, nMBY, &pEncLocal );

        MBEstimation( pEncGlobal, &pEncLocal);
		
        MBCompensation( pEncGlobal, &pEncLocal);

		if( IS_SKIP( pEncLocal.nMBType ) )
			skip++;
		else{
			if(pEncGlobal->pic_type  == P_PIC_TYPE ){
			
				LargeUEBits( &pEncGlobal->out.bs, skip );  /* skip run */
				skip = 0;
			}

			WriteMBCavlc( pEncGlobal, &pEncLocal );
		}

        EncodeMBEnd( pEncGlobal, &pEncLocal);

        RCMBUpdate( pEncGlobal,  &pEncLocal);

        nMBX++;
        if( nMBX == mb_with)
		{
            nMBY++;
            nMBX = 0;
        }
    }

	if( skip > 0 )
		LargeUEBits( &pEncGlobal->out.bs, skip );  // last skip run 

	// rbsp_slice_trailing_bits 
	RBSPBits( &pEncGlobal->out.bs );
	FlushBits( &pEncGlobal->out.bs );

    if( NALEnd( pEncGlobal ) )
        return -1;

    return 0;
}


static VO_S32 voH264EncSlices( H264ENC *pEncGlobal )
{
    VO_S32 i_slice_num = 0;
    VO_S32 last_thread_mb;


	InitSliceHdr( pEncGlobal, &pEncGlobal->sh);

	last_thread_mb = pEncGlobal->sh.nEndMB;

    while( pEncGlobal->sh.nStartMB <= last_thread_mb ){
        pEncGlobal->sh.nEndMB = last_thread_mb;
        if( pEncGlobal->InternalParam.i_slice_max_mbs )
            pEncGlobal->sh.nEndMB = pEncGlobal->sh.nStartMB + pEncGlobal->InternalParam.i_slice_max_mbs - 1;
        else if( pEncGlobal->InternalParam.i_slice_count)
        {
            VO_S32 height = pEncGlobal->mb_height;
            VO_S32 width = pEncGlobal->mb_width;
            i_slice_num++;
            pEncGlobal->sh.nEndMB = (height * i_slice_num + pEncGlobal->InternalParam.i_slice_count/2) / pEncGlobal->InternalParam.i_slice_count * width - 1;
        }
        pEncGlobal->sh.nEndMB = AVC_MIN( pEncGlobal->sh.nEndMB, last_thread_mb );
        if( voH264ENCSlice(pEncGlobal ) )
            return -1;
        pEncGlobal->sh.nStartMB = pEncGlobal->sh.nEndMB + 1;
    }

    return 0;
}


VO_U32     voH264ENCProcess( VO_HANDLE hH264Enc,  VO_VIDEO_BUFFER  *pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType)      
{
	VO_S32 nOverHead;

	H264ENC *pEncGlobal = (H264ENC *)hH264Enc;

#ifdef LICENSE_CHECK
		VO_VIDEO_FORMAT video_format;
#endif


	pEncGlobal->pOutBuf = pOutput->Buffer;//YU_TBD mearge with nal_buf
#ifdef LICENSE_CHECK
		video_format.Height = pEncGlobal->OutParam.height;
		video_format.Width	= pEncGlobal->OutParam.width;
		video_format.Type = VO_VIDEO_FRAME_I;
		if(voCheckLibCheckVideo (pEncGlobal->phLicenseCheck, pInput, &video_format)!= VO_ERR_NONE)
			printf("license error\n");
		
#endif

	
	//YU_TBD change the buffer to pOutBuf
	InitBits( &pEncGlobal->out.bs, pEncGlobal->out.pBitstream );

	//YU_TBD 
    SwitchPic( pEncGlobal );

	/* copy to internal extended frame buffer*/
	if( PreprocessPic( pEncGlobal, pEncGlobal->fdec, pInput ) )
		return -1;

#ifdef CAL_PSNR
	PreprocessPic( pEncGlobal, pEncGlobal->orig, pInput );//YU_TBD change to copy picture
#endif



    pEncGlobal->nGFrameNum++;

	if( pEncGlobal->nLFrameNum  >= pEncGlobal->OutParam.max_iframe_interval ){
		pEncGlobal->pic_type = IDR_PIC_TYPE;
		*pType = VO_VIDEO_FRAME_I;
        pEncGlobal->nLFrameNum = 0;
	}else{
		pEncGlobal->pic_type = P_PIC_TYPE;
		*pType = VO_VIDEO_FRAME_P;
	}

  

	pEncGlobal->out.nNAL = 0;

    nOverHead = NALU_nOverHead;

    /* Write SPS and PPS */
	//YU_TBD keyframe, idr frame
    if(pEncGlobal->pic_type == IDR_PIC_TYPE){
		if( pEncGlobal->nGFrameNum == 0 ){

			/* generate sequence parameters */
			InitNAL( pEncGlobal, VO_NAL_SPS, VO_NAL_PRIORITY_HIGHEST );// YU_TBD remove it
			WriteSPS(pEncGlobal,  &pEncGlobal->out.bs);
			if( NALEnd( pEncGlobal ) )// YU_TBD remove it
				return -1;
			nOverHead += pEncGlobal->out.nal[pEncGlobal->out.nNAL-1].i_payload + NALU_nOverHead;// YU_TBD remove it

			/* generate picture parameters */
			InitNAL( pEncGlobal, VO_NAL_PPS, VO_NAL_PRIORITY_HIGHEST );// YU_TBD remove it
			WritePPS( pEncGlobal, &pEncGlobal->out.bs);
			if( NALEnd( pEncGlobal ))// YU_TBD remove it
				return -1;
			nOverHead += pEncGlobal->out.nal[pEncGlobal->out.nNAL-1].i_payload + NALU_nOverHead;// YU_TBD remove it
		}
    }

	if(pEncGlobal->pic_type == P_PIC_TYPE){
		FillBorder( pEncGlobal, pEncGlobal->fref0);
	}

    RCBefore( pEncGlobal,nOverHead*8 );


	if( voH264EncSlices( pEncGlobal ) )
		return -1;

    pEncGlobal->nLFrameNum++;
/*	if(pEncGlobal->pic_type ==P_PIC_TYPE)
	{
		VO_S32 i;
		FILE *fp = fopen("e:\\me.txt", "a+");
		
		fprintf(fp,"\n %d **************************************", pEncGlobal->nGFrameNum);

		

		fclose(fp);

	}*/
	//if(pEncGlobal->pic_type ==P_PIC_TYPE)
	
    pOutput->Length = voH264EncPicEnd( pEncGlobal);
    /*{
		VO_S32 i;
		FILE *fp = fopen("/sdcard/dump.264", "ab+");
		

		fwrite(pOutput->Buffer, 1, pOutput->Length, fp);

		fclose(fp);

	}*/

	return 0;
}

static VO_S32 voH264EncPicEnd( H264ENC *pEncGlobal)
{
    VO_S32 frame_size;
	frame_size = EncoderEncapsulateNALS( pEncGlobal );

    pEncGlobal->out.nNAL = 0;

	

	if(pEncGlobal->pic_type == IDR_PIC_TYPE) {
		pEncGlobal->nIDRPicID++;
		pEncGlobal->nIDRPicID = pEncGlobal->nIDRPicID % 65536;
	}


    /* ---------------------- Update encoder state ------------------------- */

    /* update rc */

    if( RCMBEnd( pEncGlobal, frame_size))
        return -1;

//	DeblockFrame(pEncGlobal);
#ifdef CAL_PSNR
 	sse_y = vo_sse(pEncGlobal->orig->plane[0], pEncGlobal->fdec->plane[0],
				pEncGlobal->i_stride[0], pEncGlobal->i_width[0],
				pEncGlobal->height[0]);
			
 	sse_u = vo_sse(pEncGlobal->orig->plane[1], pEncGlobal->fdec->plane[1],
				pEncGlobal->i_stride[1], pEncGlobal->i_width[1],
				pEncGlobal->height[1]);
			
 	sse_v = vo_sse(pEncGlobal->orig->plane[2], pEncGlobal->fdec->plane[2],
				pEncGlobal->i_stride[2], pEncGlobal->i_width[2],
				pEncGlobal->height[2]);

	total_psnr_y += SSEPSNR(sse_y, pEncGlobal->i_width[0] * pEncGlobal->height[0]);
	total_psnr_u += SSEPSNR(sse_u, pEncGlobal->i_width[1] * pEncGlobal->height[1]);
	total_psnr_v += SSEPSNR(sse_v, pEncGlobal->i_width[2] * pEncGlobal->height[2]);
#endif//CAL_PSNR

#ifdef RECON_FILE

	for(i=0; i< pEncGlobal->height[0]; i++){
		fwrite(pEncGlobal->fdec->plane[0]+i*pEncGlobal->i_stride[0], 
			sizeof(VO_U8)*pEncGlobal->i_width[0],1,rec_file);
	}
	for(i=0; i< pEncGlobal->height[1]; i++){
		fwrite(pEncGlobal->fdec->plane[1]+i*pEncGlobal->i_stride[1], 
			sizeof(VO_U8)*pEncGlobal->i_width[1],1,rec_file);
	}

	for(i=0; i< pEncGlobal->height[2]; i++){
		fwrite(pEncGlobal->fdec->plane[2]+i*pEncGlobal->i_stride[2], 
			sizeof(VO_U8)*pEncGlobal->i_width[2],1,rec_file);
	}

	

#endif//RECON_FILE


    return frame_size;
}



/****************************************************************************
 * voH264ENCUninit:
 ****************************************************************************/
VO_U32  VO_API  voH264ENCUninit  ( VO_HANDLE hEnc)
{
	H264ENC *pEncGlobal = (H264ENC *) hEnc;

    pEncGlobal->nGFrameNum++;
#ifdef RECON_FILE
	fclose(rec_file);
#endif

#ifdef LICENSE_CHECK
	//printf("phLicenseCheck:%d\n",((H264ENC *)hEnc)->phLicenseCheck);

	voCheckLibUninit (((H264ENC *)hEnc)->phLicenseCheck);
#endif
    /* rc */
    //RCDelete( pEncGlobal );

	//voFree( pEncGlobal->buffer_total );
	voH264Free(pEncGlobal,pEncGlobal->buffer_total);
	voH264Free(pEncGlobal, pEncGlobal->out.nal );
    /* InternalParam */

    //DeleteCQM4x4( pEncGlobal );
	//voFree( pEncGlobal->nMBType);
	//voFree( pEncGlobal->mv);
	//voFree( pEncGlobal->ref);
	//YU_TBD remove it
	

    /*for( i = 0; i < 92; i++ )
    {
        if( pEncGlobal->cost_mv[i] )
            voFree( pEncGlobal->cost_mv[i] - 2*4*2048 );
        if( pEncGlobal->cost_mv_fpel[i][0] )
            for( j = 0; j < 4; j++ )
                voFree( pEncGlobal->cost_mv_fpel[i][j] - 2*2048 );
    }*/

	//for( i=0; i<3; i++ )
	//	voFree( pEncGlobal->mb.LastMBRowLine[i] - 8 );

    //voFree( pEncGlobal->mvr);
    //voFree( pEncGlobal->mzc_mbrow );
    //voFree( pEncGlobal->qp );



	//voFree( pEncGlobal->fenc_buf);
	//voFree( pEncGlobal->fdec_buf);

	//YU_TBD free frame buffer
	//AVCDeleteFrame( pEncGlobal->fdec );

	//AVCDeleteFrame( pEncGlobal->fref0 );
#ifdef CAL_PSNR
	//AVCDeleteFrame( pEncGlobal->orig );
#endif

	//voFree(pEncGlobal->out.pBitstream );
	//voFree(pEncGlobal->out.nal);
	voH264Free(pEncGlobal,pEncGlobal);
	//voFree(pEncGlobal);



	return 0;
}

#define CACHE_LINE 64
VO_U32 VO_API voH264ENCInit(VO_HANDLE * phEnc, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
  VO_U32 codec_idx;
#ifdef LICENSE_CHECK
  VO_LIB_OPERATOR *pLibOP = NULL;
  VO_PTR phLicenseCheck = NULL;
  VO_U32 iRet;
  //printf("license\n");
#endif
  VO_U32 default_malloc;

  switch(vType)
  {
	case VO_VIDEO_CodingH264:
	  codec_idx = VO_INDEX_ENC_H264;
	  break;
	default:
	  return VO_ERR_NOT_IMPLEMENT;
  }

#ifdef LICENSE_CHECK
  if(pUserData)
  {
	if(pUserData->memflag & 0XF0)
	{//valid libOperator flag
	  pLibOP = pUserData->libOperator;
	}
  }
  iRet = voCheckLibInit (&phLicenseCheck, codec_idx, 0, voH264EnchInst, pLibOP);
 if(iRet != VO_ERR_NONE)
 {
  //*phDec = NULL;
  if(phLicenseCheck)
  {
   voCheckLibUninit(phLicenseCheck);
   phLicenseCheck = NULL;
  }
  return iRet;
 }
  
#endif
  default_malloc = 1;
  if(pUserData)
  {
	if((pUserData->memflag & 0x0F) == VO_IMF_USERMEMOPERATOR) 
	{
	  VO_U8 *tmp, *mem_ptr;
	  VO_MEM_OPERATOR* pMemOp = (VO_MEM_OPERATOR*)pUserData->memData;
	  voMemAlloc(tmp, pMemOp, codec_idx, sizeof(H264ENC)+CACHE_LINE);
	  if(tmp == NULL) 
		return(VO_ERR_OUTOF_MEMORY);

	  mem_ptr = (VO_U8 *)( (VO_U32) (tmp + CACHE_LINE - 1) & ( ~(VO_U32) (CACHE_LINE - 1) ) );
	  if (mem_ptr == tmp)
		mem_ptr += CACHE_LINE;
	  *(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);
	  *phEnc = (VO_VOID *)mem_ptr;
	  memset(mem_ptr, 0, sizeof(H264ENC));
	  ((H264ENC *)(*phEnc))->memOperater = pMemOp;
	  default_malloc = 0;
	}
  }
  if(default_malloc)
  {
	//CHECKED_MALLOCZERO( pEncGlobal, sizeof(H264ENC) );
	//if(!((*phEnc) = (H264ENC *) voMalloc(sizeof(H264ENC))))
	if(!(*phEnc = (H264ENC *) voH264Malloc (NULL,sizeof(H264ENC),CACHE_LINE)))
	  return VO_ERR_OUTOF_MEMORY;
	memset((*phEnc), 0, sizeof(H264ENC));
	((H264ENC *)(*phEnc))->memOperater = NULL;
  }

	IniInternParam( &(((H264ENC *)(*phEnc))->InternalParam));
	((H264ENC *)(*phEnc))->fastskip = 600;
	((H264ENC *)(*phEnc))->me_method = VO_ME_DIA;
	((H264ENC *)(*phEnc))->codec_id = codec_idx;
	((H264ENC *)(*phEnc))->OutParam.b_deblocking_filter = 0;
#ifdef LICENSE_CHECK	
	((H264ENC *)(*phEnc))->phLicenseCheck = phLicenseCheck; 
#endif

	return VO_ERR_NONE;
}



/**
 * Set parameter of the Encoder instance.
 * \param hCodec [in] H264 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param nID [in] Parameter ID, Supports: VOMPEG4PARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voH264ENCSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{

	H264ENC *pEncGlobal		= (H264ENC *)hCodec;
	OUT_PARAM *pOutParam	= &pEncGlobal->OutParam;
	switch(uParamID){
	case VO_PID_ENC_H264_PROFILE:
//		pOutParam->profile					= *((VO_S32*)pData);
//		pEncGlobal->nConfig |= 1;
		break;
	case VO_PID_ENC_H264_LEVEL:
//		pOutParam->level					= *((VO_S32*)pData);
//		pEncGlobal->nConfig |= (1<<1);
		break;
	case VO_PID_ENC_H264_BITRATE: 
		pOutParam->i_bitrate				= *((VO_S32*)pData);		//!< 450000;//28000;
		pEncGlobal->nConfig |= (1<<2);
		break;
	case VO_PID_ENC_H264_FRAMERATE:
		//edit by Really Yang 110114
		pOutParam->fps_num					= (VO_S32)(*((float*)pData));
		//end of edit
		pEncGlobal->nConfig |= (1<<3);
		break;
	case VO_PID_ENC_H264_RATECONTROL:
//		pOutParam->bitrateControl			= *((VO_S32*)pData);
//		pEncGlobal->nConfig |= (1<<4);
		break;
	case VO_PID_ENC_H264_WIDTH:
		pOutParam->width					= *((VO_S32*)pData);
		pEncGlobal->nConfig |= (1<<5);
		break;
	case VO_PID_ENC_H264_HEIGHT:
		pOutParam->height       			= *((VO_S32*)pData);
		pEncGlobal->nConfig |= (1<<6);
		break;
	case VO_PID_ENC_H264_KEYFRAME_INTERVAL:
		pOutParam->max_iframe_interval  	= *((VO_S32*)pData);
		pEncGlobal->nConfig |= (1<<8);

		break;
	case VO_PID_ENC_H264_VIDEO_QUALITY:
		pEncGlobal->enc_quality = *((VO_S32*)pData);
		if(pEncGlobal->enc_quality == VO_ENC_MID_QUALITY)
		{
			pEncGlobal->InternalParam.i_analyse_flags |= VO_ANALYSE_HPEL;
			pEncGlobal->OutParam.b_deblocking_filter = 1;
		}
		else if(pEncGlobal->enc_quality == VO_ENC_HIGH_QUALITY)
		{
			pEncGlobal->InternalParam.i_analyse_flags |= VO_ANALYSE_HPEL;
			pEncGlobal->OutParam.b_deblocking_filter = 1;
			pEncGlobal->me_method = VO_ME_HEX;
			pEncGlobal->fastskip = 0;
		}
		break;
//	case VO_PID_ENC_H264_FORCE_KEY_FRAME:
//		pEncGlobal->frame_info.force_intra_frame = *((VO_S32*)pData);
		break;
//	case VO_PID_ENC_H264_INPUT_ROTATION:
//		pEncGlobal->custom_config.rt_type = *((VO_S32*)pData);
		break;
//	case VO_PID_ENC_H264_VP_SIZE:
//		pEncGlobal->vp_byte_size = *((VO_S32*)pData);
//		break;
	default :
		return VO_ERR_WRONG_PARAM_ID;
	}
	/* parameters setting completed*/
	if((pEncGlobal->nConfig&0x16C)== 0x16C && pEncGlobal->nConfig != CONFIG_OK)
	{
    	/*{
    		
    		FILE *fp = fopen("/sdcard/param.txt", "w");
    				
    		fprintf(fp,"bitrate:%d\n\r", pOutParam->i_bitrate);
    		fprintf(fp,"fps_num:%d\n\r", pOutParam->fps_num);
    		fprintf(fp,"width:%d\n\r", pOutParam->width);
    		fprintf(fp,"height:%d\n\r", pOutParam->height);				
    		
    		fclose(fp);
    	}*/

		if(H264EncCreate(pEncGlobal, pOutParam))
			return VO_ERR_ENC_H264_CONFIG;
		pEncGlobal->nConfig = CONFIG_OK; 
	}

	return VO_ERR_NONE;
}

/**
 * Get parameter of the Encoder .
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param nID [in] Parameter ID, Supports: VOMPEG4PARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voH264ENCGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{

	H264ENC *pEncGlobal		= (H264ENC *)hCodec;
	//vtPARAMETERS *m_264Par		= &pEncGlobal->vH264par;

	if(pEncGlobal->nConfig != CONFIG_OK)
		return VO_ERR_WRONG_STATUS;

	switch(uParamID){
	case VO_PID_ENC_H264_OUTBUF_SIZE: 
		*((VO_U32 *)pData) = (pEncGlobal->OutParam.width*pEncGlobal->OutParam.height *3/2);
		break;
	default :
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


VO_S32 VO_API voGetH264EncAPI (VO_VIDEO_ENCAPI * pDecHandle)
{
	VO_VIDEO_ENCAPI *pH264Enc = pDecHandle;

	if(!pH264Enc)
		return VO_ERR_INVALID_ARG;

	pH264Enc->Init		= voH264ENCInit;
	pH264Enc->Uninit	= voH264ENCUninit;
	pH264Enc->SetParam	= voH264ENCSetParameter;
	pH264Enc->GetParam	= voH264ENCGetParameter;
	pH264Enc->Process	= voH264ENCProcess;

	return VO_ERR_NONE;
}


