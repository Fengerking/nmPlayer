 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/
/** \file     h265dec_sao.c
    \brief    Sample Adaptive Offset(SAO) processing module
    \author   Lina Lv
		\change
							1. 2012.12.06, using one test case(very basic, without NDB_FILTER and PCM LF ) , 
								 it has same output as HM-9.0rc1(disable deblock)
							2. clean warnings
*/



#include <stdlib.h>
#include <string.h>
#include "h265dec_sao.h"
#include "h265dec_utils.h"
#include "h265_decoder.h"


static const VO_U8 g_bitDepth = 8;

#define SAO_DEBUG 0  //if enable, will be very slowly. so just enable it when SAO almost has bug

#if SAO_DEBUG
int g_frame_cnt = 0;
#endif

// ====================================================================================================================
// Tables
// ====================================================================================================================
const VO_U32 m_auiEoTable[9] =
{
  1, //0    
  2, //1   
  0, //2
  3, //3
  4, //4
  0, //5  
  0, //6  
  0, //7 
  0
};

//map between SAO type and boundary 
//0x03 sands SAO_EO_0 only depends on SGU_L and SGU_R 
const VO_U32 SAOTypeBorderMap[4] = {0x03, 0xc, 0x9f, 0x6f};

// ====================================================================================================================
// Functions
// ====================================================================================================================
/** allocate memory for SAO parameters
 * \param    *pcSaoParam
 */
TComSampleAdaptiveOffset* allocSaoParam(VO_VOID *ph_dec)

{
  H265_DEC *p_dec = (H265_DEC *)ph_dec;
  TComSampleAdaptiveOffset* pcSaoParam = (TComSampleAdaptiveOffset*) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(TComSampleAdaptiveOffset), CACHE_LINE);
  H265_DEC_SPS *p_sps = p_dec->sps;		//Harry: should use sps_id to index, or use p_sps in slice
  VO_U32 uiPixelRange = 1 << g_bitDepth;  //lina , g_bitDepth too many definition
  VO_U32 uiBoRangeShift = g_bitDepth - SAO_BO_BITS;
  VO_U32 k2;
  //VO_U8 uiMaxY  = (VO_U8)(uiPixelRange - 1);  
  //VO_U32 uiCRangeExt = uiMaxY>>1;



  if(!p_dec) //Harry: Here is too late if dec is NULL
  {
    return NULL;
  }
  pcSaoParam->m_iPicWidth = p_sps->pic_width_in_luma_samples ;
  pcSaoParam->m_iPicHeight = p_sps->pic_height_in_luma_samples ;
  pcSaoParam->m_uiMaxCUWidth = 1<<p_sps->log2_luma_coding_block_size;  
  pcSaoParam->m_uiMaxCUHeight = 1<<p_sps->log2_luma_coding_block_size;

	pcSaoParam->numCuInWidth = ( p_sps->pic_width_in_luma_samples + pcSaoParam->m_uiMaxCUWidth - 1 ) >> p_sps->log2_luma_coding_block_size;
	pcSaoParam->numCuInHeight = ( p_sps->pic_height_in_luma_samples + pcSaoParam->m_uiMaxCUWidth - 1 ) >> p_sps->log2_luma_coding_block_size;

  pcSaoParam->saoLcuParam = (SaoLcuParam *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(SaoLcuParam)*(3*pcSaoParam->numCuInWidth*pcSaoParam->numCuInHeight), CACHE_LINE); 
  pcSaoParam->pSaoOffsetTable = (SaoOffsetTable *) AlignMalloc (p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(SaoOffsetTable)*(3*pcSaoParam->numCuInWidth*pcSaoParam->numCuInHeight+1), CACHE_LINE);
  pcSaoParam->pSaoOffsetTable++; 
  pcSaoParam->m_sliceIDMapLCU = (VO_U32 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U32)*pcSaoParam->numCuInWidth*pcSaoParam->numCuInHeight+1, CACHE_LINE);
 


  pcSaoParam->m_lumaTableBo = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*uiPixelRange, CACHE_LINE); 
  for (k2=0; k2<uiPixelRange; k2++)
  {
    pcSaoParam->m_lumaTableBo[k2] = (VO_U8)(1 + (k2>>uiBoRangeShift));
  }
 
  pcSaoParam->m_iUpBuff1All[0] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(2*p_sps->pic_width_in_luma_samples), CACHE_LINE);    
  pcSaoParam->m_iUpBuff1All[0] ++;
  pcSaoParam->m_iUpBuff1All[1] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(2*p_sps->pic_width_in_luma_samples), CACHE_LINE);   
  pcSaoParam->m_iUpBuff1All[1] ++;
  pcSaoParam->m_iUpBuff1All[2] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(2*p_sps->pic_width_in_luma_samples), CACHE_LINE);   
  pcSaoParam->m_iUpBuff1All[2] ++;   

  pcSaoParam->m_pTmpU[0] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(p_sps->pic_width_in_luma_samples+16)*3, CACHE_LINE);  //3 stands y, u,v
  pcSaoParam->m_pTmpU[1] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(p_sps->pic_width_in_luma_samples+16)*3, CACHE_LINE); 
 
  pcSaoParam->m_pTmpL[0] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(pcSaoParam->m_uiMaxCUHeight+64+68)*3, CACHE_LINE);  //3 stands y, u,v
  pcSaoParam->m_pTmpL[1] = (VO_U8 *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8)*(pcSaoParam->m_uiMaxCUHeight+64+68)*3, CACHE_LINE);  //+64, up, low part; +68, last LCU row

  pcSaoParam->m_vNDFBlock = (NDBFBlockInfo *) AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(NDBFBlockInfo)*((pcSaoParam->numCuInWidth)*(pcSaoParam->numCuInHeight)), CACHE_LINE); 
  SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_vNDFBlock,VO_TRUE,sizeof(NDBFBlockInfo)*((pcSaoParam->numCuInWidth)*(pcSaoParam->numCuInHeight)));
  
  pcSaoParam->m_bUseNIF = VO_FALSE;

  

  return pcSaoParam;
}



/** free memory of SAO parameters
 * \param   pcSaoParam
 */
VO_VOID freeSaoParam(H265_DEC *p_dec, TComSampleAdaptiveOffset *pcSaoParam)
{
  int i = 0;
	if(pcSaoParam==NULL){
		return;
	}

 
  if( pcSaoParam->pSaoOffsetTable)
  {
    pcSaoParam->pSaoOffsetTable--;
    AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->pSaoOffsetTable);
	  pcSaoParam->pSaoOffsetTable = NULL;
  }

 
  if (pcSaoParam->m_lumaTableBo)
  {
    AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_lumaTableBo);
	  pcSaoParam->m_lumaTableBo = NULL;
  }

  if( pcSaoParam->saoLcuParam) 
  {
    AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->saoLcuParam);
    pcSaoParam->saoLcuParam = NULL;
  }

  for(i=0; i<3; i++){

    if (pcSaoParam->m_iUpBuff1All[i])
    {
      pcSaoParam->m_iUpBuff1All[i]--;
      AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_iUpBuff1All[i]);
	    pcSaoParam->m_iUpBuff1All[i] = NULL;
    }
  }

  for(i=0; i<2; i++){

    if (pcSaoParam->m_pTmpU[i])
    {
      AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_pTmpU[i]);
      pcSaoParam->m_pTmpU[i] = NULL;
    }

    if (pcSaoParam->m_pTmpL[i])
    {
      AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_pTmpL[i]);
      pcSaoParam->m_pTmpL[i] = NULL;
    }
  }


  if(pcSaoParam->m_vNDFBlock){
    AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_vNDFBlock);
    pcSaoParam->m_vNDFBlock = NULL;
  } 
  if(pcSaoParam->m_sliceIDMapLCU){
    AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam->m_sliceIDMapLCU);
    pcSaoParam->m_sliceIDMapLCU = NULL;

  }
  if(pcSaoParam){
    AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pcSaoParam);
    pcSaoParam = NULL;
  }

} 

/** get the sign of input variable
 * \param   x
 */
VOINLINE VO_U8 xSign(VO_S32 x)
{
  if(x>0){
    return 2;
  }else if(x<0){
    return 0;
  }else{
    return 1;
  }  
}

VO_VOID setNDBBorderAvailabilityPerLCU(H265_DEC_SLICE *p_slice, TComSampleAdaptiveOffset* p_SAO, VO_U32 uiAddr, VO_U32 uiContainLastLCULine){
  //H265_DEC_PPS *pPPS = p_slice->p_pps;
  VO_U32 frameWidthInCU = p_SAO->numCuInWidth; 
  VO_U32 frameHeightInCU = p_SAO->numCuInHeight; 
  VO_U32 sliceID = 0, pRefID = 0;
  VO_BOOL *pbAvail;
  VO_BOOL *pbAvailBorder = NULL;
  VO_BOOL bPicRBoundary, bPicBBoundary, bPicTBoundary, bPicLBoundary;  
  //VO_BOOL onlyOneSliceInPic = p_SAO->m_uiNumSlicesInPic==1?VO_TRUE:VO_FALSE;

  pbAvailBorder = p_SAO->m_vNDFBlock[uiAddr].isBorderAvailable;
  sliceID = p_SAO->m_sliceIDMapLCU[uiAddr];
  bPicRBoundary = bPicBBoundary = bPicTBoundary = bPicLBoundary = VO_FALSE;

  //slice boundary. 
  //check picture boundary in this function, because only small amount of LCU need to do it

  //left
  if( uiAddr % frameWidthInCU == 0)
  {         
    bPicLBoundary = VO_TRUE;          
  }
  //right
  if( (uiAddr % frameWidthInCU) == (frameWidthInCU -1) )
  {          
    bPicRBoundary = VO_TRUE;         
  }
  //top
  if( uiAddr < frameWidthInCU)
  {          
    bPicTBoundary = VO_TRUE;     
  }
  //down
  if( uiAddr + frameWidthInCU >= frameWidthInCU*frameHeightInCU)
  {         
    bPicBBoundary = VO_TRUE;        
  }

  //     SGU_L
  pbAvail = &(pbAvailBorder[SGU_L]);
  if(!bPicLBoundary)
  {
    pRefID = p_SAO->m_sliceIDMapLCU[uiAddr-1];
    *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
  }

  //       SGU_R
  pbAvail = &(pbAvailBorder[SGU_R]);
  if(!bPicRBoundary)
  {    
    pRefID = p_SAO->m_sliceIDMapLCU[uiAddr+1];
    *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
  }

  if(p_SAO->uiLCURowStartY==0){
    //       SGU_T
    pbAvail = &(pbAvailBorder[SGU_T]);
    if(!bPicTBoundary)
    {    
      pRefID = p_SAO->m_sliceIDMapLCU[uiAddr-frameWidthInCU];
      *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
    }

    //       SGU_TL
    pbAvail = &(pbAvailBorder[SGU_TL]);
    if(!bPicTBoundary && !bPicLBoundary)
    {
      pRefID = p_SAO->m_sliceIDMapLCU[uiAddr-frameWidthInCU-1];
      *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
    }

    //       SGU_TR
    pbAvail = &(pbAvailBorder[SGU_TR]);
    if(!bPicTBoundary && !bPicRBoundary)
    {
      pRefID = p_SAO->m_sliceIDMapLCU[uiAddr-frameWidthInCU+1];
      *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
    }
  } 

  if(uiContainLastLCULine){
    //       SGU_B
    pbAvail = &(pbAvailBorder[SGU_B]);
    if(!bPicBBoundary)
    {
      pRefID = p_SAO->m_sliceIDMapLCU[uiAddr+frameWidthInCU];
      *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
    }

    //       SGU_BL
    pbAvail = &(pbAvailBorder[SGU_BL]);
    if(!bPicBBoundary && !bPicLBoundary)
    {
      pRefID = p_SAO->m_sliceIDMapLCU[uiAddr+frameWidthInCU-1];
      *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
    }

    //       SGU_BR
    pbAvail = &(pbAvailBorder[SGU_BR]);
    if(!bPicBBoundary && !bPicRBoundary)
    {   
      pRefID = p_SAO->m_sliceIDMapLCU[uiAddr+frameWidthInCU+1];
      *pbAvail = (VO_BOOL)( (*pbAvail) & ((pRefID == sliceID)?(VO_TRUE):(VO_FALSE)) );
    }
  }
}

VO_VOID storeBoundary(H265_DEC_SLICE *p_slice,TComSampleAdaptiveOffset* pSAO, VO_U32 iAddr, VO_S32 iSaoType, VO_S32 iYCbCr, VO_U8 *left, VO_U8 *top, VO_U32 curBlockWidth, VO_U32 uiUBufOffset, VO_U32 uiLBufOffset){
  VO_BOOL *pbBorderAvail = pSAO->m_vNDFBlock[iAddr].isBorderAvailable;
  VO_U8  *right, *bottom;
  VO_U8  *pRec       = pSAO->pLCURec;
  VO_U32 i;
  VO_U32 uiCurLCURowStartY, uiCurLCURowEndY;  
  VO_U32 uiStride   = pSAO->uiLCUStride; 
  VO_U32 uiContainLastLCULine = 0;
  VO_U32 isLastLCURow = iAddr/pSAO->numCuInWidth==pSAO->numCuInHeight-1?1:0;
  VO_S32 iIsChroma = (iYCbCr == 0) ? 0:1;
  uiCurLCURowStartY = pSAO->uiLCURowStartY >> iIsChroma;
  uiCurLCURowEndY = pSAO->uiLCURowEndY >> iIsChroma;  

  if(pSAO->uiLCURowEndY > pSAO->m_uiMaxCUHeight -4){
    uiContainLastLCULine = 1;
  }else if(pSAO->uiLCURowEndY==pSAO->m_iPicHeight-(iAddr/pSAO->numCuInWidth)*pSAO->m_uiMaxCUHeight){
    uiContainLastLCULine = 1;
  }

  if(!p_slice->p_pps->pps_loop_filter_across_slices_enabled_flag){
    setNDBBorderAvailabilityPerLCU(p_slice, pSAO, iAddr, uiContainLastLCULine);
  }

  if(pSAO->uiLCURowStartY==0){
    //can NOT merge TL, TR into T
    if(!pbBorderAvail[SGU_TL]){
      pRec       = pSAO->pLCURec;
      top[0]     = pRec[0];      
    }

    if(!pbBorderAvail[SGU_T]){
      pRec       = pSAO->pLCURec; 
      CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, top, pRec, sizeof(VO_U8)*(curBlockWidth));
    }

    if(!pbBorderAvail[SGU_TR]){
      pRec       = pSAO->pLCURec;
      top[curBlockWidth-1] = pRec[curBlockWidth-1];      
    }
  }

  if(!pbBorderAvail[SGU_L]){
    pRec       = pSAO->pLCURec ;  
    for (i=uiCurLCURowStartY;i<uiCurLCURowEndY;i++)
    {
      left[i] = pRec[0];
      pRec += uiStride;
    }
  }

  if(!pbBorderAvail[SGU_R]){
    if(!pSAO->isNeedCopyL){                //in this case, caller will not copy L buffer    
      right = pSAO->m_pTmpL[1-pSAO->m_CurLeftIndex]+uiLBufOffset;
      pRec       = pSAO->pLCURec + curBlockWidth -1;  
      for (i=uiCurLCURowStartY;i<uiCurLCURowEndY;i++)
      {
        right[i] = pRec[0];
        pRec += uiStride;
      }
    }
  }

  if(uiContainLastLCULine&&isLastLCURow){    //if last LCU row, caller will not copy U buffer 
    //can NOT merge SGU_BL,  SGU_BR into SGU_B
    if(!pbBorderAvail[SGU_BL]){
      bottom = pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]];
      pRec       = pSAO->pLCURec;
      pRec      += uiStride*(uiCurLCURowEndY - uiCurLCURowStartY -1);
      bottom[0]    = pRec[0];      
    }

    if(!pbBorderAvail[SGU_B]){
     
      bottom = pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]] + uiUBufOffset;
      pRec       = pSAO->pLCURec; 
      pRec      += uiStride*(uiCurLCURowEndY - uiCurLCURowStartY -1);
       CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, bottom, pRec, sizeof(VO_U8)*(curBlockWidth));
    }

    if(!pbBorderAvail[SGU_BR]){
      bottom = pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]];
      pRec       = pSAO->pLCURec;
      pRec      += uiStride*(uiCurLCURowEndY - uiCurLCURowStartY -1);
      bottom[curBlockWidth-1] = pRec[curBlockWidth-1];      
    }
  }
}

VO_VOID restoreBoundary(H265_DEC_SLICE *p_slice,TComSampleAdaptiveOffset* pSAO, VO_U32 iAddr, VO_S32 iSaoType, VO_S32 iYCbCr, VO_U8 *left, VO_U8 *top, VO_U32 curBlockWidth, VO_U32 uiUBufOffset, VO_U32 uiLBufOffset){
  VO_BOOL *pbBorderAvail = pSAO->m_vNDFBlock[iAddr].isBorderAvailable;  
  VO_U8  *right, *bottom;
  VO_U8  *pRec       = pSAO->pLCURec;
  VO_U32 i;
  VO_U32 uiCurLCURowStartY, uiCurLCURowEndY;
  VO_U32 uiStride   = pSAO->uiLCUStride;
  VO_U32 uiContainLastLCULine = 0;
  //VO_U32 isLastLCURow = iAddr/pSAO->numCuInWidth==pSAO->numCuInHeight-1?1:0;
  VO_S32 iIsChroma = (iYCbCr == 0) ? 0:1;
  VO_U32 uiTLMask=0, uiTRMask=0, uiBLMask=0, uiBRMask=0;   //must set as 0

  uiCurLCURowStartY = pSAO->uiLCURowStartY >> iIsChroma;
  uiCurLCURowEndY = pSAO->uiLCURowEndY >> iIsChroma;

  if(pSAO->uiLCURowEndY > pSAO->m_uiMaxCUHeight -4){
    uiContainLastLCULine = 1;
  }else if(pSAO->uiLCURowEndY==pSAO->m_iPicHeight-(iAddr/pSAO->numCuInWidth)*pSAO->m_uiMaxCUHeight){
    uiContainLastLCULine = 1;
  }

  if(pSAO->uiLCURowStartY==0){
    uiTLMask   = ( SAOTypeBorderMap[iSaoType] & (1<<SGU_TL) ) >> SGU_TL;
    if(!pbBorderAvail[SGU_TL] && uiTLMask ){
      pRec       = pSAO->pLCURec;
      pRec[0]    = top[0];         
    }

    uiTRMask   = ( SAOTypeBorderMap[iSaoType] & (1<<SGU_TR) )  >> SGU_TR;
    if(!pbBorderAvail[SGU_TR] && uiTRMask ){
      pRec       = pSAO->pLCURec;
      pRec[curBlockWidth-1] = top[curBlockWidth-1];
    }

    if(!pbBorderAvail[SGU_T]  &&  ( SAOTypeBorderMap[iSaoType] & (1<<SGU_T) )  ){
      pRec       = pSAO->pLCURec; 
      CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, &pRec[uiTLMask], &top[uiTLMask], sizeof(VO_U8)*(curBlockWidth-uiTLMask-uiTRMask));//cur TL, TR
    }

  }

  if(uiContainLastLCULine){
    uiBLMask   = ( SAOTypeBorderMap[iSaoType] & (1<<SGU_BL) ) >>SGU_BL ;
    if(!pbBorderAvail[SGU_BL] && uiBLMask ){
      bottom = pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]]+uiUBufOffset;
      pRec       = pSAO->pLCURec;
      pRec      += uiStride*(uiCurLCURowEndY - uiCurLCURowStartY -1);
      pRec[0]    = bottom[0];
    } 
    uiBRMask   = ( SAOTypeBorderMap[iSaoType] & (1<<SGU_BR) )>>SGU_BR;
    if(!pbBorderAvail[SGU_BR] && uiBRMask ){
      bottom = pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]]+uiUBufOffset;
      pRec       = pSAO->pLCURec;
      pRec      += uiStride*(uiCurLCURowEndY - uiCurLCURowStartY -1);
      pRec[curBlockWidth-1] = bottom[curBlockWidth-1]; 

    }
    if(!pbBorderAvail[SGU_B] && ( SAOTypeBorderMap[iSaoType] & (1<<SGU_B) )  ){
      bottom = pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]]+uiUBufOffset;
      pRec       = pSAO->pLCURec;
      pRec      += uiStride*(uiCurLCURowEndY - uiCurLCURowStartY -1);
      CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, &pRec[uiBLMask], &bottom[uiBLMask], sizeof(VO_U8)*(curBlockWidth-uiBLMask-uiBRMask));//cur TL, TR
    }
  }

  if(!pbBorderAvail[SGU_L] && ( SAOTypeBorderMap[iSaoType] & (1<<SGU_L) )  ){

    pRec       = pSAO->pLCURec;     
    pRec += (uiTLMask*uiStride);      
    for (i=uiCurLCURowStartY+uiTLMask;i<uiCurLCURowEndY-uiBLMask;i++)
    {
      pRec[0] = left[i];
      pRec += uiStride;
    }     
  }

  if(!pbBorderAvail[SGU_R] && ( SAOTypeBorderMap[iSaoType] & (1<<SGU_R) )  ){
    pRec       = pSAO->pLCURec+curBlockWidth-1;     
    pRec += (uiTRMask*uiStride);    
    right = pSAO->m_pTmpL[1-pSAO->m_CurLeftIndex]+uiLBufOffset;
    for (i=uiCurLCURowStartY+uiTRMask;i<uiCurLCURowEndY-uiBRMask;i++)
    {
      pRec[0] = right[i];
      pRec += uiStride;
    } 
  }  
}


/** sample adaptive offset process for one LCU
 * \param   iAddr, iSaoType, iYCbCr
 */
VO_VOID processSao(VO_VOID *phSlice,TComSampleAdaptiveOffset* pSAO, VO_S32 iYCbCr, VO_S32 iAddr)
{
	H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)phSlice;  

  TComPic *p_pic = pSlice->cur_pic;
  SaoLcuParam* pCurLcuSAOParam = &pSAO->saoLcuParam[iAddr];
  SaoLcuParam* pNextLcuSAOParam = NULL;

  VO_U8  left[MAX_LCU_SIZE], top[MAX_LCU_SIZE];
  VO_S32 iIsChroma = (iYCbCr == 0) ? 0:1;
  VO_S32 isUpPart = (pSAO->uiLCURowStartY == 0) ? 1:0; 
  VO_U32 idxXLCU = iAddr%pSAO->numCuInWidth;
  VO_S32 idxYLCU = iAddr/pSAO->numCuInWidth;

  VO_U8  *pRec;
  VO_U32 uiPicWidthTmp = 1;  //not last LCU column , copy extra one pixel
  VO_U32  i;
  VO_S32  iSAOType;

  VO_U32  uiStartX, uiEndX;

  VO_U32  uiStride;
  VO_U32  uiRecOffset;
 
  VO_U32  uiUBufOffset, uiLBufOffset;

  VO_U32 uiIsLastLcuRow = idxYLCU==(pSAO->numCuInHeight-1)?1:0;

  volatile VO_S32 offset[LUMA_GROUP_NUM+1];  //volatile is must, otherwise, SAO ASM crash. because offset will not be set 0
 

  uiStartX = pSAO->m_uiMaxCUWidth * idxXLCU;
  uiEndX = uiStartX + pSAO->m_uiMaxCUWidth;
  if(uiEndX>pSAO->m_iPicWidth){
    uiEndX = pSAO->m_iPicWidth;
    uiPicWidthTmp = 0;
  }

	uiStartX>>=iIsChroma;
	uiEndX>>=iIsChroma;
	uiStride = p_pic->pic_stride[iYCbCr];
	uiRecOffset = (uiStride*(pSAO->m_uiMaxCUHeight*idxYLCU+pSAO->uiLCURowStartY)>>iIsChroma)+uiStartX;
	pRec = p_pic->pic_buf[iYCbCr] + uiRecOffset; 

  pSAO->pLCURec         = pRec;
  pSAO->uiLCUStride     = uiStride;

  //copy U buffer 
  uiUBufOffset = (pSAO->m_iPicWidth+16)*iYCbCr + uiStartX;
  uiPicWidthTmp += (uiEndX - uiStartX);  
  if(pSAO->uiLCURowStartY==0){
    if(idxYLCU==0){      
      pSAO->m_pTmpUTopIndex[iYCbCr] = 0;
      CopyMem(pSlice->p_user_op_all->p_user_op, pSlice->p_user_op_all->codec_id, &pSAO->m_pTmpU[pSAO->m_pTmpUTopIndex[iYCbCr]][uiUBufOffset], pRec, sizeof(VO_U8)*uiPicWidthTmp);
    }
  }
  else
  {
    pRec+=(uiStride*(((pSAO->uiLCURowEndY - pSAO->uiLCURowStartY)>>iIsChroma)-1));     
    {
       CopyMem(pSlice->p_user_op_all->p_user_op, pSlice->p_user_op_all->codec_id, &pSAO->m_pTmpU[1-pSAO->m_pTmpUTopIndex[iYCbCr]][uiUBufOffset], pRec, sizeof(VO_U8)*(uiPicWidthTmp));
    }
    pRec = pSAO->pLCURec;
  }

  //copy L buffer 
  uiLBufOffset = (pSAO->m_uiMaxCUHeight+64+68)*iYCbCr + ((pSAO->uiLCURowStartY) >> iIsChroma) + (uiIsLastLcuRow?(pSAO->m_uiMaxCUHeight+64):0);
  if(uiStartX==0){ //only copy L1 when firstly process this LCU row 
    pSAO->m_pTmpLLeftIndex[2*iYCbCr+isUpPart] = 0;
    for (i=pSAO->uiLCURowStartY>>iIsChroma;i<(pSAO->uiLCURowEndY>>iIsChroma)+1;i++)
    {
      pSAO->m_pTmpL[0][i+uiLBufOffset] = pRec[0];
      pRec+=uiStride;
    }
  }
  pSAO->m_CurLeftIndex = pSAO->m_pTmpLLeftIndex[2*iYCbCr+isUpPart];
  pSAO->isNeedCopyL = (idxXLCU==pSAO->numCuInWidth-1) ? VO_FALSE : VO_TRUE; //last column  
  if(pSAO->isNeedCopyL){
    pNextLcuSAOParam = pCurLcuSAOParam+1;
    if(pNextLcuSAOParam->sao_type_idx[iYCbCr]<0 || pNextLcuSAOParam->sao_type_idx[iYCbCr]==SAO_EO_1 || pNextLcuSAOParam->sao_type_idx[iYCbCr]==SAO_BO){
      pSAO->isNeedCopyL = VO_FALSE;
    } 
  } 
  if (pSAO->isNeedCopyL)
  {
    VO_S32 widthShift; 
    pRec = pSAO->pLCURec;       
    widthShift = (pSAO->m_uiMaxCUWidth>>iIsChroma)-1;
    for (i=pSAO->uiLCURowStartY>>iIsChroma;i<(pSAO->uiLCURowEndY>>iIsChroma)+1;i++)
    {      
      pSAO->m_pTmpL[1-pSAO->m_CurLeftIndex][i+uiLBufOffset] = pRec[widthShift];
      pRec+=uiStride;
    }
  }

  //check SAO and do SAO if needed
  iSAOType = pCurLcuSAOParam->sao_type_idx[iYCbCr]; 
  if (iSAOType>=0)
  {
    offset[0] = 0;
    pSAO->m_iUpBuff1 = pSAO->m_iUpBuff1All[iYCbCr];   
    pSAO->pSaoOffsetTableLCU = &pSAO->pSaoOffsetTable[(3*idxYLCU+iYCbCr)*pSAO->numCuInWidth+idxXLCU];
    pSAO->uiLCULPelX         = idxXLCU * pSAO->m_uiMaxCUWidth;
    pSAO->uiLCUTPelY         = idxYLCU * pSAO->m_uiMaxCUHeight + pSAO->uiLCURowStartY;   
    //if(pCurLcuSAOParam->mergeLeftFlag){
    //  SaoLcuParam *pLastLcuSAOParam = pCurLcuSAOParam -1;       
    //  pSAO->pSaoOffsetTableLCU = pSAO->pSaoOffsetTableLCU-1;
    //  //merge case: move left until the one that is not merge case
    //  while(pLastLcuSAOParam->mergeLeftFlag){
    //    pSAO->pSaoOffsetTableLCU = pSAO->pSaoOffsetTableLCU-1;
    //    pLastLcuSAOParam = pLastLcuSAOParam -1;
    //  }
    //}else
		{
      if(pSAO->uiLCURowStartY==0)
      {
        if (iSAOType == SAO_BO)
        {
          VO_U8* ppLumaTable = NULL;
          for (i=0; i<SAO_MAX_BO_CLASSES+1;i++)
          {
            offset[i] = 0;
          }
          for (i=0; i<4; i++)
          {
            offset[ (pCurLcuSAOParam->sao_band_position[iYCbCr] +i)%SAO_MAX_BO_CLASSES + 1] = pCurLcuSAOParam->sao_offset_abs[iYCbCr][i] ;
          }

          ppLumaTable = pSAO->m_lumaTableBo;

          for (i=0;i<(VO_U32)(1<<g_bitDepth);i++)
          {
            pSAO->pSaoOffsetTableLCU->m_iOffsetBo[i] = (VO_U8)Clip((VO_S32)(i + offset[ppLumaTable[i]]));
          }

        }else{
          //must be edge offset
          pSAO->pSaoOffsetTableLCU->m_iOffsetEo[0]= pCurLcuSAOParam->sao_offset_abs[iYCbCr][0];
          pSAO->pSaoOffsetTableLCU->m_iOffsetEo[1]= pCurLcuSAOParam->sao_offset_abs[iYCbCr][1];
          pSAO->pSaoOffsetTableLCU->m_iOffsetEo[2]= 0;
          pSAO->pSaoOffsetTableLCU->m_iOffsetEo[3]= -pCurLcuSAOParam->sao_offset_abs[iYCbCr][2];
          pSAO->pSaoOffsetTableLCU->m_iOffsetEo[4]= -pCurLcuSAOParam->sao_offset_abs[iYCbCr][3];
        }
      }//end of if(pSAO->uiLCURowStartY==0)
    }//end of  if (!mergeLeftFlag  ) 

   
    if(pSAO->m_bUseNIF&&iSAOType!=SAO_BO)
    {    
      storeBoundary(pSlice, pSAO, iAddr, iSAOType, iYCbCr, left, top, uiEndX-uiStartX, uiUBufOffset, uiLBufOffset);
    } 

    processSaoBlock(pSAO, iSAOType, (iYCbCr!=0)? 1:0, &pSAO->m_pTmpU[pSAO->m_pTmpUTopIndex[iYCbCr]][(pSAO->m_iPicWidth+16)*iYCbCr],&pSAO->m_pTmpL[pSAO->m_CurLeftIndex][uiLBufOffset]);

    if(pSAO->m_bUseNIF&&iSAOType!=SAO_BO)
    {    
      restoreBoundary(pSlice, pSAO, iAddr, iSAOType, iYCbCr, left, top, uiEndX-uiStartX, uiUBufOffset, uiLBufOffset);
    }

  }//end of  if one LCU SAO done  
 
 
  pSAO->m_pTmpLLeftIndex[2*iYCbCr+isUpPart] = 1 - pSAO->m_pTmpLLeftIndex[2*iYCbCr+isUpPart];
  if(idxYLCU!=0||pSAO->uiLCURowStartY>0)
  { 
    pSAO->m_pTmpUTopIndex[iYCbCr] = 1 - pSAO->m_pTmpUTopIndex[iYCbCr];
  }

}

#if !SAO_ASM_ENABLED
VO_VOID processSaoEO0(VO_S32 iStartX, VO_S32 uiCurLCURowStartY, VO_S32 iEndX, VO_S32 uiCurLCURowEndY, VO_U8* pRec, VO_U32 uiStride, VO_U8 *pTmpL, VO_S8 *iOffsetEo){

  VO_S32 x,y;
  VO_U32 uiEdgeType;
  VO_U8 iSignLeft, iSignRight ;
  for (y=uiCurLCURowStartY; y<uiCurLCURowEndY; y++)
  {
    iSignLeft = xSign(pRec[iStartX] - pTmpL[y]);
    for (x=iStartX; x< iEndX; x++)
    {
      iSignRight =  xSign(pRec[x] - pRec[x+1]); 
      uiEdgeType =  iSignRight + iSignLeft ;
      iSignLeft  = 2 - iSignRight;

      pRec[x] = Clip(pRec[x] + iOffsetEo[uiEdgeType]);
    }
    pRec += uiStride;
  }

}

VO_VOID processSaoEO1(VO_S32 iLcuWidth, VO_S32 iStartY, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, VO_U8 *pTmpBuff1, VO_S8 *iOffsetEo){

  VO_S32 x,y;
  VO_U32 uiEdgeType;
  VO_U8 iSignDown;
  for (y=iStartY; y<iEndY; y++)
  {
    for (x=0; x<iLcuWidth; x++)
    {
      iSignDown  = xSign(pRec[x] - pRec[x+uiStride]); 
      uiEdgeType = iSignDown + pTmpBuff1[x] ;
      pTmpBuff1[x]= 2 - iSignDown;

      pRec[x] = Clip(pRec[x] + iOffsetEo[uiEdgeType]);
    }
    pRec += uiStride;
  }

}

VO_VOID processSaoEO2(VO_S32 iStartX, VO_S32 iStartY, VO_S32 iEndX, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo){

  VO_S32 x,y;
  VO_U32 uiEdgeType;
  VO_U8 iSignDown1, iSignDown2;
  VO_U8 previous=0;
  for (y=iStartY; y<iEndY; y++)
  {

    for (x=iStartX; x<iEndX; x++)
    {        

      iSignDown1      =  xSign(pRec[x] - pRec[x+uiStride+1]) ;
      uiEdgeType      =  iSignDown1 + pTmpBuff1[x] ;
      pTmpBuff1[x] = previous;
      previous = 2 - iSignDown1; 
      pRec[x] = Clip(pRec[x] + iOffsetEo[uiEdgeType]);
    }
    iSignDown2 = xSign(pRec[uiStride+iStartX] - pTmpL[y]);
    pTmpBuff1[iStartX] = iSignDown2;
    pRec += uiStride;
  }

}

VO_VOID processSaoEO3(VO_S32 iStartX, VO_S32 iStartY, VO_S32 iEndX, VO_S32 iEndY, VO_U8* pRec, VO_U32 uiStride, VO_U8 *pTmpBuff1, VO_U8 *pTmpL, VO_S8 *iOffsetEo){

  VO_S32 x,y;
  VO_U32 uiEdgeType;
  VO_U8 iSignDown1;
  for (y=iStartY; y<iEndY; y++)
  {
    x=iStartX;
    iSignDown1      =  xSign(pRec[x] - pTmpL[y+1]) ;
    uiEdgeType      =  iSignDown1 + pTmpBuff1[x] ;  
    pRec[x] = Clip(pRec[x] + iOffsetEo[uiEdgeType]);
    for (x=iStartX+1; x<iEndX; x++)
    {
      iSignDown1      =  xSign(pRec[x] - pRec[x+uiStride-1]) ;
      uiEdgeType      =  iSignDown1 + pTmpBuff1[x] ;
      pTmpBuff1[x-1] = 2 - iSignDown1; 
      pRec[x] = Clip(pRec[x] + iOffsetEo[uiEdgeType]);
    }
    pTmpBuff1[iEndX-1] = xSign(pRec[iEndX-1 + uiStride] - pRec[iEndX]);
    pRec += uiStride;
  } 
}

VO_VOID processSaoBo(VO_S32 iLcuWidth, VO_S32 uiCurLCURowStartY, VO_S32 uiCurLCURowEndY, VO_U8* pRec, VO_U32 uiStride, VO_U8 *iOffsetBo){
  VO_S32 x,y;
  for (y=uiCurLCURowStartY; y<uiCurLCURowEndY; y++)
  {
    for (x=0; x<iLcuWidth; x++)
    {
      pRec[x] = iOffsetBo[pRec[x]];
    }
    pRec += uiStride;
  }
}
#endif

/** sample adaptive offset process for one LCU crossing LCU boundary
 * \param   iAddr, iSaoType, iYCbCr
 */
VO_VOID processSaoBlock(TComSampleAdaptiveOffset *pSAO, VO_S32 iSaoType, VO_S32 iIsChroma, VO_U8 *pTmpU, VO_U8 *pTmpL)
{
//  VO_S32 iCuHeightTmp;  

  VO_S32 x;
  //VO_S32 i;


  VO_U8* pRec       = pSAO->pLCURec;
  VO_U32 uiStride   = pSAO->uiLCUStride;
  VO_S32 iLcuWidth  = pSAO->m_uiMaxCUWidth;


  VO_U32 iPicWidthTmp;
  VO_U32 iPicHeightTmp;


  VO_U32 uiCurLCURowStartY, uiCurLCURowEndY;
  VO_U32 uiLPelX    = pSAO->uiLCULPelX;
  VO_U32 uiTPelY    = pSAO->uiLCUTPelY;
  VO_U32 uiRPelX;
  VO_U32 uiBPelY;

  VO_U8 *pTmpBuff1; 

  iPicWidthTmp  = pSAO->m_iPicWidth  >> iIsChroma;
  iPicHeightTmp = pSAO->m_iPicHeight >> iIsChroma;

  iLcuWidth     = iLcuWidth    >> iIsChroma; 
  uiLPelX       = uiLPelX      >> iIsChroma;
  uiTPelY       = uiTPelY      >> iIsChroma;
  uiCurLCURowStartY = pSAO->uiLCURowStartY >> iIsChroma;
  uiCurLCURowEndY = pSAO->uiLCURowEndY >> iIsChroma;

  uiRPelX       = uiLPelX + iLcuWidth  ;
  uiBPelY       = uiTPelY + (uiCurLCURowEndY-uiCurLCURowStartY) ;
  uiRPelX       = uiRPelX > iPicWidthTmp  ? iPicWidthTmp  : uiRPelX;
  iLcuWidth     = uiRPelX - uiLPelX;

  pTmpU+=uiLPelX;

 

  switch (iSaoType)
  {
  case SAO_EO_0: // dir: -
    {
      VO_S32 iStartX,iEndX;  //x=iStartX-1, type must VO_S32  

      iStartX = (uiLPelX == 0) ? 1 : 0;
      iEndX   = (uiRPelX == iPicWidthTmp) ? iLcuWidth-1 : iLcuWidth;  ///not equal to uiRPelX - uiLPelX;
#if SAO_ASM_ENABLED
      processSaoEO0_asm(iStartX,uiCurLCURowStartY,iEndX,uiCurLCURowEndY,pRec,uiStride,pTmpL,pSAO->pSaoOffsetTableLCU->m_iOffsetEo);
#else
      processSaoEO0(iStartX,uiCurLCURowStartY,iEndX,uiCurLCURowEndY,pRec,uiStride,pTmpL,pSAO->pSaoOffsetTableLCU->m_iOffsetEo);
#endif  
      break;
    }

  case SAO_EO_1: // dir: |
    {
      VO_S32 iStartY,  iEndY;  //x=iStartX-1, type must VO_S32

 
      pTmpBuff1 = &(pSAO->m_iUpBuff1[2*pSAO->uiLCULPelX]);

      iStartY = (uiTPelY == 0) ? 1 : ((pSAO->uiLCURowStartY==0) ? 0 : uiCurLCURowStartY);
      if (uiTPelY == 0)
      {
        pRec += uiStride;
      }
      iEndY   = (uiBPelY == iPicHeightTmp) ? uiCurLCURowEndY-1 : uiCurLCURowEndY;

      if(pSAO->uiLCURowStartY==0){
        for (x=0; x< iLcuWidth; x++)
        {
          pTmpBuff1[x] = xSign(pRec[x] - pTmpU[x]);
        }
      }
#if SAO_ASM_ENABLED
      processSaoEO1_asm(iLcuWidth,iStartY,iEndY,pRec,uiStride,pTmpBuff1,pSAO->pSaoOffsetTableLCU->m_iOffsetEo); 
#else
      processSaoEO1(iLcuWidth,iStartY,iEndY,pRec,uiStride,pTmpBuff1,pSAO->pSaoOffsetTableLCU->m_iOffsetEo);
#endif
      break;
    }

  case SAO_EO_2: // dir: 135
    {

      VO_S32 iStartX,iStartY, iEndX, iEndY;  //x=iStartX-1, type must VO_S32 

    
      pTmpBuff1 = &(pSAO->m_iUpBuff1[2*pSAO->uiLCULPelX]);

      iStartX = (uiLPelX == 0)            ? 1 : 0;
      iEndX   = (uiRPelX == iPicWidthTmp) ? iLcuWidth-1 : iLcuWidth;

      iStartY = (uiTPelY == 0) ? 1 : ((pSAO->uiLCURowStartY==0) ? 0 : uiCurLCURowStartY);
      if (iStartY == 1)
      {
        pRec += uiStride;
      }    
      iEndY   = (uiBPelY == iPicHeightTmp) ? uiCurLCURowEndY-1 : uiCurLCURowEndY;

      if(pSAO->uiLCURowStartY==0){
        for (x=iStartX; x<iEndX; x++)
        {
          pTmpBuff1[x] = xSign(pRec[x] - pTmpU[x-1]);
        }
      } 
#if SAO_ASM_ENABLED
      processSaoEO2_asm(iStartX,iStartY,iEndX,iEndY,pRec,uiStride,pTmpBuff1,pTmpL,pSAO->pSaoOffsetTableLCU->m_iOffsetEo); 
#else
      processSaoEO2(iStartX,iStartY,iEndX,iEndY,pRec,uiStride,pTmpBuff1,pTmpL,pSAO->pSaoOffsetTableLCU->m_iOffsetEo); 
#endif
      break;
    } 

  case SAO_EO_3: // dir: 45
    {
      VO_S32 iStartX,iStartY, iEndX, iEndY;  //x=iStartX-1, type must VO_S32    

    
      pTmpBuff1 = &(pSAO->m_iUpBuff1[2*pSAO->uiLCULPelX]);

      iStartX = (uiLPelX == 0) ? 1 : 0;
      iEndX   = (uiRPelX == iPicWidthTmp) ? iLcuWidth-1 : iLcuWidth;

      iStartY = (uiTPelY == 0) ? 1 : ((pSAO->uiLCURowStartY==0) ? 0 : uiCurLCURowStartY);
      if (iStartY == 1)
      {
        pRec += uiStride;
      }
      iEndY   = (uiBPelY == iPicHeightTmp) ? uiCurLCURowEndY-1 : uiCurLCURowEndY;

      if(pSAO->uiLCURowStartY==0){
        for (x=iStartX; x<iEndX; x++)
        {
          pTmpBuff1[x] = xSign(pRec[x] - pTmpU[x+1]);
        }
      }
#if SAO_ASM_ENABLED   
      processSaoEO3_asm(iStartX,iStartY,iEndX,iEndY,pRec,uiStride,pTmpBuff1,pTmpL,pSAO->pSaoOffsetTableLCU->m_iOffsetEo);
#else
      processSaoEO3(iStartX,iStartY,iEndX,iEndY,pRec,uiStride,pTmpBuff1,pTmpL,pSAO->pSaoOffsetTableLCU->m_iOffsetEo);
#endif
      break;
    } 

  case SAO_BO:
    {

#if SAO_ASM_ENABLED
      processSaoBo_asm(iLcuWidth,uiCurLCURowStartY,uiCurLCURowEndY,pRec,uiStride,pSAO->pSaoOffsetTableLCU->m_iOffsetBo);
#else
      processSaoBo(iLcuWidth,uiCurLCURowStartY,uiCurLCURowEndY,pRec,uiStride,pSAO->pSaoOffsetTableLCU->m_iOffsetBo);
#endif
      break;
    }

  default: 
      break;
  }

}



VO_VOID setNDBBorderAvailabilityPerPPS(H265_DEC_SLICE *p_slice, VO_VOID *phPPS, TComSampleAdaptiveOffset* pSAO, VO_U32 *puiTileIdxMap){
  VO_U32 idxX;
  VO_U32 idxY;
  VO_U32 uiAddr;
  H265_DEC_PPS *pPPS = (H265_DEC_PPS *)phPPS;
  VO_U32 numTiles = (pPPS->num_tile_rows_minus1+1)*(pPPS->num_tile_columns_minus1+1);
  VO_U32 frameWidthInCU = pSAO->numCuInWidth; 
  VO_U32  frameHeightInCU = pSAO->numCuInHeight; 
  VO_BOOL m_bIndependentSliceBoundaryForNDBFilter;
  VO_BOOL m_bIndependentTileBoundaryForNDBFilter;
  VO_BOOL bTopTileBoundary = VO_FALSE, bDownTileBoundary= VO_FALSE, bLeftTileBoundary= VO_FALSE, bRightTileBoundary= VO_FALSE;
  VO_U32  iTileID;
  VO_BOOL *pbAvailBorder;


  m_bIndependentTileBoundaryForNDBFilter  = (pPPS->loop_filter_across_tiles_enabled_flag)?(VO_FALSE) :((numTiles > 1)?(VO_TRUE):(VO_FALSE));
  m_bIndependentSliceBoundaryForNDBFilter = (pPPS->pps_loop_filter_across_slices_enabled_flag)?(VO_FALSE):VO_TRUE;

  //case 1: cross both slice and tile
  if(!m_bIndependentTileBoundaryForNDBFilter && !m_bIndependentSliceBoundaryForNDBFilter)
  {   
    return;  
  }

  pSAO->m_bUseNIF = VO_TRUE;

  SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, pSAO->m_vNDFBlock, VO_TRUE, frameWidthInCU*frameHeightInCU*sizeof(NDBFBlockInfo)); //1. default value
 
  //2. set picture boundary
  for (idxY = 0; idxY< frameHeightInCU; idxY++)
  {      
    for (idxX = 0; idxX < frameWidthInCU; idxX++)
    {
      uiAddr = idxY * frameWidthInCU + idxX;   
      pbAvailBorder = pSAO->m_vNDFBlock[uiAddr].isBorderAvailable;    
      //left
      if( uiAddr % frameWidthInCU == 0)
      {         
          pbAvailBorder[SGU_L] = pbAvailBorder[SGU_TL] = pbAvailBorder[SGU_BL] = VO_FALSE;          
      }
      //right
      if( (uiAddr % frameWidthInCU) == (frameWidthInCU -1) )
      {          
          pbAvailBorder[SGU_R] = pbAvailBorder[SGU_TR] = pbAvailBorder[SGU_BR] = VO_FALSE;         
      }
      //top
      if( uiAddr < frameWidthInCU)
      {          
          pbAvailBorder[SGU_T] = pbAvailBorder[SGU_TL] = pbAvailBorder[SGU_TR] = VO_FALSE;          
      }
      //down
      if( uiAddr + frameWidthInCU >= frameWidthInCU*frameHeightInCU)
      {         
          pbAvailBorder[SGU_B] = pbAvailBorder[SGU_BL] = pbAvailBorder[SGU_BR] = VO_FALSE;          
      }
    }
  }
  
  if(!m_bIndependentTileBoundaryForNDBFilter){
    //3. only slice boundary, set in setNDBBorderAvailabilityPerLCU.     
    return;
  }
  
  //4. tile boundary
  for (idxY = 0; idxY< frameHeightInCU; idxY++)
  {    
    for (idxX = 0; idxX < frameWidthInCU; idxX++)
    {
      uiAddr = idxY * frameWidthInCU + idxX;
      iTileID = puiTileIdxMap[pPPS->CtbAddrRsToTs[uiAddr]];
      pbAvailBorder = pSAO->m_vNDFBlock[uiAddr].isBorderAvailable;
      //left
      if( uiAddr % frameWidthInCU != 0)
      {
        bLeftTileBoundary = ( puiTileIdxMap[pPPS->CtbAddrRsToTs[uiAddr -1]] != iTileID )?VO_TRUE:VO_FALSE;
        if(bLeftTileBoundary)
        {
          pbAvailBorder[SGU_L] = pbAvailBorder[SGU_TL] = pbAvailBorder[SGU_BL] = VO_FALSE;
        }
      }
      //right
      if( (uiAddr % frameWidthInCU) != (frameWidthInCU -1) )
      {
        bRightTileBoundary = ( puiTileIdxMap[pPPS->CtbAddrRsToTs[uiAddr +1]] != iTileID)?VO_TRUE:VO_FALSE;
        if(bRightTileBoundary)
        {
          pbAvailBorder[SGU_R] = pbAvailBorder[SGU_TR] = pbAvailBorder[SGU_BR] = VO_FALSE;
        }
      }
      //top
      if( uiAddr >= frameWidthInCU)
      {
        bTopTileBoundary = ( puiTileIdxMap[pPPS->CtbAddrRsToTs[uiAddr - frameWidthInCU]] !=  iTileID )?VO_TRUE:VO_FALSE;
        if(bTopTileBoundary)
        {
          pbAvailBorder[SGU_T] = pbAvailBorder[SGU_TL] = pbAvailBorder[SGU_TR] = VO_FALSE;
        }
      }
      //down
      if( uiAddr + frameWidthInCU < frameWidthInCU*frameHeightInCU)
      {
        bDownTileBoundary = ( puiTileIdxMap[pPPS->CtbAddrRsToTs[uiAddr + frameWidthInCU]] != iTileID)?VO_TRUE:VO_FALSE;
        if(bDownTileBoundary)
        {
          pbAvailBorder[SGU_B] = pbAvailBorder[SGU_BL] = pbAvailBorder[SGU_BR] = VO_FALSE;
        }
      }
    }
  } 
}


VO_VOID SAOProcessOneLCU(VO_VOID* phSlice, VO_S32 LCURowIndex)
{
  H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)phSlice;  
  TComSampleAdaptiveOffset* pSAO = pSlice->m_saoParam;

  if (pSlice->slice_sao_luma_flag)
  { 
    processSao(phSlice, pSAO,  0, LCURowIndex*pSAO->numCuInWidth + pSAO->uiStartXByLCU); 

  }

  if(pSlice->slice_sao_chroma_flag)
  {    
    processSao(phSlice, pSAO,  1, LCURowIndex*pSAO->numCuInWidth + pSAO->uiStartXByLCU);
    processSao(phSlice, pSAO,  2, LCURowIndex*pSAO->numCuInWidth + pSAO->uiStartXByLCU);
       
  }
}

#if !USE_LCU_SYNC
//help function, delete in future when multi-core is ready
VO_VOID SAOProcessOneLCURow(VO_VOID* phSlice, VO_S32 LCURowIndex)
{
  H265_DEC_SLICE *pSlice = (H265_DEC_SLICE *)phSlice;  
  TComSampleAdaptiveOffset* pSAO = pSlice->m_saoParam;
  VO_U32 nX;

  for(nX=0; nX<pSAO->numCuInWidth; nX++){
    pSAO->uiStartXByLCU = nX;
    pSAO->uiEndXByLCU = nX+1;
    if (LCURowIndex)
    {
      pSAO->uiLCURowStartY = pSAO->m_uiMaxCUHeight-10;
      pSAO->uiLCURowEndY = pSAO->m_uiMaxCUHeight;
      SAOProcessOneLCU(phSlice, LCURowIndex-1);   
    }

    pSAO->uiLCURowStartY = 0;
    if (LCURowIndex < pSAO->numCuInHeight-1)
    {
      pSAO->uiLCURowEndY = pSAO->m_uiMaxCUHeight-10;
    }
    else
    {
      pSAO->uiLCURowEndY = pSAO->m_iPicHeight-pSAO->m_uiMaxCUHeight*(pSAO->numCuInHeight-1);
    }		
    SAOProcessOneLCU(phSlice, LCURowIndex);
  }
	
}
#endif
