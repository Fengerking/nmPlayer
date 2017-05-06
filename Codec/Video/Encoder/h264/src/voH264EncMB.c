/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#include "voH264EncGlobal.h"
#include "voH264EncME.h"
#include "voH264MC.h"
#include "voH264EncRC.h"
#include "voH264Dct.h"
#include "voH264Quant.h"
#include <string.h>
#if defined(VOARMV7)

extern void HpelFilter16X16_V_ARMV7(VO_U8 *dst, VO_S32 dst_stride,
			VO_U8 *src, VO_S32 src_stride
					);
extern void HpelFilter16X16_H_ARMV7(VO_U8 *dst, VO_S32 dst_stride,
			VO_U8 *src, VO_S32 src_stride
					);
extern void HpelFilter16X16_C_ARMV7(VO_U8 *dst, VO_S32 dst_stride,
			VO_U8 *src, VO_S32 src_stride,VO_S16 *buf
					);
#endif
#define AVC_MID3(a,b,c,p)\
{\
  (p) = ((a)-(b))&(((a)-(b))>>31);\
  (a) -= (p);\
  (b) += (p);\
  (b) -= ((b)-(c))&(((b)-(c))>>31);\
  (b) += ((a)-(b))&(((a)-(b))>>31);\
  (p) = (b);\
}\

#define LEFTPOS 11
#define TOPPOS 4
#define TOPRIGHTPOS 8
#define TOPLEFTPOS 3

void GetPredictMV( H264ENC *pEncGlobal, VO_S16 mvp[2] )
{
  VO_S32 index = 0;
  VO_S32 refa = pEncGlobal->mb.ref[LEFTPOS];//left
  VO_S32 refb = pEncGlobal->mb.ref[TOPPOS];//top
  VO_S32 refc = pEncGlobal->mb.ref[TOPRIGHTPOS];//top right
  VO_S16 *mva  = pEncGlobal->mb.mv[LEFTPOS];
  VO_S16 *mvb  = pEncGlobal->mb.mv[TOPPOS];
  VO_S16 *mvc;
  if( refc == -2 )
  {
    refc = pEncGlobal->mb.ref[TOPLEFTPOS];
    mvc = pEncGlobal->mb.mv[TOPLEFTPOS];
  }
  else
  {
  	mvc = pEncGlobal->mb.mv[TOPRIGHTPOS];
  }
  
  index = (!refa) + (!refb) + (!refc);
  if( index > 1 )
  {
median:
	AVC_MID3(mva[0],mvb[0],mvc[0],mvp[0]);
	AVC_MID3(mva[1],mvb[1],mvc[1],mvp[1]);
  }
  else if( index == 1 )
  {
    M32(mvp) = !refa ? M32(mva) : (!refb ? M32(mvb) : M32(mvc));
  }
  else if( refb == -2 && refc == -2 && refa != -2 )
  {
    CP32( mvp, mva );
  }
  else
  {
    goto median;
  }
}
void PredicMVSkip( H264ENC *pEncGlobal, VO_S16 mv[2] )
{
    VO_S32 refa = pEncGlobal->mb.ref[LEFTPOS];
    VO_S32 refb = pEncGlobal->mb.ref[TOPPOS];
    VO_S16 *mva  = pEncGlobal->mb.mv[LEFTPOS];
    VO_S16 *mvb  = pEncGlobal->mb.mv[TOPPOS];

    if( refa == -2 || refb == -2 || !( refa | M32( mva ) ) || !( refb | M32( mvb ) ) )
        M32( mv ) = 0;
    else
        GetPredictMV( pEncGlobal, mv );
}

/* This just improves encoder performance, it's not part of the spec */
void GetCandidateMV( H264ENC *pEncGlobal, VO_S16 mvc[8][2], VO_S32 *i_mvc , H264ENC_L *pEncLocal)
{
  VO_S32 mv_stride = pEncGlobal->mb_width * 16;
  VO_S32 mv_index = pEncLocal->nMV4;
  VO_S32 i = 0;

  // spatial left
  if( pEncLocal->nAvailNeigh & MB_LEFT &&!IS_SKIP( pEncLocal->nLeftMBType )&&!IS_INTRA( pEncLocal->nLeftMBType ))
	CP32(mvc[i++],pEncGlobal->mv[mv_index-4]);
  
  if( pEncLocal->nAvailNeigh & MB_TOP )
  {
    mv_index -= mv_stride;
	// spatial top
    if( !IS_SKIP( pEncLocal->nTopMBType)&&!IS_INTRA( pEncLocal->nTopMBType )  )
	  CP32(mvc[i++],pEncGlobal->mv[mv_index]);
	// spatial topleft
    if( pEncLocal->nAvailNeigh & MB_TOPLEFT && !IS_SKIP( pEncLocal->nTopLeftMBType )&&!IS_INTRA( pEncLocal->nTopLeftMBType ))
      CP32(mvc[i++],pEncGlobal->mv[mv_index-4]);
	// spatial topright
    if( pEncLocal->nMBX < pEncGlobal->mb_stride - 1 && !IS_SKIP( pEncLocal->nTopRightMBType ) &&!IS_INTRA( pEncLocal->nTopRightMBType ))
      CP32(mvc[i++],pEncGlobal->mv[mv_index+4]);
  }

  //temporal current
  mv_index = pEncLocal->nMV4;
  CP32(mvc[i++],pEncGlobal->mv[mv_index]); 
  //temporal right
  if( pEncLocal->nMBX < pEncGlobal->mb_width-1 )
	CP32(mvc[i++],pEncGlobal->mv[mv_index+4]);
  //temporal bottom
  if( pEncLocal->nMBY < pEncGlobal->mb_height-1 )
	CP32(mvc[i++],pEncGlobal->mv[mv_index+mv_stride]);

  *i_mvc = i;
}


static VO_NOINLINE void MCBlock( H264ENC *pEncGlobal, VO_S32 x, VO_S32 y, VO_S32 width, VO_S32 height , H264ENC_L *pEncLocal)
{
    const VO_S32 i8 = cache_pos[0]+x+8*y;
    const VO_S32 mvx   = pEncGlobal->mb.mv[i8][0] + 4*4*x;
    VO_S32 mvy   = pEncGlobal->mb.mv[i8][1] + 4*4*y;
	VO_U32 flags = pEncGlobal->InternalParam.i_analyse_flags;
	VO_S32 dmvx;
	VO_S32 dmvy;

	/*YU_TBD: do not copy on integer pixel*/
	VO_S32 src_stride = 32;
	VO_U8 *src = 0;
	if(flags & VO_ANALYSE_HPEL)
	{
		dmvx   = mvx - pEncLocal->mvi[0];
		dmvy   = mvy - pEncLocal->mvi[1];
	}
	else
	{
		dmvx   = 0;
		dmvy   = 0;
	}
	if(dmvx==0&&dmvy==0){
    	src = pEncGlobal->pRef[0] + (mvy>>2)*pEncGlobal->i_stride[0] + (mvx>>2);
		src_stride = pEncGlobal->i_stride[0];
	}
	else if(dmvx==0&&dmvy==-2)
		src = pEncGlobal->pRef[4];
	else if(dmvx==0&&dmvy==2)
		src = pEncGlobal->pRef[4] + 32;
	else if(dmvx==-2&&dmvy==0)
		src = pEncGlobal->pRef[3];
	else if(dmvx==2&&dmvy==0)
		src = pEncGlobal->pRef[3] + 1;

    MCLuma( &pEncGlobal->pDec[0][4*y*FDEC_STRIDE+4*x], FDEC_STRIDE,
                   src, src_stride, 4*width, 4*height);

	src_stride = pEncGlobal->i_stride[1];
    src = pEncGlobal->pRef[1] + (mvy>>3)*src_stride + (mvx>>3);
	//edit by Really Yang 1210
    //MCChroma( &pEncGlobal->pDec[1][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
    //                 src, src_stride,
    //                 mvx&0x07, mvy&0x07, 2*width, 2*height );
#if defined(VOARMV7)
	MCChroma8x8_ARMV7( &pEncGlobal->pDec[1][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                     src, src_stride,
                     mvx&0x07, mvy&0x07);
#else
	MCChroma8x8_c( &pEncGlobal->pDec[1][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                     src, src_stride,
                     mvx&0x07, mvy&0x07);
#endif
	//end of edit

	src_stride = pEncGlobal->i_stride[2];
    src = pEncGlobal->pRef[2] + (mvy>>3)*src_stride + (mvx>>3);
	//edit by Really Yang 1210
    //MCChroma( &pEncGlobal->pDec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
    //                 src, src_stride,
    //                 mvx&0x07, mvy&0x07, 2*width, 2*height );
#if defined(VOARMV7)
	MCChroma8x8_ARMV7( &pEncGlobal->pDec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                     src, src_stride,
                     mvx&0x07, mvy&0x07);
#else
	MCChroma8x8_c( &pEncGlobal->pDec[2][2*y*FDEC_STRIDE+2*x], FDEC_STRIDE,
                     src, src_stride,
                     mvx&0x07, mvy&0x07);
#endif
	//end of edit
}

/* get ready mc data in pEncGlobal->pDec*/
void InterMBCompesation( H264ENC *pEncGlobal , H264ENC_L *pEncLocal)
{

	//YU_TBD, maybe we can split luma and chroma
	//if( pEncGlobal->mb.i_partition == D_16x16 )
	//{
		MCBlock( pEncGlobal, 0, 0, 4, 4, pEncLocal );
	//}

 
}

/***************************************************************************************************************************************************************/

static VO_NOINLINE void CopyLeftPix8( VO_U8 *dst, VO_U8 *src )
{
    // input pointers are offset by 4 rows because that's faster (smaller instruction size on x86)
    VO_U32 i;
    for( i = 2; i != 0; i-- )
    {
        dst[0] = src[0];
        dst[32] = src[32];
		dst[64] = src[64];
		dst[96] = src[96];
		dst += 128;
		src += 128;
    }
}


#define LUMA_START_POS 12
#define CB_START_POS 9
#define CR_START_POS 33
void EncodeMBStart( H264ENC *pEncGlobal, VO_S32 nMBX, VO_S32 nMBY, H264ENC_L *pEncLocal )
{
	const VO_S32 mb_stride = pEncGlobal->mb_stride; 
    VO_S32 nMBXY = nMBY * mb_stride + nMBX;
    VO_S32 i_top_y = nMBY - 1;
    VO_S32 i_top_xy = nMBXY - mb_stride;
	VO_U8 *exmb_nzc  = pEncGlobal->mb.exmb_nzc;
	const VO_S32 nMV4 = 4*(nMBY * (mb_stride * 4) + nMBX);
    VO_S32 i;


	pEncLocal->nQP = RCMBQp( pEncGlobal );

    /* init index */
    pEncLocal->nMBX = nMBX;
    pEncLocal->nMBY = nMBY;
    pEncLocal->nMBXY = nMBXY;
    pEncLocal->nMV4 = nMV4;
    pEncLocal->nAvailNeigh = 0;

    /* top mb */
    if( i_top_xy >= pEncGlobal->sh.nStartMB ){
        pEncLocal->nTopMBType = pEncGlobal->nMBType[nMBX];
        pEncLocal->nAvailNeigh |= MB_TOP;

        /* load exmb_nzc */
        CP32( &exmb_nzc[LUMA_START_POS - 8], &pEncGlobal->mzc_mbrow[nMBX][12] );

        /* shift because cache_pos[16] is misaligned */
        M32( &exmb_nzc[CB_START_POS - 9] ) = M16( &pEncGlobal->mzc_mbrow[nMBX][18] ) << 8;
        M32( &exmb_nzc[CR_START_POS - 9] ) = M16( &pEncGlobal->mzc_mbrow[nMBX][22] ) << 8;
    }else{
        pEncLocal->nTopMBType = -1;

        /* load exmb_nzc */
        M32( &exmb_nzc[LUMA_START_POS - 8] ) = 0x80808080U;
        M32( &exmb_nzc[CB_START_POS   - 9] ) = 0x80808080U;
        M32( &exmb_nzc[CR_START_POS   - 9] ) = 0x80808080U;
    }

	/*left mb*/
    if( nMBX > 0 && nMBXY > pEncGlobal->sh.nStartMB ){
        pEncLocal->nLeftMBType = pEncGlobal->nMBType[nMBX - 1];
        pEncLocal->nAvailNeigh |= MB_LEFT;
		
        /* load exmb_nzc */
        exmb_nzc[11     ] = exmb_nzc[11 + 4 ];   
        exmb_nzc[11 + 8 ] = exmb_nzc[11 + 8  + 4];
        exmb_nzc[11 + 16] = exmb_nzc[11 + 16 + 4];
        exmb_nzc[11 + 24] = exmb_nzc[11 + 24 + 4];
        exmb_nzc[8      ] = exmb_nzc[8 + 2];
        exmb_nzc[8 + 8  ] = exmb_nzc[8 + 8 + 2];
        exmb_nzc[32     ] = exmb_nzc[32 + 2];
        exmb_nzc[32 +  8] = exmb_nzc[32 + 8 + 2];
    }else{
        pEncLocal->nLeftMBType = -1;

        /* load exmb_nzc */
        exmb_nzc[11     ] =
        exmb_nzc[11 + 8 ] =
        exmb_nzc[11 + 16] =
        exmb_nzc[11 + 24] =
        exmb_nzc[8      ] =
        exmb_nzc[8 + 8  ] =
    	exmb_nzc[32     ] =
		exmb_nzc[32 +  8] = 0x80;
	}

	/* mb top right*/
    if( nMBX < pEncGlobal->mb_width - 1 && i_top_xy + 1 >= pEncGlobal->sh.nStartMB ){
        pEncLocal->nAvailNeigh |= MB_TOPRIGHT;
        pEncLocal->nTopRightMBType = pEncGlobal->nMBType[ nMBX + 1 ];
    }else
        pEncLocal->nTopRightMBType = -1;

	/* mb top left*/
    if( nMBX > 0 && i_top_xy - 1 >= pEncGlobal->sh.nStartMB ){
        pEncLocal->nAvailNeigh |= MB_TOPLEFT;
		/* nTopLeftMBType is ready*/
    }else
        pEncLocal->nTopLeftMBType = -1;


	if( pEncGlobal->pic_type  == P_PIC_TYPE ){

		const VO_S32 ref8 = 2*(0 * (mb_stride * 2) + nMBX);
		const VO_S32 stride8 = mb_stride* 2;
		const VO_S32 stride4 = stride8 * 2;

		VO_S32 i_top_4x4 = (4*i_top_y+3) * stride4 + 4*nMBX;
	    VO_S32 i_top_8x8 = (2*0+1) * stride8 + 2*nMBX;

		if( pEncLocal->nAvailNeigh & MB_TOPLEFT ){
			const VO_S32 iv = i_top_4x4 - 1;
			pEncGlobal->mb.ref[3]  = pEncLocal->TopLeftRef[3]; 
			CP32( pEncGlobal->mb.mv[3], pEncGlobal->mv[iv] );
		}else{
			pEncGlobal->mb.ref[3] = -2;
			M32( pEncGlobal->mb.mv[3] ) = 0;
		}

		if( pEncLocal->nAvailNeigh & MB_TOP ){
			const VO_S32 ir = i_top_8x8;
			const VO_S32 iv = i_top_4x4;
			pEncGlobal->mb.ref[4] = pEncGlobal->mb.ref[5] = pEncGlobal->ref[ir + 0];
			pEncGlobal->mb.ref[6] = pEncGlobal->mb.ref[7] = pEncGlobal->ref[ir + 1];
			CP64( pEncGlobal->mb.mv[4], pEncGlobal->mv[iv+0] );
			CP64( pEncGlobal->mb.mv[6], pEncGlobal->mv[iv+2] );
		}else{
			M64( pEncGlobal->mb.mv[4] ) = 0;
			M64( pEncGlobal->mb.mv[6] ) = 0;
			M32( &pEncGlobal->mb.ref[4] ) = (VO_U8)(-2) * 0x01010101U;
		}

		if( pEncLocal->nAvailNeigh & MB_TOPRIGHT ){
			const VO_S32 ir = i_top_8x8 + 2;
			const VO_S32 iv = i_top_4x4 + 4;
			pEncGlobal->mb.ref[8]  = pEncGlobal->ref[ir];
			CP32( pEncGlobal->mb.mv[8], pEncGlobal->mv[iv] );
		}else{
			pEncGlobal->mb.ref[8] = -2;
			M32( pEncGlobal->mb.mv[8] ) = 0;
		}

		if( pEncLocal->nAvailNeigh & MB_LEFT ){
			const VO_S32 ir = ref8 - 1;
			const VO_S32 iv = nMV4 - 1;

			pEncGlobal->mb.ref[11 ] =
				pEncGlobal->mb.ref[11 + 8] = pEncGlobal->ref[ir + 0*stride8];
			pEncGlobal->mb.ref[11 + 16] =
				pEncGlobal->mb.ref[11 + 24] = pEncGlobal->ref[ir + 1*stride8];

			CP32( pEncGlobal->mb.mv[11     ], pEncGlobal->mv[iv + 0*stride4] );
			CP32( pEncGlobal->mb.mv[11 + 8 ], pEncGlobal->mv[iv + 1*stride4] );
			CP32( pEncGlobal->mb.mv[11 + 16], pEncGlobal->mv[iv + 2*stride4] );
			CP32( pEncGlobal->mb.mv[11 + 24], pEncGlobal->mv[iv + 3*stride4] );
		}else{
			for( i = 0; i < 4; i++ ){
				pEncGlobal->mb.ref[11+i*8] = -2;
				M32( pEncGlobal->mb.mv[11+i*8] ) = 0;
			}
		}
		PredicMVSkip( pEncGlobal, pEncGlobal->mb.pskip_mv );
	}

	for(i = 0; i < 3; i++){//YU_TBD
		const VO_S32 w = 16 >> !!i;
		const VO_S32 i_stride = pEncGlobal->i_stride[!!i];
		const VO_S32 i_pix_offset =  w * (nMBX + nMBY * i_stride);

		const VO_U8 *intra_fdec = &pEncGlobal->mb.LastMBRowLine[i][nMBX * w];
		//edit by Really Yang 1210
		//CopyBlock[!!i]( pEncGlobal->pCur[i], FENC_STRIDE,&pEncGlobal->fdec->plane[i][i_pix_offset], i_stride, w );
		CopyBlock[!!i]( &pEncGlobal->fdec->plane[i][i_pix_offset], i_stride,pEncGlobal->pCur[i], FENC_STRIDE );
		//end of edit	

		if( nMBY > 0 ){
			pEncGlobal->pDec[i][-1-FDEC_STRIDE] = pEncGlobal->mb.top_left_pix[i];
			memcpy( &pEncGlobal->pDec[i][-FDEC_STRIDE], intra_fdec, w);
		}

		if(pEncGlobal->pic_type  == P_PIC_TYPE){
			pEncGlobal->pRef[i] = &pEncGlobal->fref0->plane[i][i_pix_offset];
		}
	}

	//add by Really Yang 1119
	if(pEncGlobal->pic_type  == P_PIC_TYPE){
		pEncGlobal->pRef[3] = &pEncLocal->pix[0][0];        //H
		pEncGlobal->pRef[4] = &pEncLocal->pix[1][0];     //V
		pEncGlobal->pRef[5] = &pEncLocal->pix[2][0];    //C     
	}
	//end of add
}

void EncodeMBEnd( H264ENC *pEncGlobal ,H264ENC_L *pEncLocal)
{
    const VO_S32 nMBXY = pEncLocal->nMBXY;
	const VO_S32 nMBX = pEncLocal->nMBX;
    const VO_S32 i_mb_type = MBTypeFix[pEncLocal->nMBType];

    const VO_S32 nMV4 = pEncLocal->nMV4;

    VO_U8 *nzc = pEncGlobal->mzc_mbrow[nMBX];

    VO_S32 y, i;

	/* backup before deblock, for intra prediction*/
	if( pEncLocal->nMBY != pEncGlobal->mb_height){
		for( i=0; i<3; i++ ){
			VO_S32 t = (16>>!!i);
			VO_U8 *dst = pEncGlobal->mb.LastMBRowLine[i] + nMBX * t;
			pEncGlobal->mb.top_left_pix[i] = dst[t - 1];/*top_left*/
			memcpy( dst, pEncGlobal->pDec[i] + (t  - 1) * FDEC_STRIDE, t);/* only copy 16 or 8*/
		}
	}


	for(i = 0; i < 3; i++){
		VO_S32 w = 16 >> !!i;
		VO_S32 i_stride = pEncGlobal->i_stride[!!i];
		VO_S32 i_pix_offset =  w * (nMBX + pEncLocal->nMBY * i_stride);
		//edit by Really Yang 1210
		//CopyBlock[!!i](&pEncGlobal->fdec->plane[i][i_pix_offset], i_stride,pEncGlobal->pDec[i], FDEC_STRIDE, w );
		CopyBlock[!!i](pEncGlobal->pDec[i], FDEC_STRIDE,&pEncGlobal->fdec->plane[i][i_pix_offset],i_stride );
		//end of edit
	}
	
	if( pEncLocal->nMBType != I16x16 && pEncLocal->nLumaCBP == 0 && pEncLocal->nChromaCBP == 0 )
		pEncLocal->nQP = pEncLocal->nLastQP;
	pEncGlobal->qp[nMBXY] = (VO_S8)pEncLocal->nQP;

	if(pEncGlobal->pic_type  == P_PIC_TYPE ){
		const VO_S32 mb_stride = pEncGlobal->mb_stride;
		const VO_S32 stride8 =  mb_stride * 2;
		const VO_S32 stride4 = stride8 * 2;
		const VO_S32 ref8 = 2*(0 * (mb_stride * 2) + nMBX);

		pEncLocal->TopLeftRef[3] = pEncGlobal->ref[ref8+1+1*stride8];
        if( !IS_INTRA( i_mb_type ) ){
            pEncGlobal->ref[ref8+0+0*stride8] = 0;
            pEncGlobal->ref[ref8+1+0*stride8] = 0;
            pEncGlobal->ref[ref8+0+1*stride8] = 0;
            pEncGlobal->ref[ref8+1+1*stride8] = 0;
            for( y = 0; y < 4; y++ ){
                CP64( pEncGlobal->mv[nMV4+y*stride4+0], pEncGlobal->mb.mv[LUMA_START_POS+8*y+0] );
                CP64( pEncGlobal->mv[nMV4+y*stride4+2], pEncGlobal->mb.mv[LUMA_START_POS+8*y+2] );
            }
        }else{
			M16( &pEncGlobal->ref[ref8+0*stride8] ) = (VO_U8)(-1) * 0x0101;
			M16( &pEncGlobal->ref[ref8+1*stride8] ) = (VO_U8)(-1) * 0x0101;
			for( y = 0; y < 4; y++ )
			{
				M64( pEncGlobal->mv[nMV4+y*stride4+0] ) = 0;
				M64( pEncGlobal->mv[nMV4+y*stride4+2] ) = 0;
			}
        }
    }
	if(pEncGlobal->OutParam.b_deblocking_filter&&pEncGlobal->sh.bDisableDeblockIdc==0)
		DeblocMB(pEncGlobal, pEncLocal);

	/*prepare for intra prediction*/
	
	if(nMBX != pEncGlobal->mb_width ){
		/*left*/
		CopyLeftPix8( pEncGlobal->pDec[0]-1, pEncGlobal->pDec[0]+15 );
		CopyLeftPix8( pEncGlobal->pDec[0]-1+8*FDEC_STRIDE, pEncGlobal->pDec[0]+15+8*FDEC_STRIDE );
		CopyLeftPix8( pEncGlobal->pDec[1]-1, pEncGlobal->pDec[1]+ 7 );
		CopyLeftPix8( pEncGlobal->pDec[2]-1, pEncGlobal->pDec[2]+ 7 );
	}



	pEncLocal->nTopLeftMBType = pEncGlobal->nMBType[nMBX];
    pEncGlobal->nMBType[nMBX] = (VO_S8)i_mb_type;


	/* save non zero count */
	CP32( &nzc[0*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+0*8] );
	CP32( &nzc[1*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+1*8] );
	CP32( &nzc[2*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+2*8] );
	CP32( &nzc[3*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+3*8] );
	M16( &nzc[16+0*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CB_START_POS-1] ) >> 8);
	M16( &nzc[16+1*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CB_START_POS+8-1] ) >> 8);
	M16( &nzc[16+2*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CR_START_POS-1] ) >> 8);
	M16( &nzc[16+3*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CR_START_POS+8-1] ) >> 8);

//	if( pEncLocal->nMBType != I16x16 && pEncLocal->nLumaCBP == 0 && pEncLocal->nChromaCBP == 0 )
//		pEncLocal->nQP = pEncLocal->nLastQP;

//	pEncGlobal->qp[nMBXY] = pEncLocal->nQP;
	pEncLocal->nLastQP = pEncLocal->nQP;
	pEncGlobal->pic_cost += pEncLocal->nMBCost;

    
}

#define CLEAR_16x16_NNZ \
{\
    M32( &pEncGlobal->mb.exmb_nzc[LUMA_START_POS] ) = 0;\
    M32( &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+8] ) = 0;\
    M32( &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+2*8] ) = 0;\
    M32( &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+3*8] ) = 0;\
}

static void GetIntraLumaResidual( H264ENC *pEncGlobal, VO_S32 nQP , H264ENC_L *pEncLocal)
{
    VO_U8  *p_src = pEncGlobal->pCur[0];
    VO_U8  *p_dst = pEncGlobal->pDec[0];

    VO_ALIGNED_ARRAY_16( VO_S16, dct4x4,[16*16] );
    VO_ALIGNED_ARRAY_16( VO_S16, dct_dc4x4,[16] );
	VO_ALIGNED_ARRAY_16( VO_S32, bnz,[16] );

    VO_S32 i4x4, nz;
    VO_S32 decimate_score = pEncGlobal->pic_type  == P_PIC_TYPE ? 0 : 9;

 
    Sub16x16Dct( dct4x4, p_src, p_dst, FDEC_STRIDE);

	for( i4x4 = 0; i4x4 < 16; i4x4++ ){
		dct_dc4x4[BlkIndex[i4x4]] = dct4x4[i4x4<<4];
		dct4x4[i4x4<<4] = 0;
		bnz[i4x4] =  Quant4x4( &dct4x4[i4x4<<4], &pEncGlobal->quant_table[nQP<<4], &pEncGlobal->quant_bias[0][nQP<<4] );
	}
	

    for( i4x4 = 0; i4x4 < 16; i4x4++ ){
        /* quant/scan/dequant */
        nz = bnz[i4x4];
        pEncGlobal->mb.exmb_nzc[cache_pos[i4x4]] = (VO_U8)nz;

        if( nz ){
            Zigzag4x4( pEncGlobal->dct.ac_4x4[i4x4], &dct4x4[i4x4<<4] );
             Dequant4x4( &dct4x4[i4x4<<4], pEncGlobal->dequant_table, nQP );
            if( decimate_score < 6 ) decimate_score += NoneResDis( pEncGlobal->dct.ac_4x4[i4x4]+1,15 );
            pEncLocal->nLumaCBP = 0xf;
        }
    }

    /* Writing the 16 CBFs in an i16x16 block is quite costly, so decimation can save many bits. */
    /* More useful with CAVLC, but still useful with CABAC. */
    if( decimate_score < 6 ){
        pEncLocal->nLumaCBP = 0;
        CLEAR_16x16_NNZ
    }

    Dct4x4DC( dct_dc4x4 );

    nz =  Quant4x4Dc( dct_dc4x4, pEncGlobal->quant_table[nQP<<4]>>1, pEncGlobal->quant_bias[0][nQP<<4]<<1 );

    pEncGlobal->mb.exmb_nzc[cache_pos[24]] = (VO_U8)nz;

    if( nz ){
        Zigzag4x4( pEncGlobal->dct.luma_dc, dct_dc4x4 );

        /* output samples to fdec */
        Idct4x4Dc( dct_dc4x4 );
         Dequant4x4Dc( dct_dc4x4, pEncGlobal->dequant_table, nQP );  /* XXX not inversed */
        if( pEncLocal->nLumaCBP )
            for( i4x4 = 0; i4x4 < 16; i4x4++ )
                dct4x4[i4x4<<4] = dct_dc4x4[BlkIndex[i4x4]];
    }

    /* put pixels to fdec */
    if( pEncLocal->nLumaCBP )
        Add16x16Idct( p_dst, dct4x4 );
    else if( nz )
        Add16x16IdctDc( p_dst, dct_dc4x4 );
}

static void GetInterLumaResidual( H264ENC *pEncGlobal, VO_S32 nQP , H264ENC_L *pEncLocal)
{
    VO_S32 idx,nz;
	VO_S32 i8x8, i4x4;
    VO_S32 b_decimate = pEncGlobal->InternalParam.b_dct_decimate;
	VO_S32 i_decimate_mb = 0;
	VO_ALIGNED_ARRAY_16( VO_S16, dct4x4,[16*16] );
	VO_ALIGNED_ARRAY_16( VO_S32, bnz,[16] );

	Sub16x16Dct( dct4x4, pEncGlobal->pCur[0], pEncGlobal->pDec[0], FDEC_STRIDE );

	for( i4x4 = 0; i4x4 < 16; i4x4++ ){
		bnz[i4x4] =  Quant4x4( &dct4x4[i4x4<<4], &pEncGlobal->quant_table[nQP<<4], &pEncGlobal->quant_bias[1][nQP<<4] );
	}

	idx = 0;
	for( i8x8 = 0; i8x8 < 4; i8x8++ ){
		VO_S32 i_decimate_8x8 = 0;
		VO_S32 cbp = 0;

		/* encode one 4x4 block */
		for( i4x4 = 0; i4x4 < 4; i4x4++ ){
			nz = bnz[idx];
			pEncGlobal->mb.exmb_nzc[cache_pos[idx]] = (VO_U8)nz;

			if( nz ){
				Zigzag4x4( pEncGlobal->dct.ac_4x4[idx], &dct4x4[idx<<4] );
				 Dequant4x4( &dct4x4[idx<<4], pEncGlobal->dequant_table, nQP );
				if( b_decimate && i_decimate_8x8 < 6 )
					i_decimate_8x8 += NoneResDis( pEncGlobal->dct.ac_4x4[idx],16 );
				cbp = 1;
			}

			idx++;
		}

		i_decimate_mb += i_decimate_8x8;

		if( b_decimate && (i_decimate_8x8 < 4)){
			M16( &pEncGlobal->mb.exmb_nzc[cache_pos[i8x8*4]] ) = 0;
			M16( &pEncGlobal->mb.exmb_nzc[cache_pos[i8x8*4+2]] ) = 0;
		}else{
			pEncLocal->nLumaCBP |= cbp<<i8x8;
		}
	}

	if( b_decimate && i_decimate_mb < 6){
		pEncLocal->nLumaCBP = 0;
		CLEAR_16x16_NNZ
	}else if(pEncLocal->nLumaCBP){
		for( i8x8 = 0; i8x8 < 4; i8x8++ )
			if( pEncLocal->nLumaCBP&(1<<i8x8) )
				Add8x8Idct( &pEncGlobal->pDec[0][(i8x8&1)*8 + (i8x8>>1)*8*FDEC_STRIDE], &dct4x4[i8x8<<6] );
	}
}

static VOINLINE VO_S32 IdctDQR2x2Dc( VO_S16 src[4], VO_S16 dct[4], VO_S32 *dequant_mf, VO_S32 nQP )
{
    VO_S16 dst[4];
    IdctDQ2x2Dc4( dst, dct, dequant_mf, nQP );
    return ((src[0] ^ (dst[0]+32))| (src[1] ^ (dst[1]+32))
          | (src[2] ^ (dst[2]+32))| (src[3] ^ (dst[3]+32))) >> 6;
}

static VOINLINE VO_S32 RefineChromaDc( H264ENC *pEncGlobal, VO_S32 b_inter, VO_S32 nQP, VO_S16 dct4[4] )
{
    VO_S16 src[4];
    VO_S32 i;
    VO_S32 nz = 0;

    if( pEncGlobal->dequant_table[nQP%6<<4] << (nQP/6) > 32*64 )
        return 1;

    IdctDQ2x2Dc4( src, dct4, pEncGlobal->dequant_table, nQP );
    src[0] += 32;src[1] += 32;src[2] += 32;src[3] += 32;

    if( !((src[0]|src[1]|src[2]|src[3])>>6) )
        return 0;

    for( i = 3; i >= 0; i-- )
    {
        VO_S32 sign = dct4[i] < 0 ? -1 : 1;
        VO_S32 level = dct4[i];

        if( !level )
            continue;

        while( level )
        {
            dct4[i] = (VO_S16)(level - sign);
            if( IdctDQR2x2Dc( src, dct4, pEncGlobal->dequant_table, nQP ) )
                break;
            level -= sign;
        }

        nz |= level;
        dct4[i] = (VO_S16)level;
    }

    return !!nz;
}

void GetChromaResidual( H264ENC *pEncGlobal, H264ENC_L *pEncLocal )
{
    VO_S32 i, ch, nz, nz_dc;
	const VO_S32 b_inter = !IS_INTRA( pEncLocal->nMBType );
    VO_S32 b_decimate = b_inter && pEncGlobal->InternalParam.b_dct_decimate;
	VO_S32 nQP = pEncLocal->nChromaQP;
    VO_ALIGNED_ARRAY_16( VO_S16, dct_DC,[4] );


    /* Early termination: check variance of chroma residual before encoding.
     * Don't bother trying early termination at low QPs.
     * Values are experimentally derived. */
    if( b_decimate && nQP >= 18 ){
        VO_S32 thresh = (Lambda2Tab[nQP] + 32) >> 6;
        VO_S32 ssd[2];
        VO_S32 score  = Var2_8x8( pEncGlobal->pCur[1], pEncGlobal->pDec[1], FENC_STRIDE, FDEC_STRIDE, &ssd[0] );
            score += Var2_8x8( pEncGlobal->pCur[2], pEncGlobal->pDec[2], FENC_STRIDE, FDEC_STRIDE, &ssd[1] );

        if( score < thresh*4 ){
            pEncGlobal->mb.exmb_nzc[cache_pos[16]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[17]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[18]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[19]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[20]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[21]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[22]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[23]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[25]] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[26]] = 0;

            for( ch = 0; ch < 2; ch++ ){
                if( ssd[ch] > thresh ){
                    Sub8x8DctDc( dct_DC, pEncGlobal->pCur[1+ch], pEncGlobal->pDec[1+ch]);
                    Dct2x2DcOnly( dct_DC );
                    nz_dc =  Quant2x2Dc( dct_DC, pEncGlobal->quant_table[nQP<<4]>>1, pEncGlobal->quant_bias[b_inter][nQP<<4]<<1 );
   
                    if( nz_dc ){
                        if( !RefineChromaDc( pEncGlobal, b_inter, nQP, dct_DC ) )
                            continue;

                        pEncGlobal->mb.exmb_nzc[cache_pos[25]+ch] = 1;
                        Zigzag2x2Dc( pEncGlobal->dct.chroma_dc[ch], dct_DC );
                        IdctDQ2x2Dc4( dct_DC, dct_DC, pEncGlobal->dequant_table, nQP );
                        Add8x8IdctDc( pEncGlobal->pDec[1+ch], dct_DC );
                        pEncLocal->nChromaCBP = 1;
                    }
                }
            }
            return;
        }
    }

    for( ch = 0; ch < 2; ch++ ){
        VO_U8  *p_src = pEncGlobal->pCur[1+ch];
        VO_U8  *p_dst = pEncGlobal->pDec[1+ch];
        VO_S32 i_decimate_score = 0;
        VO_S32 nz_ac = 0;

        VO_ALIGNED_ARRAY_16( VO_S16, dct16,[4*16] );
        Sub8x8Dct( dct16, p_src, p_dst ,FDEC_STRIDE);
        Dct2x2Dc ( dct_DC, dct16 );

        /* calculate dct coeffs */
		for( i = 0; i < 4; i++ ){
			nz =  Quant4x4( &dct16[i<<4], &pEncGlobal->quant_table[nQP<<4], &pEncGlobal->quant_bias[b_inter][nQP<<4] );
			pEncGlobal->mb.exmb_nzc[cache_pos[16+i+ch*4]] = (VO_U8)nz;
			if( nz ){
				nz_ac = 1;
				Zigzag4x4( pEncGlobal->dct.ac_4x4[16+i+ch*4], &dct16[i<<4] );
				 Dequant4x4( &dct16[i<<4], pEncGlobal->dequant_table, nQP );
				if( b_decimate )
					i_decimate_score += NoneResDis( pEncGlobal->dct.ac_4x4[16+i+ch*4]+1,15 );
			}
		}

		/*dc*/
		nz_dc =  Quant2x2Dc( dct_DC, pEncGlobal->quant_table[nQP<<4]>>1, pEncGlobal->quant_bias[b_inter][nQP<<4]<<1 );

		pEncGlobal->mb.exmb_nzc[cache_pos[25+ch]] = (VO_U8)nz_dc;

        if( (b_decimate && i_decimate_score < 7) || !nz_ac ){
            /* Decimate the block */
            pEncGlobal->mb.exmb_nzc[cache_pos[16+0]+24*ch] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[16+1]+24*ch] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[16+2]+24*ch] = 0;
            pEncGlobal->mb.exmb_nzc[cache_pos[16+3]+24*ch] = 0;
            if( !nz_dc ) /* Whole block is empty */
                continue;
            if( !RefineChromaDc( pEncGlobal, b_inter, nQP, dct_DC ) ){
                pEncGlobal->mb.exmb_nzc[cache_pos[25+ch]] = 0;
                continue;
            }
            /* DC-only */
            Zigzag2x2Dc( pEncGlobal->dct.chroma_dc[ch], dct_DC );
            IdctDQ2x2Dc4( dct_DC, dct_DC, pEncGlobal->dequant_table, nQP );
            Add8x8IdctDc( p_dst, dct_DC );
        }else{
            pEncLocal->nChromaCBP = 1;
            if( nz_dc ){
                Zigzag2x2Dc( pEncGlobal->dct.chroma_dc[ch], dct_DC );
                IdctDQ2x2Dc( dct_DC, dct16, pEncGlobal->dequant_table, nQP );
            }
            Add8x8Idct( p_dst, dct16 );
        }
    }

    if( pEncLocal->nChromaCBP )
        pEncLocal->nChromaCBP = 2;    /* dc+ac (we can't do only ac) */
    else if( pEncGlobal->mb.exmb_nzc[cache_pos[25]] |
             pEncGlobal->mb.exmb_nzc[cache_pos[26]] )
        pEncLocal->nChromaCBP = 1;    /* dc only */
}



/*****************************************************************************
 * MBCompensation:
 *****************************************************************************/
void MBCompensation( H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
	VO_S32 nQP = pEncLocal->nQP;

    pEncLocal->nLumaCBP = 0x00;
	pEncLocal->nChromaCBP = 0x00;
    pEncGlobal->mb.exmb_nzc[cache_pos[24]] = 0;


    if(  IS_INTRA( pEncLocal->nMBType) ){
        const VO_S32 nLumaMode = pEncLocal->nLumaMode;
        const VO_S32 nChormaMode = pEncLocal->nChormaMode;
        PredictIntraLuma16x16[nLumaMode]( pEncGlobal->pDec[0], FDEC_STRIDE, pEncGlobal->pDec[0], FDEC_STRIDE);

        GetIntraLumaResidual( pEncGlobal, nQP ,pEncLocal );
 
        PredictIntraChroma8x8[nChormaMode]( pEncGlobal->pDec[1], FDEC_STRIDE,  pEncGlobal->pDec[1], FDEC_STRIDE);
        PredictIntraChroma8x8[nChormaMode]( pEncGlobal->pDec[2], FDEC_STRIDE,  pEncGlobal->pDec[2], FDEC_STRIDE);

	} else {   /* Inter MB */
        if( !pEncGlobal->mb.b_skip_mc )
            InterMBCompesation( pEncGlobal ,pEncLocal);/*luma and chroma together*/
		
		if(pEncLocal->nMBType == PSKIP){
			memset( pEncGlobal->mb.exmb_nzc, 0, AVC_SCAN8_SIZE );
			return;
		}
		GetInterLumaResidual(pEncGlobal, nQP , pEncLocal);

    }
    GetChromaResidual( pEncGlobal, pEncLocal);

    /* Check for P_SKIP*/
	if( pEncLocal->nMBType == PL0 &&// pEncGlobal->mb.i_partition == D_16x16 &&
		!(pEncLocal->nLumaCBP | pEncLocal->nChromaCBP) &&
		M32( pEncGlobal->mb.mv[cache_pos[0]] ) == M32( pEncGlobal->mb.pskip_mv )
		&& pEncGlobal->mb.ref[cache_pos[0]] == 0 )
	{
		pEncLocal->nMBType  = PSKIP;
	}

}                  

/*****************************************************************************
 * PredictSkipMB:
 *  Check if the current MB could be encoded as a [PB]_SKIP (it supposes you use
 *  the previous QP
 *****************************************************************************/
VO_S32 PredictSkipMB( H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
    VO_ALIGNED_ARRAY_16( VO_S16, dct4x4,[4*16] );
    VO_ALIGNED_ARRAY_16( VO_S16, dct2x2,[4] );
    VO_ALIGNED_ARRAY_16( VO_S16, dctscan,[16] );

	VO_S32 nQP = pEncLocal->nQP;
    VO_S32 mvp[2];
    VO_S32 ch, thresh;

    VO_S32 i8x8, i4x4;
    VO_S32 i_decimate_mb;

	VO_U8 *src;
	VO_S32 i_src_stride = pEncGlobal->i_stride[0];
	//add by Really Yang 1124
	VO_S32 mvh[2];
    //end of add 

	/* Get the MV */
	mvp[0] = AVSClip3( pEncGlobal->mb.pskip_mv[0], pEncLocal->min_MV[0], pEncLocal->max_MV[0] );
	mvp[1] = AVSClip3( pEncGlobal->mb.pskip_mv[1], pEncLocal->min_MV[1], pEncLocal->max_MV[1] );

	src = pEncGlobal->pRef[0] + (mvp[1]>>2)*i_src_stride + (mvp[0]>>2);
	
    //add by Really Yang 1124
	mvh[0] = mvp[0]&3;
	mvh[1] = mvp[1]&3;

	if(mvh[0]&&mvh[1]){
#if defined(VOARMV7)

		HpelFilter16X16_C_ARMV7(pEncLocal->pix[3],32,src,i_src_stride,pEncLocal->scratch_buffer);
#else
		HpelFilter16X16_C_C(pEncLocal->pix[3],32,src,i_src_stride,pEncLocal->scratch_buffer);
#endif
		src = pEncLocal->pix[3];
		i_src_stride = 32;
	}
	else if(mvh[0]){
#if defined(VOARMV7)
		HpelFilter16X16_H_ARMV7(pEncLocal->pix[3],32,src,i_src_stride);
#else
		HpelFilter16X16_H_C(pEncLocal->pix[3],32,src,i_src_stride);
#endif
		src = pEncLocal->pix[3];
		i_src_stride = 32;
	}
	else if(mvh[1]){
#if defined(VOARMV7)
		HpelFilter16X16_V_ARMV7(pEncLocal->pix[3],32,src,i_src_stride);
#else
		HpelFilter16X16_V_C(pEncLocal->pix[3],32,src,i_src_stride);
#endif
		src = pEncLocal->pix[3];
		i_src_stride = 32;
	}
	//end of add

	/* Motion compensation */
	MCLuma( pEncGlobal->pDec[0], FDEC_STRIDE, src, i_src_stride, 16, 16);

		
    for( i8x8 = 0, i_decimate_mb = 0; i8x8 < 4; i8x8++ )
    {
        VO_S32 fenc_offset = (i8x8&1) * 8 + (i8x8>>1) * FENC_STRIDE * 8;
        VO_S32 fdec_offset = (i8x8&1) * 8 + (i8x8>>1) * FDEC_STRIDE * 8;
        /* get luma diff */
        Sub8x8Dct( dct4x4, pEncGlobal->pCur[0] + fenc_offset,
                                    pEncGlobal->pDec[0] + fdec_offset ,FDEC_STRIDE);
        /* encode one 4x4 block */
        for( i4x4 = 0; i4x4 < 4; i4x4++ ){
            if( ! Quant4x4( &dct4x4[i4x4<<4], &pEncGlobal->quant_table[nQP<<4], &pEncGlobal->quant_bias[1][nQP<<4] ) )
                continue;
            Zigzag4x4( dctscan, &dct4x4[i4x4<<4] );
            i_decimate_mb += NoneResDis( dctscan,16 );
            if( i_decimate_mb >= 6 )
                return 0;
        }
    }

    /* encode chroma */
    nQP = pEncLocal->nChromaQP;
    thresh = (Lambda2Tab[nQP] + 32) >> 6;

    for( ch = 0; ch < 2; ch++ )
    {
        VO_U8  *p_src = pEncGlobal->pCur[1+ch];
        VO_U8  *p_dst = pEncGlobal->pDec[1+ch];
		VO_S32 ssd;

		i_src_stride = pEncGlobal->i_stride[1+ch];
		src = pEncGlobal->pRef[1+ch] + (mvp[1]>>3)*i_src_stride + (mvp[0]>>3);
		//edit by Really Yang 1210
		//MCChroma( pEncGlobal->pDec[1+ch], FDEC_STRIDE, src, i_src_stride, mvp[0]&0x07, mvp[1]&0x07, 8, 8 );
#if defined(VOARMV7)

		MCChroma8x8_ARMV7( pEncGlobal->pDec[1+ch], FDEC_STRIDE, src, i_src_stride, mvp[0]&0x07, mvp[1]&0x07);
#else
		MCChroma8x8_c( pEncGlobal->pDec[1+ch], FDEC_STRIDE, src, i_src_stride, mvp[0]&0x07, mvp[1]&0x07);
#endif
		//end of edit
        /* there is almost never a termination during chroma, but we can't avoid the check entirely */
        /* so instead we check SSD and skip the actual check if the score is low enough. */
        ssd = ssd_8x8( p_dst, p_src, FDEC_STRIDE, FENC_STRIDE );
        if( ssd < thresh )
            continue;

        Sub8x8Dct( dct4x4, p_src, p_dst ,FDEC_STRIDE);

        /* calculate dct DC */
        Dct2x2Dc ( dct2x2, dct4x4 );
        if(  Quant2x2Dc( dct2x2, pEncGlobal->quant_table[nQP<<4]>>1, pEncGlobal->quant_bias[1][nQP<<4]<<1 ) )
            return 0;

        /* If there wasn't a termination in DC, we can check against a much higher threshold. */
        if( ssd < thresh*4 )
            continue;

        /* calculate dct coeffs */
        for( i4x4 = 0, i_decimate_mb = 0; i4x4 < 4; i4x4++ ){
            if( ! Quant4x4( &dct4x4[i4x4<<4], &pEncGlobal->quant_table[nQP<<4], &pEncGlobal->quant_bias[1][nQP<<4] ) )
                continue;
            Zigzag4x4( dctscan, &dct4x4[i4x4<<4] );
            i_decimate_mb += NoneResDis( dctscan+1,15 );
            if( i_decimate_mb >= 7 )
                return 0;
        }
    }

    pEncGlobal->mb.b_skip_mc = 1;
    return 1;
}



