/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
 * Copyright (c) 2010-2013, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     HMLog.c
    \brief    trying to dump log  same as HM
    \Caution  1. don't use struct, so can be reused by HM
*/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "HMLog.h"

#if H265_LOG

char *filename = "v2_log_debug.txt";
FILE *fpLog = NULL;

char *filenameYUV[3] = {"v2_log_debug.txt","v2_log_u.txt","v2_log_v.txt" }; //filenameYUV[0] should be same as filename
FILE *fpLogYUV[3] = {NULL, NULL, NULL};

int g_start_print_POC = -65536; //[ include  
int g_end_print_POC = 65535;    //) not include  
int g_cur_print_POC;      //used to control which frames should be printed

int g_CABAC_cnt = 0;
int g_intraPred_cnt = 0;
int g_MC_cnt = 0;
int g_motioninfo_cnt = 0;
int g_dequant_out_cnt[3]  = {0, 0, 0};
int g_residual_cnt[3] = {0, 0, 0};


FILE *getLogFP(VO_S32 cIdx){
  if(cIdx==0){
    if(fpLog==NULL){
      fpLog = fopen(filename, "wb");
    }
    if(!strcmp(filename,filenameYUV[0])){
      fpLogYUV[0] = fpLog;
    }
    return fpLog;
  }else{
    if(fpLogYUV[cIdx]==NULL){
      fpLogYUV[cIdx] = fopen(filenameYUV[cIdx], "wb");     
    }
    return fpLogYUV[cIdx];
  }
}

VO_U32 isNeededPrint(){
  if(g_cur_print_POC>=g_start_print_POC&&g_cur_print_POC<g_end_print_POC){
    return 1;
  }else{
    return 0;
  }
}

VO_VOID printCABACState(VO_U32 m_uiRange, VO_U32 m_uiValue){
#if CABAC_LOG
  getLogFP(0);
  if(!isNeededPrint()){
    return;
  }
  m_uiValue>>=17;
  fprintf(fpLog, "CABAC: %d\t%d\t%d\n",  g_CABAC_cnt , m_uiRange, m_uiValue);
  fflush(fpLog);
  g_CABAC_cnt++;
#endif
}


VO_VOID printSliceInfo(VO_U32  sliceType, VO_S32 iPOC){
#if SLICE_LOG
  int iCompIdx;
  char slice_type;
  g_cur_print_POC = iPOC;
  if(!isNeededPrint()){
    return;
  }
  getLogFP(0);
  switch (sliceType)
  {
  case 2://I_SLICE:           // change initialization table to B_SLICE intialization
    slice_type = 'I'; 
    break;
  case 1://P_SLICE:           // change initialization table to B_SLICE intialization
    slice_type = 'P'; 
    break;
  case 0://B_SLICE:           // change initialization table to P_SLICE intialization
    slice_type = 'B'; 
    break;
  default     :           // should not occur
    break;   
  }

  fprintf(fpLog, "POC type:  %c,  POC: %d\n",  slice_type , iPOC);
  fflush(fpLog);
#endif
}


VO_VOID printLCUInfo(VO_U32 lcuAddr){
#if LCU_LOG
  if(!isNeededPrint()){
    return;
  }
  getLogFP(0);
  fprintf(fpLog, "LCU addr: %d\n",  lcuAddr);
  fflush(fpLog);
#endif
}

VO_VOID printQPInfo(VO_U32 *QP, VO_U32 PicWidthInMinTbsY, VO_U32 size){
#if QP_INFO_LOG
  int i,j;
  if(!isNeededPrint()){
    return;
  }
  getLogFP(0);

  for ( j = 0;j < size; j++ ) {
    for ( i = 0; i < size; i++ ) {
      if((j*size+i)%16==0&&(j*size+i)!=0){
        fprintf(fpLog, "\n");
      }
      fprintf(fpLog, "%d\t",  (QP[i]&0x00ff0000)>>16);
      
    }
    QP += PicWidthInMinTbsY ;
  }

  fprintf(fpLog, "\n\n");
  fflush(fpLog);
#endif
}

VO_VOID printIntraPredictionInput(VO_S32 predModeIntra,
                             VO_S32 cIdx,
                             VO_U8* top,
                             VO_U8* left,
                             VO_S32 nTbS,
                             VO_U32 tmp_bl,
                             VO_U32 tmp_l,
                             VO_U32 tmp_tl,
                             VO_U32 tmp_t,
                             VO_U32 tmp_tr)
{
#if INTRA_PRED_LOG
    VO_U8* p_ceof_value = top -1;      
    VO_U32 height = nTbS*2+1; 
    if(!isNeededPrint()){
      return;
    }
    getLogFP(0);   
    fprintf(fpLog,"******************Intra caller = [%d]*****************.\n",g_intraPred_cnt);
    fprintf(fpLog,"predmode = [%d], cidx = [%d]\n",predModeIntra,cIdx);
    fprintf(fpLog,"orig  bl_avail= [%d], l_avail= [%d],tl_avail= [%d], t_avail= [%d], tr_avail = [%d]\n",
      tmp_bl,tmp_l,tmp_tl,tmp_t,tmp_tr);
    //       fprintf(fpLog,"after bl_avail= [%d], l_avail= [%d],tl_avail= [%d], t_avail= [%d], tr_avail = [%d]\n",
    //         bl_avail,l_avail,tl_avail, t_avail, tr_avail); 

    fprintf(fpLog,"top[%d][%d] = {\n",cIdx,height);
    {
      VO_U32 i,j;      
      for(j = 0; j < height; j++) {
        fprintf(fpLog,"%d, ",(p_ceof_value[j]));
        //fprintf(fpLog,"\n");
      }
      fprintf(fpLog,"\n}\n "); 
    }    
    p_ceof_value = left-1;
    fprintf(fpLog,"left[%d][%d] = {\n",cIdx,height);
    {
      VO_U32 i,j;      
      for(j = 0; j < height; j++) {
        fprintf(fpLog,"%d, ",(p_ceof_value[j]));
        //fprintf(fpLog,"\n");
      }
      fprintf(fpLog,"\n}\n "); 
    } 
    fflush(fpLog);
    g_intraPred_cnt++; 
#endif
}

VO_VOID printIntraPredictionOutput(VO_S32 cIdx,
  VO_U8* p_ref,
  VO_S32 pic_stride,
  VO_U32 log2TrafoSize){ 
#if INTRA_PRED_LOG
    VO_U8* p_ceof_value = p_ref;      
    VO_U32 height = 1<<log2TrafoSize;
    if(!isNeededPrint()){
      return;
    }    
    getLogFP(0);
    fprintf(fpLog,"intra_pred_buf[%d][%d * %d] = {\n",cIdx,height, height);
    {
      VO_U32 i,j;      
      for (i = 0; i < height; i++) {
        for(j = 0; j < height; j++) {
          fprintf(fpLog,"%d, ",(p_ceof_value[i * pic_stride + j]));
        }
        fprintf(fpLog,"\n");
      }
      fprintf(fpLog,"\n}\n "); 
    } 
    fflush(fpLog);
#endif
}

VO_VOID printxPredInterLuma(VO_U8* p_ceof_value, VO_U32 width, VO_U32 height, VO_U32 stride){
#if INTER_LOG
  if(!isNeededPrint()){
    return;
  }
  getLogFP(0);
  fprintf(fpLog,"******************MC caller = [%d]*****************.\n",g_MC_cnt);
  fprintf(fpLog,"**inter_luma_Uni_pred_buf[%d * %d] = {\n",width, height);
  {
    VO_U32 i,j;      
    for (i = 0; i < height; i++) {
      for(j = 0; j < width; j++) {
        fprintf(fpLog,"%d, ",(p_ceof_value[i * stride + j]));
      }
      fprintf(fpLog,"\n");
    }
    fprintf(fpLog,"\n}\n "); 
  }
  fflush(fpLog);
  g_MC_cnt++;
 #endif
}

VO_VOID printxPredInterChroma(VO_U8* dstCb, VO_U8* dstCr, VO_U32 width, VO_U32 height, VO_U32 stride){
#if INTER_LOG
  VO_U8* p_ceof_value = dstCb;  
  if(!isNeededPrint()){
    return;
  }
  getLogFP(0);
  fprintf(fpLog,"**inter_chromaU_Uni_pred_buf[%d * %d] = {\n",width, height);
  {
    VO_U32 i,j;      
    for (i = 0; i < height; i++) {
      for(j = 0; j < width; j++) {
        fprintf(fpLog,"%d, ",(p_ceof_value[i * stride + j]));
      }
      fprintf(fpLog,"\n");
    }
    fprintf(fpLog,"\n}\n "); 
  } 
  p_ceof_value = dstCr;
  fprintf(fpLog,"**inter_chromaV_Uni_pred_buf[%d * %d] = {\n",width, height);
  {
    VO_U32 i,j;      
    for (i = 0; i < height; i++) {
      for(j = 0; j < width; j++) {
        fprintf(fpLog,"%d, ",(p_ceof_value[i * stride + j]));
      }
      fprintf(fpLog,"\n");
    }
    fprintf(fpLog,"\n}\n "); 
  }
  fflush(fpLog);
#endif
}

VO_VOID printMotionInfo(VO_U8 pred_mode, VO_U8 inter_dir, VO_U32 xPb,  VO_U32 yPb, VO_S8 iRefIdx0, VO_S8 iRefIdx1, VO_S32 m_acMv0, VO_S32 m_acMv1){
#if MOTION_INFO_LOG
  if(!isNeededPrint()){
    return;
  }
  getLogFP(0);
  if ( pred_mode != 1 ) //MODE_INTRA
    
    fprintf(fpLog, "g_motioninfo_cnt = %d, [xPb, yPb] = [%d, %d], pred_mode = %d, ref_idx0 = %d, ref_idx1 = %d, inter_dir = %d, mv0 = %d, mv1 = %d\n",\
    g_motioninfo_cnt++, xPb,yPb, pred_mode, iRefIdx0,\
    iRefIdx1, inter_dir,\
    m_acMv0,m_acMv1);
    fflush(fpLog);
#endif
}

VO_VOID printReconInput(VO_U32 cIdx, VO_U8 *pPred, VO_U32 pred_stride, VO_U32 x0, VO_U32 y0, VO_S16 *TransCoeffLevel, VO_U32  log2TrafoSize, VO_U32 qp, VO_U32 transform_skip_flag, VO_U32 cu_transquant_bypass_flag){
#if 1
  VO_S16 * pSrc = TransCoeffLevel;
  VO_U32 height = 1 << log2TrafoSize;
  VO_U32 stride = 1 << log2TrafoSize;
  VO_U32 i,j;
  VO_S16 *p_ceof_value = pSrc;
  FILE *fpLogCur = getLogFP(cIdx);
  if(!isNeededPrint()){
    return;
  }

  fprintf(fpLogCur,"******************Intra buffer  = [%d]*****************.\n",g_residual_cnt[cIdx]);
  fprintf(fpLogCur,"qp = [%d].bypass = [%d], transform_skip = [%d].\n",qp,cu_transquant_bypass_flag, transform_skip_flag);

  fprintf(fpLogCur,"pred[%d * %d] = {\n",height, height);
  for (j = y0; j < y0+height; j++) {
    for(i = x0+0; i < x0+height; i++) {
      fprintf(fpLogCur,"%d, ",(pPred[j * pred_stride + i]));
    }
    fprintf(fpLogCur,"\n");
  }
  fprintf(fpLogCur,"\n}\n "); 
  fflush(fpLogCur);
  g_residual_cnt[cIdx]++;
#endif
}

VO_VOID printDeQuantOut(VO_U32 cIdx, VO_U32 qp, VO_S16 *m_plTempCoeff, VO_U32 uiWidth, VO_U32 height, VO_U32 stride, VO_U32 x, VO_U32 y){
  VO_U32 i,j;
  FILE *fpLogCur = getLogFP(cIdx);
  if(!isNeededPrint()){
    return;
  }

#if 1   //for analyze QP value
  if(cIdx==0){
    fprintf(fpLogCur," g_dequant_out_cnt=%d, qp = %d,  coeff[%d * %d], (x,y) = \n", g_dequant_out_cnt[cIdx], qp, height, height, x, y);

  }
  g_dequant_out_cnt[cIdx]++;
#endif

#if 0// no print coeff buff
  fprintf(fpLogCur,"******************After dequant   = [%d]*****************.\n",g_dequant_out_cnt[cIdx]);
  fprintf(fpLogCur," qp = %d,  coeff[%d * %d] = {\n", qp, height, height);
  for (i = 0; i < height; i++) {
    for(j = 0; j < height; j++) {
      fprintf(fpLogCur,"%d, ",(m_plTempCoeff[i * stride + j]));
    }
    fprintf(fpLogCur,"\n");
  }
  fprintf(fpLogCur,"\n}\n "); 
  fflush(fpLogCur);
  g_dequant_out_cnt[cIdx] ++;
#endif
}

VO_VOID printResidualOutput(VO_U32 cIdx, VO_U8 *p_recon_buf, VO_U32 log2TrafoSize, VO_U32 pred_stride){
#if RECON_LOG

  VO_U32 height = 1 << log2TrafoSize;
  VO_U32 stride = pred_stride;
  VO_U32 i,j;
  FILE *fpLogCur = getLogFP(cIdx);
  if(!isNeededPrint()){
    return;
  }

  fprintf(fpLogCur,"p_construction_pixel_buff[%d * %d] = {\n",height, height);
  {   
    for (i = 0; i < height; i++) {
      for(j = 0; j < height; j++) {
        fprintf(fpLogCur,"%d, ",(p_recon_buf[i * stride + j]));
      }
      fprintf(fpLogCur,"\n");
    }
    fprintf(fpLogCur,"\n}\n "); 
  }
  fflush(fpLogCur);
#endif
}


VO_VOID printSAOParam(VO_VOID *p_void_slice, VO_U32 ctb_addr_in_rs){
#if 0
	H265_DEC_SLICE *p_slice = (H265_DEC_SLICE *)p_void_slice;
	TComSampleAdaptiveOffset *pSAO = p_slice->cur_pic->m_saoParam;
	VO_U32 pSAOParamPanelOffset = pSAO->numCuInWidth*pSAO->numCuInHeight;
	SaoLcuParam* const pCurLcuSAOParamY = &pSAO->saoLcuParam[ctb_addr_in_rs];
	SaoLcuParam* pCurLcuSAOParam = pCurLcuSAOParamY;  
	int cIdx;	

	for(cIdx=0; cIdx<3; cIdx++){
		//fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_merge_left_flag);
		//fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_merge_up_flag);
		fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_type_idx[cIdx]);
		if(pCurLcuSAOParam->sao_type_idx[cIdx]>=SAO_EO_0&& pCurLcuSAOParam->sao_type_idx[cIdx]<=SAO_EO_3){
			if(cIdx==0||cIdx==1){
				pCurLcuSAOParam->sao_band_position[cIdx] = pCurLcuSAOParam->sao_type_idx[cIdx];
			}else{
				pCurLcuSAOParam->sao_band_position[cIdx] = 0;
			}

		}
		if(pCurLcuSAOParam->sao_type_idx[cIdx]==SAO_NONE){
			pCurLcuSAOParam->sao_band_position[cIdx] = 0;
			pCurLcuSAOParam->sao_offset_abs[cIdx][0] = pCurLcuSAOParam->sao_offset_abs[cIdx][1] = pCurLcuSAOParam->sao_offset_abs[cIdx][2] = pCurLcuSAOParam->sao_offset_abs[cIdx][3] = 0;

		}
		fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_band_position[cIdx]);
		fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_offset_abs[cIdx][0]);
		fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_offset_abs[cIdx][1]);
		fprintf(fpLog, "%d\t", pCurLcuSAOParam->sao_offset_abs[cIdx][2]);
		fprintf(fpLog, "%d\n", pCurLcuSAOParam->sao_offset_abs[cIdx][3]);
		pCurLcuSAOParam+=pSAOParamPanelOffset;
	}
#endif

}

#endif

//! 
