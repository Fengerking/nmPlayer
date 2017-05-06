/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/************************************************************************
* @file h265dec_deblock.c
*
* H.265 decoder deblock filter functions define
*
* @author  Huaping Liu
* @date    2012-12-10
************************************************************************/
#include "h265_decoder.h"
#include "h265dec_deblock.h"
//#include "h265dec_pic.h"
//#include "h265dec_cu.h"
#include "h265dec_debug.h"
#include "stdlib.h"//YU_TBD

/* Temp table */
static const VO_U8 voG_aucChromaScale[58]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
	17,18,19,20,21,22,23,24,25,26,27,28,29,29,30,31,32,
	33,33,34,34,35,35,36,36,37,37,38,39,40,41,42,43,44,
	45,46,47,48,49,50,51
};

#define   voEDGE_VER    0
#define   voEDGE_HOR    1
#define   MIN_QP        0
#define   MAX_QP        51
#define   voQpUV(iQpY)  ( ((iQpY) < 0) ? (iQpY) : (((iQpY) > 57) ? ((iQpY)-6) : voG_aucChromaScale[(iQpY)]) )
#define   voDEFAULT_INTRA_TC_OFFSET           2 ///< Default intra TC offset
#define   voDEBLOCK_SMALLEST_BLOCK            8

// ====================================================================================================================
// Tables
// ====================================================================================================================
static const VO_U8 votccliptable_8x8[78] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
	1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,5,5,
	6,6,7,8,9,10,11,13,14,16,18,20,22,24,
	24,24,24,24,24,24,24,24,24,24,24,24
};

static const VO_U8 vobetacliptable[76] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	6,7,8,9,10,11,12,
	13,14,15,16,17,18,20,22,
	24,26,28,30,32,34,36,38,
	40,42,44,46,48,50,52,54,
	56,58,60,62,64,
	64,64,64,64,64,64,64,64,64,64,64,64
};

static const VO_U8* const vobetatable_8x8 = vobetacliptable+12;

static const VO_U8* const votctable_8x8 = votccliptable_8x8+12;


/* local inline functions */
static __inline int voCalcDP( VO_U8* piSrc, VO_S32 iOffset)
{
	return ABS( piSrc[-iOffset*3] - 2*piSrc[-iOffset*2] + piSrc[-iOffset] ) ;
}

static __inline int voCalcDQ( VO_U8* piSrc, VO_S32 iOffset)
{
	return ABS( piSrc[0] - 2*piSrc[iOffset] + piSrc[iOffset*2] );
}

static __inline VO_BOOL CheckTile_SliceEdge( H265_DEC_SLICE * p_slice, 
											 const VO_S32 xCb, 
											 const VO_S32 yCb, 
	                                         const VO_S32 xNbY, 
	                                         const VO_S32 yNbY)
{
	VO_S32 minBlockAddrCurr = p_slice->p_pps->MinTbAddrZs[yCb * p_slice->p_pps->MinTbAddrZsStride + xCb];
	VO_S32 minBlockAddrN = p_slice->p_pps->MinTbAddrZs[yNbY * p_slice->p_pps->MinTbAddrZsStride + xNbY];
	H265_DEC_PPS *p_pps = p_slice->p_pps;
  if (minBlockAddrN < 0 || 
		  minBlockAddrN > minBlockAddrCurr ||
          ((minBlockAddrN < p_slice->p_pps->tile_start_zs[p_slice->currEntryNum] ||minBlockAddrN < p_slice->SliceMinTbAddrZs) && 
          !p_pps->pps_loop_filter_across_slices_enabled_flag)|| 
          (minBlockAddrN < p_slice->p_pps->tile_start_zs[p_slice->currEntryNum] && !p_pps->loop_filter_across_tiles_enabled_flag)){
			return VO_FALSE;
	} else {
		return VO_TRUE;
	}
}

//#define CHECK_TILE_SLICE_EDGE_HOR(across_flag,y4b,nIndex) \
//	if(!pSlice->p_pps->pps_loop_filter_across_slices_enabled_flag)\
//	{\
//	  VO_S32* min_tb_addr_zs = &pSlice->p_pps->MinTbAddrZs[nIndex];\
//	  if(min_tb_addr_zs[-(VO_S32)pSlice->PicWidthInMinTbsY]<pSlice->m_uiDependentSliceCurStartCUAddr)\
//		across_flag =1;\
//	}\
//	if(!pSlice->p_pps->loop_filter_across_tiles_enabled_flag)\
//	{\
//	  if(!(y4b>(VO_U32)pSlice->tile_start_y[entry]))\
//		across_flag=1;\
//	}
//
//#define CHECK_TILE_SLICE_EDGE_VER(across_flag, x4b,nIndex) \
//	if(!pSlice->p_pps->pps_loop_filter_across_slices_enabled_flag)\
//	{\
//	  VO_S32* min_tb_addr_zs = &pSlice->p_pps->MinTbAddrZs[nIndex];\
//	  if(!(x4b>(VO_U32)pSlice->tile_start_x[entry])||min_tb_addr_zs[-1]<pSlice->m_uiDependentSliceCurStartCUAddr)\
//		across_flag = 1;\
//	}\
//	if(!pSlice->p_pps->loop_filter_across_tiles_enabled_flag)\
//	{\
//	  if(!(x4b>(VO_U32)pSlice->tile_start_x[entry]))\
//		across_flag = 1;\
//	}

// VO_U32 bs_times = 0;
/* Ver and Hor different Branch */
static VO_U8 voGetBsInfo(H265_DEC_SLICE *pSlice, 
						VO_U32 nOffset,
						VO_U32 nIndex4bk)
{
	VO_U8   uiBs = 0;
	const PicMvField *p_mv_field;

	//-- Set BS for not Intra MB : BS = 0 or 1
	p_mv_field = pSlice->p_motion_field + nIndex4bk;
	if(pSlice->slice_type == B_SLICE)
	{
		VO_S32 *piRefP0, *piRefP1,*piRefQ0,*piRefQ1;
		VO_S32 iRefIdxP0,iRefIdxP1,iRefIdxQ0,iRefIdxQ1;
		VO_S32 nP0MV,nP1MV,nQ0MV,nQ1MV;
		VO_S16 nP0Hor,nP0Ver,nP1Hor,nP1Ver,nQ0Hor,nQ0Ver,nQ1Hor,nQ1Ver;
		iRefIdxP0 = (p_mv_field-nOffset)->m_iRefIdx[REF_PIC_LIST_0];
		iRefIdxQ0 = (p_mv_field)->m_iRefIdx[REF_PIC_LIST_0];
		iRefIdxP1 = (p_mv_field-nOffset)->m_iRefIdx[REF_PIC_LIST_1];
		iRefIdxQ1 = (p_mv_field)->m_iRefIdx[REF_PIC_LIST_1];			


		if((iRefIdxP0 != iRefIdxQ0)||(iRefIdxP1 != iRefIdxQ1))
		{	
			piRefP0 = (iRefIdxP0 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_0][iRefIdxP0];
			piRefP1 = (iRefIdxP1 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_1][iRefIdxP1];
			piRefQ0 = (iRefIdxQ0 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_0][iRefIdxQ0];
			piRefQ1 = (iRefIdxQ1 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_1][iRefIdxQ1];
			if(piRefP0 != piRefQ1||piRefP1 != piRefQ0)
				return 1;
		}

		nP0MV = (VO_S32)((p_mv_field-nOffset)->m_acMv[REF_PIC_LIST_0]); 
		nP1MV = (VO_S32)((p_mv_field-nOffset)->m_acMv[REF_PIC_LIST_1]); 
		nQ0MV = (VO_S32)((p_mv_field)->m_acMv[REF_PIC_LIST_0]); 
		nQ1MV = (VO_S32)((p_mv_field)->m_acMv[REF_PIC_LIST_1]); 

		if(nP0MV == nQ0MV && nP1MV== nQ1MV && nP0MV == nQ1MV && nP1MV== nQ0MV)
			return 0;

		piRefP0 = (iRefIdxP0 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_0][iRefIdxP0];
		piRefP1 = (iRefIdxP1 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_1][iRefIdxP1];
		
		if ( piRefP0 != piRefP1 )   // Different L0 & L1
		{
			if( (iRefIdxP0 == iRefIdxQ0)&&(iRefIdxP1 == iRefIdxQ1) )
			{
				if(nP0MV == nQ0MV && nP1MV== nQ1MV)
					return 0;
				nP0Hor = (VO_S16)(nP0MV&0xffff); nP0Ver = (VO_S16)(nP0MV>>16);
				nP1Hor = (VO_S16)(nP1MV&0xffff); nP1Ver = (VO_S16)(nP1MV>>16);
				nQ0Hor = (VO_S16)(nQ0MV&0xffff); nQ0Ver = (VO_S16)(nQ0MV>>16);
				nQ1Hor = (VO_S16)(nQ1MV&0xffff); nQ1Ver = (VO_S16)(nQ1MV>>16);
				uiBs = (ABS(nP0Hor-nQ0Hor) >= 4);
				if(uiBs)
					return 1;
				else
				{
					uiBs |= (ABS(nP0Ver-nQ0Ver) >= 4);
					if(uiBs)
						return 1;
					else
						uiBs |= (ABS(nP1Hor-nQ1Hor) >= 4) | (ABS(nP1Ver-nQ1Ver) >= 4);
				}
				return uiBs;
			}

			piRefQ0 = (iRefIdxQ0 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_0][iRefIdxQ0];
			piRefQ1 = (iRefIdxQ1 < 0) ? NULL : (VO_S32 *)pSlice->ref_pic_list[REF_PIC_LIST_1][iRefIdxQ1];
			if(piRefP0 == piRefQ1 && piRefP1 == piRefQ0 )
			{
				if(nP0MV == nQ1MV && nP1MV== nQ0MV)
					return 0;
				nP0Hor = (VO_S16)(nP0MV&0xffff); nP0Ver = (VO_S16)(nP0MV>>16);
				nP1Hor = (VO_S16)(nP1MV&0xffff); nP1Ver = (VO_S16)(nP1MV>>16);
				nQ0Hor = (VO_S16)(nQ0MV&0xffff); nQ0Ver = (VO_S16)(nQ0MV>>16);
				nQ1Hor = (VO_S16)(nQ1MV&0xffff); nQ1Ver = (VO_S16)(nQ1MV>>16);
				uiBs = (ABS(nP0Hor-nQ1Hor) >= 4) | (ABS(nP0Ver-nQ1Ver) >= 4) |
						(ABS(nP1Hor-nQ0Hor) >= 4) | (ABS(nP1Ver-nQ0Ver) >= 4);					
				return uiBs;
			}
		}

		nP0Hor = (VO_S16)(nP0MV&0xffff); nP0Ver = (VO_S16)(nP0MV>>16);
		nP1Hor = (VO_S16)(nP1MV&0xffff); nP1Ver = (VO_S16)(nP1MV>>16);
		nQ0Hor = (VO_S16)(nQ0MV&0xffff); nQ0Ver = (VO_S16)(nQ0MV>>16);
		nQ1Hor = (VO_S16)(nQ1MV&0xffff); nQ1Ver = (VO_S16)(nQ1MV>>16);

		// Same L0 & L1
		uiBs = ( (ABS(nP0Hor - nQ0Hor) >= 4) | (ABS(nP0Ver - nQ0Ver) >= 4) |
			(ABS(nP1Hor - nQ1Hor) >= 4) | (ABS(nP1Ver - nQ1Ver) >= 4) ) &&
			( (ABS(nP0Hor - nQ1Hor) >= 4) | (ABS(nP0Ver - nQ1Ver) >= 4) |
			(ABS(nP1Hor - nQ0Hor) >= 4) | (ABS(nP1Ver - nQ0Ver) >= 4) );
		return uiBs;			
	}
	else  // pcSlice->isInterP()
	{
		VO_S32 iRefIdx0,  iRefIdx1;
		VO_S32 nP0MV,nQ0MV;
		VO_S16 nP0Hor,nP0Ver,nQ0Hor,nQ0Ver;

		nP0MV = (p_mv_field-nOffset)->m_acMv[REF_PIC_LIST_0];
		nQ0MV = p_mv_field->m_acMv[REF_PIC_LIST_0];
		iRefIdx0 = (p_mv_field- nOffset)->m_iRefIdx[REF_PIC_LIST_0];
		iRefIdx1 = (p_mv_field)->m_iRefIdx[REF_PIC_LIST_0];

		if(nP0MV == nQ0MV)
		{
			uiBs = (iRefIdx0 != iRefIdx1);
		}
		else
		{					
			nP0Hor = (VO_S16)(nP0MV&0xffff); 
			nP0Ver = (VO_S16)(nP0MV>>16);
			nQ0Hor = (VO_S16)(nQ0MV&0xffff); 
			nQ0Ver = (VO_S16)(nQ0MV>>16);
			uiBs = (iRefIdx0 != iRefIdx1) | (ABS(nP0Hor-nQ0Hor) >= 4) | (ABS(nP0Ver-nQ0Ver) >= 4);
		}				
		
		return uiBs;
	}
}

/* Local functions */
static void voSetEdgefilterMultipleInterVer(H265_DEC_SLICE *pSlice, 
					 VO_U32  BaseUnits, 
					 VO_S32	 iEdgeIdx,
					 VO_U32 x4b,
					 VO_U32 y4b
					 )
{
	VO_U32  ui = 0, nIndex = 0;
	//VO_U32 across_flag = 0;

	x4b += iEdgeIdx;

	if (!(x4b & 0x1))
	{
		nIndex =  (y4b)*pSlice->PicWidthInMinTbsY + x4b;
		//CHECK_TILE_SLICE_EDGE_VER(across_flag, x4b,nIndex);
		//if(!across_flag)
		{
			for(ui = 0; ui < BaseUnits; ui++ )
			{
				if(pSlice->p_pBsVer[nIndex])
				{
					nIndex += pSlice->PicWidthInMinTbsY;
					continue;
				}
				pSlice->p_pBsVer[nIndex] = voGetBsInfo( pSlice, 1, nIndex);
				nIndex += pSlice->PicWidthInMinTbsY;
			}
		}
	}
}

static void voSetEdgefilterMultipleInterHor(H265_DEC_SLICE *pSlice, 
					 VO_U32  BaseUnits, 
					 VO_S32	 iEdgeIdx,
					 VO_U32 x4b,
					 VO_U32 y4b
					 )
{
	VO_U32  ui = 0, nIndex = 0;

	y4b += iEdgeIdx;
	if (!(y4b & 0x1))
	{
		nIndex = (y4b)*pSlice->PicWidthInMinTbsY + x4b;
		//CHECK_TILE_SLICE_EDGE_HOR(across_flag,y4b,nIndex)
		//if(!across_flag)
		{
			for(ui = 0; ui < BaseUnits; ui++ )
			{
				if(pSlice->p_pBsHor[nIndex])
				{
					nIndex ++;
					continue;
				}
				pSlice->p_pBsHor[nIndex] = voGetBsInfo( pSlice, pSlice->PicWidthInMinTbsY, nIndex);
				nIndex++;
			}
		}
	}
}
void voSetEdgefilterTUVer(void *pGst, VO_U32 BaseUnits, VO_U32 nIndex)
{ 
  VO_U32 ui = 0;
  H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pGst;
  for (ui = 0; ui < BaseUnits; ui++)
  {
	VO_U32 *p_predmode = pSlice->deblock_para + nIndex;
	if((*(p_predmode-1)& 0xff) == MODE_INTRA || (*(p_predmode)& 0xff) == MODE_INTRA)
	  pSlice->p_pBsVer[nIndex] = 2;
	else
	{
	  VO_U32 *p_cbf = pSlice->deblock_para + nIndex;
	  if((*p_cbf& 0xff00) || (*(p_cbf-1)& 0xff00) ) 
		pSlice->p_pBsVer[nIndex] = 1;
	  else
		pSlice->p_pBsVer[nIndex] = voGetBsInfo( pSlice, 1 ,nIndex);
	}
	nIndex += pSlice->PicWidthInMinTbsY;
  }
}

void voSetEdgefilterTUHor(void *pGst, VO_U32 BaseUnits, VO_U32 nIndex)
{ 
  VO_U32 ui = 0;
  H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pGst;
  for (ui = 0; ui < BaseUnits; ui++)
  {
	VO_U32 *p_predmode = pSlice->deblock_para + nIndex;
	if((*(p_predmode-pSlice->PicWidthInMinTbsY)& 0xff) == MODE_INTRA || (*(p_predmode)& 0xff) == MODE_INTRA)
	  pSlice->p_pBsHor[nIndex] = 2;
	else
	{
      VO_U32 *p_cbf = pSlice->deblock_para + nIndex;
	  if((*p_cbf& 0xff00) || (*(p_cbf-pSlice->PicWidthInMinTbsY)& 0xff00) ) 
		pSlice->p_pBsHor[nIndex] = 1;
	  else
		pSlice->p_pBsHor[nIndex] = voGetBsInfo( pSlice, pSlice->PicWidthInMinTbsY ,nIndex);
	}
	nIndex++;
  }
}

/*void voSetEdgefilterTU(void *pGst, 
							 VO_U32 BaseUnits, 
							 VO_U32 x4b,
							 VO_U32 y4b,
							 VO_U32 nV,
							 VO_U32 nH)
{  
	VO_U32 ui = 0, nIndex = 0;
	H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pGst;
	VO_U32 across_flag = 0;

	//voEDGE_VER 
	if (!(x4b & 0x1) && x4b > 0 && nV)
	{
		nIndex = (y4b)*pSlice->PicWidthInMinTbsY + x4b;
		//CHECK_TILE_SLICE_EDGE_VER(across_flag, x4b,nIndex)
		//if(!across_flag)
		{
			for (ui = 0; ui < BaseUnits; ui++)
			{
				//VO_U8 *p_predmode = pSlice->p_predmode_tmp + nIndex;
				VO_U32 *p_predmode = pSlice->deblock_para + nIndex;

				if((*(p_predmode-1) & 0xff) == MODE_INTRA || (*(p_predmode) & 0xff) == MODE_INTRA)
					pSlice->p_pBsVer[nIndex] = 2;
				else
				{
					//VO_U8 *p_cbf = pSlice->luma_cbf + nIndex;
					VO_U32 *p_cbf = pSlice->deblock_para + nIndex;

					if(((*p_cbf & 0xff00)>>8) || ((*(p_cbf-1) & 0xff00)>>8)) 
						pSlice->p_pBsVer[nIndex] = 1;
					else
						pSlice->p_pBsVer[nIndex] = voGetBsInfo( pSlice, 1 ,nIndex);
				}
				nIndex += pSlice->PicWidthInMinTbsY;
			}
		}
	}

	if (!(y4b & 0x1) && y4b > 0 && nH)
	{
		//voEDGE_HOR
		across_flag = 0;
		nIndex = (y4b)*pSlice->PicWidthInMinTbsY + x4b ;
		//CHECK_TILE_SLICE_EDGE_HOR(across_flag,y4b,nIndex)
		//if(!across_flag)
		{
			for (ui = 0; ui < BaseUnits; ui++)
			{	
				//VO_U8 *p_predmode = pSlice->p_predmode_tmp + nIndex;
				VO_U32 *p_predmode = pSlice->deblock_para + nIndex;

				if((*(p_predmode-pSlice->PicWidthInMinTbsY) & 0xff) == MODE_INTRA || (*(p_predmode)&0xff) == MODE_INTRA)
					pSlice->p_pBsHor[nIndex] = 2;
				else
				{
					//VO_U8 *p_cbf = pSlice->luma_cbf + nIndex;
					VO_U32 *p_cbf = pSlice->deblock_para + nIndex;
					if(((*p_cbf & 0xff00)>>8) || ((*(p_cbf-pSlice->PicWidthInMinTbsY)&0xff00)>>8)) 
						pSlice->p_pBsHor[nIndex] = 1;
					else
						pSlice->p_pBsHor[nIndex] = voGetBsInfo(pSlice, pSlice->PicWidthInMinTbsY, nIndex);
				}
				nIndex++;
			}
		}
	}
}
*/
void voSetEdgefilterCU(void *pGst, 
							 VO_U32  BaseUnits, 
							 VO_U32 x4b,
							 VO_U32 y4b,
							 VO_S32 entry)
{  
	VO_U32 ui = 0, nIndex = 0,across_flag=0;
	H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pGst;

	//voEDGE_VER 
	if (x4b > 0)
	{
		nIndex = (y4b)*pSlice->PicWidthInMinTbsY + x4b ;
		if(CheckTile_SliceEdge(pSlice, x4b, y4b, x4b-1, y4b))
		{
			for (ui = 0; ui < BaseUnits; ui++)
			{	
				//VO_U8 *p_predmode = pSlice->p_predmode_tmp + nIndex;
				VO_U32 *p_predmode = pSlice->deblock_para + nIndex;
				if((*(p_predmode-1)&0xff) == MODE_INTRA || (*(p_predmode)&0xff) == MODE_INTRA)
					pSlice->p_pBsVer[nIndex] = 2;
				else
				{
					//VO_U8 *p_cbf = pSlice->luma_cbf + nIndex;
					VO_U32 *p_cbf = pSlice->deblock_para + nIndex;
					if(((*p_cbf & 0xff00)>>8) || ((*(p_cbf-1) & 0xff00)>>8)) 
						pSlice->p_pBsVer[nIndex] = 1;
					else
						pSlice->p_pBsVer[nIndex] = voGetBsInfo( pSlice, 1, nIndex);
				}
				nIndex += pSlice->PicWidthInMinTbsY;
			}
		}
	}

	if (y4b > 0)
	{
		//voEDGE_HOR
		across_flag = 0;
		nIndex = (y4b)*pSlice->PicWidthInMinTbsY + x4b;
		if(CheckTile_SliceEdge(pSlice, x4b, y4b, x4b, y4b-1))
		{
			for (ui = 0; ui < BaseUnits; ui++)
			{
				//VO_U8 *p_predmode = pSlice->p_predmode_tmp + nIndex;
				VO_U32 *p_predmode = pSlice->deblock_para + nIndex;
				if((*(p_predmode-pSlice->PicWidthInMinTbsY) & 0xff) == MODE_INTRA || (*(p_predmode)&0xff) == MODE_INTRA)
					pSlice->p_pBsHor[nIndex] = 2;
				else
				{
					//VO_U8 *p_cbf = pSlice->luma_cbf + nIndex;
					VO_U32 *p_cbf = pSlice->deblock_para + nIndex;
					if(((*p_cbf&0xff00)>>8) || ((*(p_cbf-pSlice->PicWidthInMinTbsY)&0xff00)>>8)) 
						pSlice->p_pBsHor[nIndex] = 1;
					else
						pSlice->p_pBsHor[nIndex] = voGetBsInfo(pSlice, pSlice->PicWidthInMinTbsY,nIndex);
				}
				nIndex++;
			}
		}
	}
}

void voSetEdgefilterPU( void* pGst, VO_U32 BaseUnits, VO_U32 x4b, VO_U32 y4b, VO_U32 uiDepth)
{
	H265_DEC_SLICE* pSlice = (H265_DEC_SLICE*)pGst;

	switch(pSlice->CuPartMode)
	{
	case PART_2NxN:
		{
			voSetEdgefilterMultipleInterHor( pSlice, BaseUnits, BaseUnits>>1, x4b, y4b);
			break;
		}
	case PART_Nx2N:
		{
			voSetEdgefilterMultipleInterVer( pSlice, BaseUnits, BaseUnits>>1, x4b, y4b);
			break;
		}
  case PART_NxN:
    {
      voSetEdgefilterMultipleInterHor( pSlice, BaseUnits, BaseUnits>>1, x4b, y4b);
      voSetEdgefilterMultipleInterVer( pSlice, BaseUnits, BaseUnits>>1, x4b, y4b);   
      break;
    }
	case PART_2NxnU:
		{
			if (uiDepth < 2)
			{
				voSetEdgefilterMultipleInterHor( pSlice, BaseUnits, BaseUnits>>2, x4b, y4b);
			}
			break;
		}
	case PART_2NxnD:
		{
			if (uiDepth < 2)
			{
				voSetEdgefilterMultipleInterHor( pSlice, BaseUnits, BaseUnits - (BaseUnits>>2), x4b, y4b);
			}
			break;
		}
	case PART_nLx2N:
		{
			if (uiDepth < 2)
			{
				voSetEdgefilterMultipleInterVer( pSlice, BaseUnits, BaseUnits>>2, x4b, y4b);
			}
			break;
		}
	case PART_nRx2N:
		{
			if (uiDepth < 2)
			{
				voSetEdgefilterMultipleInterVer( pSlice, BaseUnits, BaseUnits - (BaseUnits>>2), x4b, y4b);
			}	
			break;
		}
	default:
		{
			break;
		}
	}
}
/* sw and Pels filter function */
/*
* Decision between strong and weak filter
*/
// static __inline VO_BOOL voUseStrongFilteringC(VO_S32 offset,       /* i: offset value for picture data */
// 											  VO_S32 d,            /* i: d value */
// 											  VO_S32 beta,         /* i: beta value */
// 											  VO_S32 tc,           /* i: tc value */
// 											  VO_U8  *piSrc        /* i: pointer to picture data */
// 											  )
// {
// 	VO_U8 m4  = piSrc[0];
// 	VO_U8 m3  = piSrc[-offset];
// 	VO_U8 m7  = piSrc[ offset*3];
// 	VO_U8 m0  = piSrc[-offset*4];
// 	VO_S32 d_strong = ABS(m0-m3) + ABS(m7-m4);
// 	return ( (d_strong < (beta>>3)) && (d<(beta>>2)) && ( ABS(m3-m4) < ((tc*5+1)>>1)) );
// }


/* Pel Filter Chroma 2 lines Cb and 2 lines Cr one Time */
// static __inline void voPelAllChromaFilter(VO_U8   *pCbSrc,
// 										  VO_U8   *pCrSrc,
// 										  VO_S32  iStride,
// 										  VO_S32  iOffset,
// 										  VO_U8   tc,
// 										  VO_BOOL bNoP,
// 										  VO_BOOL bNoQ
// 										  )
// {
// 	VO_S32 i;
// 	VO_U8  m2, m3, m4, m5;
// 	VO_S8  delta;
// 	for (i = 0; i < 4; i++)
// 	{
// 		m2  = pCbSrc[-iOffset*2];
// 		m3  = pCbSrc[-iOffset];
// 		m4  = pCbSrc[0];
// 		m5  = pCbSrc[ iOffset];
// 		delta = Clip3(-tc,tc, (((( m4 - m3 ) << 2 ) + m2 - m5 + 4 ) >> 3) );
// 		pCbSrc[-iOffset] = Clip(m3+delta);
// 		pCbSrc[0] = Clip(m4-delta);
// 		if (bNoP)
// 		{
// 			pCbSrc[-iOffset] = m3;
// 		}
// 		if (bNoQ)
// 		{
// 			pCbSrc[0] = m4;
// 		}
// 
// 		m2  = pCrSrc[-iOffset*2];
// 		m3  = pCrSrc[-iOffset];
// 		m4  = pCrSrc[0];
// 		m5  = pCrSrc[iOffset];
// 		delta = Clip3(-tc,tc, (((( m4 - m3 ) << 2 ) + m2 - m5 + 4 ) >> 3) );
// 		pCrSrc[-iOffset] = Clip(m3+delta);
// 		pCrSrc[0] = Clip(m4-delta);
// 		if (bNoP)
// 		{
// 			pCrSrc[-iOffset] = m3;
// 		}
// 		if (bNoQ)
// 		{
// 			pCrSrc[0] = m4;
// 		}
// 
// 		pCbSrc += iStride;
// 		pCrSrc += iStride;
// 	}
// }
// 
// /* Strong Luma Filter */
// static __inline void voPelAllLumaFilterS(VO_U8   *piSrc,
// 										 VO_S32  iStride,
// 										 VO_S32  iOffset,
// 										 VO_U8   tc,
// 										 VO_S32  nIdx,
// 										 VO_BOOL bNoP,
// 										 VO_BOOL bNoQ
// 										 )
// {
// 	VO_U8  nTemp;
// 	VO_S32 i;
// 	VO_U8  m0,m1,m2,m3,m4,m5,m6,m7;
// 	nTemp = tc << 1;
// 	for (i = 0; i < 4*nIdx; i++)
// 	{	
// 		m0  = piSrc[-iOffset*4];
// 		m1  = piSrc[-iOffset*3];
// 		m2  = piSrc[-iOffset*2];
// 		m3  = piSrc[-iOffset];
// 		m4  = piSrc[0];
// 		m5  = piSrc[ iOffset];
// 		m6  = piSrc[ iOffset*2];
// 		m7  = piSrc[ iOffset*3];
// 
// 		piSrc[-iOffset*3] = Clip3(m1-nTemp, m1+nTemp, ((2*m0 + 3*m1 + m2 + m3 + m4 + 4 )>>3));
// 		piSrc[-iOffset*2] = Clip3(m2-nTemp, m2+nTemp, ((m1 + m2 + m3 + m4 + 2)>>2));
// 		piSrc[-iOffset]   = Clip3(m3-nTemp, m3+nTemp, ((m1 + 2*m2 + 2*m3 + 2*m4 + m5 + 4) >> 3));
// 		piSrc[0]          = Clip3(m4-nTemp, m4+nTemp, ((m2 + 2*m3 + 2*m4 + 2*m5 + m6 + 4) >> 3));
// 		piSrc[ iOffset]   = Clip3(m5-nTemp, m5+nTemp, ((m3 + m4 + m5 + m6 + 2)>>2));
// 		piSrc[ iOffset*2] = Clip3(m6-nTemp, m6+nTemp, ((m3 + m4 + m5 + 3*m6 + 2*m7 +4 )>>3));
// 
// 		if (bNoP)
// 		{
// 			piSrc[-iOffset] = m3;
// 			piSrc[-iOffset*2] = m2;
// 			piSrc[-iOffset*3] = m1;
// 		}
// 		if (bNoQ)
// 		{
// 			piSrc[0] = m4;
// 			piSrc[ iOffset] = m5;
// 			piSrc[ iOffset*2] = m6;
// 		}
// 		piSrc += iStride;
// 	}
// }
// 
// /* Weak Luma Filter */
// static __inline void voPelAllLumaFilterW(VO_U8   *piSrc,
// 										 VO_S32  iStride,
// 										 VO_S32  iOffset,
// 										 VO_U8   tc,
// 										 VO_BOOL bFilterSecondP,
// 										 VO_BOOL bFilterSecondQ,
// 										 VO_BOOL bNoP,
// 										 VO_BOOL bNoQ
// 										 )
// {
// 	VO_S8  delta;
// 	VO_U8  tc2 = tc >> 1;
// 	VO_U8  iThrCut = tc * 10; 
// 	VO_S32 i;
// 	VO_U8  m1,m2,m3,m4,m5,m6;
// 	for (i = 0; i < 4; i++)
// 	{	
// 		m1  = piSrc[-iOffset*3];
// 		m2  = piSrc[-iOffset*2];
// 		m3  = piSrc[-iOffset];
// 		m4  = piSrc[0];
// 		m5  = piSrc[ iOffset];
// 		m6  = piSrc[ iOffset*2];
// 		/* Weak filter */
// 		delta = (9*(m4-m3) - 3*(m5-m2) + 8)>>4 ;
// 		if ( ABS(delta) < iThrCut )
// 		{
// 			delta = Clip3(-tc, tc, delta);        
// 			piSrc[-iOffset] = Clip(m3+delta);
// 			piSrc[0] = Clip(m4-delta);
// 			if(bFilterSecondP)
// 			{
// 				VO_S8 delta1 = Clip3(-tc2, tc2, (( ((m1+m3+1)>>1)- m2+delta)>>1));
// 				piSrc[-iOffset*2] = Clip(m2+delta1);
// 			}
// 			if(bFilterSecondQ)
// 			{
// 				VO_S8 delta2 = Clip3(-tc2, tc2, (( ((m6+m4+1)>>1)- m5-delta)>>1));
// 				piSrc[ iOffset] = Clip(m5+delta2);
// 			}
// 		}
// 		if (bNoP)
// 		{
// 			piSrc[-iOffset] = m3;
// 			piSrc[-iOffset*2] = m2;
// 		}
// 		if (bNoQ)
// 		{
// 			piSrc[0] = m4;
// 			piSrc[ iOffset] = m5;
// 		}
// 		piSrc += iStride;
// 	}
// }

/* Ver or Hor Luma filter */
// static void voLumaFilterVer(H265_DEC_SLICE *pSlice, 
// 							VO_U32 *pBs, 
// 							VO_S32 x,
// 							VO_S32 y 
// 							)
// {
// 	//H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)(&p_dec->slice);
// 	VO_S32   iQP;
// 	VO_U8    *piTmpSrc; 
// 	VO_U32   uiBs = 0, uiBs1 = 0;
// 	VO_S32   iSrcStep;
// 	VO_S32   iIndexTC;           
// 	VO_S32   iIndexB; 
// 	VO_U8    iTc[2];
// 	VO_S32   iBeta;
// 	VO_S32   dp0, dq0, dp3, dq3;
// 	VO_S32   d0, d3, dp[2], dq[2];
// 	VO_U8    *pAddr;
// 	VO_BOOL  nFlag = 0, nFlag1 = 0, nSw = 0, nSw1 = 0;
// 	VO_BOOL  bNoP[2] = {VO_FALSE, VO_FALSE}, bNoQ[2] = {VO_FALSE, VO_FALSE};
// 	VO_S32   index;
// 	VO_S32   nDiv2 = pSlice->p_pps->pps_tc_offset_div2 << 1;
// 	VO_S32   nBDiv2 = pSlice->p_pps->pps_beta_offset_div2 << 1;
// 	iSrcStep = pSlice->cur_pic->pic_stride[ 0 ];
// 	piTmpSrc = pSlice->cur_pic->pic_buf[ 0 ]+y*iSrcStep+x;
// 
// 	uiBs = *(pBs++);
// 	uiBs1 = *pBs;
// //	if (uiBs || uiBs1)
// //	{
// //		if (pSlice->p_pps->transquant_bypass_enabled_flag || pSlice->p_pps->cu_qp_delta_enabled_flag)
// //		{
// 			index = (y>>2)*pSlice->PicWidthInMinTbsY+(x>>2);
// 			y += 4;
// //		}
// //	}
// 	if (!uiBs)
// 	{
// 		goto VerSec;
// 	}
// 
// 	//if (pSlice->p_pps->transquant_bypass_enabled_flag)
// 	//{
// 	//	bNoP[0] = pSlice->skip_filter[index];
// 	//	bNoQ[0] = pSlice->skip_filter[index - 1];
// 	//	if (bNoP[0] && bNoQ[0])
// 	//	{
// 	//		goto VerSec;
// 	//	}
// 	//}
// 
// 	if(!pSlice->p_pps->cu_qp_delta_enabled_flag)
// 	{ 
// 		iQP = pSlice->slice_qp;
// 	}
// 	else
// 	{
// 		VO_S32 iQP_Q = (pSlice->deblock_para[index] & 0xff0000) >> 16; //pSlice->qp_buffer[index];
// 		VO_S32 iQP_P = (pSlice->deblock_para[index - 1] & 0xff0000) >> 16; //pSlice->qp_buffer[index-1];
// 		iQP = (iQP_P + iQP_Q + 1) >> 1;
// 	}
// 
// 
// 	iIndexTC = Clip3(0, MAX_QP+voDEFAULT_INTRA_TC_OFFSET, (VO_S32)(iQP + voDEFAULT_INTRA_TC_OFFSET*(uiBs-1) + nDiv2));
// 	iIndexB = Clip3(0, MAX_QP, iQP + nBDiv2);
// 
// 	iTc[0] =  votctable_8x8[iIndexTC];
// 	iBeta = vobetatable_8x8[iIndexB];
// 	pAddr = piTmpSrc;
// 	dp0 = voCalcDP( pAddr, 1);
// 	dq0 = voCalcDQ( pAddr, 1);
// 	dp3 = voCalcDP( pAddr + iSrcStep*3, 1);
// 	dq3 = voCalcDQ( pAddr + iSrcStep*3, 1);
// 	d0 = dp0 + dq0;
// 	d3 = dp3 + dq3;
// 	dp[0] = (dp0 + dp3) < ((iBeta+(iBeta>>1))>>3);
// 	dq[0] = (dq0 + dq3) < ((iBeta+(iBeta>>1))>>3);
// 	if (d0 + d3 < iBeta)
// 	{
// 		nFlag = 1;
// 		nSw = voUseStrongFilteringC( 1, d0<<1, iBeta, iTc[0], pAddr)
// 			&& voUseStrongFilteringC( 1, d3<<1, iBeta, iTc[0], pAddr+iSrcStep*3);
// 	}
// 
// VerSec:
// 	if (!uiBs1)
// 	{
// 		goto VerPer;
// 	}
// 
// //	if (pSlice->p_pps->transquant_bypass_enabled_flag || pSlice->p_pps->cu_qp_delta_enabled_flag)
// //	{
// 		index = (y>>2)*pSlice->PicWidthInMinTbsY + (x>>2);
// //	}
// 
// 	//if (pSlice->p_pps->transquant_bypass_enabled_flag)
// 	//{
// 	//	bNoP[1] = pSlice->skip_filter[index];
// 	//	bNoQ[1] = pSlice->skip_filter[index - 1];
// 	//	if (bNoP[1] && bNoQ[1])
// 	//	{
// 	//		goto VerPer;
// 	//	}
// 	//}
// 	if(!pSlice->p_pps->cu_qp_delta_enabled_flag)
// 	{       
// 		iQP = pSlice->slice_qp;
// 	}
// 	else
// 	{
// 		VO_S32 iQP_Q = (pSlice->deblock_para[index] & 0xff0000) >> 16;//pSlice->qp_buffer[index];
// 		VO_S32 iQP_P = (pSlice->deblock_para[index-1] & 0xff0000) >> 16;//pSlice->qp_buffer[index-1];
// 		iQP = (iQP_P + iQP_Q + 1) >> 1;
// 	}
// 
// 	iIndexTC = Clip3(0, MAX_QP+voDEFAULT_INTRA_TC_OFFSET, (VO_S32)(iQP + voDEFAULT_INTRA_TC_OFFSET*(uiBs1-1) + nDiv2));
// 	iIndexB = Clip3(0, MAX_QP, iQP + nBDiv2);
// 
// 	iTc[1] =  votctable_8x8[iIndexTC];
// 	iBeta = vobetatable_8x8[iIndexB];
// 	pAddr = piTmpSrc+ (iSrcStep<<2);
// 	dp0 = voCalcDP( pAddr, 1);
// 	dq0 = voCalcDQ( pAddr, 1);
// 	dp3 = voCalcDP( pAddr + iSrcStep*3, 1);
// 	dq3 = voCalcDQ( pAddr + iSrcStep*3, 1);
// 	d0 = dp0 + dq0;
// 	d3 = dp3 + dq3;
// 	dp[1] = (dp0 + dp3) < (iBeta+(iBeta>>1))>>3;
// 	dq[1] = (dq0 + dq3) < (iBeta+(iBeta>>1))>>3;
// 	if (d0 + d3 < iBeta)
// 	{
// 		nFlag1 = 1;
// 		nSw1 = voUseStrongFilteringC( 1, d0<<1, iBeta, iTc[1], pAddr)
// 			&& voUseStrongFilteringC( 1, d3<<1, iBeta, iTc[1], pAddr+iSrcStep*3);
// 
// 		if(!nFlag && (!nSw1))
// 		{
// 			pAddr = piTmpSrc + (iSrcStep << 2);
// #if DEBLOCK_ASM_ENABLED
// 			voPelLumaWeakASM_V(pAddr, iSrcStep, iTc[1], dp[1], dq[1], 1);
// #else
// 			voPelAllLumaFilterW(pAddr, iSrcStep, 1, iTc[1], dp[1], dq[1], bNoP[1], bNoQ[1]);
// #endif
// 			return;
// 		}else if (!nFlag && nSw1)
// 		{
// #if DEBLOCK_ASM_ENABLED
// 			voLumaInFilterASM_V(pAddr, iSrcStep, iTc[1], 1);
// #else
// 			voPelAllLumaFilterS(pAddr, iSrcStep, 1, iTc[1], 1, bNoP[1], bNoQ[1]);
// #endif
//             return;
// 		}
// 	}
// 
// VerPer:
// 
// 	if (!nFlag && !nFlag1)
// 	{
// 		return;
// 	}
// 
// 	pAddr = piTmpSrc;
// 	///* According nSw select 4 line or 8 line */
// 	if (nFlag && nFlag1 && !nSw && !nSw1) // 8 lines weak filter 
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		if ((iTc[0] == iTc[1]) && (dp[0] == dp[1]) && (dq[0] == dq[1]))
// 		{
// 			voPelLumaWeakASM_V(pAddr, iSrcStep, iTc[0], dp[0], dq[0], 2);
// 		}else
// 		{
// 			voPelLumaWeakASM_V(pAddr, iSrcStep, iTc[0], dp[0], dq[0], 1);
// 			pAddr = piTmpSrc + (iSrcStep << 2);
// 			voPelLumaWeakASM_V(pAddr, iSrcStep, iTc[1], dp[1], dq[1], 1);
// 		}
// #else
// 		voPelAllLumaFilterW(pAddr, iSrcStep, 1, iTc[0], dp[0], dq[0], bNoP[0], bNoQ[0]);
// 		pAddr = piTmpSrc + (iSrcStep << 2);
// 		voPelAllLumaFilterW(pAddr, iSrcStep, 1, iTc[1], dp[1], dq[1], bNoP[1], bNoQ[1]);
// #endif
// 	}else if (nFlag && nFlag1 && nSw && nSw1)    // 8 line strong filter
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voLumaInFilterASM_V(pAddr, iSrcStep, iTc[0], 2);
// #else
// 		voPelAllLumaFilterS(pAddr, iSrcStep, 1, iTc[0], 2, bNoP[0], bNoQ[0]);
// #endif
// 	}else if (nFlag && nFlag1 && nSw && !nSw1)   // first 4 lines strong, last 4 lines weak
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voLumaInFilterASM_V(pAddr, iSrcStep, iTc[0], 1);
// 		pAddr = piTmpSrc + (iSrcStep << 2);
//         voPelLumaWeakASM_V(pAddr, iSrcStep, iTc[1], dp[1], dq[1], 1);
// #else
// 		voPelAllLumaFilterS(pAddr, iSrcStep, 1, iTc[0], 1, bNoP[0], bNoQ[0]);
// 		pAddr = piTmpSrc + (iSrcStep << 2);
// 		voPelAllLumaFilterW(pAddr, iSrcStep, 1, iTc[1], dp[1], dq[1], bNoP[1], bNoQ[1]);
// #endif
// 	}else if (nFlag && nFlag1 && !nSw && nSw1)   // first 4 lines weak, last 4 lines strong 
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voPelLumaWeakASM_V(pAddr, iSrcStep, iTc[0], dp[0], dq[0], 1);
// 		pAddr = piTmpSrc + (iSrcStep << 2);
// 		voLumaInFilterASM_V(pAddr, iSrcStep, iTc[1], 1);
// #else
// 		voPelAllLumaFilterW(pAddr, iSrcStep, 1, iTc[0], dp[0], dq[0], bNoP[0], bNoQ[0]);
// 		pAddr = piTmpSrc + (iSrcStep << 2);
// 		voPelAllLumaFilterS(pAddr, iSrcStep, 1, iTc[1], 1, bNoP[1], bNoQ[1]);
// #endif
// 	}else if (nFlag && !nSw) // first 4 lines weak, N/A
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voPelLumaWeakASM_V(piTmpSrc, iSrcStep, iTc[0], dp[0], dq[0], 1);
// #else
// 		voPelAllLumaFilterW(piTmpSrc, iSrcStep, 1, iTc[0], dp[0], dq[0], bNoP[0], bNoQ[0]);
// #endif
// 	}else if (nFlag && nSw)  // first 4 lines strong, N/A
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voLumaInFilterASM_V(piTmpSrc, iSrcStep, iTc[0], 1);
// #else
// 		voPelAllLumaFilterS(piTmpSrc, iSrcStep, 1, iTc[0], 1, bNoP[0], bNoQ[0]);
// #endif
// 	}
// }

// static void voLumaFilterHor(H265_DEC_SLICE *pSlice, 
// 							VO_U32 *pBs, 
// 							VO_S32 x,
// 							VO_S32 y
// 							)
// {
// 	//H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)(&p_dec->slice);
// 	VO_S32   iQP = 0;
// 	VO_U8    *piTmpSrc; 
// 	VO_U32   uiBs = 0, uiBs1 = 0;
// 	VO_S32   iOffset;
// 	VO_S32   iIndexTC;           
// 	VO_S32   iIndexB; 
// 	VO_U8    iTc[2];
// 	VO_S32   iBeta;
// 	VO_S32   dp0, dq0, dp3, dq3;
// 	VO_S32   d0, d3, dp[2], dq[2];
// 	VO_U8    *pAddr;
// 	VO_BOOL  nFlag = 0, nFlag1 = 0, nSw = 0, nSw1 = 0;
// 	VO_BOOL  bNoP[2] = {VO_FALSE, VO_FALSE}, bNoQ[2] = {VO_FALSE, VO_FALSE};
// 	VO_S32   index;
// 	VO_S32   nDiv2 = pSlice->p_pps->pps_tc_offset_div2 << 1;
// 	VO_S32   nBDiv2 = pSlice->p_pps->pps_beta_offset_div2 << 1;
// 
// 	iOffset = pSlice->cur_pic->pic_stride[ 0 ];
// 	piTmpSrc = pSlice->cur_pic->pic_buf[ 0 ]+y*iOffset+x;
// 
// 	uiBs = *(pBs++);
// 	uiBs1 = *(pBs);
// //	if (uiBs || uiBs1)
// //	{
// //		if (pSlice->p_pps->transquant_bypass_enabled_flag || pSlice->p_pps->cu_qp_delta_enabled_flag)
// //		{
// 			index = (y>>2)*pSlice->PicWidthInMinTbsY+(x>>2);
// 			x += 4;
// //		}
// //	}
// 	if (!uiBs)
// 	{
// 		goto HorSec;
// 	}
// 
// 	//if (pSlice->p_pps->transquant_bypass_enabled_flag)
// 	//{
// 	//	bNoP[0] = pSlice->skip_filter[index];
// 	//	bNoQ[0] = pSlice->skip_filter[index - p_dec->picWidthInMin4bs];
// 	//	if (bNoP[0] && bNoQ[0])
// 	//	{
// 	//		goto HorSec;
// 	//	}
// 	//}
// 
// 	if(!pSlice->p_pps->cu_qp_delta_enabled_flag)
// 	{   
// 		iQP = pSlice->slice_qp;
// 	}
// 	else
// 	{
// 		VO_S32 iQP_Q = (pSlice->deblock_para[index] & 0xff0000) >> 16;//pSlice->qp_buffer[index];
// 		VO_S32 iQP_P = (pSlice->deblock_para[index - pSlice->PicWidthInMinTbsY] & 0xff0000) >> 16;//pSlice->qp_buffer[index - pSlice->PicWidthInMinTbsY];
// 		iQP = (iQP_P + iQP_Q + 1) >> 1;
// 	}	
// 
// 	iIndexTC = Clip3(0, MAX_QP+voDEFAULT_INTRA_TC_OFFSET, (VO_S32)(iQP + voDEFAULT_INTRA_TC_OFFSET*(uiBs-1) + nDiv2));
// 	iIndexB = Clip3(0, MAX_QP, iQP + nBDiv2);
// 	iTc[0] =  votctable_8x8[iIndexTC];
// 	iBeta = vobetatable_8x8[iIndexB];
// 
// 	pAddr = piTmpSrc;
// 	dp0 = voCalcDP( pAddr, iOffset);
// 	dq0 = voCalcDQ( pAddr, iOffset);
// 	dp3 = voCalcDP( pAddr + 3, iOffset);
// 	dq3 = voCalcDQ( pAddr + 3, iOffset);
// 	d0 = dp0 + dq0;
// 	d3 = dp3 + dq3;
// 	dp[0] = (dp0 + dp3) < (iBeta+(iBeta>>1))>>3;
// 	dq[0] = (dq0 + dq3) < (iBeta+(iBeta>>1))>>3;
// 
// 	if (d0 + d3 < iBeta)
// 	{ 
// 		nFlag = 1;
// 		nSw =  voUseStrongFilteringC( iOffset, d0<<1, iBeta, iTc[0], pAddr)
// 			&& voUseStrongFilteringC( iOffset, d3<<1, iBeta, iTc[0], pAddr + 3);
// 	}
// 
// HorSec:
// 	if (!uiBs1)
// 	{
// 		goto HorPer;
// 	}
// //	if (pSlice->p_pps->transquant_bypass_enabled_flag || pSlice->p_pps->cu_qp_delta_enabled_flag)
// //	{
// 		index = (y>>2) * pSlice->PicWidthInMinTbsY + (x>>2);
// //	}
// 
// 	if(!pSlice->p_pps->cu_qp_delta_enabled_flag)
// 	{  
// 		iQP = pSlice->slice_qp;
// 	}
// 	else
// 	{
// 		VO_S32 iQP_Q = (pSlice->deblock_para[index] & 0xff0000) >> 16; //pSlice->qp_buffer[index];
// 		VO_S32 iQP_P = (pSlice->deblock_para[index - pSlice->PicWidthInMinTbsY] & 0xff0000) >> 16;//pSlice->qp_buffer[index - pSlice->PicWidthInMinTbsY];
// 		iQP = (iQP_P + iQP_Q + 1) >> 1;
// 	}
// 
// 	//if (pSlice->p_pps->transquant_bypass_enabled_flag)
// 	//{
// 	//	bNoP[1] = pSlice->skip_filter[index];
// 	//	bNoQ[1] = pSlice->skip_filter[index - p_dec->picWidthInMin4bs];
// 	//	if (bNoP[1] && bNoQ[1])
// 	//	{
// 	//		goto HorPer;
// 	//	}
// 	//}
// 
// 	iIndexTC = Clip3(0, MAX_QP+voDEFAULT_INTRA_TC_OFFSET, (VO_S32)(iQP + voDEFAULT_INTRA_TC_OFFSET*(uiBs1-1) + nDiv2));
// 	iIndexB = Clip3(0, MAX_QP, iQP + nBDiv2);
// 	iTc[1] =  votctable_8x8[iIndexTC];
// 	iBeta = vobetatable_8x8[iIndexB];
// 
// 	pAddr = piTmpSrc + 4;
// 	dp0 = voCalcDP( pAddr, iOffset);
// 	dq0 = voCalcDQ( pAddr, iOffset);
// 	dp3 = voCalcDP( pAddr + 3, iOffset);
// 	dq3 = voCalcDQ( pAddr + 3, iOffset);
// 	d0 = dp0 + dq0;
// 	d3 = dp3 + dq3;
// 	dp[1] = (dp0 + dp3) < (iBeta+(iBeta>>1))>>3;
// 	dq[1] = (dq0 + dq3) < (iBeta+(iBeta>>1))>>3;
// 	if (d0 + d3 < iBeta)
// 	{ 
// 		nFlag1 = 1;
// 		nSw1 =  voUseStrongFilteringC( iOffset, d0<<1, iBeta, iTc[1], pAddr)
// 			&& voUseStrongFilteringC( iOffset, d3<<1, iBeta, iTc[1], pAddr + 3);
// 
// 		if (!nFlag && !nSw1)
// 		{
// #if DEBLOCK_ASM_ENABLED
// 			voPelLumaWeakASM_H(pAddr, iOffset, iTc[1], dp[1], dq[1], 1);
// #else
// 			voPelAllLumaFilterW(pAddr, 1, iOffset, iTc[1], dp[1], dq[1], bNoP[1], bNoQ[1]);
// #endif
// 			return;
// 		}else if (!nFlag && nSw1)
// 		{
// #if DEBLOCK_ASM_ENABLED
// 			voLumaInFilterASM_H(pAddr, iOffset, iTc[1], 1);
// #else
// 			voPelAllLumaFilterS(pAddr, 1, iOffset, iTc[1], 1, bNoP[1], bNoQ[1]);
// #endif
// 			return;
// 		} 
// 	}
// 
// HorPer:
// 	if (!nFlag && !nFlag1)
// 	{
// 		return;
// 	}
// 
// 	pAddr = piTmpSrc;
// 	///* According nSw select 4 line or 8 line */
// 	if (nFlag && nFlag1 && !nSw && !nSw1) // 8 lines weak filter 
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		if ((iTc[0] == iTc[1]) && (dp[0] == dp[1]) &&(dq[0] == dq[1]))
// 		{
// 			voPelLumaWeakASM_H(pAddr, iOffset, iTc[0], dp[0], dq[0], 2);
// 		}
// 		else
// 		{
// 			voPelLumaWeakASM_H(pAddr, iOffset, iTc[0], dp[0], dq[0], 1);
// 			pAddr = piTmpSrc + 4;
// 			voPelLumaWeakASM_H(pAddr, iOffset, iTc[1], dp[1], dq[1], 1);
// 		}
// #else
// 		voPelAllLumaFilterW(pAddr, 1, iOffset, iTc[0], dp[0], dq[0], bNoP[0], bNoQ[0]);
// 		pAddr = piTmpSrc + 4;
// 		voPelAllLumaFilterW(pAddr, 1, iOffset, iTc[1], dp[1], dq[1], bNoP[1], bNoQ[1]);
// #endif
// 	}else if (nFlag && nFlag1 && nSw && nSw1)    // 8 line strong filter
// 	{
// #if DEBLOCK_ASM_ENABLED   
// 		voLumaInFilterASM_H(pAddr, iOffset, iTc[0], 2);
// #else
// 		voPelAllLumaFilterS(pAddr, 1, iOffset, iTc[0], 2, bNoP[0], bNoQ[0]); 
// #endif
// 	}else if (nFlag && nFlag1 && nSw && !nSw1)   // first 4 lines strong, last 4 lines weak
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voLumaInFilterASM_H(pAddr, iOffset, iTc[0], 1);
// 		pAddr = piTmpSrc + 4;
// 		voPelLumaWeakASM_H(pAddr, iOffset, iTc[1], dp[1], dq[1], 1);
// #else
// 		voPelAllLumaFilterS(pAddr, 1, iOffset, iTc[0], 1, bNoP[0], bNoQ[0]);
// 		pAddr = piTmpSrc + 4;
// 		voPelAllLumaFilterW(pAddr, 1, iOffset, iTc[1], dp[1], dq[1], bNoP[1], bNoQ[1]);
// #endif
// 	}else if (nFlag && nFlag1 && !nSw && nSw1)   // first 4 lines weak, last 4 lines strong 
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voPelLumaWeakASM_H(pAddr, iOffset, iTc[0], dp[0], dq[0], 1);
// 		pAddr = piTmpSrc + 4;
// 		voLumaInFilterASM_H(pAddr, iOffset, iTc[1], 1);
// #else
// 		voPelAllLumaFilterW(pAddr, 1, iOffset, iTc[0], dp[0], dq[0], bNoP[0], bNoQ[0]);
// 		pAddr = piTmpSrc + 4;
// 		voPelAllLumaFilterS(pAddr, 1, iOffset, iTc[1], 1, bNoP[1], bNoQ[1]);
// #endif
// 	}else if (nFlag && !nSw)
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voPelLumaWeakASM_H(piTmpSrc, iOffset, iTc[0], dp[0], dq[0], 1);
// #else
// 		voPelAllLumaFilterW(piTmpSrc, 1, iOffset, iTc[0], dp[0], dq[0], bNoP[0], bNoQ[0]);
// #endif
// 	}else if (nFlag && nSw)
// 	{
// #if DEBLOCK_ASM_ENABLED
// 		voLumaInFilterASM_H(piTmpSrc, iOffset, iTc[0], 1);
// #else
// 		voPelAllLumaFilterS(piTmpSrc, 1, iOffset, iTc[0], 1, bNoP[0], bNoQ[0]);
// #endif
// 	}
// }

/* Ver or Hor Chroma filter */
// static void voChromaFilterVer(H265_DEC_SLICE *pSlice, VO_S32 x, VO_S32 y)
// {
// 	//H265_DEC_SLICE *pSlice = (H265_DEC_SLICE*)(&p_dec->slice);
// 	VO_U8    *piSrcCb;
// 	VO_U8    *piSrcCr;
// 	VO_S32   iQP;
// 	VO_S32   iSrcStep;
// 	VO_S32   iIndexTC;
// 	VO_U8    iTc;
// 	VO_S32   chromaQPOffset; 
// 	VO_BOOL  bNoP = VO_FALSE, bNoQ = VO_FALSE;
// 	VO_S32   index = (y>>2)*pSlice->PicWidthInMinTbsY+(x>>2);
// 	VO_S32   nDiv2 = pSlice->p_pps->pps_tc_offset_div2 << 1;
// 
// 	iSrcStep  = pSlice->cur_pic->pic_stride[ 1 ];
// 	piSrcCb = pSlice->cur_pic->pic_buf[ 1 ]+(y>>1)*iSrcStep+(x>>1);
// 	piSrcCr = pSlice->cur_pic->pic_buf[ 2 ]+(y>>1)*iSrcStep+(x>>1);
// 
// 
// 	if(!pSlice->p_pps->cu_qp_delta_enabled_flag)
// 	{
// 		iQP = pSlice->slice_qp;
// 	}
// 	else
// 	{
// 		VO_S32 iQP_Q = (pSlice->deblock_para[index] & 0xff0000) >> 16;//pSlice->qp_buffer[index];
// 		VO_S32 iQP_P = (pSlice->deblock_para[index-1] & 0xff0000) >> 16;//pSlice->qp_buffer[index-1];
// 		iQP = (iQP_P + iQP_Q + 1) >> 1;
// 	}
// 
// 	//if (pSlice->p_pps->transquant_bypass_enabled_flag)
// 	//{
// 	//	bNoP = pSlice->skip_filter[index];
// 	//	bNoQ = pSlice->skip_filter[index - 1];
// 	//}
// 	//if(bNoP && bNoQ)
// 	//{
// 	//	return;
// 	//}
// 
// 	chromaQPOffset = pSlice->p_pps->pps_chroma_qp_offset[0]; //pSlice->p_pps->cb_qp_offset;
// 	iQP = voQpUV( (iQP) + chromaQPOffset );
// 	iIndexTC = Clip3(0, MAX_QP+voDEFAULT_INTRA_TC_OFFSET, iQP + voDEFAULT_INTRA_TC_OFFSET + nDiv2);
// 
// 	iTc =  votctable_8x8[iIndexTC];
// 	/* 2 line Pels Chroma filter Cb and Cr*/
// #if DEBLOCK_ASM_ENABLED 
// 	voChromaInFilterASM_V(piSrcCb, piSrcCr, iSrcStep, iTc);
// #else
// 	voPelAllChromaFilter(piSrcCb, piSrcCr, iSrcStep, 1, iTc, bNoP, bNoQ);
// #endif
// }

// static void voChromaFilterHor(H265_DEC_SLICE *pSlice, VO_S32 x, VO_S32 y)
// {
// 	//H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)(&p_dec->slice);
// 	VO_U8    *piSrcCb;
// 	VO_U8    *piSrcCr;
// 	VO_S32   iQP;
// 	VO_S32   iOffset;
// 	VO_S32   iIndexTC;
// 	VO_U8    iTc;
// 	VO_S32   chromaQPOffset; 
// 	VO_BOOL  bNoP = VO_FALSE, bNoQ = VO_FALSE;
// 	VO_S32   index = (y>>2)*pSlice->PicWidthInMinTbsY+(x>>2);
// 	VO_S32   nDiv2 = pSlice->p_pps->pps_tc_offset_div2 << 1;
// 
// 	iOffset   = pSlice->cur_pic->pic_stride[ 1 ];
// 	piSrcCb = pSlice->cur_pic->pic_buf[ 1 ]+(y>>1)*iOffset+(x>>1);
// 	piSrcCr = pSlice->cur_pic->pic_buf[ 2 ]+(y>>1)*iOffset+(x>>1);
// 
// 	if(!pSlice->p_pps->cu_qp_delta_enabled_flag)
// 	{
// 		iQP = pSlice->slice_qp;
// 	}
// 	else
// 	{
// 		VO_S32 iQP_Q = (pSlice->deblock_para[index] & 0xff0000) >> 16;//pSlice->qp_buffer[index];
// 		VO_S32 iQP_P = (pSlice->deblock_para[index-pSlice->PicWidthInMinTbsY] & 0xff0000) >> 16;//(pSlice->qp_buffer[index-pSlice->PicWidthInMinTbsY];
// 		iQP = (iQP_P + iQP_Q + 1) >> 1;
// 	}
// 
// 	//if (pSlice->p_pps->transquant_bypass_enabled_flag)
// 	//{
// 	//	bNoP = pSlice->skip_filter[index];
// 	//	bNoQ = pSlice->skip_filter[index - p_dec->picWidthInMin4bs];
// 	//}
// 	//if(bNoP && bNoQ)
// 	//{
// 	//	return;
// 	//}
// 	chromaQPOffset = pSlice->p_pps->pps_chroma_qp_offset[0];;
// 	iQP = voQpUV((iQP) + chromaQPOffset );
// 	iIndexTC = Clip3(0, MAX_QP+voDEFAULT_INTRA_TC_OFFSET, iQP + voDEFAULT_INTRA_TC_OFFSET + nDiv2);
// 	iTc =  votctable_8x8[iIndexTC];
// 	/* 2 line Pels Chroma filter Cb and Cr */
// #if DEBLOCK_ASM_ENABLED
// 	voChromaInFilterASM_H(piSrcCb, piSrcCr, iOffset, iTc);
// #else
// 	voPelAllChromaFilter(piSrcCb, piSrcCr, 1, iOffset, iTc, bNoP, bNoQ);
// #endif 
// }
/***************************** End C optimization ***********************************************/

//VO_VOID voParpareParm(H265_DEC *p_dec, void *pGst, VO_U32 nRow, VO_U32 nCol, VO_S32 entry)
//{
//	if(nRow == 0 && nCol == 0)
//	{
//		H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)pGst;
//		VO_U32 nNumPart = pSlice->m_uiNumPartitions;	
//		VO_U32 nNumCUCol = pSlice->m_uiWidthInCU;
//		pSlice->p_pBsVer = pSlice->m_puBsBuffer[voEDGE_VER];
//		pSlice->p_pBsHor = pSlice->m_puBsBuffer[voEDGE_HOR];
//		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id,  pSlice->p_pBsVer, 0, sizeof( VO_U8 ) * nNumPart * nNumCUCol * pSlice->m_uiHeightInCU );
//		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id,  pSlice->p_pBsHor, 0, sizeof( VO_U8 ) * nNumPart * nNumCUCol * pSlice->m_uiHeightInCU);
//	}
//	return;
//}

#if !DEBLOCK_ASM_ENABLED
static void voLumaFilterVer_C(VO_U8* src, VO_U32 stride, VO_S32 iBeta, VO_S32 iTc)
{
    VO_S32 dp0 = ABS( src[-3] - 2*src[-2] + src[-1] );
	VO_S32 dq0 = ABS( src[0] - 2*src[1] + src[2] );
	VO_S32 dp3 = ABS( src[stride*3-3] - 2*src[stride*3-2] + src[stride*3-1] );
	VO_S32 dq3 = ABS( src[stride*3] - 2*src[stride*3+1] + src[stride*3+2] );
	VO_S32 d0 = dp0 + dq0;
	VO_S32 d3 = dp3 + dq3;
	if (d0 + d3 < iBeta)  
	{
	    VO_S32 tc_s = (iTc*5+1)>>1;
	    if(ABS(src[-4] -  src[-1]) + ABS( src[3] -  src[0]) < (iBeta>>3) && 
		   ABS(src[-1] -  src[0]) < tc_s &&
           ABS(src[stride*3-4] - src[stride*3-1]) + ABS(src[stride*3+3] - src[stride*3]) < (iBeta>>3) && 
           ABS(src[stride*3-1] - src[stride*3]) < tc_s &&
           (d0 << 1) < (iBeta>>2) && (d3 << 1) < (iBeta>>2)) //strong
        {
            VO_S32 i;
            VO_S32 tc2 = iTc << 1;
            for(i = 0; i < 4; i++) 
			{
                VO_S32 p3 = src[-4];VO_S32 p2 = src[-3];VO_S32 p1 = src[-2];VO_S32 p0 = src[-1];
                VO_S32 q0 = src[0]; VO_S32 q1 = src[1]; VO_S32 q2 = src[2]; VO_S32 q3 = src[3];
				
                src[-1] = Clip3( p0-tc2, p0+tc2,( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3);
                src[-2] = Clip3( p1-tc2, p1+tc2,( p2 + p1 + p0 + q0 + 2 ) >> 2);
                src[-3] = Clip3( p2-tc2, p2+tc2,( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3);
                src[0] = Clip3( q0-tc2, q0+tc2,( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3);
                src[1] = Clip3( q1-tc2, q1+tc2,( p0 + q0 + q1 + q2 + 2 ) >> 2);
                src[2] = Clip3( q2-tc2, q2+tc2,( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3);
                src += stride;
            }
        }
		else //weak
    	{
    	    VO_S32 i;
    	    VO_S32 nd_p = 1;
            VO_S32 nd_q = 1;
            VO_S32 tc_2 = iTc >> 1;
            if (dp0 + dp3 < ((iBeta+(iBeta>>1))>>3))
                nd_p = 2;
            if (dq0 + dq3 < ((iBeta+(iBeta>>1))>>3))
                nd_q = 2;
    
            for(i = 0; i < 4; i++) 
    		{
                VO_S32 p2 = src[-3];VO_S32 p1 = src[-2];VO_S32 p0 = src[-1];
                VO_S32 q0 = src[0]; VO_S32 q1 = src[1]; VO_S32 q2 = src[2];
                VO_S32 delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4;
                if (ABS(delta) < 10 * iTc) 
    			{
                    delta = Clip3( -iTc, iTc,delta);
                    src[-1] = Clip(p0 + delta);
                    src[0] = Clip(q0 - delta);
                    if(nd_p > 1) 
    				{
                        VO_S32 delta_p = Clip3( -tc_2, tc_2,(((p2 + p0 + 1) >> 1) - p1 + delta) >> 1);
                        src[-2] = Clip(p1 + delta_p);
                    }
                    if(nd_q > 1) 
    				{
                        VO_S32 delta_q = Clip3( -tc_2, tc_2,(((q2 + q0 + 1) >> 1) - q1 - delta) >> 1);
                        src[1] = Clip(q1 + delta_q);
                    }
                }
                src += stride;
            }
    	}
	}
	
    return;
}

static void voLumaFilterHor_C(VO_U8* src, VO_U32 stride, VO_S32 iBeta, VO_S32 iTc)
{
    VO_S32 dp0 = ABS( src[-3*stride] - 2*src[-2*stride] + src[-stride] );
	VO_S32 dq0 = ABS( src[0] - 2*src[stride] + src[2*stride] );
	VO_S32 dp3 = ABS( src[3-3*stride] - 2*src[3-2*stride] + src[3-stride] );
	VO_S32 dq3 = ABS( src[3] - 2*src[3+stride] + src[3+2*stride] );
	VO_S32 d0 = dp0 + dq0;
	VO_S32 d3 = dp3 + dq3;
	if (d0 + d3 < iBeta)  
	{
	    VO_S32 tc_s = (iTc*5+1)>>1;
	    if(ABS(src[-4*stride] -  src[-stride]) + ABS( src[3*stride] -  src[0]) < (iBeta>>3) && 
		   ABS(src[-stride] -  src[0]) < tc_s &&
           ABS(src[3-4*stride] - src[3-stride]) + ABS(src[3+3*stride] - src[3]) < (iBeta>>3) && 
           ABS(src[3-stride] - src[3]) < tc_s &&
           (d0 << 1) < (iBeta>>2) && (d3 << 1) < (iBeta>>2)) //strong
        {
            VO_S32 i;
            VO_S32 tc2 = iTc << 1;
            for(i = 0; i < 4; i++) 
			{
                VO_S32 p3 = src[-4*stride];VO_S32 p2 = src[-3*stride];VO_S32 p1 = src[-2*stride];VO_S32 p0 = src[-stride];
                VO_S32 q0 = src[0]; VO_S32 q1 = src[stride]; VO_S32 q2 = src[2*stride]; VO_S32 q3 = src[3*stride];
				src[-3*stride] = Clip3( p2-tc2, p2+tc2,( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3);
				src[-2*stride] = Clip3( p1-tc2, p1+tc2,( p2 + p1 + p0 + q0 + 2 ) >> 2);
                src[-stride] = Clip3( p0-tc2, p0+tc2,( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3); 
                src[0] = Clip3( q0-tc2, q0+tc2,( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3);
                src[stride] = Clip3( q1-tc2, q1+tc2,( p0 + q0 + q1 + q2 + 2 ) >> 2);
                src[2*stride] = Clip3( q2-tc2, q2+tc2,( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3);
                src += 1;
            }
        }
		else //weak
    	{
    	    VO_S32 i;
    	    VO_S32 nd_p = 1;
            VO_S32 nd_q = 1;
            VO_S32 tc_2 = iTc >> 1;
            if (dp0 + dp3 < ((iBeta+(iBeta>>1))>>3))
                nd_p = 2;
            if (dq0 + dq3 < ((iBeta+(iBeta>>1))>>3))
                nd_q = 2;
    
            for(i = 0; i < 4; i++) 
    		{
                VO_S32 p2 = src[-3*stride];VO_S32 p1 = src[-2*stride];VO_S32 p0 = src[-stride];
                VO_S32 q0 = src[0]; VO_S32 q1 = src[stride]; VO_S32 q2 = src[2*stride];
                VO_S32 delta = (9*(q0 - p0) - 3*(q1 - p1) + 8) >> 4;
                if (ABS(delta) < 10 * iTc) 
    			{
                    delta = Clip3( -iTc, iTc,delta);
                    src[-stride] = Clip(p0 + delta);
                    src[0] = Clip(q0 - delta);
                    if(nd_p > 1) 
    				{
                        VO_S32 delta_p = Clip3( -tc_2, tc_2,(((p2 + p0 + 1) >> 1) - p1 + delta) >> 1);
                        src[-2*stride] = Clip(p1 + delta_p);
                    }
                    if(nd_q > 1) 
    				{
                        VO_S32 delta_q = Clip3( -tc_2, tc_2,(((q2 + q0 + 1) >> 1) - q1 - delta) >> 1);
                        src[stride] = Clip(q1 + delta_q);
                    }
                }
                src += 1;
            }
    	}
	}
	
    return;
}


static void voLumaFilterCross_C(VO_U8* src, VO_U32 stride, VO_U32 pbeta, VO_U32 ptc)
{
    VO_U8* tc = (VO_U8*)&ptc;
	VO_U8* beta = (VO_U8*)&pbeta;
    src-=4*stride;
    if (tc[0])
		voLumaFilterVer_C(src,stride,beta[0],tc[0]);	
	src+=4*stride;
	if (tc[1])
		voLumaFilterVer_C(src,stride,beta[1],tc[1]);
	src-=4;
	if (tc[2])	
		voLumaFilterHor_C(src,stride,beta[2],tc[2]);	
	src+=4;
	if (tc[3])
		voLumaFilterHor_C(src,stride,beta[3],tc[3]);	
	return;
}

static void voChromaFilterVer_C(VO_U8* src,VO_U32 stride, VO_S32 iTc)
{
    VO_S32 i;
	VO_U8  p1, p0, q0, q1;
	VO_S8  delta;
	for (i = 0; i < 4; i++)
	{
		p1  = src[-2];
		p0  = src[-1];
		q0  = src[0];
		q1  = src[1];
		delta = Clip3(-iTc,iTc, (((( q0 - p0 ) << 2 ) + p1 - q1 + 4 ) >> 3) );
		src[-1] = Clip(p0+delta);
		src[0] = Clip(q0-delta);

		src += stride;
	}
    return;
}

static void voChromaFilterHor_C(VO_U8* src,VO_U32 stride, VO_S32 iTc)
{
    VO_S32 i;
	VO_U8  p1, p0, q0, q1;
	VO_S8  delta;
	for (i = 0; i < 4; i++)
	{
		p1  = src[-2*stride];
		p0  = src[-stride];
		q0  = src[0];
		q1  = src[stride];
		delta = Clip3(-iTc,iTc, (((( q0 - p0 ) << 2 ) + p1 - q1 + 4 ) >> 3) );
		src[-stride] = Clip(p0+delta);
		src[0] = Clip(q0-delta);

		src += 1;
	}
    return;
}


static void voChromaFilterCross_C(VO_U8* src, VO_U32 stride, VO_U32 ptc)
{
    VO_U8* tc = (VO_U8*)&ptc;
    src-=4*stride;
    if (tc[0])
	    voChromaFilterVer_C(src,stride,tc[0]);
	src+=4*stride;
	if (tc[1])
	    voChromaFilterVer_C(src,stride,tc[1]);
	src-=4;
	if (tc[2])
		voChromaFilterHor_C(src,stride,tc[2]);	
	src+=4;
	if (tc[3])
		voChromaFilterHor_C(src,stride,tc[3]);		
	return;
}
#endif


#if USE_LCU_SYNC
// VO_S32 voDeblockingFilterLCUStep(struct H265_DEC_SLICE * const pSlice, const VO_U32 nRow)
// {
// 	VO_U32 mCUSize = (1 << pSlice->CtbLog2SizeY); //pSlice->p_sps->max_cu_width;
// 	VO_U32 nX = 0,nY;
// 	VO_U32 yEnd, xStart, xEnd;
// 	VO_U32 uiBs[2] = {0};
// 	VO_U8  *pBs_V, *pBs_H;
// 	VO_U32 nYLev; 
// 	VO_U32 nCol = pSlice->PicWidthInCtbsY; //pSlice->m_uiWidthInCU; 
// 	VO_U32 poffset = nRow*mCUSize;
// 	VO_U32 nTemp = pSlice->PicWidthInMinTbsY;
// 	VO_U32 nOffset = nRow * mCUSize * nTemp >> 2;  // >> 2, because m_puBsBuffer is mapped by 4*4  
// 
// // 	xEnd = pSlice->p_sps->pic_width_in_luma_samples;
// 	xStart = pSlice->filterX_Pos;
// 	xEnd = xStart + mCUSize;
// 	pSlice->filterX_Pos = xEnd; //update for next LCU
// 
// 	pBs_V = pSlice->p_pBsVer + sizeof( VO_U8 ) * nOffset;
// 	pBs_H = pSlice->p_pBsHor + sizeof( VO_U8 ) * nOffset;
// 
// 	yEnd = (nRow == (pSlice->PicHeightInCtbsY - 1)) ? (pSlice->p_sps->pic_height_in_luma_samples - (poffset)) : mCUSize;
// 	//Vertical Filter
// 	for (nY = 0; nY < yEnd; nY += 8)
// 	{
// 		nYLev = (nY>>2) * nTemp;
// 		for (nX = xStart; nX < xEnd; nX += 16)
// 		{
// 			uiBs[0] = *(pBs_V + nYLev + (nX >> 2));
// 			uiBs[1] = *(pBs_V + nYLev + nTemp + (nX >> 2));
// 
// 			//fprintf(dump, "Ver: paras1 = %d, paras2 = %d \n", uiBs[0], uiBs[1]);
// 
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterVer(pSlice, uiBs, nX, nY+poffset);
// 				if ( (uiBs[0] == 2) && (nX & 15) == 0)
// 				{
// 					voChromaFilterVer(pSlice, nX, nY+poffset);
// 				}
// 			}
// 			uiBs[0] = *(pBs_V + nYLev + ((nX+8) >> 2));
// 			uiBs[1] = *(pBs_V + nYLev + nTemp + ((nX+8) >> 2));
// 
// 			//fprintf(dump, "Ver: paras3 = %d, paras4 = %d \n", uiBs[0], uiBs[1]);
// 
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterVer(pSlice, uiBs, nX+8, nY+poffset);
// 				if ( (uiBs[0] == 2) && ((nX+8) & 15) == 0)
// 				{
// 					voChromaFilterVer(pSlice, nX+8, nY+poffset);
// 				}
// 			}
// 		}
// 	}
// 	//Hor Filter
// 	if (xStart > 0)
// 	{
// 		assert(xStart >= 8);
// 		xStart -= 8;
// 	}
// 	if (xEnd < pSlice->p_sps->pic_width_in_luma_samples)
// 	{
// 		xEnd -= 8;
// 	}
// 	else
// 	{
// 		xEnd = pSlice->p_sps->pic_width_in_luma_samples;
// 
// 		//init X pos for next row start
// 		pSlice->filterX_Pos = 0;
// 	}
// 	nY = (nRow == 0) ? 8 : 0;
// 	for (; nY < yEnd; nY += 8)
// 	{
// 		nYLev = (nY>>2) * nTemp;
// 		for (nX = xStart; nX < xEnd; nX += 8)
// 		{
// 			uiBs[0] = *(pBs_H + nYLev + (nX>>2));
// 			uiBs[1] = *(pBs_H + nYLev + (nX>>2)+1);
// 
// 			//fprintf(dump, "Hor: paras1 = %d, paras2 = %d \n", uiBs[0], uiBs[1]);
// 
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterHor(pSlice,uiBs, nX, nY+poffset);
// 				if ((uiBs[0] == 2) && ((nY) & 15) == 0)
// 				{
// 					voChromaFilterHor(pSlice, nX, nY+poffset);
// 				}
// 			}
// 		}
// 	}
// 	return 0;
// }
//VO_S32 count = 0;
VO_S32 voDeblockingFilterLCUStepNew(struct H265_DEC_SLICE * const pSlice, const VO_U32 nRow)
{
	VO_U32 mCUSize = (1 << pSlice->CtbLog2SizeY);
	VO_U32 nX,nY;
	VO_U32 yEnd, xStart, xEnd;
	VO_U8  *pBs_V, *pBs_H;
	VO_S32  *pQP;
	VO_U32 nYLev; 
	//VO_U32 nOffset = nRow * pSlice->m_uiWidthInCU * pSlice->m_uiNumPartitions;
	VO_U32 poffset = nRow*mCUSize;
	VO_U32 nTemp = pSlice->PicWidthInCtbsY<<(pSlice->CtbLog2SizeY-2);
	VO_U32 nOffset = nRow * mCUSize * nTemp>>2 ;  // >> 2, because m_puBsBuffer is mapped by 4*4  

	//new params
	VO_S32   nDiv2 = pSlice->p_pps->pps_tc_offset_div2 << 1;
	VO_S32   nBDiv2 = pSlice->p_pps->pps_beta_offset_div2 << 1;
	VO_S32 stride = pSlice->cur_pic->pic_stride[ 0 ];
	VO_U8 bs/*,*tc,*beta*/;
	VO_U32 ptc,pbeta;
    VO_S32 pps_chroma_qp_offset_U,pps_chroma_qp_offset_V;

// 	xEnd = pSlice->p_sps->pic_width_in_luma_samples;
	xStart = pSlice->filterX_Pos;
	xEnd = xStart + mCUSize;
	if (xEnd < pSlice->p_sps->pic_width_in_luma_samples)
	{
		pSlice->filterX_Pos = xEnd; //update for next LCU
	}
	else
	{
		xEnd = pSlice->p_sps->pic_width_in_luma_samples;
		//init X pos for next row start
		pSlice->filterX_Pos = 0;
	}

	pBs_V = pSlice->p_pBsVer + sizeof( VO_U8 ) * nOffset;
	pBs_H = pSlice->p_pBsHor + sizeof( VO_U8 ) * nOffset;
	pQP = (VO_S32 *)(pSlice->deblock_para +  nOffset);


	yEnd = (nRow == (pSlice->PicHeightInCtbsY - 1)) ? (pSlice->p_sps->pic_height_in_luma_samples - (poffset)) : mCUSize;
// 	tc = (VO_U8*)&ptc;
// 	beta = (VO_U8*)&pbeta;
	nY = 0;
	if(nRow==0)
	{
		nY=8;
		for (nX = xStart; nX < xEnd; nX += 8)
		{
			VO_S32 index = (nX>>2);
			bs = *(pBs_V + index);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					pSlice->slice_qp;
				VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];				
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+nX;

#if DEBLOCK_ASM_ENABLED
				voLumaFilterVer_ASM(src,stride,iBeta,iTc);
#else
				voLumaFilterVer_C(src,stride,iBeta,iTc);
#endif
			}
		}
	}
	for (nY=nY; nY < yEnd; nY += 8)
	{
		nYLev = (nY>>2) * nTemp;

		nX = xStart;
		if (nX == 0)
		{
			nX = 8;
			//first hor edge
			bs = *(pBs_H + nYLev);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[nYLev]& 0xff0000)>>16)+((pQP[nYLev-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride;
#if DEBLOCK_ASM_ENABLED
				voLumaFilterHor_ASM(src,stride,iBeta,iTc);
#else
				voLumaFilterHor_C(src,stride,iBeta,iTc);
#endif


			}
		}
		for (; nX < xEnd; nX += 8)
		{
			VO_S32 index = nYLev+(nX>>2)-nTemp;
			ptc = pbeta = 0;
			bs = *(pBs_V + index);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				ptc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				pbeta = vobetatable_8x8[qp + nBDiv2];
			}	
			index+=nTemp;
			bs = *(pBs_V + index);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_U32 tmpTC =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];                
				VO_U32 tmpBeta = vobetatable_8x8[qp + nBDiv2];
                ptc += (tmpTC << 8);//ptc |= tmpTC << 8;
                pbeta +=(tmpBeta << 8);
			}
			index-=1;
			bs = *(pBs_H + index);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_U32 tmpTC =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				VO_U32 tmpBeta = vobetatable_8x8[qp + nBDiv2];
                ptc += (tmpTC << 16);//ptc |= tmpTC << 16;
                pbeta +=(tmpBeta << 16);
			}
			index+=1;
			bs = *(pBs_H + index);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_U32 tmpTC =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				VO_U32 tmpBeta = vobetatable_8x8[qp + nBDiv2];
                ptc += (tmpTC << 24);//ptc |= tmpTC << 24;
                pbeta +=(tmpBeta << 24);
			}
			if(ptc)
			{
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride+nX;
#if DEBLOCK_ASM_ENABLED
                voLumaFilterCross_ASM(src,stride,pbeta,ptc);
#else
				voLumaFilterCross_C(src,stride,pbeta,ptc);
#endif
			}
		}
		if (xEnd == pSlice->p_sps->pic_width_in_luma_samples)
		{
			//last hor edge
			nX = xEnd-4;
			bs = *(pBs_H + nYLev + (nX>>2));
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[nYLev+(nX>>2)]& 0xff0000)>>16)+((pQP[nYLev+(nX>>2)-nTemp]& 0xff0000)>>16)+1)>>1:
					pSlice->slice_qp;
				VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride+nX;
#if DEBLOCK_ASM_ENABLED
				voLumaFilterHor_ASM(src,stride,iBeta,iTc);
#else
				voLumaFilterHor_C(src,stride,iBeta,iTc);
#endif
			}
		}
	}
	if(nRow == (pSlice->PicHeightInCtbsY - 1))
	{
		nY = pSlice->p_sps->pic_height_in_luma_samples - 4-poffset;
		nYLev = (nY>>2) * nTemp;
		for (nX = xStart; nX < xEnd; nX += 8)
		{
			VO_S32 index = nYLev+(nX>>2);
			bs = *(pBs_V + index);
			if (bs)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
				VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride+nX;
#if DEBLOCK_ASM_ENABLED
				voLumaFilterVer_ASM(src,stride,iBeta,iTc);
#else
				voLumaFilterVer_C(src,stride,iBeta,iTc);
#endif

			}
		}
	}
	//chroma
	stride = pSlice->cur_pic->pic_stride[ 1 ];
    pps_chroma_qp_offset_U = pSlice->p_pps->pps_chroma_qp_offset[0];
    pps_chroma_qp_offset_V = pSlice->p_pps->pps_chroma_qp_offset[1];
	nY = 0;
	if(nRow==0)
	{
		nY=16;
		for (nX = xStart; nX < xEnd; nX += 16)
		{
			VO_S32 index = (nX>>2);
			bs = *(pBs_V + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+(nX>>1);
#if DEBLOCK_ASM_ENABLED
				voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
				src = pSlice->cur_pic->pic_buf[ 2 ]+(nX>>1);
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
				iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
				voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
			}
		}
	}
	for (nY = nY; nY < yEnd; nY += 16)
	{
		nYLev = (nY>>2) * nTemp;

		nX = xStart;
		if (nX == 0)
		{
			nX = 16;
			//first hor edge
			bs = *(pBs_H + nYLev);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[nYLev]& 0xff0000)>>16)+((pQP[nYLev-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride;
#if DEBLOCK_ASM_ENABLED
				voChromaFilterHor_ASM(src,stride,iTc);
#else
				voChromaFilterHor_C(src,stride,iTc);
#endif
				src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride;
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
				iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
				voChromaFilterHor_ASM(src,stride,iTc);
#else
				voChromaFilterHor_C(src,stride,iTc);
#endif

			}
		}
		for (; nX < xEnd; nX += 16)
		{
			VO_S32 index = nYLev+(nX>>2)-2*nTemp;
			ptc = pbeta = 0;
			bs = *(pBs_V + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				ptc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
				pbeta = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];	
			}
			index+=2*nTemp;
			bs = *(pBs_V + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_U32 tmp =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
                ptc += (tmp << 8);//ptc |= tmpTC << 8;
				tmp = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];	                
                pbeta +=(tmp << 8);
			}
			index-=2;
			bs = *(pBs_H + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_U32 tmp =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
                ptc += (tmp << 16);//ptc |= tmpTC << 16;
				tmp = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];	
                pbeta +=(tmp << 16);
			}
			index+=2;
			bs = *(pBs_H + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_U32 tmp =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
                ptc += (tmp << 24);
				tmp = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];		
                pbeta += (tmp << 24);
			}
			if(ptc)
			{
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride+((nX)>>1);
#if DEBLOCK_ASM_ENABLED
				voChromaFilterCross_ASM(src,stride,ptc);
#else
				voChromaFilterCross_C(src,stride,ptc);
#endif
			}
			if(pbeta)
			{
				VO_U8* src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride+((nX)>>1);
#if DEBLOCK_ASM_ENABLED
				voChromaFilterCross_ASM(src,stride,pbeta);
#else
				voChromaFilterCross_C(src,stride,pbeta);
#endif

			}
		}
		if (xEnd == pSlice->p_sps->pic_width_in_luma_samples)
		{
			//last hor edge
			nX = xEnd-8;
			bs = *(pBs_H + nYLev + (nX>>2));
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[nYLev+(nX>>2)]& 0xff0000)>>16)+((pQP[nYLev+(nX>>2)-nTemp]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride+(nX>>1);
#if DEBLOCK_ASM_ENABLED
				voChromaFilterHor_ASM(src,stride,iTc);
#else
				voChromaFilterHor_C(src,stride,iTc);
#endif

				src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride+(nX>>1);
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
				iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
				voChromaFilterHor_ASM(src,stride,iTc);
#else
				voChromaFilterHor_C(src,stride,iTc);
#endif
			}
		}
	}
	nY = pSlice->p_sps->pic_height_in_luma_samples - 8-poffset;
	if(nRow == (pSlice->PicHeightInCtbsY - 1)&&((nY&15)!=0))
	{
		nYLev = (nY>>2) * nTemp;
		for (nX = xStart; nX < xEnd; nX += 16)
		{
			VO_S32 index = nYLev+(nX>>2);
			bs = *(pBs_V + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					(((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
				pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pps_chroma_qp_offset_U );
				VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride+(nX>>1);
#if DEBLOCK_ASM_ENABLED
				voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
				src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride+(nX>>1);
				qpc = voQpUV( (qp) + pps_chroma_qp_offset_V );
				iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
				voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
			}
		}
	}
	return 0;
}


#else


// VO_S32 voDeblockingFilterInRow(void *pGst, VO_U32 nRow)
// {
// 	//H265_DEC *p_dec = (H265_DEC *)pDec;
// 	H265_DEC_SLICE * pSlice = (H265_DEC_SLICE *)pGst;
// 	VO_U32 mCUSize = (1 << pSlice->CtbLog2SizeY); //pSlice->p_sps->max_cu_width;
// 	VO_U32 nX = 0,nY;
// 	VO_U32 yEnd, xEnd;
// 	VO_U32 uiBs[2] = {0};
// 	VO_U8  *pBs_V, *pBs_H;
// 	VO_U32 nYLev; 
// 	VO_U32 nCol = pSlice->PicWidthInCtbsY; //pSlice->m_uiWidthInCU; 
// 	VO_U32 poffset = nRow*mCUSize;
// 	VO_U32 nTemp = pSlice->PicWidthInMinTbsY;
// 	VO_U32 nOffset = nRow * mCUSize * nTemp >> 2;  // >> 2, because m_puBsBuffer is mapped by 4*4  
// 
// 	xEnd = pSlice->p_sps->pic_width_in_luma_samples;
// 
// 	pBs_V = pSlice->p_pBsVer + sizeof( VO_U8 ) * nOffset;
// 	pBs_H = pSlice->p_pBsHor + sizeof( VO_U8 ) * nOffset;
// 
// 	yEnd = (nRow == (pSlice->PicHeightInCtbsY - 1)) ? (pSlice->p_sps->pic_height_in_luma_samples - (poffset)) : mCUSize;
// 	//Vertical Filter
// 	for (nY = 0; nY < yEnd; nY += 8)
// 	{
// 		nYLev = (nY>>2) * nTemp;
// 		for (nX = 0; nX < xEnd; nX += 16)
// 		{
// 			uiBs[0] = *(pBs_V + nYLev + (nX >> 2));
// 			uiBs[1] = *(pBs_V + nYLev + nTemp + (nX >> 2));
// 
// 			//fprintf(dump, "Ver: paras1 = %d, paras2 = %d \n", uiBs[0], uiBs[1]);
// 
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterVer(pSlice, uiBs, nX, nY+poffset);
// 				if ( (uiBs[0] == 2) && (nX & 15) == 0)
// 				{
// 					voChromaFilterVer(pSlice, nX, nY+poffset);
// 				}
// 			}
// 			uiBs[0] = *(pBs_V + nYLev + ((nX+8) >> 2));
// 			uiBs[1] = *(pBs_V + nYLev + nTemp + ((nX+8) >> 2));
// 
// 			//fprintf(dump, "Ver: paras3 = %d, paras4 = %d \n", uiBs[0], uiBs[1]);
// 
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterVer(pSlice, uiBs, nX+8, nY+poffset);
// 				if ( (uiBs[0] == 2) && ((nX+8) & 15) == 0)
// 				{
// 					voChromaFilterVer(pSlice, nX+8, nY+poffset);
// 				}
// 			}
// 		}
// 	}
// 	//Hor Filter
// 	nY = (nRow == 0) ? 8 : 0;
// 	for (; nY < yEnd; nY += 8)
// 	{
// 		nYLev = (nY>>2) * nTemp;
// 		for (nX = 0; nX < xEnd; nX += 16)
// 		{
// 			uiBs[0] = *(pBs_H + nYLev + (nX>>2));
// 			uiBs[1] = *(pBs_H + nYLev + (nX>>2)+1);
// 
// 			//fprintf(dump, "Hor: paras1 = %d, paras2 = %d \n", uiBs[0], uiBs[1]);
// 
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterHor(pSlice,uiBs, nX, nY+poffset);
// 				if ((uiBs[0] == 2) && ((nY) & 15) == 0)
// 				{
// 					voChromaFilterHor(pSlice, nX, nY+poffset);
// 				}
// 			}
// 
// 			uiBs[0] = *(pBs_H + nYLev + ((nX+8)>>2));
// 			uiBs[1] = *(pBs_H + nYLev + ((nX+8)>>2)+1);
// 
// 			//fprintf(dump, "Hor: paras3 = %d, paras4 = %d \n", uiBs[0], uiBs[1]);
// 			if (uiBs[0]||uiBs[1])
// 			{
// 				voLumaFilterHor(pSlice,uiBs, nX+8, nY+poffset);
// 				if ((uiBs[0] == 2) && ((nY) & 15) == 0)
// 				{
// 					voChromaFilterHor(pSlice, nX+8, nY+poffset);
// 				}
// 			}
// 		}
// 	}
// 	return 0;
// }

VO_S32 voDeblockingFilterInRowNew(void *pGst, VO_U32 nRow)
{
    //H265_DEC *p_dec = (H265_DEC *)pDec;
	H265_DEC_SLICE * pSlice = (H265_DEC_SLICE *)pGst;
	VO_U32 mCUSize = (1 << pSlice->CtbLog2SizeY);
	VO_U32 nX,nY;
	VO_U32 yEnd, xEnd;
	VO_U8  *pBs_V, *pBs_H;
	VO_S32  *pQP;
	VO_U32 nYLev; 
	//VO_U32 nOffset = nRow * pSlice->m_uiWidthInCU * pSlice->m_uiNumPartitions;
	VO_U32 poffset = nRow*mCUSize;
	VO_U32 nTemp = pSlice->PicWidthInCtbsY<<(pSlice->CtbLog2SizeY-2);
	VO_U32 nOffset = nRow * mCUSize * nTemp>>2 ;  // >> 2, because m_puBsBuffer is mapped by 4*4  

    //new params
    VO_S32   nDiv2 = pSlice->p_pps->pps_tc_offset_div2 << 1;
	VO_S32   nBDiv2 = pSlice->p_pps->pps_beta_offset_div2 << 1;
	VO_S32 stride = pSlice->cur_pic->pic_stride[ 0 ];
    VO_U8 bs,*tc,*beta;
	VO_U32 ptc,pbeta;

	xEnd = pSlice->p_sps->pic_width_in_luma_samples;

	pBs_V = pSlice->p_pBsVer + sizeof( VO_U8 ) * nOffset;
	pBs_H = pSlice->p_pBsHor + sizeof( VO_U8 ) * nOffset;
	pQP = pSlice->deblock_para +  nOffset;


	yEnd = (nRow == (pSlice->PicHeightInCtbsY - 1)) ? (pSlice->p_sps->pic_height_in_luma_samples - (poffset)) : mCUSize;
	tc = (VO_U8*)&ptc;
	beta = (VO_U8*)&pbeta;
	nY = 0;
	if(nRow==0)
	{
	    for (nX = 0; nX < xEnd; nX += 8)
		{
		    VO_S32 index = (nX>>2);
			bs = *(pBs_V + index);
			if (bs)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
	            VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	            VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];				
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+nX;

#if DEBLOCK_ASM_ENABLED
				voLumaFilterVer_ASM(src,stride,iBeta,iTc);
#else
                voLumaFilterVer_C(src,stride,iBeta,iTc);
#endif
			}
		}
		nY+=8;
	}
	for (nY=nY; nY < yEnd; nY += 8)
	{
		nYLev = (nY>>2) * nTemp;
		//first hor edge
		bs = *(pBs_H + nYLev);
		if (bs)
		{
			VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					    (((pQP[nYLev]& 0xff0000)>>16)+((pQP[nYLev-nTemp]& 0xff0000)>>16)+1)>>1:
					    pSlice->slice_qp;
	        VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	        VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];
			VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride;
#if DEBLOCK_ASM_ENABLED
			voLumaFilterHor_ASM(src,stride,iBeta,iTc);
#else
		    voLumaFilterHor_C(src,stride,iBeta,iTc);
#endif

			
		}
		for (nX = 8; nX < xEnd; nX += 8)
		{
		    VO_S32 index = nYLev+(nX>>2)-nTemp;
			ptc = pbeta = 0;
			bs = *(pBs_V + index);
			if (bs)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
	            tc[0] =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	            beta[0] = vobetatable_8x8[qp + nBDiv2];
			}	
            index+=nTemp;
			bs = *(pBs_V + index);
			if (bs)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
	            tc[1] =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	            beta[1] = vobetatable_8x8[qp + nBDiv2];
			}
			index-=1;
			bs = *(pBs_H + index);
			if (bs)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
	            tc[2] =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	            beta[2] = vobetatable_8x8[qp + nBDiv2];
			}
			index+=1;
			bs = *(pBs_H + index);
			if (bs)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
	            tc[3] =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	            beta[3] = vobetatable_8x8[qp + nBDiv2];
			}
			if(ptc)
			{
			    VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride+nX;
#if DEBLOCK_ASM_ENABLED
                voLumaFilterCross_ASM(src,stride,pbeta,ptc);
#else
				voLumaFilterCross_C(src,stride,pbeta,ptc);
#endif
			}
		}
		//last hor edge
		nX = xEnd-4;
		bs = *(pBs_H + nYLev + (nX>>2));
		if (bs)
		{
			VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					    (((pQP[nYLev+(nX>>2)]& 0xff0000)>>16)+((pQP[nYLev+(nX>>2)-nTemp]& 0xff0000)>>16)+1)>>1:
					    pSlice->slice_qp;
	        VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	        VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];
			VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride+nX;
#if DEBLOCK_ASM_ENABLED
			voLumaFilterHor_ASM(src,stride,iBeta,iTc);
#else
            voLumaFilterHor_C(src,stride,iBeta,iTc);
#endif
		}
		
	}
	if(nRow == (pSlice->PicHeightInCtbsY - 1))
	{
	    nY = pSlice->p_sps->pic_height_in_luma_samples - 4-poffset;
		nYLev = (nY>>2) * nTemp;
		for (nX = 0; nX < xEnd; nX += 8)
		{
		    VO_S32 index = nYLev+(nX>>2);
			bs = *(pBs_V + index);
			if (bs)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
	            VO_S32 iTc =  votctable_8x8[qp + voDEFAULT_INTRA_TC_OFFSET*(bs-1) + nDiv2];
	            VO_S32 iBeta = vobetatable_8x8[qp + nBDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 0 ]+(nY+poffset)*stride+nX;
#if DEBLOCK_ASM_ENABLED
				voLumaFilterVer_ASM(src,stride,iBeta,iTc);
#else
				voLumaFilterVer_C(src,stride,iBeta,iTc);
#endif

			}
		}
	}
	//chroma
	stride = pSlice->cur_pic->pic_stride[ 1 ];
	nY = 0;
	if(nRow==0)
	{
	    for (nX = 0; nX < xEnd; nX += 16)
		{
		    VO_S32 index = (nX>>2);
			bs = *(pBs_V + index);
			if (bs==2)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	            VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+(nX>>1);
#if DEBLOCK_ASM_ENABLED
                voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
				src = pSlice->cur_pic->pic_buf[ 2 ]+(nX>>1);
				qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
				iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
                voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
			}
		}
		nY+=16;
	}
    for (nY = nY; nY < yEnd; nY += 16)
	{
		nYLev = (nY>>2) * nTemp;
		//first hor edge
		bs = *(pBs_H + nYLev);
		if (bs==2)
		{
			VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					    (((pQP[nYLev]& 0xff0000)>>16)+((pQP[nYLev-nTemp]& 0xff0000)>>16)+1)>>1:
					    pSlice->slice_qp;
			VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	        VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
			VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride;
#if DEBLOCK_ASM_ENABLED
            voChromaFilterHor_ASM(src,stride,iTc);
#else
			voChromaFilterHor_C(src,stride,iTc);
#endif
			src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride;
			qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
			iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
			voChromaFilterHor_ASM(src,stride,iTc);
#else
			voChromaFilterHor_C(src,stride,iTc);
#endif

		}
		for (nX = 16; nX < xEnd; nX += 16)
		{
		    VO_S32 index = nYLev+(nX>>2)-2*nTemp;
			ptc = pbeta = 0;
			bs = *(pBs_V + index);
			if (bs==2)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	            tc[0] =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
				beta[0] = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];		
			}
			index+=2*nTemp;
			bs = *(pBs_V + index);
			if (bs==2)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	            tc[1] =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
				beta[1] = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];		
			}
			index-=2;
			bs = *(pBs_H + index);
			if (bs==2)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	            tc[2] =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
				beta[2] = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];		
			}
			index+=2;
			bs = *(pBs_H + index);
			if (bs==2)
			{
			    VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-nTemp]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	            tc[3] =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
				beta[3] = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];		
			}
			if(ptc)
			{
			    VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride+((nX)>>1);
#if DEBLOCK_ASM_ENABLED
                voChromaFilterCross_ASM(src,stride,ptc);
#else
			    voChromaFilterCross_C(src,stride,ptc);
#endif
			}
			if(pbeta)
			{
			    VO_U8* src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride+((nX)>>1);
#if DEBLOCK_ASM_ENABLED
				voChromaFilterCross_ASM(src,stride,pbeta);
#else
			    voChromaFilterCross_C(src,stride,pbeta);
#endif

			}
		}
		//last hor edge
		nX = xEnd-8;
		bs = *(pBs_H + nYLev + (nX>>2));
		if (bs==2)
		{
			VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					    (((pQP[nYLev+(nX>>2)]& 0xff0000)>>16)+((pQP[nYLev+(nX>>2)-nTemp]& 0xff0000)>>16)+1)>>1:
					    pSlice->slice_qp;
			VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	        VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
			VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride+(nX>>1);
#if DEBLOCK_ASM_ENABLED
			voChromaFilterHor_ASM(src,stride,iTc);
#else
			voChromaFilterHor_C(src,stride,iTc);
#endif

			src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride+(nX>>1);
			qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
			iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
			voChromaFilterHor_ASM(src,stride,iTc);
#else
			voChromaFilterHor_C(src,stride,iTc);
#endif
		}
	}
	nY = pSlice->p_sps->pic_height_in_luma_samples - 8-poffset;
	if(nRow == (pSlice->PicHeightInCtbsY - 1)&&((nY&15)!=0))
	{
	    //nY = pSlice->p_sps->pic_height_in_luma_samples - 8-poffset;
		nYLev = (nY>>2) * nTemp;
		for (nX = 0; nX < xEnd; nX += 16)
		{
		    VO_S32 index = nYLev+(nX>>2);
			bs = *(pBs_V + index);
			if (bs==2)
			{
				VO_S32 qp = pSlice->p_pps->cu_qp_delta_enabled_flag?
					        (((pQP[index]& 0xff0000)>>16)+((pQP[index-1]& 0xff0000)>>16)+1)>>1:
					        pSlice->slice_qp;
				VO_S32 qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[0] );
	            VO_S32 iTc =  votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
				VO_U8* src = pSlice->cur_pic->pic_buf[ 1 ]+((nY+poffset)>>1)*stride+(nX>>1);
#if DEBLOCK_ASM_ENABLED
                voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
				src = pSlice->cur_pic->pic_buf[ 2 ]+((nY+poffset)>>1)*stride+(nX>>1);
				qpc = voQpUV( (qp) + pSlice->p_pps->pps_chroma_qp_offset[1] );
				iTc = votctable_8x8[qpc + voDEFAULT_INTRA_TC_OFFSET + nDiv2];
#if DEBLOCK_ASM_ENABLED
                voChromaFilterVer_ASM(src,stride,iTc);
#else
				voChromaFilterVer_C(src,stride,iTc);
#endif
			}
		}
	}
	return 0;
}




#endif


