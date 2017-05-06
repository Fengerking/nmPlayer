 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_mvpred.c
    \brief    mv prediction
    \author   Renjie Yu
	\change
*/

/*
Note
cb: Coding block
ctb: Coding tree block
RS: Raster Scan

*/

#include "h265_decoder.h"
#include "h265dec_utils.h"

VO_VOID xInitTiles(H265_DEC* p_dec,VO_U32 nRowsInTile,VO_U32 nColumnsInTile)
{
#if 0
  VO_U32  uiTileIdx;
  VO_U32  uiColumnIdx = 0;
  VO_U32  uiRowIdx = 0;
  VO_U32  uiRightEdgePosInCU;
  VO_U32  uiBottomEdgePosInCU;
  VO_U32  i, j;

  //initialize each tile of the current picture
  for( uiRowIdx=0; uiRowIdx < nRowsInTile; uiRowIdx++ )
  {
    for( uiColumnIdx=0; uiColumnIdx < nColumnsInTile; uiColumnIdx++ )
    {
      uiTileIdx = uiRowIdx * nColumnsInTile + uiColumnIdx;

      //initialize the RightEdgePosInCU for each tile
      uiRightEdgePosInCU = 0;
      for( i=0; i <= uiColumnIdx; i++ )
      {
        uiRightEdgePosInCU += p_dec->m_apcTComTile[uiRowIdx * nColumnsInTile + i].m_uiTileWidth;
      }
      p_dec->m_apcTComTile[uiTileIdx].m_uiRightEdgePosInCU = uiRightEdgePosInCU-1;
      
	  //initialize the BottomEdgePosInCU for each tile
      uiBottomEdgePosInCU = 0;
      for( i=0; i <= uiRowIdx; i++ )
      {
        uiBottomEdgePosInCU += p_dec->m_apcTComTile[i * nColumnsInTile + uiColumnIdx].m_uiTileHeight;
      }
      p_dec->m_apcTComTile[uiTileIdx].m_uiBottomEdgePosInCU = uiBottomEdgePosInCU-1;

      //initialize the FirstCUAddr for each tile
      p_dec->m_apcTComTile[uiTileIdx].m_uiFirstCUAddr = (p_dec->m_apcTComTile[uiTileIdx].m_uiBottomEdgePosInCU - p_dec->m_apcTComTile[uiTileIdx].m_uiTileHeight +1)*p_slice->PicWidthInCtbsY + 
       p_dec->m_apcTComTile[uiTileIdx].m_uiRightEdgePosInCU - p_dec->m_apcTComTile[uiTileIdx].m_uiTileWidth + 1;
    }
  }

  //initialize the TileIdxMap
  for( i=0; i<p_slice->PicSizeInCtbsY; i++)
  {
    for(j=0; j < nColumnsInTile; j++)
    {
      if(i % p_slice->PicWidthInCtbsY <= p_dec->m_apcTComTile[j].m_uiRightEdgePosInCU)
      {
        uiColumnIdx = j;
        j = nColumnsInTile;
      }
    }
    for(j=0; j < nRowsInTile; j++)
    {
      if(i/p_slice->PicWidthInCtbsY <= p_dec->m_apcTComTile[j*nColumnsInTile].m_uiBottomEdgePosInCU)
      {
        uiRowIdx = j;
        j = nRowsInTile;
      }
    }
    p_dec->m_puiTileIdxMap[i] = uiRowIdx * nColumnsInTile + uiColumnIdx;
  }
#endif
}


VO_S32 ActivateParameterSets(H265_DEC* p_dec, H265_DEC_SLICE *p_slice)
{
#if 0
	H265_DEC_PPS *p_pps = p_slice->p_pps;
	H265_DEC_SPS *p_sps = p_dec->sps;//YU_TBD


	//pps->setMinCuDQPSize( sps->getMaxCUWidth() >> ( pps->getMaxCuDQPDepth()) );
	p_slice->m_uiMinCuDQPSize = p_sps->max_cu_width >> p_pps->diff_cu_qp_delta_depth;


	/* init deblock filtering */
	//H265DeblockInit((VO_VOID **)&(p_dec->m_df), g_uiMaxCUDepth, p_slice->PicWidthInCtbsY);

	if(!p_dec->params_init_done)
	{
	  VO_U32 p,i,j,rowsInTile,columnsInTile;
	  VO_S32 uiCummulativeTileHeight,uiCummulativeTileWidth;
	  
	  rowsInTile    = p_pps->num_tile_rows_minus1 + 1;
	  columnsInTile = p_pps->num_tile_columns_minus1 + 1;

	  if (p_dec->m_apcTComTile)	//Harry: for multi pps
	  {
		  AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_dec->m_apcTComTile);
	  }
	  p_dec->m_apcTComTile = ( H265_DEC_TILE* )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, sizeof( H265_DEC_TILE )*(p_pps->num_tile_columns_minus1+1)*(p_pps->num_tile_rows_minus1+1) , CACHE_LINE);
	  assert(p_dec->m_apcTComTile);

	  if(p_slice->p_pps->uniform_spacing_flag )
	  {
		//set the width for each tile
		for (j = 0; j < rowsInTile; j++) {
			for(p = 0; p < columnsInTile; p++) {
				p_dec->m_apcTComTile[j * columnsInTile + p].m_uiTileWidth = 
					(p+1)*p_slice->PicWidthInCtbsY/columnsInTile - (p*p_slice->PicWidthInCtbsY)/columnsInTile;
			}
		}

		//set the height for each tile
		for (j = 0; j < columnsInTile; j++) {
			for(p = 0; p < rowsInTile; p++) {
				p_dec->m_apcTComTile[ p * columnsInTile + j ].m_uiTileHeight = 
					(p+1)*p_dec->m_uiHeightInCU/rowsInTile - p*p_dec->m_uiHeightInCU/rowsInTile ;   
			}
		}
	  } else {
		//set the width for each tile
		for(j = 0; j < rowsInTile; j++) {
			uiCummulativeTileWidth = 0;
			for (i = 0; i < columnsInTile - 1; i++) {
				p_dec->m_apcTComTile[j * columnsInTile + i].m_uiTileWidth = p_slice->p_pps->column_width[i] ;
				uiCummulativeTileWidth += p_slice->p_pps->column_width[i];
			}
			p_dec->m_apcTComTile[j * columnsInTile+ i].m_uiTileWidth = p_slice->PicWidthInCtbsY - uiCummulativeTileWidth ;
		}

		//set the height for each tile
		for (j = 0; j < columnsInTile; j++) {
			uiCummulativeTileHeight = 0;
			for(i = 0; i < p_slice->p_pps->num_tile_rows_minus1; i++) { 
				p_dec->m_apcTComTile[i * columnsInTile+ j].m_uiTileHeight = p_slice->p_pps->row_height[i];
				uiCummulativeTileHeight += p_slice->p_pps->row_height[i];
			}
			p_dec->m_apcTComTile[i * columnsInTile+ j].m_uiTileHeight = p_dec->m_uiHeightInCU - uiCummulativeTileHeight;
		}
	  }
	  xInitTiles(p_dec,rowsInTile,columnsInTile);

      //-------Joyce add : scalingList----
      if (p_slice->p_sps->scaling_list_enabled_flag)
      {
        p_slice->pSliceScalingListEntity = &(p_slice->p_sps->spsScalingListEntity);
        if (p_slice->p_pps->pps_scaling_list_data_present_flag)
        {
            p_slice->pSliceScalingListEntity = &(p_slice->p_pps->ppsScalingListEntity);
        }
        //p_slice->pSliceScalingListEntity->m_useTransformSkip = p_slice->p_pps->m_useTransformSkip;//-----useless
        if (!p_slice->p_pps->pps_scaling_list_data_present_flag && ! p_slice->p_sps->sps_scaling_list_data_present_flag)
        {
            //TComScalingList_destroy(p_slice->pSliceScalingListEntity);
            //TComScalingList_init(p_slice->pSliceScalingListEntity);
            //TComSlice_setDefaultScalingList(p_slice->pSliceScalingListEntity);
        }
        TComTrQuant_setScalingListDec(p_dec, p_slice->pSliceScalingListEntity);
        //p_slice->p_sps->scaling_list_enabled_flag  represent m_cTrQuant.setUseScalingList(VO_TRUE);
      }
      else
      {
        TComTrQuant_setFlatScalingList(p_dec);
      }
      //---------------end Joyce add-----------
	  
	  p_dec->params_init_done = 1;
	}
#endif
	return VO_ERR_NONE;
}



// VO_U32 bflag = 0;//YU_TBD

/* Get a pic with equal poc*/


VO_VOID IniDecodeSlice(H265_DEC *p_dec) 
{
#if 0
	 H265_DEC_SLICE *p_slice = &p_dec->slice;
	 H265_DEC_SPS *p_sps = p_slice->p_sps;

	p_slice->partition_num = p_slice->partition_num;
	//p_slice->m_puiInverseCUOrderMap = p_dec->m_puiInverseCUOrderMap;
	p_slice->m_uhTotalDepth = p_sps->max_cu_depth;//YU_TBD remove p_slice->m_uhTotalDepth 
	p_slice->PicSizeInCtbsY = p_slice->PicSizeInCtbsY;
	p_slice->m_bCUAbove = p_dec->m_bCUAbove;
	p_slice->m_bCUAboveLeft = p_dec->m_bCUAboveLeft;
	p_slice->m_bCUAboveRight = p_dec->m_bCUAboveRight;
	p_slice->m_bCULeft = p_dec->m_bCULeft;
	p_slice->last_coded_qp = p_slice->slice_qp;
#endif
}

//YU_TBD: use pu level slice number may simplify drivepuavail
// 6.4.2
VOINLINE static VO_BOOL DerivePUAvail( H265_DEC_SLICE * p_slice, 
	const VO_S32 xCb, 
	const VO_S32 yCb, 
	const VO_S32 nCbS,
	const VO_S32 xPb,
	const VO_S32 yPb, 
	const VO_S32 nPbW, 
	const VO_S32 nPbH, 
	const VO_S32 xNbY, 
	const VO_S32 yNbY, 
	const VO_S32 partIdx,
	PicMvField **PbMvField )
{
	if ( ( xCb <= xNbY ) && ( yCb <= yNbY ) && ( ( xCb + nCbS ) > xNbY ) && ( ( yCb + nCbS ) > yNbY ) ) { //sameCb==1
    //if (minBlockAddrCbStart <= minBlockAddrN && minBlockAddrN <= minBlockAddrCbEnd){
		VO_U32 availability = ( ( nPbW << 1 ) != nCbS ) || ( ( nPbH << 1 ) != nCbS ) || ( partIdx != 1 ) || ( ( xCb + nPbW ) <= xNbY ) || ( ( yCb + nPbH ) > yNbY );
        if ( availability ) {
			*PbMvField  = p_slice->p_motion_field + \
				( yNbY >> 2 ) * p_slice->PicWidthInMinTbsY + ( xNbY >> 2 );

      if (( *PbMvField )->pred_mode == MODE_INTRA )
        return VO_FALSE;
    }
		return ( VO_BOOL )availability;
	} else {//6.4.1
//         VO_U32 MinTbLog2SizeY = p_slice->MinTbLog2SizeY;
//         VO_U32 NPbOffset;
// 
//         if ( xNbY < 0 || yNbY < 0 || xNbY >= ( VO_S32 ) p_slice->p_sps->pic_width_in_luma_samples || yNbY >= ( VO_S32 )p_slice->p_sps->pic_height_in_luma_samples )
//             return VO_FALSE;
// 
//         NPbOffset = ( yNbY >> MinTbLog2SizeY ) * p_slice->PicWidthInMinTbsY + ( xNbY >> MinTbLog2SizeY );
//         *PbMvField  = p_slice->p_motion_field + NPbOffset;
// 
//         if (( *PbMvField )->pred_mode == MODE_INTRA )
//             return VO_FALSE;
// #if 1 //YU_TBD, support tile and slice
// // 		if ( //( xNbY >> MinTbLog2SizeY ) >= p_slice->tile_start_x && ( yNbY >> MinTbLog2SizeY ) >= p_slice->tile_start_y &&
// // 		    //p_slice->slice_number[ NPbOffset ] == p_slice->cur_slice_number ) 
// // 		    //add by DTT
// // 		    p_slice->p_pps->MinTbAddrZs[(yNbY>>MinTbLog2SizeY)*p_slice->PicWidthInMinTbsY+(xNbY>>MinTbLog2SizeY)]<=p_slice->p_pps->MinTbAddrZs[(yPb>>MinTbLog2SizeY)*p_slice->PicWidthInMinTbsY+(xPb>>MinTbLog2SizeY)]
// // 		&&p_slice->p_pps->MinTbAddrZs[(yNbY>>MinTbLog2SizeY)*p_slice->PicWidthInMinTbsY+(xNbY>>MinTbLog2SizeY)]>=p_slice->SliceMinTbAddrZs)
// // 	            return VO_TRUE;
// 
//         //YU_TBD: refine addr
//         if ( //( xNbY >> MinTbLog2SizeY ) >= p_slice->tile_start_x && ( yNbY >> MinTbLog2SizeY ) >= p_slice->tile_start_y &&
//             //p_slice->slice_number[ NPbOffset ] == p_slice->cur_slice_number ) 
//             //add by DTT
//             p_slice->p_pps->MinTbAddrZs[(yNbY>>MinTbLog2SizeY)*(p_slice->PicWidthInMinTbsY + 2)+(xNbY>>MinTbLog2SizeY)]<=p_slice->p_pps->MinTbAddrZs[(yPb>>MinTbLog2SizeY)*(p_slice->PicWidthInMinTbsY + 2)+(xPb>>MinTbLog2SizeY)]
//         &&p_slice->p_pps->MinTbAddrZs[(yNbY>>MinTbLog2SizeY)*(p_slice->PicWidthInMinTbsY + 2)+(xNbY>>MinTbLog2SizeY)]>=p_slice->SliceMinTbAddrZs){
//             return VO_TRUE;
//         }
//     
// #endif

        {           
            VO_S32 minBlockAddrCurr = p_slice->p_pps->MinTbAddrZs[(yPb >> 2) * p_slice->p_pps->MinTbAddrZsStride + (xPb>>2)];
            VO_S32 minBlockAddrN = p_slice->p_pps->MinTbAddrZs[(yNbY >> 2) * p_slice->p_pps->MinTbAddrZsStride + (xNbY>>2)];
            if (/*minBlockAddrN < 0 || */
                minBlockAddrN > minBlockAddrCurr ||
                minBlockAddrN < p_slice->SliceMinTbAddrZs || 
                minBlockAddrN < p_slice->p_pps->tile_start_zs[p_slice->currEntryNum]){
                    return VO_FALSE;
            } else {
                VO_U32 NPbOffset = ( yNbY >> 2 ) * p_slice->PicWidthInMinTbsY + ( xNbY >> 2 );
                *PbMvField  = p_slice->p_motion_field + NPbOffset; 

                if (( *PbMvField )->pred_mode == MODE_INTRA )
                    return VO_FALSE;

                return VO_TRUE;
            }
        }
    }

    return VO_FALSE;
}

static VO_BOOL hasEqualMotion(const PicMvField* p_mvfiled0, 
	const PicMvField* p_mvfiled1
)
{
	VO_U32 ref_list_idx;
	VO_U32 inter_dir = p_mvfiled0->inter_dir;

	if ( inter_dir != p_mvfiled1->inter_dir ){
		return VO_FALSE;
	}

	for ( ref_list_idx = 0; ref_list_idx < 2; ref_list_idx++ ) {
		if ( inter_dir & ( 1 << ref_list_idx ) ) {
			if (p_mvfiled0->m_acMv[ ref_list_idx ] != p_mvfiled1->m_acMv[ ref_list_idx ] ||
				p_mvfiled0->m_iRefIdx[ ref_list_idx ] != p_mvfiled1->m_iRefIdx[ ref_list_idx ] ) {
				return VO_FALSE;
			}
		}
	}

	return VO_TRUE;
}

//8.5.3.2.8
static VO_BOOL xGetColMVP( H265_DEC_SLICE *p_slice,
	TComPic *pColPic,
	RefPicList eRefPicList,
	VO_S32* rcMv,
	VO_S32 xColMinPb, 
	VO_S32 yColMinPb,
	VO_S32 iRefIdx )
{
	RefPicList  listCol;
	VO_S32      refIdxCol;

	VO_S32 colPocDiff, currPocDiff;

	VO_BOOL bIsCurrRefLongTerm;
	VO_BOOL bIsColRefLongTerm;
	
    PicMvField *col_motion_buffer = pColPic->motion_buffer + \
		yColMinPb * p_slice->PicWidthInMinTbsY + xColMinPb;
	
	//Check colPb mode
    if ( col_motion_buffer->pred_mode == MODE_INTRA )
	    return VO_FALSE;

	//drive listCol, mvCol, the reference index refIdxCol 
	if ( col_motion_buffer->inter_dir == 2 ) {  
		listCol = REF_PIC_LIST_1;
    } else if ( col_motion_buffer->inter_dir == 1 ) {
		listCol = REF_PIC_LIST_0;
	} else {
		listCol = p_slice->m_bCheckLDC ? eRefPicList : ( RefPicList )p_slice->collocated_from_l0_flag;
	}
	
    refIdxCol = col_motion_buffer->m_iRefIdx[ listCol ];
	
	bIsCurrRefLongTerm = p_slice->ref_pic_list[ eRefPicList ][ iRefIdx ]->m_bIsLongTerm;
  if (p_slice->ref_pic_list[ listCol ][ refIdxCol ])
  {
    bIsColRefLongTerm = p_slice->ref_pic_list[ listCol ][ refIdxCol ]->m_bIsLongTerm;
    if ( bIsCurrRefLongTerm != bIsColRefLongTerm ) {
      return VO_FALSE;
    }
  }
  else
  {
    bIsColRefLongTerm = VO_FALSE;
  }

	if ( bIsCurrRefLongTerm != bIsColRefLongTerm )
		return VO_FALSE;

	currPocDiff = p_slice->m_iPOC - p_slice->ref_pic_list[ (VO_U32)eRefPicList ][ iRefIdx ]->m_iPOC;    
	colPocDiff  = pColPic->m_iPOC - pColPic->m_aiRefPOCList[(VO_U32)listCol][ refIdxCol ];

	if ( bIsCurrRefLongTerm || currPocDiff == colPocDiff ) {
        *rcMv = col_motion_buffer->m_acMv[ listCol ];
	} else {
		VO_S32 iTDB      = Clip3( -128, 127, currPocDiff );
		VO_S32 iTDD      = Clip3( -128, 127, colPocDiff );
		VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
		VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

		VO_S32 cColMvHor = col_motion_buffer->m_acMv[ listCol ] << 16 >> 16;
		VO_S32 cColMvVer = col_motion_buffer->m_acMv[ listCol ] >> 16;

      	cColMvHor = (VO_S16)Clip3( -32768, 32767, (iScale * cColMvHor + 127 + (iScale * cColMvHor < 0 ) ) >> 8 );
		cColMvVer = (VO_S16)Clip3( -32768, 32767, (iScale * cColMvVer + 127 + (iScale * cColMvVer < 0 ) ) >> 8 );
		*rcMv     = ( cColMvVer << 16 ) | ( cColMvHor & 0xFFFF );
	}

    return VO_TRUE;
}

VO_VOID GetInterMergeCandidates( H265_DEC_SLICE* p_slice,
	const VO_U32 xCb, 
	const VO_U32 yCb,
	const VO_U32 nCbS,                      
	const VO_U32 xPb, 
	const VO_U32 yPb,
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	const VO_U32 partIdx,
	const VO_S32 mrgCandIdx,
	PicMvField* pcMvFieldNeighbours
 )
{  
	//VO_U32 PicWidthInMinTbsY = p_slice->PicWidthInMinTbsY;
	//VO_U32 MinTbLog2SizeY = p_slice->MinTbLog2SizeY;
	VO_U32  idx;
	VO_U32  numCand, numRefIdx ,zeroIdx;
	PartMode cuPartMode;
	VO_U32 isAvailableA1 = 0;
	VO_U32 isAvailableB1 = 0;

	PicMvField *mv_field;
	PicMvField *mv_field_left;
	PicMvField *mv_field_above;
    PicMvField *mv_field_above_right;

	PicMvField* pMv_out = pcMvFieldNeighbours;
    numCand = 0;
	cuPartMode = p_slice->CuPartMode;

	//----8.5.3.2.2	Derivation process for spatial merging candidates----
	//left
	if ( DerivePUAvail( p_slice, xCb, yCb, nCbS , xPb, yPb, nPbW, nPbH, xPb - 1, yPb + nPbH - 1, partIdx, &mv_field_left ) ) {
		isAvailableA1 = //isDiffMER(p_dec, xPb -1, yPb+nPbH-1, xPb, yPb) &&
					  !( partIdx  == 1 && (cuPartMode == PART_Nx2N || cuPartMode == PART_nLx2N || cuPartMode == PART_nRx2N ) );

		if ( isAvailableA1 ) {
			// get Mv from Left		
			*pMv_out++ = *mv_field_left;

			if ( mrgCandIdx == numCand ) {
				return;
			}
			numCand++;
		}
	}

	// above
	if ( DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb + nPbW - 1, yPb - 1, partIdx, &mv_field_above ) ) {
		isAvailableB1 = //isDiffMER(p_dec, xPb + nPbW  -1, yPb - 1, xPb, yPb) &&
						!( partIdx  == 1 && (cuPartMode == PART_2NxN || cuPartMode == PART_2NxnU || cuPartMode == PART_2NxnD ) );

		if ( isAvailableB1 && !( isAvailableA1 && hasEqualMotion( mv_field_above, mv_field_left ) ) ){	
			*pMv_out++ = *mv_field_above;

			if ( mrgCandIdx == numCand ) {
				return;
			}
			numCand ++;
		}
	}

	// above right
	if ( ( xPb + nPbW ) < p_slice->p_sps->pic_width_in_luma_samples  &&  \
		DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb + nPbW , yPb - 1, partIdx, &mv_field_above_right ) ) {

		if ( !(//!isDiffMER(p_dec, xPb+nPbW, yPb-1, xPb, yPb) ||
			( isAvailableB1 && hasEqualMotion( mv_field_above_right, mv_field_above ) ) ) ) {
		
			// get Mv from above right		
			*pMv_out++ = *mv_field_above_right;

			if ( mrgCandIdx == numCand ){
				return;
			}
			numCand ++;
		}
	}

	//left bottom
	if ( ( yPb + nPbH ) < p_slice->p_sps->pic_height_in_luma_samples && \
		DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb - 1, yPb + nPbH, partIdx, &mv_field ) ) {

		if ( !(//!isDiffMER(p_dec, xPb - 1, yPb + nPbH, xPb, yPb) ||
			( isAvailableA1 && hasEqualMotion( mv_field, mv_field_left) ) ) ) {
	
			// get Mv from left bottom	
			*pMv_out++ = *mv_field;

			if ( mrgCandIdx == numCand ) {
				return;
			}
			numCand ++;
		}
	}

	// above left 
	if( numCand < 4 && DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb - 1, yPb - 1,partIdx, &mv_field ) ) {
		if ( !(//!isDiffMER(p_dec, xPb-1, yPb-1, xPb, yPb) ||
		    ( isAvailableA1 && hasEqualMotion( mv_field, mv_field_left ) ) ||
			( isAvailableB1 && hasEqualMotion( mv_field, mv_field_above ) ) ) ) {
		
			// get Mv from Left		
			*pMv_out++ = *mv_field;

			if ( mrgCandIdx == numCand ) {
				return;
			}
			numCand ++;
		}
	}

	////8.5.3.2.7
	if ( p_slice->p_sps->sps_temporal_mvp_enable_flag ) {
		TComPic *colPic = p_slice->ref_pic_list[ p_slice->slice_type == B_SLICE ? 1 - p_slice->collocated_from_l0_flag : 0 ][ p_slice->collocated_ref_idx ];

		if ( colPic ) {
			VO_U32 dir = 0;
		    VO_S32 cColMv;
		    VO_U32 bExistMV = 0;

			VO_U32 xColBr = xPb + nPbW;
            VO_U32 yColBr = yPb + nPbH;
			VO_U32 avalColBr = ( xColBr < p_slice->p_sps->pic_width_in_luma_samples && \
				 yColBr < p_slice->p_sps->pic_height_in_luma_samples && \
				( yPb >> p_slice->CtbLog2SizeY ) == ( yColBr >> p_slice->CtbLog2SizeY ) ) ;

#if USE_FRAME_THREAD
			if (p_slice->nThdNum > 1)
			{
// 				VO_S32 max_y = yP + nPSH - 1;
				WaitForProgress_mv(colPic, p_slice->m_uiCUAddr+avalColBr);
			}
#endif

			//bottom right collocated motion vector
			if ( avalColBr ) {
				bExistMV = xGetColMVP( p_slice, 
					colPic, 
					REF_PIC_LIST_0, 
					&cColMv, 
					( xColBr & ~15 ) >> 2,
					( yColBr & ~15 ) >> 2,
					0 );
			}

			//central collocated motion vector 
			if ( bExistMV == VO_FALSE ) {
				bExistMV = xGetColMVP( p_slice, 
					colPic, 
					REF_PIC_LIST_0, 
					&cColMv, 
					( ( xPb + ( nPbW >> 1 ) ) & ~15 ) >> 2,
					( ( yPb + ( nPbH >> 1 ) ) & ~15 ) >> 2, 
					0 );
			}
			if ( bExistMV ) {
				dir = 1;
				pMv_out->m_acMv[ 0 ] = cColMv ;
				pMv_out->m_iRefIdx[ 0 ] = 0;
			}

			if ( p_slice->slice_type == B_SLICE ) {
				bExistMV = 0;

				if ( avalColBr ) {
					bExistMV = xGetColMVP( p_slice, 
						colPic, 
						REF_PIC_LIST_1,
						&cColMv, 
						( ( xPb + nPbW ) & ~15 ) >> 2,
						( ( yPb + nPbH ) & ~15 ) >> 2,
						0 );
				}
				
				if( bExistMV == VO_FALSE ) {
					bExistMV = xGetColMVP( p_slice, 
						colPic,
						REF_PIC_LIST_1, 
						&cColMv, 
						( ( xPb + ( nPbW >> 1 ) ) & ~15 ) >> 2,
						( ( yPb + ( nPbH >> 1 ) ) & ~15 ) >> 2,
						0 );
				}
				if ( bExistMV ) {
					dir |= 2;
					pMv_out->m_acMv[ 1 ] = cColMv;
					pMv_out->m_iRefIdx[ 1 ] = 0;
				}
			}else{
        pMv_out->m_acMv[ 1 ] = -1;
        pMv_out->m_iRefIdx[ 1 ] = -1;
      }
		
			if ( dir != 0 ){
				pMv_out->inter_dir = ( VO_U8 )dir;
		
				if ( mrgCandIdx == numCand ) {
					return;
				}
				++pMv_out;
				numCand++;
			}
		}
	}
	
	////8.5.3.2.3	Derivation process for combined bi-predictive merging candidates
	if ( p_slice->slice_type == B_SLICE ) {
		static const VO_U32 l0CandIdx[ 12 ] = { 0 , 1, 0, 2, 1, 2, 0, 3, 1, 3, 2, 3 };
		static const VO_U32 l1CandIdx[ 12 ] = { 1 , 0, 2, 0, 2, 1, 3, 0, 3, 1, 3, 2 };
		const VO_U32 uiCutoff = numCand * ( numCand - 1 );

		for ( idx = 0; idx < uiCutoff && numCand != p_slice->max_num_merge_cand; idx++ ) {
			VO_U32 i = *( l0CandIdx + idx );
			VO_U32 j = *( l1CandIdx + idx );

			if ( ( ( pcMvFieldNeighbours + i )->inter_dir & 0x1 ) && ( ( pcMvFieldNeighbours + j )->inter_dir & 0x2 ) ) {
				VO_S32 mvL0l0Cand      = pcMvFieldNeighbours[ i ].m_acMv[ 0 ];
				VO_S32 refIdxL0l0Cand  = pcMvFieldNeighbours[ i ].m_iRefIdx[ 0 ];
				VO_S32 mvL1l1Cand      = pcMvFieldNeighbours[ j ].m_acMv[ 1 ];
				VO_S32 refIdxL1l1Cand  = pcMvFieldNeighbours[ j ].m_iRefIdx[ 1 ];

				VO_S32 iRefPOCL0 = ( *( p_slice->ref_poc_list ) )[ REF_PIC_LIST_0 ][ refIdxL0l0Cand ];
				VO_S32 iRefPOCL1 = ( *( p_slice->ref_poc_list ) )[ REF_PIC_LIST_1 ][ refIdxL1l1Cand ];

				if ( iRefPOCL0 != iRefPOCL1 || mvL0l0Cand  != mvL1l1Cand ) {
					PicMvField t = { 0, ( VO_S8 )refIdxL0l0Cand, ( VO_S8 )refIdxL1l1Cand, 3, mvL0l0Cand , mvL1l1Cand  };
					*pMv_out++ = t;

				    if ( mrgCandIdx == numCand ) {
				        return;
			        }
					numCand++;
				}
			}
		}
	}
	////8.5.3.2.4	Derivation process for zero motion vector merging candidates
	numRefIdx = ( p_slice->slice_type== B_SLICE) ? \
		MIN( p_slice->num_ref_idx[ REF_PIC_LIST_0 ], p_slice->num_ref_idx[ REF_PIC_LIST_1 ] ) : \
		p_slice->num_ref_idx[ REF_PIC_LIST_0 ];

	zeroIdx = 0;

	while ( numCand < p_slice->max_num_merge_cand ) {
		VO_U32 dir = p_slice->slice_type == B_SLICE ? 3 : 1;
    PicMvField t = { 0, ( VO_S8 )zeroIdx, ( VO_S8 )-1, ( VO_U8 )dir, 0, -1 };
    if(p_slice->slice_type == B_SLICE ){
      t.m_iRefIdx[1] = zeroIdx;
      t.m_acMv[1] = 0;
    }
	
		*pMv_out++ = t;

		if ( mrgCandIdx == numCand ) {
			return;
		}
		numCand++;

		if ( zeroIdx == numRefIdx - 1 ) {
			zeroIdx = 0;
		} else {
			++zeroIdx;
		}
	}
}

VO_VOID FillMvpCand ( H265_DEC_SLICE * p_slice, 
	const RefPicList eRefPicList, 
	const VO_U32 xCb,
	const VO_U32 yCb,
	const VO_U32 nCbS,
	const VO_U32 xPb, 
	const VO_U32 yPb, 
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	const VO_S32 iRefIdx, 
	const VO_U32 partIdx,
	VO_S32* pInfo )
{ 
	//VO_U32 PicWidthInMinTbsY   = p_slice->PicWidthInMinTbsY;
	//VO_U32 MinTbLog2SizeY      = p_slice->MinTbLog2SizeY;
	VO_BOOL availableFlagLXA   = VO_FALSE, availableFlagLXB = VO_FALSE, isScaledFlagLX = VO_FALSE;
	VO_S32 available0,available1, available2;
    VO_U32 eRefPicList2nd      = 1 - (VO_U32)eRefPicList;
	VO_S32 iCurrRefPOC         = p_slice->ref_pic_list[(VO_U32)eRefPicList][iRefIdx]->m_iPOC;
	VO_S32 iCurrPOC            = p_slice->m_iPOC;
	VO_BOOL bIsCurrRefLongTerm = p_slice->ref_pic_list[(VO_U32)eRefPicList][iRefIdx]->m_bIsLongTerm;

	PicMvField *PbMVField0, *PbMVField1, *PbMVField2;
	VO_U32 can_num = 0;

	if ( iRefIdx < 0) {
		return;
	}

	//1. -- 8.5.3.2.6	Derivation process for motion vector predictor candidates (Spatial)
 	//1. below left
	available0 = DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb - 1, yPb + nPbH, partIdx, &PbMVField0 );
	available1 = DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb - 1, yPb + nPbH - 1, partIdx, &PbMVField1 );

	if ( available0 || available1 )
		isScaledFlagLX = VO_TRUE;

	// 2. Left predictor search
	//LB 
	if ( available0 && availableFlagLXA == VO_FALSE) {

		if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList ][ PbMVField0->m_iRefIdx[ eRefPicList ] ] == iCurrRefPOC ) {
	  
			*( pInfo + can_num++ ) = PbMVField0->m_acMv[ eRefPicList ];  
			availableFlagLXA = VO_TRUE;
	    } else if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList2nd ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList2nd ][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ] == iCurrRefPOC ) {

			*( pInfo + can_num++ ) = PbMVField0->m_acMv[ eRefPicList2nd ];  
			availableFlagLXA = VO_TRUE;
		}
	}

	//L
	if ( available1 && availableFlagLXA == VO_FALSE) {
		if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList ][ PbMVField1->m_iRefIdx[ eRefPicList ] ] == iCurrRefPOC ) {
	  
			*( pInfo + can_num++ ) = PbMVField1->m_acMv[ eRefPicList ];  
			availableFlagLXA = VO_TRUE;
	    } else if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList2nd ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList2nd ][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ] == iCurrRefPOC ) {

			*( pInfo + can_num++ ) = PbMVField1->m_acMv[ eRefPicList2nd ];  
			availableFlagLXA = VO_TRUE;
		}
	}
  
	if( available0 && availableFlagLXA == VO_FALSE) {
		if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList ) ) && \
			( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList][ PbMVField0->m_iRefIdx[ eRefPicList ] ]->m_bIsLongTerm ) ) {
			VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList ][ PbMVField0->m_iRefIdx[ eRefPicList ] ]->m_iPOC;
		    VO_S32 rcMv;
		  
			if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList ][ PbMVField0->m_iRefIdx[ eRefPicList ] ]->m_iPOC ) {
			    rcMv = PbMVField0->m_acMv[ eRefPicList ]; 
			} else {
			    VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
		        VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
		        VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
		        VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

				VO_S32 MvHor = PbMVField0->m_acMv[ eRefPicList ] << 16 >> 16;
				VO_S32 MvVer = PbMVField0->m_acMv[ eRefPicList ] >> 16;

      			MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
				MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

				rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
			}
			*( pInfo + can_num++ ) = rcMv;
			availableFlagLXA = VO_TRUE;

	    } else if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList2nd ) ) && \
			( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList2nd][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ]->m_bIsLongTerm ) ) {
			VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC;
		    VO_S32 rcMv;
		  
			if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC ) {
			    rcMv = PbMVField0->m_acMv[ eRefPicList2nd ]; 
			} else {
			    VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
		        VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
		        VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
		        VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

				VO_S32 MvHor = PbMVField0->m_acMv[ eRefPicList2nd ] << 16 >> 16;
				VO_S32 MvVer = PbMVField0->m_acMv[ eRefPicList2nd ] >> 16;

      			MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
				MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

				rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
			}
			*( pInfo + can_num++ ) = rcMv;
			availableFlagLXA = VO_TRUE;
		}
	}

	if( available1 && availableFlagLXA == VO_FALSE) {
		if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList ) ) && \
			( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList][ PbMVField1->m_iRefIdx[ eRefPicList ] ]->m_bIsLongTerm ) ) {
			VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList ][ PbMVField1->m_iRefIdx[ eRefPicList ] ]->m_iPOC;
		    VO_S32 rcMv;
		  
			if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList ][ PbMVField1->m_iRefIdx[ eRefPicList ] ]->m_iPOC ) {
			    rcMv = PbMVField1->m_acMv[ eRefPicList ]; 
			} else {
			    VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
		        VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
		        VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
		        VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

				VO_S32 MvHor = PbMVField1->m_acMv[ eRefPicList ] << 16 >> 16;
				VO_S32 MvVer = PbMVField1->m_acMv[ eRefPicList ] >> 16;

      			MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
				MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

				rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
			}
			*( pInfo + can_num++ ) = rcMv;
			availableFlagLXA = VO_TRUE;

	    } else if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList2nd ) ) && \
			( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList2nd][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ]->m_bIsLongTerm ) ) {
			VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC;
		    VO_S32 rcMv;
		  
			if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC ) {
			    rcMv = PbMVField1->m_acMv[ eRefPicList2nd ]; 
			} else {
			    VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
		        VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
		        VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
		        VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

				VO_S32 MvHor = PbMVField1->m_acMv[ eRefPicList2nd ] << 16 >> 16;
				VO_S32 MvVer = PbMVField1->m_acMv[ eRefPicList2nd ] >> 16;

      			MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
				MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

				rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
			}
			*( pInfo + can_num++ ) = rcMv;
			availableFlagLXA = VO_TRUE;
		}
	}

	// Above predictor search
  //AR
	available0 = DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb + nPbW, yPb - 1, partIdx, &PbMVField0 );

	if ( available0 && availableFlagLXB == VO_FALSE) {
		if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList ][ PbMVField0->m_iRefIdx[ eRefPicList ] ] == iCurrRefPOC ) {
	  
			*( pInfo + can_num++ ) = PbMVField0->m_acMv[ eRefPicList ];  
			availableFlagLXB = VO_TRUE;
	    } else if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList2nd ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList2nd ][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ] == iCurrRefPOC ) {

			*( pInfo + can_num++ ) = PbMVField0->m_acMv[ eRefPicList2nd ];  
			availableFlagLXB = VO_TRUE;
		}
	}

	//A
	available1 = DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb + nPbW - 1, yPb - 1, partIdx, &PbMVField1 );
	if ( available1 && availableFlagLXB == VO_FALSE) {

		if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList ][ PbMVField1->m_iRefIdx[ eRefPicList ] ] == iCurrRefPOC ) {
	  
			*( pInfo + can_num++ ) = PbMVField1->m_acMv[ eRefPicList ];  
			availableFlagLXB = VO_TRUE;
	    } else if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList2nd ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList2nd ][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ] == iCurrRefPOC ) {

			*( pInfo + can_num++ ) = PbMVField1->m_acMv[ eRefPicList2nd ];  
			availableFlagLXB = VO_TRUE;
		}
	}

	//AL 
    //x_al = (xPb-1)>>2;
	//y_al = (yPb-1)>>2;
	available2 = DerivePUAvail( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, xPb - 1, yPb - 1, partIdx, &PbMVField2 );
	if ( available2 && availableFlagLXB == VO_FALSE) {

		if ( ( PbMVField2->inter_dir & ( 1 << eRefPicList ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList ][ PbMVField2->m_iRefIdx[ eRefPicList ] ] == iCurrRefPOC ) {
	  
			*( pInfo + can_num++ ) = PbMVField2->m_acMv[ eRefPicList ];  
			availableFlagLXB = VO_TRUE;
	    } else if ( ( PbMVField2->inter_dir & ( 1 << eRefPicList2nd ) ) &&
			( *( p_slice->ref_poc_list ) )[ eRefPicList2nd ][ PbMVField2->m_iRefIdx[ eRefPicList2nd ] ] == iCurrRefPOC ) {

			*( pInfo + can_num++ ) = PbMVField2->m_acMv[ eRefPicList2nd ];  
			availableFlagLXB = VO_TRUE;
		}
	}

	if ( isScaledFlagLX == VO_FALSE && availableFlagLXB == VO_TRUE ) {
		availableFlagLXA = VO_TRUE;
		availableFlagLXB = VO_FALSE;
	}

	if ( isScaledFlagLX == VO_FALSE && availableFlagLXB == VO_FALSE ) {
		if( available0 && availableFlagLXB == VO_FALSE) {

			if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList ) ) && \
				( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList][ PbMVField0->m_iRefIdx[ eRefPicList ] ]->m_bIsLongTerm ) ) {
				VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList ][ PbMVField0->m_iRefIdx[ eRefPicList ] ]->m_iPOC;
				VO_S32 rcMv;
		  
				if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList ][ PbMVField0->m_iRefIdx[ eRefPicList ] ]->m_iPOC ) {
					rcMv = PbMVField0->m_acMv[ eRefPicList ]; 
				} else {
					VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
					VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
					VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
					VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

					VO_S32 MvHor = PbMVField0->m_acMv[ eRefPicList ] << 16 >> 16;
					VO_S32 MvVer = PbMVField0->m_acMv[ eRefPicList ] >> 16;

      				MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
					MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

					rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
				}
				*( pInfo + can_num++ ) = rcMv;
				availableFlagLXB = VO_TRUE;

			} else if ( ( PbMVField0->inter_dir & ( 1 << eRefPicList2nd ) ) && \
				( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList2nd][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ]->m_bIsLongTerm ) ) {
				VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC;
				VO_S32 rcMv;
		  
				if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField0->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC ) {
					rcMv = PbMVField0->m_acMv[ eRefPicList2nd ]; 
				} else {
					VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
					VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
					VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
					VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

					VO_S32 MvHor = PbMVField0->m_acMv[ eRefPicList2nd ] << 16 >> 16;
					VO_S32 MvVer = PbMVField0->m_acMv[ eRefPicList2nd ] >> 16;

      				MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
					MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

					rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
				}
				*( pInfo + can_num++ ) = rcMv;
				availableFlagLXB = VO_TRUE;
			}
		}

		if( available1 && availableFlagLXB == VO_FALSE) {
			if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList ) ) && \
				( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList][ PbMVField1->m_iRefIdx[ eRefPicList ] ]->m_bIsLongTerm ) ) {
				VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList ][ PbMVField1->m_iRefIdx[ eRefPicList ] ]->m_iPOC;
				VO_S32 rcMv;
		  
				if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList ][ PbMVField1->m_iRefIdx[ eRefPicList ] ]->m_iPOC ) {
					rcMv = PbMVField1->m_acMv[ eRefPicList ]; 
				} else {
					VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
					VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
					VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
					VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

					VO_S32 MvHor = PbMVField1->m_acMv[ eRefPicList ] << 16 >> 16;
					VO_S32 MvVer = PbMVField1->m_acMv[ eRefPicList ] >> 16;

      				MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
					MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

					rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
				}
				*( pInfo + can_num++ ) = rcMv;
				availableFlagLXB = VO_TRUE;

			} else if ( ( PbMVField1->inter_dir & ( 1 << eRefPicList2nd ) ) && \
				( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList2nd][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ]->m_bIsLongTerm ) ) {
				VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC;
				VO_S32 rcMv;
		  
				if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField1->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC ) {
					rcMv = PbMVField1->m_acMv[ eRefPicList2nd ]; 
				} else {
					VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
					VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
					VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
					VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

					VO_S32 MvHor = PbMVField1->m_acMv[ eRefPicList2nd ] << 16 >> 16;
					VO_S32 MvVer = PbMVField1->m_acMv[ eRefPicList2nd ] >> 16;

      				MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
					MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

					rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
				}
				*( pInfo + can_num++ ) = rcMv;
				availableFlagLXB = VO_TRUE;
			}
		}

		if ( available2 && availableFlagLXB == VO_FALSE ) {

			if ( ( PbMVField2->inter_dir & ( 1 << eRefPicList ) ) && \
				( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList][ PbMVField2->m_iRefIdx[ eRefPicList ] ]->m_bIsLongTerm ) ) {
				VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList ][ PbMVField2->m_iRefIdx[ eRefPicList ] ]->m_iPOC;
				VO_S32 rcMv;
		  
				if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList ][ PbMVField2->m_iRefIdx[ eRefPicList ] ]->m_iPOC ) {
					rcMv = PbMVField2->m_acMv[ eRefPicList ]; 
				} else {
					VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
					VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
					VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
					VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

					VO_S32 MvHor = PbMVField2->m_acMv[ eRefPicList ] << 16 >> 16;
					VO_S32 MvVer = PbMVField2->m_acMv[ eRefPicList ] >> 16;

      				MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
					MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

					rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
				}
				*( pInfo + can_num++ ) = rcMv;
				availableFlagLXB = VO_TRUE;

			} else if ( ( PbMVField2->inter_dir & ( 1 << eRefPicList2nd ) ) && \
				( bIsCurrRefLongTerm == p_slice->ref_pic_list[ eRefPicList2nd][ PbMVField2->m_iRefIdx[ eRefPicList2nd ] ]->m_bIsLongTerm ) ) {
				VO_S32 iNeibRefPOC = p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField2->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC;
				VO_S32 rcMv;
		  
				if ( bIsCurrRefLongTerm || iCurrRefPOC == p_slice->ref_pic_list[ eRefPicList2nd ][ PbMVField2->m_iRefIdx[ eRefPicList2nd ] ]->m_iPOC ) {
					rcMv = PbMVField2->m_acMv[ eRefPicList2nd ]; 
				} else {
					VO_S32 iTDB      = Clip3( -128, 127, iCurrPOC - iCurrRefPOC );
					VO_S32 iTDD      = Clip3( -128, 127, iCurrPOC - iNeibRefPOC );
					VO_S32 iX        = ( 0x4000 + ABS( iTDD / 2 ) ) / iTDD;
					VO_S32 iScale    = Clip3( -4096, 4095, ( iTDB * iX + 32 ) >> 6 );

					VO_S32 MvHor = PbMVField2->m_acMv[ eRefPicList2nd ] << 16 >> 16;
					VO_S32 MvVer = PbMVField2->m_acMv[ eRefPicList2nd ] >> 16;

      				MvHor = (VO_S16)Clip3( -32768, 32767, (iScale * MvHor + 127 + (iScale * MvHor < 0 ) ) >> 8 );
					MvVer = (VO_S16)Clip3( -32768, 32767, (iScale * MvVer + 127 + (iScale * MvVer < 0 ) ) >> 8 );

					rcMv  = ( MvVer << 16 ) | ( MvHor & 0xFFFF );
				}
				*( pInfo + can_num++ ) = rcMv;
				availableFlagLXB = VO_TRUE;
			}
		}
	}

	if ( can_num == 2 ) {
		if ( *( pInfo ) == *( pInfo + 1 ) ) {
			can_num = 1;
		} else {
			return;
		}
	}

	//2. ----8.5.3.2.7	Derivation process for temporal luma motion vector prediction--
	if ( p_slice->enable_temporal_mvp_flag ) {
		TComPic *colPic = p_slice->ref_pic_list[ p_slice->slice_type == B_SLICE ? 1 - p_slice->collocated_from_l0_flag : 0 ][ p_slice->collocated_ref_idx ];
		VO_S32 cColMv;
		VO_BOOL avalColBr = VO_TRUE;

        if ( ( xPb + nPbW ) >= p_slice->p_sps->pic_width_in_luma_samples || 
			( yPb + nPbH ) >= p_slice->p_sps->pic_height_in_luma_samples ||
		    ( yPb >> p_slice->CtbLog2SizeY ) != ( ( yPb + nPbH ) >> p_slice->CtbLog2SizeY ) )
		    avalColBr = VO_FALSE;
#if USE_FRAME_THREAD
		if (p_slice->nThdNum > 1)
		{
// 			VO_S32 max_y = yP + nPSH - 1;
			WaitForProgress_mv(colPic, p_slice->m_uiCUAddr+avalColBr);
		}
#endif

		if ( avalColBr && \
			xGetColMVP( p_slice, 
			colPic, 
			eRefPicList, 
			&cColMv, 
			( ( xPb + nPbW ) & ~15 ) >> 2, 
			( ( yPb + nPbH ) & ~15 ) >> 2, 
			iRefIdx ) ) {
		    *( pInfo + can_num++ ) = cColMv;
		} else if( xGetColMVP( p_slice, 
			colPic, 
			eRefPicList,
			&cColMv, 
			( ( xPb + ( nPbW >> 1 ) ) & ~15 ) >> 2,
			( ( yPb + ( nPbH >> 1 ) ) & ~15 ) >> 2, 
			iRefIdx ) ) {
		    *( pInfo + can_num++ ) = cColMv;
		}
	}

	//---4.	The motion vector predictor list is modified as follows
	while ( can_num < AMVP_MAX_NUM_CANDS ) {
  		*( pInfo + can_num++ ) = 0;
	}
	return ;
}

#if 0
VO_S8  getRefQP( H265_DEC_SLICE *p_slice,
	VO_S32 x_4b,
	VO_S32 y_4b  )
{

//	H265_DEC_SLICE *p_slice = &p_dec->slice;
	//VO_U32 lPartIdx = 0, aPartIdx = 0;
	VO_S32  qPY_B = 0, qPY_A = 0;
	VO_U32 cULeft;//  = getQpMinCuLeft ( p_dec,&lPartIdx, p_slice->m_uiAbsIdxInLCU + uiCurrAbsIdxInLCU,VO_TRUE,VO_TRUE );
	VO_U32 cUAbove;// = getQpMinCuAbove( p_dec,&aPartIdx, p_slice->m_uiAbsIdxInLCU + uiCurrAbsIdxInLCU,VO_TRUE,VO_TRUE );
	//VO_S8 *slice_number;// = &p_slice->slice_number[y_4b*p_dec->picWidthInMin4bs+x_4b];
	//VO_U32 cULeft_T = (x_4b%(p_dec->m_uiMaxCUWidth>>2))!=0;
	const VO_U32 last_coded_qp = p_slice->last_coded_qp;

	const VO_U32 qPY_PREV = p_slice->QpY;

	x_4b &= ~((p_slice->m_uiMinCuDQPSize>>2)-1);
	y_4b &= ~((p_slice->m_uiMinCuDQPSize>>2)-1);
	cUAbove = (y_4b%(p_dec->m_uiMaxCUHeight>>2))!=0;

// 	if(x_4b==p_slice->tile_start_x&&!cUAbove)
// 	{
// 		if(p_slice->m_uiCUAddr ==0 || y_4b<=p_slice->tile_start_y
// 			|| ( p_dec->pps->tiles_or_entropy_coding_sync_idc&& p_slice->m_uiCUAddr % p_dec->m_uiWidthInCU == 0))
// 		{
// 			qPY_B = qPY_A = p_slice->slice_qp;
// 		}
// 	}  
	if ( cUAbove ) {
		qPY_A = p_slice->qp_buffer[(y_4b-1)*p_dec->picWidthInMin4bs+x_4b];
	} else if (x_4b==p_slice->tile_start_x) {
		if(p_slice->m_uiCUAddr ==0 || y_4b<=p_slice->tile_start_y
			|| ( p_dec->pps->tiles_or_entropy_coding_sync_idc&& p_slice->m_uiCUAddr % p_dec->m_uiWidthInCU == 0)) {
			qPY_B = qPY_A = p_slice->slice_qp;
			assert(qPY_B);
		}
	}

	if ( !qPY_A ) {
		qPY_A = last_coded_qp;
	}

	cULeft = (x_4b%(p_dec->m_uiMaxCUWidth>>2))!=0;
	if ( cULeft ) {
		qPY_B = p_slice->qp_buffer[y_4b*p_dec->picWidthInMin4bs+x_4b-1];
	}
	if ( !qPY_B ) {
		qPY_B = last_coded_qp;
	}


	return (VO_S8)((qPY_B+qPY_A+1)>>1);

// 	qPY_B = p_slice->qp_buffer[y_4b*p_dec->picWidthInMin4bs+x_4b-1]? p_slice->qp_buffer[y_4b*p_dec->picWidthInMin4bs+x_4b-1]:last_coded_qp;

// 	qPY_A = p_slice->qp_buffer[(y_4b-1)*p_dec->picWidthInMin4bs+x_4b]?p_slice->qp_buffer[(y_4b-1)*p_dec->picWidthInMin4bs+x_4b]:last_coded_qp;
	  
//     if(x_4b==p_slice->tile_start_x&&(y_4b%(p_dec->m_uiMaxCUHeight>>2))==0)
//     {
//       if(p_slice->m_uiCUAddr >0&&y_4b>p_slice->tile_start_y
// 	    && !( p_dec->pps->tiles_or_entropy_coding_sync_idc&& p_slice->m_uiCUAddr % p_dec->m_uiWidthInCU == 0))
// 		{
// 		  
// 		}
// 	  else
// 	  {
// 		last_coded_qp = p_slice->slice_qp;
// 	  }
// 
//     }  
// 	return (((cULeft ? qPY_B: last_coded_qp) + (cUAbove? qPY_A: last_coded_qp) + 1) >> 1);

}

#endif