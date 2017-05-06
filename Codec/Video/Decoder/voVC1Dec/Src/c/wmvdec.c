//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "codehead.h"
#include "localhuffman_wmv.h"

//#define VOANDROID_DUMPVC1 1
//#define VOANDROID_DUMPYUV 1
//#define TRACEVC1 1

//#define OUTPUTFRMAE 1

//3  cur ref0 ref1
//2  multi,411
#define MAX_FRAMES_INTERNAL  (3+2) //20130109  since multi-core maybe block n frame for output.
														 //max = 2 frames need to store for dual-thread;
                                                         //max = 4 frames need to store for quad-core 
														 //*2 : post and cur

#ifdef TRACEVC1
#define TRACEVC1FILE "VC1_DEC_LOG.txt"
#ifdef VOANDROID
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
static unsigned long voGetTime(){
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#else
#define voGetTime clock
#endif 
#endif //TRACEVC1

#ifdef VOANDROID_DUMPVC1
#ifdef VOANDROID
char* dumpfile="/mnt/sdcard/smoothstream/dump_new.vc1";
char vc1file[255];
#else
char* dumpfile="D:/dump.vc1";
char vc1file[255];
#endif
#endif


//#define VO_PID_DEC_WMV_BASE              (VO_PID_COMMON_BASE | VO_INDEX_DEC_WMV) 
//
//typedef enum
//{
//	/*!< Decoder*/
//    VO_PID_DEC_WMV_GET_LASTVIDEOBUFFER   = VO_PID_DEC_WMV_BASE | 0x0001,  /*!<Get the last nFrame video buffer,the parameter is address of structure VO_VIDEO_BUFFER,(invalid)*/
//    VO_PID_DEC_WMV_GET_LASTVIDEOINFO     = VO_PID_DEC_WMV_BASE | 0x0002,  /*!<Get the last nFrame video information,the parameter is address of structure VO_VIDEO_OUTPUTINFO,(invalid)*/
//};

/*There is no license check lib of RVDS, so need time limitation.*/
#ifdef RVDS
#define TIME_LIMIT	//decoder stop working after 60 minutes.
#define MAX_TIME	(30*60*60)	//30fps*60seconds*60minutes
#endif

/* Enable license check. */
#if defined(NDEBUG) || defined(_NDEBUG)
#define CHKLCS_ON
#endif
//#define MARK_POST 1

#ifdef CHKLCS_ON
#include "voCheck.h"
#endif

#include "voWmvPort.h"

#ifdef macintosh
#include "cpudetect_mac.h"
#endif


extern Bool_WMV g_bSupportAltiVec_WMV;
extern Bool_WMV g_bSupportWMMX_WMV;


VO_HANDLE g_VC1_hModule = NULL; 	//for CheckLcs lib.


#if !defined(_MAC)   && !defined(__arm)
#   include "memory.h"
#endif
#include "math.h"

// define this to force a particular level at each I-frame.  For debugging/dev only
// #define _QUICK_DEBLOCK_POSTFILTER_

//#pragma warning( disable: 4719 )

// ===========================================================================
// private functions
// ===========================================================================
#include "strmdec_wmv.h"
#include "repeatpad_wmv.h"

extern I32_WMV voVC1CreateThreads(tWMVDecInternalMember *pWMVDec);
extern void voVC1DestoryThreads(tWMVDecInternalMember *pWMVDec);
I32_WMV YUV411_To_YUV420( U8_WMV **pSrcBuff, U8_WMV **pDstBuff, I32_WMV *iSrcStride, I32_WMV iW, I32_WMV iH );

tWMVDecodeStatus voVC1ParpareBufferIVOP(  tWMVDecInternalMember *pWMVDec );
tWMVDecodeStatus voVC1ParpareBufferPVOP(  tWMVDecInternalMember *pWMVDec );
tWMVDecodeStatus voVC1ParpareBufferBVOP(  tWMVDecInternalMember *pWMVDec );

tWMVDecodeStatus voVC1ParpareBufferIVOPNew(tWMVDecInternalMember *pGlobleWMVDec,  tWMVDecInternalMember *pWMVDec );
tWMVDecodeStatus voVC1ParpareBufferPVOPNew(tWMVDecInternalMember *pGlobleWMVDec,  tWMVDecInternalMember *pWMVDec );
tWMVDecodeStatus voVC1ParpareBufferBVOPNew(tWMVDecInternalMember *pGlobleWMVDec, tWMVDecInternalMember *pWMVDec );

void UpdateThreadFIFONew(tWMVDecInternalMember *pSrcWMVDec,tWMVDecInternalMember *pDstWMVDec)
{
#ifdef USE_FRAME_THREAD
	VO_U32 i=0;

	pDstWMVDec->m_memManageFIFO.img_seq = pSrcWMVDec->m_memManageFIFO.img_seq;                                                
	for(i=0;i<pDstWMVDec->ntotal_frames;i++)                                  
	{
		pDstWMVDec->m_memManageFIFO.img_seq[i]= pSrcWMVDec->m_memManageFIFO.img_seq[i];                                  
	}                                                                     
	pDstWMVDec->m_pfrmCurrQ= pSrcWMVDec->m_pfrmCurrQ; 
	pDstWMVDec->m_pfrmRef0Q= pSrcWMVDec->m_pfrmRef0Q; 
	pDstWMVDec->m_pfrmRef1Q= pSrcWMVDec->m_pfrmRef1Q; 
	pDstWMVDec->m_pfrmPostQ= pSrcWMVDec->m_pfrmPostQ; 
	pDstWMVDec->m_pfrmPostPrevQ= pSrcWMVDec->m_pfrmPostPrevQ;                                              
	pDstWMVDec->m_memManageFIFO.r_idx= pSrcWMVDec->m_memManageFIFO.r_idx; 
	pDstWMVDec->m_memManageFIFO.w_idx= pSrcWMVDec->m_memManageFIFO.w_idx; 
	pDstWMVDec->m_memManageFIFO.buf_num= pSrcWMVDec->m_memManageFIFO.buf_num;

	pDstWMVDec->m_pfrmRef0Process= pSrcWMVDec->m_pfrmRef0Process; 
	pDstWMVDec->m_pfrmMultiresQ= pSrcWMVDec->m_pfrmMultiresQ; 
	pDstWMVDec->m_pfrmYUV411toYUV420Q= pSrcWMVDec->m_pfrmYUV411toYUV420Q; 
    pDstWMVDec->m_ppxliCurrQY= pSrcWMVDec->m_ppxliCurrQY; 
    pDstWMVDec->m_ppxliCurrQU= pSrcWMVDec->m_ppxliCurrQU; 
    pDstWMVDec->m_ppxliCurrQV= pSrcWMVDec->m_ppxliCurrQV; 
    pDstWMVDec->m_ppxliRef0Y= pSrcWMVDec->m_ppxliRef0Y; 
    pDstWMVDec->m_ppxliRef0U= pSrcWMVDec->m_ppxliRef0U;                                                 
    pDstWMVDec->m_ppxliRef0V= pSrcWMVDec->m_ppxliRef0V;                                                 
    pDstWMVDec->m_ppxliRef0YPlusExp= pSrcWMVDec->m_ppxliRef0YPlusExp;                                          
    pDstWMVDec->m_ppxliPostQY= pSrcWMVDec->m_ppxliPostQY;                                                
    pDstWMVDec->m_ppxliPostQU= pSrcWMVDec->m_ppxliPostQU;                                                
    pDstWMVDec->m_ppxliPostQV= pSrcWMVDec->m_ppxliPostQV;                                                
    pDstWMVDec->m_ppxliPostPrevQY= pSrcWMVDec->m_ppxliPostPrevQY;                                            
    pDstWMVDec->m_ppxliPostPrevQU= pSrcWMVDec->m_ppxliPostPrevQU;                                            
    pDstWMVDec->m_ppxliPostPrevQV= pSrcWMVDec->m_ppxliPostPrevQV;                                            
                                                                                                                                       
    pDstWMVDec->m_ppxliMultiresY= pSrcWMVDec->m_ppxliMultiresY;                                         
    pDstWMVDec->m_ppxliMultiresU= pSrcWMVDec->m_ppxliMultiresU;                                         
    pDstWMVDec->m_ppxliMultiresV= pSrcWMVDec->m_ppxliMultiresV;                                                                                                                                                                                 
    pDstWMVDec->m_ppxliRef1Y= pSrcWMVDec->m_ppxliRef1Y;                                             
    pDstWMVDec->m_ppxliRef1U= pSrcWMVDec->m_ppxliRef1U;           
    pDstWMVDec->m_ppxliRef1V= pSrcWMVDec->m_ppxliRef1V;           
    pDstWMVDec->m_ppxliRef1YPlusExp= pSrcWMVDec->m_ppxliRef1YPlusExp;    

	pDstWMVDec->m_iRndCtrl =  pSrcWMVDec->m_iRndCtrl;
	pDstWMVDec->m_bPrevAnchor = pSrcWMVDec->m_bPrevAnchor;
	pDstWMVDec->m_rgmv1_EMB = pSrcWMVDec->m_rgmv1_EMB;
	pDstWMVDec->m_iBState = pSrcWMVDec->m_iBState;
	pDstWMVDec->m_iReconRangeState = pSrcWMVDec->m_iReconRangeState;
	pDstWMVDec->m_iRangeState  = pSrcWMVDec->m_iRangeState;
	pDstWMVDec->m_tFrmTypePrevious = pSrcWMVDec->m_tFrmTypePrevious;

	//201318	
	pDstWMVDec->bUseRef0Process = pSrcWMVDec->bUseRef0Process;

	//for output
	// 20130116

	pDstWMVDec->m_pfrmDisplayPre		 = pSrcWMVDec->m_pfrmDisplayPre;
#if 0
	pDstWMVDec->m_pvopcRenderThis	 = pSrcWMVDec->m_pvopcRenderThis;
#endif
	pDstWMVDec->m_pfrmDisplayPreBak		 = pSrcWMVDec->m_pfrmDisplayPreBak;
	pDstWMVDec->m_pvopcRenderThisBak	 = pSrcWMVDec->m_pvopcRenderThisBak;

	//
	pDstWMVDec->m_bPrevAnchor		 = pSrcWMVDec->m_bPrevAnchor;
	pDstWMVDec->m_iPrevIFrame = pSrcWMVDec->m_iPrevIFrame;
	pDstWMVDec->m_IBPFlag = pSrcWMVDec->m_IBPFlag;
	pDstWMVDec->m_ref0Indx = pSrcWMVDec->m_ref0Indx;	
	pDstWMVDec->m_pfrmRef0Buffer[0] = pSrcWMVDec->m_pfrmRef0Buffer[0];
	pDstWMVDec->m_pfrmRef0Buffer[1] = pSrcWMVDec->m_pfrmRef0Buffer[1];
	pDstWMVDec->m_pfrmRef0Buffer[2] = pSrcWMVDec->m_pfrmRef0Buffer[2];
	pDstWMVDec->m_pfrmRef0Buffer[3] = pSrcWMVDec->m_pfrmRef0Buffer[3];

	pDstWMVDec->m_iResIndexPrev = pSrcWMVDec->m_iResIndexPrev; 

    //for interlace
	pDstWMVDec->m_pFieldMvX_Pbuf = pSrcWMVDec->m_pFieldMvX_Pbuf;
	pDstWMVDec->m_pFieldMvY_Pbuf = pSrcWMVDec->m_pFieldMvY_Pbuf;

	pDstWMVDec->m_fDecodedI = pSrcWMVDec->m_fDecodedI;
#endif
	return ;
}

VO_S32 CopyEntryPointParametes(tWMVDecInternalMember* pDstWMVDec,tWMVDecInternalMember* pSrcWMVDec)
{
	pDstWMVDec->m_bSwitchingEntryPoint=     pSrcWMVDec->m_bSwitchingEntryPoint;        
	pDstWMVDec->m_bExtendedDeltaMvMode=     pSrcWMVDec->m_bExtendedDeltaMvMode;        
	pDstWMVDec->m_bExplicitSeqQuantizer=    pSrcWMVDec->m_bExplicitSeqQuantizer;       
	pDstWMVDec->m_bExplicitFrameQuantizer=  pSrcWMVDec->m_bExplicitFrameQuantizer;     
	pDstWMVDec->m_bUse3QPDZQuantizer=       pSrcWMVDec->m_bUse3QPDZQuantizer;          
	pDstWMVDec->m_bTopFieldFirst=           pSrcWMVDec->m_bTopFieldFirst;              
	pDstWMVDec->m_bRepeatFirstField=        pSrcWMVDec->m_bRepeatFirstField;           
	pDstWMVDec->m_iRepeatFrameCount=        pSrcWMVDec->m_iRepeatFrameCount;           
	pDstWMVDec->m_bProgressive420=          pSrcWMVDec->m_bProgressive420;             
	pDstWMVDec->m_bInterpolateCurrentFrame= pSrcWMVDec->m_bInterpolateCurrentFrame;    
	pDstWMVDec->m_iPostProcLevel=           pSrcWMVDec->m_iPostProcLevel;              
	pDstWMVDec->m_iMVRangeIndex=            pSrcWMVDec->m_iMVRangeIndex;               
	pDstWMVDec->m_iDeltaMVRangeIndex=       pSrcWMVDec->m_iDeltaMVRangeIndex;          
	pDstWMVDec->m_iExtendedDMVX=            pSrcWMVDec->m_iExtendedDMVX;               
	pDstWMVDec->m_bLuminanceWarp=           pSrcWMVDec->m_bLuminanceWarp;              
	pDstWMVDec->m_iFrameXformMode=          pSrcWMVDec->m_iFrameXformMode;             
	pDstWMVDec->m_bMBXformSwitching=        pSrcWMVDec->m_bMBXformSwitching;           
	pDstWMVDec->m_bBrokenLink=              pSrcWMVDec->m_bBrokenLink;                 
	pDstWMVDec->m_bClosedEntryPoint=        pSrcWMVDec->m_bClosedEntryPoint;           
	pDstWMVDec->m_bPanScanPresent=          pSrcWMVDec->m_bPanScanPresent;             
	pDstWMVDec->m_bRefDistPresent=          pSrcWMVDec->m_bRefDistPresent;             
	pDstWMVDec->m_bLoopFilter=              pSrcWMVDec->m_bLoopFilter;                 
	pDstWMVDec->m_bUVHpelBilinear=          pSrcWMVDec->m_bUVHpelBilinear;             
	pDstWMVDec->m_bExtendedMvMode=          pSrcWMVDec->m_bExtendedMvMode;             
	pDstWMVDec->m_iDQuantCodingOn=          pSrcWMVDec->m_iDQuantCodingOn;             
	pDstWMVDec->m_bXformSwitch=             pSrcWMVDec->m_bXformSwitch;                
	pDstWMVDec->m_bSequenceOverlap=         pSrcWMVDec->m_bSequenceOverlap;            
	pDstWMVDec->m_bExplicitSeqQuantizer=    pSrcWMVDec->m_bExplicitSeqQuantizer;       
	pDstWMVDec->m_bUse3QPDZQuantizer=       pSrcWMVDec->m_bUse3QPDZQuantizer;          
	pDstWMVDec->m_bExplicitFrameQuantizer=  pSrcWMVDec->m_bExplicitFrameQuantizer;     
	pDstWMVDec->m_iCodedSizeFlag=           pSrcWMVDec->m_iCodedSizeFlag;              
	pDstWMVDec->m_iFrmWidthSrc=             pSrcWMVDec->m_iFrmWidthSrc;                
	pDstWMVDec->m_iFrmHeightSrc=            pSrcWMVDec->m_iFrmHeightSrc;               
	pDstWMVDec->m_bExtendedDeltaMvMode=     pSrcWMVDec->m_bExtendedDeltaMvMode;        
	pDstWMVDec->m_bRangeRedY_Flag=          pSrcWMVDec->m_bRangeRedY_Flag;             
	pDstWMVDec->m_iRangeRedY=               pSrcWMVDec->m_iRangeRedY;                  
	pDstWMVDec->m_bRangeRedUV_Flag=         pSrcWMVDec->m_bRangeRedUV_Flag;            
	pDstWMVDec->m_iRangeRedUV=              pSrcWMVDec->m_iRangeRedUV;                 
	pDstWMVDec->m_iBFrameReciprocal=        pSrcWMVDec->m_iBFrameReciprocal;           
	pDstWMVDec->m_bExplicitQuantizer=       pSrcWMVDec->m_bExplicitQuantizer;          
	pDstWMVDec->m_iMVRangeIndex=            pSrcWMVDec->m_iMVRangeIndex;               
	pDstWMVDec->m_iLogXRange=               pSrcWMVDec->m_iLogXRange;                  
	pDstWMVDec->m_iLogYRange=               pSrcWMVDec->m_iLogYRange;                  
	pDstWMVDec->m_iXMVRange=                pSrcWMVDec->m_iXMVRange;                   
	pDstWMVDec->m_iYMVRange=                pSrcWMVDec->m_iYMVRange;                   
	pDstWMVDec->m_iXMVFlag=                 pSrcWMVDec->m_iXMVFlag;                    
	pDstWMVDec->m_iYMVFlag=                 pSrcWMVDec->m_iYMVFlag;  
	return 0;
}
VO_S32 CopySQHParametes(tWMVDecInternalMember* pDstWMVDec,tWMVDecInternalMember* pSrcWMVDec)
{
	pDstWMVDec->m_bCodecIsWVC1 = pSrcWMVDec->m_bCodecIsWVC1;
	if(pDstWMVDec->m_bCodecIsWVC1)
	{
		pDstWMVDec->m_iWMV3Profile = pSrcWMVDec->m_iWMV3Profile ;
		pDstWMVDec->m_iLevel= pSrcWMVDec->m_iLevel;
		pDstWMVDec->m_iChromaFormat= pSrcWMVDec->m_iChromaFormat;
		pDstWMVDec->m_dFrameRate = pSrcWMVDec->m_dFrameRate ;
		pDstWMVDec->m_iBitRate= pSrcWMVDec->m_iBitRate;
		pDstWMVDec->m_bPostProcInfoPresent= pSrcWMVDec->m_bPostProcInfoPresent;
		pDstWMVDec->m_iMaxFrmWidthSrc= pSrcWMVDec->m_iMaxFrmWidthSrc;
		pDstWMVDec->m_iMaxFrmHeightSrc= pSrcWMVDec->m_iMaxFrmHeightSrc;
		pDstWMVDec->m_bBroadcastFlags = pSrcWMVDec->m_bBroadcastFlags ;
		pDstWMVDec->m_bInterlacedSource = pSrcWMVDec->m_bInterlacedSource ;
		pDstWMVDec->m_bTemporalFrmCntr = pSrcWMVDec->m_bTemporalFrmCntr ;
		pDstWMVDec->m_bSeqFrameInterpolation = pSrcWMVDec->m_bSeqFrameInterpolation ;   
		pDstWMVDec->tDimensions= pSrcWMVDec->tDimensions;    
		pDstWMVDec->m_bHRD_PARAM_FLAG= pSrcWMVDec->m_bHRD_PARAM_FLAG;
		pDstWMVDec->m_ihrd_num_leaky_buckets = pSrcWMVDec->m_ihrd_num_leaky_buckets ;
		pDstWMVDec->m_dFrameRate = pSrcWMVDec->m_dFrameRate ;
		pDstWMVDec->m_iFrameRate = pSrcWMVDec->m_iFrameRate ;
		pDstWMVDec->m_iBitRate = pSrcWMVDec->m_iBitRate ;
		pDstWMVDec->m_iBFrameReciprocal= pSrcWMVDec->m_iBFrameReciprocal;
		pDstWMVDec->m_iMVRangeIndex= pSrcWMVDec->m_iMVRangeIndex;
		pDstWMVDec->m_iLogXRange= pSrcWMVDec->m_iLogXRange;
		pDstWMVDec->m_iLogYRange= pSrcWMVDec->m_iLogYRange;
		pDstWMVDec->m_iXMVRange= pSrcWMVDec->m_iXMVRange;
		pDstWMVDec->m_iYMVRange = pSrcWMVDec->m_iYMVRange ;
		pDstWMVDec->m_iXMVFlag= pSrcWMVDec->m_iXMVFlag;
		pDstWMVDec->m_iYMVFlag = pSrcWMVDec->m_iYMVFlag ;  
		pDstWMVDec->m_bYUV411= pSrcWMVDec->m_bYUV411; 
		pDstWMVDec->m_bSpriteMode = pSrcWMVDec->m_bSpriteMode ;
		pDstWMVDec->m_bXintra8Switch= pSrcWMVDec->m_bXintra8Switch;
		pDstWMVDec->m_bMultiresEnabled= pSrcWMVDec->m_bMultiresEnabled;
		pDstWMVDec->m_b16bitXform = pSrcWMVDec->m_b16bitXform ;
		pDstWMVDec->m_bDCTTable_MB_ENABLED= pSrcWMVDec->m_bDCTTable_MB_ENABLED;
		pDstWMVDec->m_iBetaRTMMismatchIndex =       pSrcWMVDec->m_iBetaRTMMismatchIndex ;
		pDstWMVDec->m_iBetaRTMMismatchIndex=        pSrcWMVDec->m_iBetaRTMMismatchIndex;
	}
	else
	{
		pDstWMVDec->m_iWMV3Profile = pSrcWMVDec->m_iWMV3Profile;              
		pDstWMVDec->m_bYUV411 = pSrcWMVDec->m_bYUV411;                   
		pDstWMVDec->m_bSpriteMode = pSrcWMVDec->m_bSpriteMode;               
		pDstWMVDec->m_iFrameRate = pSrcWMVDec->m_iFrameRate;                
		pDstWMVDec->m_iBitRate = pSrcWMVDec->m_iBitRate;                  
		pDstWMVDec->m_iFrameRate = pSrcWMVDec->m_iFrameRate;                
		pDstWMVDec->m_iBitRate = pSrcWMVDec->m_iBitRate;                  
		pDstWMVDec->m_bLoopFilter = pSrcWMVDec->m_bLoopFilter;               
		pDstWMVDec->m_bXintra8Switch = pSrcWMVDec->m_bXintra8Switch;            
		pDstWMVDec->m_bMultiresEnabled = pSrcWMVDec->m_bMultiresEnabled;          
		pDstWMVDec->m_b16bitXform = pSrcWMVDec->m_b16bitXform;               
		pDstWMVDec->m_bUVHpelBilinear = pSrcWMVDec->m_bUVHpelBilinear;           
		pDstWMVDec->m_bExtendedMvMode = pSrcWMVDec->m_bExtendedMvMode;           
		pDstWMVDec->m_iDQuantCodingOn = pSrcWMVDec->m_iDQuantCodingOn;           
		pDstWMVDec->m_bXformSwitch = pSrcWMVDec->m_bXformSwitch;              
		pDstWMVDec->m_bDCTTable_MB_ENABLED = pSrcWMVDec->m_bDCTTable_MB_ENABLED;      
		pDstWMVDec->m_bSequenceOverlap = pSrcWMVDec->m_bSequenceOverlap;          
		pDstWMVDec->m_bStartCode = pSrcWMVDec->m_bStartCode;                
		pDstWMVDec->m_bPreProcRange = pSrcWMVDec->m_bPreProcRange;             
		pDstWMVDec->m_iNumBFrames = pSrcWMVDec->m_iNumBFrames;               
		pDstWMVDec->m_bExplicitSeqQuantizer = pSrcWMVDec->m_bExplicitSeqQuantizer;     
		pDstWMVDec->m_bUse3QPDZQuantizer = pSrcWMVDec->m_bUse3QPDZQuantizer;        
		pDstWMVDec->m_bExplicitQuantizer = pSrcWMVDec->m_bExplicitQuantizer;        
		pDstWMVDec->m_bSeqFrameInterpolation = pSrcWMVDec->m_bSeqFrameInterpolation;    
		pDstWMVDec->m_iBFrameReciprocal = pSrcWMVDec->m_iBFrameReciprocal;         
		pDstWMVDec->m_bXintra8Switch = pSrcWMVDec->m_bXintra8Switch;            
		pDstWMVDec->m_bXintra8 = pSrcWMVDec->m_bXintra8; 
		pDstWMVDec->m_iMVRangeIndex = pSrcWMVDec->m_iMVRangeIndex; 
		pDstWMVDec->m_iLogXRange = pSrcWMVDec->m_iLogXRange;           
		pDstWMVDec->m_iLogYRange = pSrcWMVDec->m_iLogYRange; 
		pDstWMVDec->m_iXMVRange = pSrcWMVDec->m_iXMVRange; 
		pDstWMVDec->m_iYMVRange = pSrcWMVDec->m_iYMVRange; 
		pDstWMVDec->m_iXMVFlag = pSrcWMVDec->m_iXMVFlag; 
		pDstWMVDec->m_iYMVFlag = pSrcWMVDec->m_iYMVFlag; 
		pDstWMVDec->m_iBetaRTMMismatchIndex = pSrcWMVDec->m_iBetaRTMMismatchIndex; 
		pDstWMVDec->m_iBetaRTMMismatchIndex = pSrcWMVDec->m_iBetaRTMMismatchIndex; 
		pDstWMVDec->m_bPostProcInfoPresent = pSrcWMVDec->m_bPostProcInfoPresent; 
	}
	return 0;
}

static VO_S32 CopyFIFOBuffer(tWMVDecInternalMember* pDstWMVDec,tWMVDecInternalMember* pSrcWMVDec)
{
	VO_S32 i=0;
	pDstWMVDec->m_pfrmList = pSrcWMVDec->m_pfrmList;
	pDstWMVDec->m_memManageFIFO.img_seq = pSrcWMVDec->m_memManageFIFO.img_seq;                                                
	for(i=0;i<pSrcWMVDec->ntotal_frames;i++)                                  
	{                                                                   
		pDstWMVDec->m_pfrmList[i]= pSrcWMVDec->m_pfrmList[i];                                               
		pDstWMVDec->m_memManageFIFO.img_seq[i]= pSrcWMVDec->m_memManageFIFO.img_seq[i];                                  
	}                                                                     
	pDstWMVDec->m_memManageFIFO.img_seq[pSrcWMVDec->ntotal_frames-3] = pSrcWMVDec->m_memManageFIFO.img_seq[pSrcWMVDec->ntotal_frames-3] = NULL;	    
	pDstWMVDec->m_pfrmCurrQ= pSrcWMVDec->m_pfrmCurrQ; 
	pDstWMVDec->m_nIdx= pSrcWMVDec->m_nIdx; 
	pDstWMVDec->m_pfrmRef0Q= pSrcWMVDec->m_pfrmRef0Q; 
	pDstWMVDec->m_pfrmRef1Q= pSrcWMVDec->m_pfrmRef1Q; 
	pDstWMVDec->m_pfrmPostQ= pSrcWMVDec->m_pfrmPostQ; 
	pDstWMVDec->m_pfrmPostPrevQ= pSrcWMVDec->m_pfrmPostPrevQ;                                              
	pDstWMVDec->m_memManageFIFO.r_idx= pSrcWMVDec->m_memManageFIFO.r_idx; 
	pDstWMVDec->m_memManageFIFO.w_idx= pSrcWMVDec->m_memManageFIFO.w_idx; 
	pDstWMVDec->m_memManageFIFO.buf_num= pSrcWMVDec->m_memManageFIFO.buf_num;

	pDstWMVDec->m_pfrmRef0Process= pSrcWMVDec->m_pfrmRef0Process; 
	pDstWMVDec->m_pfrmMultiresQ= pSrcWMVDec->m_pfrmMultiresQ; 
	if (pDstWMVDec->m_bYUV411) 
		pDstWMVDec->m_pfrmYUV411toYUV420Q= pSrcWMVDec->m_pfrmYUV411toYUV420Q; 
    pDstWMVDec->m_ppxliCurrQY= pSrcWMVDec->m_ppxliCurrQY; 
    pDstWMVDec->m_ppxliCurrQU= pSrcWMVDec->m_ppxliCurrQU; 
    pDstWMVDec->m_ppxliCurrQV= pSrcWMVDec->m_ppxliCurrQV; 
    pDstWMVDec->m_ppxliRef0Y= pSrcWMVDec->m_ppxliRef0Y; 
    pDstWMVDec->m_ppxliRef0U= pSrcWMVDec->m_ppxliRef0U;                                                 
    pDstWMVDec->m_ppxliRef0V= pSrcWMVDec->m_ppxliRef0V;                                                 
    pDstWMVDec->m_ppxliRef0YPlusExp= pSrcWMVDec->m_ppxliRef0YPlusExp;                                          
    pDstWMVDec->m_ppxliPostQY= pSrcWMVDec->m_ppxliPostQY;                                                
    pDstWMVDec->m_ppxliPostQU= pSrcWMVDec->m_ppxliPostQU;                                                
    pDstWMVDec->m_ppxliPostQV= pSrcWMVDec->m_ppxliPostQV;                                                
    pDstWMVDec->m_ppxliPostPrevQY= pSrcWMVDec->m_ppxliPostPrevQY;                                            
    pDstWMVDec->m_ppxliPostPrevQU= pSrcWMVDec->m_ppxliPostPrevQU;                                            
    pDstWMVDec->m_ppxliPostPrevQV= pSrcWMVDec->m_ppxliPostPrevQV;                                            
    if (pDstWMVDec->m_pfrmMultiresQ) 
    {                                                                                                                                            
        pDstWMVDec->m_ppxliMultiresY= pSrcWMVDec->m_ppxliMultiresY;                                         
        pDstWMVDec->m_ppxliMultiresU= pSrcWMVDec->m_ppxliMultiresU;                                         
        pDstWMVDec->m_ppxliMultiresV= pSrcWMVDec->m_ppxliMultiresV;                                         
    }                                                                                                                                            
    if (pDstWMVDec->m_pfrmRef1Q)                                             
    {
        pDstWMVDec->m_ppxliRef1Y= pSrcWMVDec->m_ppxliRef1Y;                                             
        pDstWMVDec->m_ppxliRef1U= pSrcWMVDec->m_ppxliRef1U;           
        pDstWMVDec->m_ppxliRef1V= pSrcWMVDec->m_ppxliRef1V;           
        pDstWMVDec->m_ppxliRef1YPlusExp= pSrcWMVDec->m_ppxliRef1YPlusExp;    
    }
	return 0;
}

VO_S32 CopyDisplayParametes(tWMVDecInternalMember* pDstWMVDec,tWMVDecInternalMember* pSrcWMVDec)
{
	//pDstWMVDec->m_pfrmCurrQ = pSrcWMVDec->m_pfrmCurrQ;
	pDstWMVDec->m_iRangeState = pSrcWMVDec->m_iRangeState;
	//pDstWMVDec->m_pfrmDisplayPre = pSrcWMVDec->m_pfrmDisplayPre;
	pDstWMVDec->m_bPrevAnchor = pSrcWMVDec->m_bPrevAnchor;
	pDstWMVDec->m_pvopcRenderThis = pSrcWMVDec->m_pvopcRenderThis ;
	pDstWMVDec->m_iDroppedPFrame = pSrcWMVDec->m_iDroppedPFrame;
	pDstWMVDec->m_tFrmType = pSrcWMVDec->m_tFrmType;
	pDstWMVDec->m_bRenderDirect = pSrcWMVDec->m_bRenderDirect;
	//pDstWMVDec->m_memManageFIFO = pSrcWMVDec->m_memManageFIFO;
	//pDstWMVDec->m_pfrmPostQ = pSrcWMVDec->m_pfrmPostQ;
	return 0;
}

tWMVDecodeStatus WMVVideoDecVOPHeaderNew(tWMVDecInternalMember* pGlobleWMVDec, tWMVDecInternalMember *pWMVDec)
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    U32_WMV uiBufferLengthFromCaller = 0;
    U8_WMV* pucBufferBits = NULL;
    Bool_WMV bNotEndOfFrame = TRUE; 

	//UPDATE thread ctx from Globle
	UpdateThreadFIFONew(pGlobleWMVDec,pWMVDec);

	pWMVDec->m_pfrmPrevious = pGlobleWMVDec->m_pfrmPrevious;

	pWMVDec->m_bLuminanceWarp = 0;
    
    WMVDecCBGetData (pWMVDec, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame) {
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL) {       
            return WMV_BrokenFrame;
        }
    }
    
    if (uiBufferLengthFromCaller == 0) {
        pWMVDec->m_bRenderDirect = TRUE_WMV;
		pGlobleWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;
        return WMV_Succeeded;
    }
    else if (uiBufferLengthFromCaller == 1 && bNotEndOfFrame == FALSE_WMV) {       
        if(pWMVDec->m_cvCodecVersion == WMV3) {
            if (pWMVDec->m_iNumBFrames == 0) //getNumBFrames
            {
				pGlobleWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                return WMV_Succeeded;
            }
            else{ 
                decodeSkipP(pWMVDec);
				pGlobleWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                return WMV_Succeeded;
            }
        }
    }
    

    if (pWMVDec->m_cvCodecVersion == WMVA)
    {
		pWMVDec->bNewheader = 0;
        tWMVStatus = ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
		if(pWMVDec->bNewheader == 0)
			pWMVDec->m_bSeqFrameWidthOrHeightChanged = 0;

        if (tWMVStatus != WMV_Succeeded) {
			pWMVDec->sequence_errorflag = 1;
				return tWMVStatus;
			}
    }

	pGlobleWMVDec->bNewheader = pWMVDec->bNewheader;
	if(pWMVDec->m_bSeqFrameWidthOrHeightChanged)
		pGlobleWMVDec->bNewSize = VO_TRUE;

	
//zou 66
#ifdef STABILITY
	if(pWMVDec->entrypoint_errorflag == 1 || pWMVDec->sequence_errorflag == 1)
		return -1;
#endif

    if (uiBufferLengthFromCaller == 0 && bNotEndOfFrame  == FALSE) {
        pWMVDec->m_bRenderDirect = TRUE_WMV;
		pGlobleWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;
        return WMV_Succeeded;
    }

    BS_reset (pWMVDec->m_pInputBitstream, 
                (U8_WMV*)pucBufferBits, 
                uiBufferLengthFromCaller, 
                bNotEndOfFrame, 
                pWMVDec->m_cvCodecVersion == WMVA);

    pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;

    pWMVDec->m_t++;
    pWMVDec->m_bRenderDirect = FALSE_WMV;
    
    //Is this the right place? 
    pWMVDec->m_uiCurrSlice = 0;

    pWMVDec->m_bSkipThisFrame = FALSE;
    if ( !pWMVDec->m_bOmittedBFrame )
        pWMVDec->m_tFrmTypePriorFrame = pWMVDec->m_tFrmType;
    else
        pWMVDec->m_bOmittedBFrame = FALSE_WMV;
    
    
    // Reset DCT tables
    if (pWMVDec->m_cvCodecVersion >= WMV3)   {
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;        
        // reset mode
        pWMVDec->m_iSkipbitCodingMode = NOT_DECIDED;
        pWMVDec->m_iMVSwitchCodingMode = NOT_DECIDED;
        pWMVDec->m_iACPredIFrameCodingMode = NOT_DECIDED;
        pWMVDec->m_iDirectCodingMode = NOT_DECIDED;
        //pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
        pWMVDec->m_iOverlapIMBCodingMode = NOT_DECIDED;
    }
    
    //printf("pWMVDec->m_cvCodecVersion = %d \n",pWMVDec->m_cvCodecVersion);
    if (pWMVDec->m_cvCodecVersion != WMVA) {
        tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);
        if (tWMVStatus != WMV_Succeeded)
			return tWMVStatus;
    } 
	else 
    {       
        WMVFrameType iFrameType;
        if (pWMVDec->m_bInterlacedSource) {
            if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
                iFrameType = PROGRESSIVE;     // Progressive
            }
            else {
                if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
                    iFrameType = INTERLACEFRAME; // Frame Interlace
                else    
                    iFrameType = INTERLACEFIELD; // Field interlace
            }
        }
        else 
            iFrameType = PROGRESSIVE;    // Progressive

		/* It will crash with VC1 INTERLACEFIELD mode,so don't support it temperarily. */
		if(iFrameType == INTERLACEFIELD && pWMVDec->m_bCodecIsWVC1) {            
            return WMV_Failed;
		}
        pWMVDec->m_bInterlaceV2 = FALSE;

        if (iFrameType == PROGRESSIVE) 
        {// Progressive Frame
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Prog;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Prog;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Prog;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Prog;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Prog;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Prog;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Prog;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Prog;            

            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = FALSE; 
            SwapMVTables (pWMVDec); // set tables    

            if (ICERR_OK != decodeVOPHeadProgressiveWMVA (pWMVDec))
			{
                return WMV_Failed;
			}
        } 
		else if (iFrameType == INTERLACEFRAME){//Frame Interlace 
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Interlace;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Interlace;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Interlace;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Interlace;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Interlace;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Interlace;
            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = TRUE;
            SwapMVTables (pWMVDec); // set tables

			if(decodeVOPHeadInterlaceV2 (pWMVDec) != ICERR_OK)
                return WMV_Failed;
        } 
		else { //FILED  
            pWMVDec->m_bInterlaceV2 = 3;            
            return WMV_Failed;
        }
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
    }

    pWMVDec->m_bFirstEscCodeInFrame = TRUE_WMV;
    pWMVDec->m_bBFrameDropped = FALSE_WMV;


	if(pWMVDec->m_nframes == 0 || pWMVDec->m_bSeqFrameWidthOrHeightChanged || pWMVDec->bNewSize)
    {
		if(pWMVDec->m_iMaxFrmWidthSrc < pWMVDec->m_iFrmWidthSrc || pWMVDec->m_iMaxFrmHeightSrc < pWMVDec->m_iFrmHeightSrc)
			return -1;
        initModeDependentMemory(pWMVDec,pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
        initFrameWidthAndHeightDependentPointers(pWMVDec);
    }
	pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;
	pWMVDec->bNewSize = FALSE;

	//TODO
	if(pWMVDec->nInputFrameIndx > 1)
	{
		pWMVDec->m_pFieldMvX_Pbuf = pGlobleWMVDec->m_pFieldMvX_Pbuf;
		pWMVDec->m_pFieldMvY_Pbuf = pGlobleWMVDec->m_pFieldMvY_Pbuf;
		//pWMVDec->m_pfrmCurrQ->pFieldMvX_Pbuf = pWMVDec->m_pFieldMvX_Pbuf;
		//pWMVDec->m_pfrmCurrQ->pFieldMvY_Pbuf = pWMVDec->m_pFieldMvY_Pbuf;
	}

#ifdef STABILITY
    if(pWMVDec->m_pX9dct == NULL)
        return -1;
#endif

	if(pWMVDec->nInputFrameIndx == 1 && pWMVDec->m_tFrmType != IVOP)
		return -1;

	if(pWMVDec->m_tFrmType == SKIPFRAME)
	{
		pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);
		pGlobleWMVDec->m_bPrevAnchor = pGlobleWMVDec->m_bPrevAnchor;
		pWMVDec->m_pfrmCurrQ = pGlobleWMVDec->m_pfrmCurrQ;
		pWMVDec->bUseRef0Process = 0;
	}
	else if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
	{
		pWMVDec->error_flag_main = 0;
        pWMVDec->error_flag_thread = 0;
		pWMVDec->frame_errorflag = 0;
		if((tWMVStatus= voVC1ParpareBufferIVOPNew(pGlobleWMVDec,pWMVDec)) != WMV_Succeeded)
			return tWMVStatus;	
		pWMVDec->bUseRef0Process = 0;
	}
	else if (pWMVDec->m_tFrmType == PVOP)
	{
		if(pWMVDec->error_flag_main || pWMVDec->error_flag_thread)
                return -1;
		if((tWMVStatus = voVC1ParpareBufferPVOPNew(pGlobleWMVDec,pWMVDec))!= WMV_Succeeded)
			return tWMVStatus;
	}
	else if (pWMVDec->m_tFrmType == BVOP)
	{
		if(pWMVDec->error_flag_main || pWMVDec->error_flag_thread)
                return -1;
		if((tWMVStatus = voVC1ParpareBufferBVOPNew(pGlobleWMVDec,pWMVDec))!= WMV_Succeeded)
			return tWMVStatus;
	}

	//post process
	//just allocate the post buffer and set the status
#ifdef ADJ_NEW
	if (!pWMVDec->m_iDroppedPFrame && pWMVDec->m_bRenderDirect != TRUE_WMV)
	{
		pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeAdvNeed = 0;
		pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeNeed = 0;

		if (pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag)) 
		{//  
			if(CopyCurrToPostBefore_new (pWMVDec) != WMV_Succeeded)
				return -1;
			//AdjustDecRangeWMVA(pWMVDec);
			pWMVDec->m_pfrmPostQ->m_AdjustDecRangeAdvNeed = 1;
			pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeAdvNeed = 1;
		}

		 if (pWMVDec->m_iRangeState)
		{//AdjustDecRange
			if(CopyCurrToPostBefore_new (pWMVDec) != WMV_Succeeded)
				return -1;
			//AdjustDecRange(pWMVDec);
			pWMVDec->m_pfrmPostQ->m_AdjustDecRangeNeed = 1;
			pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeNeed = 1;
		}
	}
#endif

	pGlobleWMVDec->m_pfrmPrevious = pWMVDec->m_pfrmCurrQ;

	return WMV_Succeeded;
}

tWMVDecodeStatus WMVVideoDecVOPHeader(tWMVDecInternalMember *pWMVDec)
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    U32_WMV uiBufferLengthFromCaller = 0;
    U8_WMV* pucBufferBits = NULL;
    Bool_WMV bNotEndOfFrame = TRUE; 
    
    WMVDecCBGetData (pWMVDec, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame) {
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL) {       
            return WMV_BrokenFrame;
        }
    }
    
    if (uiBufferLengthFromCaller == 0) {
        pWMVDec->m_bRenderDirect = TRUE_WMV;
        return WMV_Succeeded;
    }
    else if (uiBufferLengthFromCaller == 1 && bNotEndOfFrame == FALSE_WMV) {       
        if(pWMVDec->m_cvCodecVersion == WMV3) {
            if (pWMVDec->m_iNumBFrames == 0) //getNumBFrames
            {
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                return WMV_Succeeded;
            }
            else{ 
                decodeSkipP(pWMVDec);
                pWMVDec->m_bRenderDirect = TRUE_WMV;
                return WMV_Succeeded;
            }
        }
    }
    
    if (pWMVDec->m_cvCodecVersion == WMVA)
    {
        tWMVStatus = ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
        if (tWMVStatus != WMV_Succeeded) {
			pWMVDec->sequence_errorflag = 1;
				return tWMVStatus;
			}
    }
//zou 66
#ifdef STABILITY
	if(pWMVDec->entrypoint_errorflag == 1 || pWMVDec->sequence_errorflag == 1)
		return -1;
#endif

    if (uiBufferLengthFromCaller == 0 && bNotEndOfFrame  == FALSE) {
        pWMVDec->m_bRenderDirect = TRUE_WMV;
        return WMV_Succeeded;
    }

    BS_reset (pWMVDec->m_pInputBitstream, 
                (U8_WMV*)pucBufferBits, 
                uiBufferLengthFromCaller, 
                bNotEndOfFrame, 
                pWMVDec->m_cvCodecVersion == WMVA);

    pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;

    pWMVDec->m_t++;
    pWMVDec->m_bRenderDirect = FALSE_WMV;
    
    //Is this the right place? 
    pWMVDec->m_uiCurrSlice = 0;

    pWMVDec->m_bSkipThisFrame = FALSE;
    if ( !pWMVDec->m_bOmittedBFrame )
        pWMVDec->m_tFrmTypePriorFrame = pWMVDec->m_tFrmType;
    else
        pWMVDec->m_bOmittedBFrame = FALSE_WMV;
    
    
    // Reset DCT tables
    if (pWMVDec->m_cvCodecVersion >= WMV3)   {
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;        
        // reset mode
        pWMVDec->m_iSkipbitCodingMode = NOT_DECIDED;
        pWMVDec->m_iMVSwitchCodingMode = NOT_DECIDED;
        pWMVDec->m_iACPredIFrameCodingMode = NOT_DECIDED;
        pWMVDec->m_iDirectCodingMode = NOT_DECIDED;
        //pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
        pWMVDec->m_iOverlapIMBCodingMode = NOT_DECIDED;
    }
    
    //printf("pWMVDec->m_cvCodecVersion = %d \n",pWMVDec->m_cvCodecVersion);
    if (pWMVDec->m_cvCodecVersion != WMVA) {
        tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);
        if (tWMVStatus != WMV_Succeeded)
			return tWMVStatus;
    } 
	else 
    {       
        WMVFrameType iFrameType;
        if (pWMVDec->m_bInterlacedSource) {
            if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
                iFrameType = PROGRESSIVE;     // Progressive
            }
            else {
                if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
                    iFrameType = INTERLACEFRAME; // Frame Interlace
                else    
                    iFrameType = INTERLACEFIELD; // Field interlace
            }
        }
        else 
            iFrameType = PROGRESSIVE;    // Progressive

		/* It will crash with VC1 INTERLACEFIELD mode,so don't support it temperarily. */
		if(iFrameType == INTERLACEFIELD && pWMVDec->m_bCodecIsWVC1) {            
            return WMV_Failed;
		}
        pWMVDec->m_bInterlaceV2 = FALSE;

        if (iFrameType == PROGRESSIVE) 
        {// Progressive Frame
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Prog;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Prog;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Prog;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Prog;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Prog;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Prog;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Prog;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Prog;            

            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = FALSE; 
            SwapMVTables (pWMVDec); // set tables    

            if (ICERR_OK != decodeVOPHeadProgressiveWMVA (pWMVDec))
                return WMV_Failed;
        } 
		else if (iFrameType == INTERLACEFRAME){//Frame Interlace 
            pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW_Interlace;
            pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW_Interlace;
            pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv_Interlace;
            pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv_Interlace;
            pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated_Interlace;
            pWMVDec->m_p4x4ZigzagInv = grgi4x4ZigzagInv_Interlace;
            pWMVDec->m_p4x4ZigzagInvRotated = grgi4x4ZigzagInvRotated_Interlace;
            decideMMXRoutines_ZigzagOnly(pWMVDec);

            pWMVDec->m_bInterlaceV2 = TRUE;
            SwapMVTables (pWMVDec); // set tables

			if(decodeVOPHeadInterlaceV2 (pWMVDec) != ICERR_OK)
                return WMV_Failed;
        } 
		else { //FILED  
            pWMVDec->m_bInterlaceV2 = 3;            
            return WMV_Failed;
        }
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p8x4ZigzagScanOrder, pWMVDec->m_p8x4ZigzagSSIMD, 32);
        EMB_InitZigZagTable_SSIMD(pWMVDec->m_p4x4ZigzagScanOrder, pWMVDec->m_p4x4ZigzagSSIMD, 16);
    }

    pWMVDec->m_bFirstEscCodeInFrame = TRUE_WMV;
    pWMVDec->m_bBFrameDropped = FALSE_WMV;

	//ZOU TODO
	if(pWMVDec->m_nframes == 0 || pWMVDec->m_bSeqFrameWidthOrHeightChanged)
    {
		if(pWMVDec->m_iMaxFrmWidthSrc < pWMVDec->m_iFrmWidthSrc || pWMVDec->m_iMaxFrmHeightSrc < pWMVDec->m_iFrmHeightSrc)
			return -1;
        initModeDependentMemory(pWMVDec,pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
        initFrameWidthAndHeightDependentPointers(pWMVDec);
    }
	pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;

#ifdef STABILITY
    if(pWMVDec->m_pX9dct == NULL)
        return -1;
#endif

	voprintf("[threads%d] pWMVDec->m_tFrmType  = %d  m_pfrmCurrQ = 0x%x \n",pWMVDec->nthreads, pWMVDec->m_tFrmType , pWMVDec->m_pfrmCurrQ);

	
	if(pWMVDec->m_tFrmType == SKIPFRAME)
	{
		pWMVDec->m_tFrmTypePrevious = pWMVDec->m_tFrmType;
		pWMVDec->bUseRef0Process = 0;
	}
	else if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
	{
		if((tWMVStatus= voVC1ParpareBufferIVOP(pWMVDec)) != WMV_Succeeded)
			return tWMVStatus;	
		pWMVDec->m_tFrmTypePrevious = pWMVDec->m_tFrmType;
		pWMVDec->bUseRef0Process = 0;
	}
	else if (pWMVDec->m_tFrmType == PVOP)
	{
		if(pWMVDec->error_flag_main || pWMVDec->error_flag_thread)
                return -1;
		if((tWMVStatus = voVC1ParpareBufferPVOP(pWMVDec))!= WMV_Succeeded)
			return tWMVStatus;
		pWMVDec->m_tFrmTypePrevious = pWMVDec->m_tFrmType;
	}
	else if (pWMVDec->m_tFrmType == BVOP)
	{
		if(pWMVDec->error_flag_main || pWMVDec->error_flag_thread)
                return -1;
		if((tWMVStatus = voVC1ParpareBufferBVOP(pWMVDec))!= WMV_Succeeded)
			return tWMVStatus;
	}

	return WMV_Succeeded;
}


#ifdef SHAREMEMORY
tYUV420Frame_WMV*  buf_seq_ctl(FIFOTYPE *priv, tYUV420Frame_WMV* frame , const VO_U32 flag)
{
	tYUV420Frame_WMV *temp = NULL;
	VO_U32 w_idx = 0;

	w_idx = priv->w_idx;    /* Save the old index before proceeding */

	//printf("buf_seq_ctl ==== W %d  R %d \n",priv->w_idx,priv->r_idx);

	if (FIFO_WRITE == flag)
	{ /*write data*/

		int i =0;	

		//printf("[buf_seq_ctl] W------------0x%x \n",frame);
		if(NULL != frame)
		{
			/* Save it to buffer */
			if (((priv->w_idx + 1) % (priv->buf_num + 1)) == priv->r_idx)
			{
				/* Adjust read index since buffer is full */
				/* Keep the latest one and drop the oldest one */
				priv->r_idx = (priv->r_idx + 1) % (priv->buf_num + 1);
			}
			priv->img_seq[priv->w_idx] = frame;
			priv->w_idx = (priv->w_idx + 1) % (priv->buf_num + 1);
		}

		//for(i=0;i<priv->buf_num+1;i++)
		//	printf("0x%x ",priv->img_seq[i]);

	}
	else
	{  /*read data*/
#if 1
		if ((priv->r_idx == w_idx))
		{
			return NULL;/*there is no free buffer*/
		}
		//else
#endif
		{
			temp = priv->img_seq[priv->r_idx];
			//temp->m_AdjustDecRangeAdvNeed = 0;
			//temp->m_AdjustDecRangeNeed = 0;
			//printf("[buf_seq_ctl] R----%d--------0x%x \n",priv->r_idx,temp);
			priv->r_idx = (priv->r_idx + 1) % (priv->buf_num + 1);
		}
	}
	return temp;
}


I32_WMV voVC1GetFreeBufferForCur(tWMVDecInternalMember*pWMVDec)
{
	pWMVDec->m_nIdx++;
	if(pWMVDec->m_nIdx == pWMVDec->ntotal_frames-4)
		pWMVDec->m_nIdx = 0;
	pWMVDec->m_pfrmCurrQ =  buf_seq_ctl(&pWMVDec->m_memManageFIFO, NULL, FIFO_READ);
	if(pWMVDec->m_pfrmCurrQ == NULL)
	{
		reset_buf_seq(pWMVDec);
		return WMV_BadMemory;
	}

#ifdef USE_FRAME_THREAD
	pWMVDec->m_pfrmCurrQ->m_decodeprocess = -5;
#endif
	pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
	pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
	pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;
	return WMV_Succeeded;
}
#endif

I32_WMV gcd(I32_WMV m, I32_WMV n)
{
	if (m == 0)
		return n;
	if (n == 0)
		return m;
	if (m < n)
	{
		int tmp = m;
		m = n;
		n = tmp;
	}
	while (n != 0)
	{
		int tmp = m % n;
		m = n;
		n = tmp;
	}
	return m;
}

static I32_WMV GetAspectRatio(tWMVDecInternalMember*pWMVDec)
{
	I32_WMV width,height;
	I32_WMV iaspectratio = pWMVDec->tDimensions.iAspectRatio;
	switch (iaspectratio)
	{
	case 2: //12:11
		{
			width = pWMVDec->m_iFrmWidthSrc*12;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 3: //10:11
		{
			width = pWMVDec->m_iFrmWidthSrc*10;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 4: //16:11
		{
			width = pWMVDec->m_iFrmWidthSrc*16;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 5: //40:33
		{
			width = pWMVDec->m_iFrmWidthSrc*40;
		    height = pWMVDec->m_iFrmHeightSrc*33;
			break;
		}
	case 6: //24:11
		{
			width = pWMVDec->m_iFrmWidthSrc*24;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 7: //20:11
		{
			width = pWMVDec->m_iFrmWidthSrc*20;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 8: //32:11
		{
			width = pWMVDec->m_iFrmWidthSrc*10;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 9: //80:33
		{
			width = pWMVDec->m_iFrmWidthSrc*80;
		    height = pWMVDec->m_iFrmHeightSrc*33;
			break;
		}
	case 10: //18:11
		{
			width = pWMVDec->m_iFrmWidthSrc*18;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 11: //15:11
		{
			width = pWMVDec->m_iFrmWidthSrc*15;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 12: //64:33
		{
			width = pWMVDec->m_iFrmWidthSrc*64;
		    height = pWMVDec->m_iFrmHeightSrc*33;
			break;
		}
	case 13: //160:99
		{
			width = pWMVDec->m_iFrmWidthSrc*10;
		    height = pWMVDec->m_iFrmHeightSrc*11;
			break;
		}
	case 15: //160:99
		{
			width = pWMVDec->m_iFrmWidthSrc*pWMVDec->tDimensions.iAspect15HorizSize;
		    height = pWMVDec->m_iFrmHeightSrc*pWMVDec->tDimensions.iAspect15VertSize;
			break;
		}
	default:
		return 0;
	}
	
	if (width == height)
	{
		return VO_RATIO_11;
	}
	else if (width*3 == height*4)
	{
		return VO_RATIO_43;
	}
	else if (width*9 == height*16)
	{
		return VO_RATIO_169;
	}
	else if (width == height*2)
	{
		return VO_RATIO_21;
	}
	else
	{
		int m = gcd(width, height);
		if(m)
		{
			width = width/m;
			height = height/m;
		}
		if(width > 0x7fff || height > 0x7fff)
		{
			width = (width + 512) >> 10;
			height = (height + 512) >> 10;
		}
		return ((width<<16)|height);
	}
}

void V9ECBReset(tWMVDecInternalMember *pWMVDec, U8_WMV * pBuffer, int iSize)
{
    pWMVDec->g_StrmModedataCtl.iBytesReadFrm = iSize;
    pWMVDec->g_StrmModedataCtl.iCounter = 0;
    pWMVDec->g_StrmModedataCtl.pVideoBuffer = pBuffer;
}

#define GETDATA_BLOCKSIZE 0x00070800

tWMVDecodeStatus V9ECBGetData (StreamingModeDataCtl* pStrmModedataCtl,
								U32_WMV   uintPadBeginning,
								U8_WMV  **ppchBuffer, 
								U32_WMV   uintUserDataLength,
								U32_WMV  *puintActualBufferLength,
								Bool_WMV *pbNotEndOfFrame
							  )
{
    if (pStrmModedataCtl->iBytesReadFrm > 0) {
        I32_WMV iBytesAvailable;
        if (pStrmModedataCtl->iBytesReadFrm > GETDATA_BLOCKSIZE) {
            *ppchBuffer = pStrmModedataCtl->pVideoBuffer + pStrmModedataCtl->iCounter * GETDATA_BLOCKSIZE - uintPadBeginning;
            pStrmModedataCtl->iCounter++;
            pStrmModedataCtl->iBytesReadFrm -= GETDATA_BLOCKSIZE;
            iBytesAvailable = GETDATA_BLOCKSIZE;
            * pbNotEndOfFrame = TRUE_WMV;
        } 
		else {
			*ppchBuffer = pStrmModedataCtl->pVideoBuffer + pStrmModedataCtl->iCounter * GETDATA_BLOCKSIZE - uintPadBeginning;
            pStrmModedataCtl->iCounter = 0;
            iBytesAvailable = pStrmModedataCtl->iBytesReadFrm;
            pStrmModedataCtl->iBytesReadFrm = 0;
            * pbNotEndOfFrame = FALSE_WMV;
        }
        *puintActualBufferLength = iBytesAvailable + uintPadBeginning;
        return WMV_Succeeded;
    } 
	else {
        *ppchBuffer = NULL;
        *puintActualBufferLength = 0;
        *pbNotEndOfFrame = FALSE_WMV;
        return WMV_EndOfFrame;
    }
}

tWMVDecodeStatus WMVDecCBGetData ( HWMVDecoder hWMVDecoder,
								   U32_WMV   uintPadBeginning, 
								   U8_WMV  **ppchBuffer, 
								   U32_WMV   uintUserDataLength, 
                                   U32_WMV  *puintActualBufferLength, 
								   Bool_WMV *pbNotEndOfFrame 
								   )
{
	tWMVDecInternalMember*pWMVDec = (tWMVDecInternalMember*)hWMVDecoder;
	return V9ECBGetData (&pWMVDec->g_StrmModedataCtl, uintPadBeginning, ppchBuffer, uintUserDataLength, puintActualBufferLength, pbNotEndOfFrame );
}


#ifdef SHAREMEMORY
tWMVDecodeStatus GetOutputParamThreadBefore ( HWMVDecoder   hWMVDecoder,
								U8_WMV **pOutBuff,
								I32_WMV *pStride,
								I32_WMV iWidthSource,
								I32_WMV iHeightSource )
{
	I32_WMV  StrideY  = 0, StrideU  = 0, StrideV = 0;
    I32_WMV  iOffsetX = 0, iOffsetY = 0;
    I32_WMV  iPadding = 64;   
	I32_WMV  iFullSrcWidth;
	I32_WMV  iFullSrcHeight;
	I32_WMV  iSrcUVStride;
	I32_WMV  iSrcYOffset;
	I32_WMV  iSrcUOffset;
	I32_WMV  iSrcVOffset;
	U8_WMV *ppxliRenderThisY = NULL;
	U8_WMV *ppxliRenderThisU = NULL;
	U8_WMV *ppxliRenderThisV = NULL;
	tWMVDecInternalMember *pWMVDec = NULL; 
	tYUV420Frame_WMV* pCurrFrame = NULL;

	if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
	pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
	if(pWMVDec->frame_errorflag)
		return -1;
	//pWMVDec->m_bRenderFromPostBuffer = FALSE;

	voprintf(" GetOutputParam_NEW m_pfrmCurrQ = 0x%x\n",pWMVDec->m_pfrmCurrQ);

	if(pWMVDec->m_pfrmCurrQ == NULL)
		return -1;

	pCurrFrame = pWMVDec->m_pfrmCurrQ;

	//printf(" %d %d \n",pWMVDec->m_iDroppedPFrame,pWMVDec->m_bRenderDirect );
	
#ifdef ADJ_NEW
	if (!pWMVDec->m_iDroppedPFrame && pWMVDec->m_bRenderDirect != TRUE_WMV)
	{
		if ((pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag))
			|| pWMVDec->m_iRangeState) 
		{
			pCurrFrame = pWMVDec->m_pfrmPostQ;
		}
	}
	else
	{
		;//printf("HEIHEIHEI \n");
	}
#else
	if (pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag)) 
	{  //AdjustDecRangeWMVA
		pCurrFrame->m_AdjustDecRangeAdvNeed = 1; //ZOU MASK
	}
	 else
		 pCurrFrame->m_AdjustDecRangeAdvNeed = 0;
	
	if (pWMVDec->m_iRangeState) 
		pCurrFrame->m_AdjustDecRangeNeed = 1; ////ZOU MASK
	else
		pCurrFrame->m_AdjustDecRangeNeed = 0;
#endif

	 if(pWMVDec->m_frameNum == 0 && pWMVDec->m_outputMode == 0) // delay a frame
	 {
		 pWMVDec->m_pfrmDisplayPre = pCurrFrame;
		 pWMVDec->m_pvopcRenderThis = NULL;

		 pWMVDec->m_pfrmDisplayPreBak = pWMVDec->m_pfrmCurrQ;
		 pWMVDec->m_pvopcRenderThisBak = NULL;

		 pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);
		 voprintf(" pWMVDec->m_pvopcRenderThis = 0x%x   0x%x  return\n",pWMVDec->m_pvopcRenderThis,pWMVDec->m_pfrmDisplayPre);
		 return 0;
	 }

	 voprintf("pWMVDec->m_pfrmDisplayPre = 0x%x \n",pWMVDec->m_pfrmDisplayPre);
	 voprintf("pWMVDec->m_tFrmType = %d %d %d %d",pWMVDec->m_tFrmType,pWMVDec->m_outputMode,pWMVDec->m_iDroppedPFrame,pWMVDec->m_bRenderDirect);

	 if(pWMVDec->m_outputMode)
	  {
		  pWMVDec->m_pvopcRenderThis = pCurrFrame;
		   pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
		  goto POSTPROCESS;
	  }

	 if (pWMVDec->m_iDroppedPFrame) 
	 {  //use last P frame
		 if (pWMVDec->m_tFrmType == SKIPFRAME && pWMVDec->m_bPrevAnchor)
		 {
			 pWMVDec->m_pvopcRenderThis = pCurrFrame;
			 pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
		 }
		 else
		 {
			 pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmDisplayPre;
			 pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmDisplayPreBak;
		 }
		 goto RENDER;
	 }

	if ( pWMVDec->m_bRenderDirect == TRUE_WMV) //skip
	{  //use current frame
		pWMVDec->m_pfrmDisplayPre = pCurrFrame;
		pWMVDec->m_pvopcRenderThis = pCurrFrame;

		pWMVDec->m_pfrmDisplayPreBak = pWMVDec->m_pfrmCurrQ;
		pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
		goto RENDER;
	}

      //******************************/
	  //Normal Case
	  if(pWMVDec->m_tFrmType == BVOP || pWMVDec->m_tFrmType == BIVOP )
	  {//render directly
		  pWMVDec->m_pvopcRenderThis = pCurrFrame;
		  pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
	  }
	  else
	  {
		  pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmDisplayPre;
		  pWMVDec->m_pfrmDisplayPre = pCurrFrame;

		  pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmDisplayPreBak;
		  pWMVDec->m_pfrmDisplayPreBak = pWMVDec->m_pfrmCurrQ;
	  }

	  
POSTPROCESS:
	   if(pWMVDec->m_pvopcRenderThis == 0)
		  return -1;	  

#ifdef ADJ_NEW 
	  if(!pWMVDec->m_iDroppedPFrame)
	  { 
		  if(pWMVDec->m_pvopcRenderThis->m_frameType != BVOP)
		  {
			  if ( pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeNeed == 1 || pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeAdvNeed ==1) 
				buf_seq_ctl(&pWMVDec->m_memManageFIFO,pWMVDec->m_pvopcRenderThisBak, FIFO_WRITE);
		  }
	  }
	  else
	  {
		  printf("HEHEHEHEHHE \n");
	  }
#else
	  if (pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeNeed == 1) 
	  {//AdjustDecRange  
		  if(CopyCurrToPostBefore (pWMVDec) != WMV_Succeeded)
			  return -1;
		  //AdjustDecRange (pWMVDec);
		  pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
		  pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeNeed = 1;
	  }	 

	  if(pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeAdvNeed ==1)
	  {
		  if(CopyCurrToPostBefore (pWMVDec) != WMV_Succeeded)
			  return -1;
		  //AdjustDecRangeWMVA(pWMVDec);
		  pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
		  pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeAdvNeed = 1;
	  }
#endif

	  //zou 
	  if(!pWMVDec->m_iDroppedPFrame)
		  buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pvopcRenderThis, FIFO_WRITE);
	  
	  //printf("m_frameType = %d  \n",pWMVDec->m_pvopcRenderThis->m_frameType);
RENDER:
	  voprintf(" pWMVDec->m_pvopcRenderThis = 0x%X \n",pWMVDec->m_pvopcRenderThis);
	  pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);

	  pWMVDec->m_pvopcRenderThis->m_removeable = 0;

	 return 0;
}

tWMVDecodeStatus GetOutputParamThreadAfter( HWMVDecoder   hWMVDecoder,
								U8_WMV **pOutBuff,
								I32_WMV *pStride,
								I32_WMV iWidthSource,
								I32_WMV iHeightSource )
{
	I32_WMV  StrideY  = 0, StrideU  = 0, StrideV = 0;
    I32_WMV  iOffsetX = 0, iOffsetY = 0;
    I32_WMV  iPadding = 64;   
	I32_WMV  iFullSrcWidth;
	I32_WMV  iFullSrcHeight;
	I32_WMV  iSrcUVStride;
	I32_WMV  iSrcYOffset;
	I32_WMV  iSrcUOffset;
	I32_WMV  iSrcVOffset;
	U8_WMV *ppxliRenderThisY = NULL;
	U8_WMV *ppxliRenderThisU = NULL;
	U8_WMV *ppxliRenderThisV = NULL;
	tWMVDecInternalMember *pWMVDec = NULL; 
	if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
	pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
	if(pWMVDec->frame_errorflag)
		return -1;
	//pWMVDec->m_bRenderFromPostBuffer = FALSE;

	voprintf(" GetOutputParam_NEW m_pfrmCurrQ = 0x%x\n",pWMVDec->m_pfrmCurrQ);

	if(pWMVDec->m_pfrmCurrQ == NULL)
		return -1;

	if(pWMVDec->m_pvopcRenderThis == NULL)
		return 0;
	  
	  //printf("m_frameType = %d  \n",pWMVDec->m_pvopcRenderThis->m_frameType);
	pWMVDec->m_pvopcRenderThis->m_removeable = 1;
	  
	return 0;
}

tWMVDecodeStatus GetOutputParam_NEW ( HWMVDecoder   hWMVDecoder,
								U8_WMV **pOutBuff,
								I32_WMV *pStride,
								I32_WMV iWidthSource,
								I32_WMV iHeightSource )
{
	I32_WMV  StrideY  = 0, StrideU  = 0, StrideV = 0;
    I32_WMV  iOffsetX = 0, iOffsetY = 0;
    I32_WMV  iPadding = 64;   
	I32_WMV  iFullSrcWidth;
	I32_WMV  iFullSrcHeight;
	I32_WMV  iSrcUVStride;
	I32_WMV  iSrcYOffset;
	I32_WMV  iSrcUOffset;
	I32_WMV  iSrcVOffset;
	U8_WMV *ppxliRenderThisY = NULL;
	U8_WMV *ppxliRenderThisU = NULL;
	U8_WMV *ppxliRenderThisV = NULL;
	tWMVDecInternalMember *pWMVDec = NULL; 

	tYUV420Frame_WMV* pCurrFrame = NULL;

	if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
	pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
	if(pWMVDec->frame_errorflag)
		return -1;

	//printf("pWMVDec->m_frameNum = %d  %d %d\n",pWMVDec->m_frameNum,pWMVDec->m_bPreProcRange,pWMVDec->m_tFrmType);

	if(pWMVDec->m_pfrmCurrQ == NULL)
		return -1;

	pCurrFrame = pWMVDec->m_pfrmCurrQ;

#ifdef ADJ_NEW
	if (!pWMVDec->m_iDroppedPFrame && pWMVDec->m_bRenderDirect != TRUE_WMV)
	{
		if ((pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag))
			|| pWMVDec->m_iRangeState) 
			pCurrFrame = pWMVDec->m_pfrmPostQ;
	}
#else
	 if (pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag)) 
	 {  //AdjustDecRangeWMVA
		 pCurrFrame->m_AdjustDecRangeAdvNeed = 1;
	 }
	 else
		 pCurrFrame->m_AdjustDecRangeAdvNeed = 0;
	
	  if (pWMVDec->m_iRangeState) 
		  pCurrFrame->m_AdjustDecRangeNeed = 1;
	  else
		  pCurrFrame->m_AdjustDecRangeNeed = 0;	
#endif

	 if(pWMVDec->m_frameNum == 0 && pWMVDec->m_outputMode == 0) // delay a frame
	 {
		 pWMVDec->m_pfrmDisplayPre = pCurrFrame;
		 pWMVDec->m_pvopcRenderThis = NULL;

		 pWMVDec->m_pfrmDisplayPreBak = pWMVDec->m_pfrmCurrQ;
		 pWMVDec->m_pvopcRenderThisBak = NULL;

		 pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);
		 return 0;
	 }

	 if(pWMVDec->m_outputMode)
	  {
		  pWMVDec->m_pvopcRenderThis = pCurrFrame;
		  pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
		  goto POSTPROCESS;
	  }

	 if (pWMVDec->m_iDroppedPFrame) 
	 {  //use last P frame
		 if (pWMVDec->m_tFrmType == SKIPFRAME && pWMVDec->m_bPrevAnchor)
		 {
			 pWMVDec->m_pvopcRenderThis = pCurrFrame;
			 pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
		 }
		 else
		 {
			 pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmDisplayPre;
			 pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmDisplayPreBak;
		 }
		 goto RENDER;
	 }

	if ( pWMVDec->m_bRenderDirect == TRUE_WMV) //skip
	{  //use current frame
		pWMVDec->m_pfrmDisplayPre = pCurrFrame;
		pWMVDec->m_pvopcRenderThis = pCurrFrame;

		pWMVDec->m_pfrmDisplayPreBak = pWMVDec->m_pfrmCurrQ;
		pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
		goto RENDER;
	}

	  //Normal Case
	  if(pWMVDec->m_tFrmType == BVOP || pWMVDec->m_tFrmType == BIVOP )
	  {//render directly
		  pWMVDec->m_pvopcRenderThis = pCurrFrame;
		  pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmCurrQ;
	  }
	  else
	  {
		  pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmDisplayPre;
		  pWMVDec->m_pfrmDisplayPre = pCurrFrame;

		  pWMVDec->m_pvopcRenderThisBak = pWMVDec->m_pfrmDisplayPreBak;
		  pWMVDec->m_pfrmDisplayPreBak = pWMVDec->m_pfrmCurrQ;
	  }

POSTPROCESS:
	  if(pWMVDec->m_pvopcRenderThis == 0)
		  return -1;	  

#ifdef ADJ_NEW
	  if(!pWMVDec->m_iDroppedPFrame)
	  {
		  if(pWMVDec->m_pvopcRenderThis->m_frameType != BVOP)
		  {
			  if ( pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeNeed == 1 || pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeAdvNeed ==1) 
				buf_seq_ctl(&pWMVDec->m_memManageFIFO,pWMVDec->m_pvopcRenderThisBak, FIFO_WRITE);
		  }
	  }
#else
	  if (pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeNeed == 1) 
	  {//AdjustDecRange  
		  if(CopyCurrToPost (pWMVDec) != WMV_Succeeded)
			  return -1;
		  AdjustDecRange (pWMVDec);
		  pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
	  }

	  if(pWMVDec->m_pvopcRenderThis->m_AdjustDecRangeAdvNeed ==1)
	  {
		  if(CopyCurrToPost (pWMVDec) != WMV_Succeeded)
			  return -1;
		  AdjustDecRangeWMVA(pWMVDec);
		  pWMVDec->m_pvopcRenderThis = pWMVDec->m_pfrmPostQ;
	  }	
#endif

	  //zou 
	  if(!pWMVDec->m_iDroppedPFrame)
		  buf_seq_ctl(&pWMVDec->m_memManageFIFO, pWMVDec->m_pvopcRenderThis, FIFO_WRITE);
	  
	  //printf("m_frameType = %d  \n",pWMVDec->m_pvopcRenderThis->m_frameType);
RENDER:
	  pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);

	  if(0){
			static int a =0;
			a++;
			printf("%d pOutFrame 0x%x  type = %d \n",a,pWMVDec->m_pvopcRenderThis,pWMVDec->m_pvopcRenderThis->m_frameType);
		}

	  ppxliRenderThisY = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucYPlane);
	  ppxliRenderThisU = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucUPlane);
	  ppxliRenderThisV = (U8_WMV*) (pWMVDec->m_pvopcRenderThis->m_pucVPlane);    
	  iWidthSource = ( (iWidthSource + 15) >> 4 ) << 4;
	  
	  if (pWMVDec->m_bYUV411) 
	  {	
		  StrideY  = iWidthSource        + 64;
		  StrideU  = (iWidthSource >> 2) + 32;
		  StrideV  = StrideU;
		  iOffsetX = 0;		  
		  iOffsetY = 0;
		  ppxliRenderThisY += StrideY * 32 + 32;
		  ppxliRenderThisU += StrideU * 16 + 16;
		  ppxliRenderThisV += StrideV * 16 + 16;
	  }
	  else {
        StrideY  = 0;
        StrideU  = 0;
        StrideV  = 0;
        iOffsetX = 32;
        iOffsetY = 32;
    }

	iFullSrcHeight = iHeightSource + iPadding;
	iFullSrcWidth  = iWidthSource  + iPadding;
	iSrcUVStride   = StrideU ? StrideU : (iFullSrcWidth >> 1);

	iSrcYOffset    = iOffsetY * iFullSrcWidth + iOffsetX;
	iSrcUOffset    = (iOffsetY >> 1) * iSrcUVStride  + (iOffsetX >> 1);
	iSrcVOffset    = iSrcUOffset;

	pOutBuff[0] = ppxliRenderThisY;
	pOutBuff[1] = ppxliRenderThisU;
	pOutBuff[2] = ppxliRenderThisV;

	pOutBuff[0] +=  iSrcYOffset;
	pOutBuff[1] +=  iSrcUOffset;
	pOutBuff[2] +=  iSrcVOffset;
	pStride[0]   = iFullSrcWidth;
	pStride[1]   = iSrcUVStride;
	pStride[2]   = iSrcUVStride;
	pWMVDec->m_uiNumFramesLeftForOutput--;
	 return 0;
}
#endif
tWMVDecodeStatus InitParam ( HWMVDecoder  phWMVDecoder,
							            I32_WMV iWidthSource,
							            I32_WMV iHeightSource	) 
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

	tWMVDecInternalMember *pWMVDec    = (tWMVDecInternalMember*)phWMVDecoder;
	if( NULL == pWMVDec )
		return WMV_InValidArguments;
    
    pWMVDec -> m_pInputBitstream = (CInputBitStream_WMV *)wmvMalloc(pWMVDec, sizeof( CInputBitStream_WMV), DHEAP_STRUCT);
    if (!pWMVDec -> m_pInputBitstream ) {
        return WMV_BadMemory;
    }
	memset (pWMVDec -> m_pInputBitstream, 0, sizeof (CInputBitStream_WMV));
    //BS_construct(pWMVDec -> m_pInputBitstream, 0);
    pWMVDec->m_pInputBitstream->m_pWMVDec = pWMVDec;
    
    tWMVStatus = WMVInternalMemberInit (pWMVDec,iWidthSource,iHeightSource);
    
    return tWMVStatus;
}

//tWMVDecodeStatus InitParamThread ( HWMVDecoder  phWMVDecoder,
//							            I32_WMV iWidthSource,
//							            I32_WMV iHeightSource	) 
//{
//    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
//	tWMVDecInternalMember *pWMVDec    = (tWMVDecInternalMember*)phWMVDecoder;
//	if( NULL == pWMVDec )
//		return WMV_InValidArguments;
//    
//    pWMVDec -> m_pInputBitstream = (CInputBitStream_WMV *)wmvMalloc(pWMVDec, sizeof( CInputBitStream_WMV), DHEAP_STRUCT);
//    if (!pWMVDec -> m_pInputBitstream ) {
//        return WMV_BadMemory;
//    }
//	memset (pWMVDec -> m_pInputBitstream, 0, sizeof (CInputBitStream_WMV));
//    pWMVDec->m_pInputBitstream->m_pWMVDec = pWMVDec;
//    
//    tWMVStatus = WMVInternalMemberInit (pWMVDec,iWidthSource,iHeightSource);    
//    return tWMVStatus;
//}


Bool_WMV IsWVC1HeaderAndProcess(tWMVDecInternalMember *pWMVDec, VO_CODECBUFFER *pInput)
{
	I32_WMV iW,iH;
	I32_WMV iBuffCount = 0;
	U32_WMV uStartCode;
	U8_WMV  *pExt;
	I32_WMV iExtSize;

	iBuffCount ++; //jump the first byte.

	uStartCode = pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	uStartCode = (uStartCode << 8) | pInput->Buffer[iBuffCount++];
	if(uStartCode != 0x0000010F) {
		return FALSE;
	}
	iBuffCount += 2;//jump
	iW = pInput->Buffer[iBuffCount++];
	iW = (iW << 8) | pInput->Buffer[iBuffCount];
	iW >>= 4;
	iH = pInput->Buffer[iBuffCount++];
	iH <<= (32-4);
	iH >>= (32-4);
	iH = (iH << 8) | pInput->Buffer[iBuffCount++];
	iW = iW*2 + 2;
	iH = iH*2 + 2;

	iExtSize = pInput->Length ;
	pExt     = (U8_WMV *)pWMVDec->m_pBMPINFOHEADER + sizeof(VO_BITMAPINFOHEADER);
	pWMVDec->m_pBMPINFOHEADER->biWidth  = iW;
	pWMVDec->m_pBMPINFOHEADER->biHeight = iH;
	pWMVDec->m_pBMPINFOHEADER->biCompression = FOURCC_WVC1_WMV;
	pWMVDec->m_pBMPINFOHEADER->biSize = sizeof(VO_BITMAPINFOHEADER) + iExtSize;

	if(iExtSize > 200)
		return TRUE;
	for ( iBuffCount = 0; iBuffCount < iExtSize; iBuffCount ++ ) {
		*pExt++ = pInput->Buffer[iBuffCount];
	}	

	return TRUE;
}
tWMVDecodeStatus voWMVCheckFourCC(tWMVDecInternalMember *pWMVDec,
                                                      I32_WMV iCodecVer,
                                                      U8_WMV* pSequenceHeader,
                                                      I32_WMV uipSequenceHeaderLength)
{

    if (iCodecVer == FOURCC_WMV2_WMV || iCodecVer == FOURCC_wmv2_WMV) {
        pWMVDec->m_cvCodecVersion = WMV2;
    } else if (iCodecVer == FOURCC_WMV3_WMV || iCodecVer == FOURCC_wmv3_WMV) {
        pWMVDec->m_cvCodecVersion = WMV3;
    } else if (iCodecVer == FOURCC_WMVA_WMV || iCodecVer == FOURCC_wmva_WMV) {
        pWMVDec->m_cvCodecVersion = WMVA;
    } else if (iCodecVer == FOURCC_WMV1_WMV || iCodecVer == FOURCC_wmv1_WMV) {
        pWMVDec->m_cvCodecVersion = WMV1;
    } else if (iCodecVer == FOURCC_MP43_WMV || iCodecVer == FOURCC_mp43_WMV) {
        pWMVDec->m_cvCodecVersion = MP43;
    } else if (iCodecVer == FOURCC_MP42_WMV || iCodecVer == FOURCC_mp42_WMV) {
        pWMVDec->m_cvCodecVersion = MP42;
    } else if (iCodecVer == FOURCC_MP4S_WMV || iCodecVer == FOURCC_mp4s_WMV) {
        pWMVDec->m_cvCodecVersion = MP4S;
    } else if (iCodecVer == FOURCC_WMVP_WMV || iCodecVer == FOURCC_wmvp_WMV) {
        pWMVDec->m_cvCodecVersion = WMV3;
    }

    if( iCodecVer == FOURCC_WMVA_WMV || iCodecVer == FOURCC_wmva_WMV ) {
        return WMV_Succeeded;
    }
    else if (iCodecVer == FOURCC_WMV3_WMV || iCodecVer == FOURCC_wmv3_WMV) {
         if ( pSequenceHeader != NULL_WMV && uipSequenceHeaderLength > 0) {
             pWMVDec->m_bYUV411 =  ((*pSequenceHeader & 0x20) == 0x20);
             pWMVDec->m_bSpriteMode = ((*pSequenceHeader & 0x10) == 0x10);
             if( ((pWMVDec->m_bSpriteMode) == 0x01))
                 return WMV_UnSupportedCompressedFormat;
         }
          return WMV_Succeeded;

    }
    else {
            return WMV_UnSupportedCompressedFormat;
       }
}

int dec_header ( HWMVDecoder hCodec, VO_CODECBUFFER *pInData )
{
    tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
	tWMVDecInternalMember *pWMVDec     = (tWMVDecInternalMember*)hCodec;
	I32_WMV                iCodecVer;
	int                    iHdrSize;
	U8_WMV                *pHdr        = NULL;
	Bool_WMV               bHostDeinterlace;
	pWMVDec->pHeader = NULL;

	if( pWMVDec == NULL ) 
		return -1;
    
	if( TRUE == IsWVC1HeaderAndProcess(pWMVDec,pInData) ){
		pWMVDec->pHeader = pWMVDec->m_pBMPINFOHEADER;
        iCodecVer = FOURCC_WVC1_WMV;
        pWMVDec->m_bCodecIsWVC1 = TRUE;
		if(pInData->Length > 200)
			return -1;
	}
	else {
		pWMVDec->pHeader = (VO_BITMAPINFOHEADER *)pInData->Buffer;
        pWMVDec->m_bCodecIsWVC1 = FALSE;
        iCodecVer = pWMVDec->pHeader->biCompression ; 
	}	

    pWMVDec->m_bCodecIsWVC1 = FALSE;
	if (iCodecVer == FOURCC_WVC1_WMV || iCodecVer == FOURCC_wvc1_WMV) {
		iCodecVer = FOURCC_WMVA_WMV;
		pWMVDec->m_bCodecIsWVC1 = TRUE;
		pWMVDec->m_bAdvancedProfile = TRUE;
	}


    pHdr     = (U8_WMV *)pWMVDec->pHeader + sizeof(VO_BITMAPINFOHEADER);
    iHdrSize = pWMVDec->pHeader->biSize - sizeof(VO_BITMAPINFOHEADER);
    bHostDeinterlace = TRUE_WMV;

     if(voWMVCheckFourCC(pWMVDec,iCodecVer,pHdr,iHdrSize)!=WMV_Succeeded)
        return -1;

#ifdef STABILITY
     if(pWMVDec->pHeader->biWidth >= 2500 || pWMVDec->pHeader->biHeight > 2500)
     {
         pWMVDec->pHeader->biWidth = 0;
         pWMVDec->pHeader->biHeight = 0;
         return -1;
     }
#endif
	 pWMVDec->headerwidth = pWMVDec->pHeader->biWidth;
	 pWMVDec->headerheight = pWMVDec->pHeader->biHeight;

	tWMVStatus = InitParam( pWMVDec,pWMVDec->pHeader->biWidth, pWMVDec->pHeader->biHeight);
	if ( tWMVStatus != WMV_Succeeded )
        return -1;

	pHdr     = (U8_WMV *)pWMVDec->pHeader + sizeof(VO_BITMAPINFOHEADER);
	iHdrSize = pWMVDec->pHeader->biSize - sizeof(VO_BITMAPINFOHEADER);

	V9ECBReset(pWMVDec, pHdr, iHdrSize );

	tWMVStatus = voVC1_WMVideoDecDecodeSequenceHeader ( (HWMVDecoder *)&pWMVDec );
	if ( tWMVStatus != WMV_Succeeded)
	{
		pWMVDec->sequence_errorflag = 1;
		return -1;
	}
	pWMVDec->sequence_errorflag = 0;

	pWMVDec->bNewheader = 1;
	return 0;
}

tFrameType_WMV voFrameTypeSwap(tFrameType_WMV m_frameType)
{
	int frametype = 0;
	switch(m_frameType)
	{
	case	IVOP:
	case	PVOP:
	case	BVOP:
		frametype = m_frameType;
		break;
	case	BIVOP:
		frametype = 0; //VO_VIDEO_FRAME_I
		break;
	case	SKIPFRAME:
		frametype = 1; //VO_VIDEO_FRAME_P
		break;
	case	SPRITE:	//??
	default:
		frametype = 1; //VO_VIDEO_FRAME_P
	}
	return frametype;
}

VO_S32 CopyParametes(tWMVDecInternalMember* pDstWMVDec,tWMVDecInternalMember* pSrcWMVDec)
{
	VO_U32 i=0;
	tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
   
	//globle
	pDstWMVDec->m_bCodecIsWVC1 = pSrcWMVDec->m_bCodecIsWVC1;
	pDstWMVDec->m_bAdvancedProfile = pSrcWMVDec->m_bAdvancedProfile;
	pDstWMVDec->m_outputMode = pSrcWMVDec->m_outputMode;
	pDstWMVDec->m_frameNum = pSrcWMVDec->m_frameNum;
	pDstWMVDec->m_cvCodecVersion = pSrcWMVDec->m_cvCodecVersion;
	pDstWMVDec->ntotal_frames = pSrcWMVDec->ntotal_frames;
	pDstWMVDec->CpuNumber = pSrcWMVDec->CpuNumber;
	pDstWMVDec->EXPANDY_REFVOP = pSrcWMVDec->EXPANDY_REFVOP;
	pDstWMVDec->EXPANDUV_REFVOP = pSrcWMVDec->EXPANDUV_REFVOP;
	
	//malloc thread private buffer
	 pSrcWMVDec->headerwidth = pSrcWMVDec->m_iMaxFrmWidthSrc;
	 pSrcWMVDec->headerheight = pSrcWMVDec->m_iMaxFrmHeightSrc;
	tWMVStatus = InitParam( pDstWMVDec,pSrcWMVDec->headerwidth, pSrcWMVDec->headerheight);
	if(tWMVStatus != WMV_Succeeded)
		return WMV_Failed;
	pDstWMVDec->m_pInputBitstream->m_bNotEndOfFrame = pSrcWMVDec->m_pInputBitstream->m_bNotEndOfFrame;
    
	//sepuence header
	CopySQHParametes(pDstWMVDec,pSrcWMVDec);
	//Entry Point
	CopyEntryPointParametes(pDstWMVDec,pSrcWMVDec);
   //FIFO 
	CopyFIFOBuffer(pDstWMVDec,pSrcWMVDec);

	return WMV_Succeeded;
}

#ifdef RVDS
int dec_frame ( 
			   HWMVDecoder          hCodec, 
			   unsigned char       *pInputBuff, 
			   int                  lInBuffLen, 
			   VO_VIDEO_BUFFER     *pOutput, 
			   VO_VIDEO_OUTPUTINFO *pOutPutInfo 
			   )
{
    tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
	tWMVDecInternalMember *pWMVDec     = (tWMVDecInternalMember*)hCodec;
	unsigned char         *pOutBuff[3] = {NULL};
	int                    stride[3]   = {0};
	unsigned char   	  *pInBuff	   = pInputBuff;
	I32_WMV				   iPicHorizSize, iPicVertSize;
	U16_WMV				   uiNumDecodedFrames;
#else
int dec_frame ( HWMVDecoder hCodec, 
			   U8_WMV *pInputBuff, 
			   int lInBuffLen, 
			   VO_VIDEO_BUFFER *pOutput, 
			   VO_VIDEO_OUTPUTINFO *pOutPutInfo )
{
    tWMVDecodeStatus       tWMVStatus  = WMV_Succeeded;
	tWMVDecInternalMember *pWMVDec     = (tWMVDecInternalMember*)hCodec;
	U8_WMV                 *pOutBuff[3] = {NULL};
	I32_WMV                 stride[3]   = {0};
	U8_WMV				  *pInBuff	   = pInputBuff;
	I32_WMV				   iPicHorizSize, iPicVertSize;
#endif

#ifdef TRACEVC1
	if(1){
		FILE*fp =fopen( TRACEVC1FILE,"a");
		fprintf(fp," dec_frame %d %d \n",pWMVDec->m_iFrmWidthSrc,pWMVDec->m_iFrmHeightSrc);
		fclose(fp);
	}
#endif
	
	{
        iPicHorizSize = pWMVDec->m_iFrmWidthSrc;
        iPicVertSize  = pWMVDec->m_iFrmHeightSrc;
    }
#ifdef STABILITY 
    if(iPicHorizSize== 0  || iPicVertSize == 0)
    {
        return 0;
    }
#endif

#ifdef CHKLCS_ON
	voCheckLibResetVideo (pWMVDec->m_phCheck, pWMVDec->m_pPrevOutputBuff);
#endif

	V9ECBReset(pWMVDec, pInBuff, (int)lInBuffLen );

    tWMVStatus = WMVideoDecDecodeDataInternal (pWMVDec);

	pOutPutInfo->InputUsed = lInBuffLen;
	if ( tWMVStatus != WMV_Succeeded ) {
		if(pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == PVOP)
		{
			pWMVDec->error_flag_main = 1;
			pWMVDec->error_flag_thread = 1;
		}
		pOutPutInfo->Format.Type = VO_VIDEO_FRAME_NULL;
		return 0;
	}

	tWMVStatus = GetOutputParam_NEW( pWMVDec, pOutBuff, stride,iPicHorizSize, iPicVertSize);
	//printf("GetOutputParam_NEW \n");
	if(tWMVStatus)
	{
		pOutput->Buffer[0]  = NULL;
		pOutput->Buffer[1]  = NULL;
		pOutput->Buffer[2]  = NULL;
		pOutput->Stride[0]  = 0; 
		pOutput->Stride[1]  = 0; 
		pOutput->Stride[2]  = 0;
		return 0;
	}
	//pWMVDec->m_outputMode == 0  output as disaplay order
	if(pWMVDec->m_frameNum == 0 && pWMVDec->m_outputMode == 0) { //Delay first frame output.
		pOutput->Buffer[0]  = NULL;
		pOutput->Buffer[1]  = NULL;
		pOutput->Buffer[2]  = NULL;
		pOutput->Stride[0]  = 0; 
		pOutput->Stride[1]  = 0; 
		pOutput->Stride[2]  = 0;
		pOutput->ColorType = VO_COLOR_YUV_PLANAR420;
		pOutput->Time = 0;
	}
	else {
		pOutput->Buffer[0]  = pOutBuff[0];
		pOutput->Buffer[1]  = pOutBuff[1];
		pOutput->Buffer[2]  = pOutBuff[2];
		pOutput->Stride[0]  = stride[0]; 
		pOutput->Stride[1]  = stride[1]; 
		pOutput->Stride[2]  = stride[2];
		pOutput->ColorType = VO_COLOR_YUV_PLANAR420;
		pOutput->Time = pWMVDec->m_pvopcRenderThis->m_timeStamp;
		pOutPutInfo->Format.Width = iPicHorizSize;
		pOutPutInfo->Format.Height = iPicVertSize; 
		pOutPutInfo->Flag = 0;
		pOutput->UserData = pWMVDec->m_pvopcRenderThis->m_UserData;
		pOutPutInfo->Format.Type = voFrameTypeSwap(pWMVDec->m_pvopcRenderThis->m_frameType);

	}
	
	if(pWMVDec->m_outputMode == 1)// output immediatly
	{
		iPicHorizSize = pWMVDec->m_iFrmWidthSrc;
		iPicVertSize  = pWMVDec->m_iFrmHeightSrc;
	}

	/* Convert YUV411 to YUV420. */
	if (pWMVDec->m_bYUV411) {
		pOutput->Buffer[0]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucYPlane;
		pOutput->Buffer[1]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucUPlane;
		pOutput->Buffer[2]  = pWMVDec->m_pfrmYUV411toYUV420Q->m_pucVPlane;
		pOutput->Stride[0]  = iPicHorizSize; 
		pOutput->Stride[1]  = iPicHorizSize>>1; 
		pOutput->Stride[2]  = iPicHorizSize>>1;
		YUV411_To_YUV420(pOutBuff,pOutput->Buffer,stride,iPicHorizSize,iPicVertSize);
	}

#ifdef CHKLCS_ON
	pWMVDec->m_pPrevOutputBuff = pOutput;
	voCheckLibCheckVideo (pWMVDec->m_phCheck, pOutput, &pOutPutInfo->Format);
#endif

	return lInBuffLen;
}

tWMVDecodeStatus WMVDecGetFrameType(tWMVDecInternalMember *pWMVDec, VO_CODECBUFFER*	pInput)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
	U32_WMV uiBufferLengthFromCaller = 0;
	U8_WMV* pucBufferBits = NULL;
	Bool_WMV bNotEndOfFrame = TRUE;   

	U32_WMV iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	U32_WMV iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc; 
	V9ECBReset(pWMVDec, pInput->Buffer, (int)pInput->Length );

	{    
		WMVDecCBGetData (pWMVDec, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
		if (bNotEndOfFrame) {
			if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL) {
				 
				return WMV_BrokenFrame;
			}
		}
    
		if (pWMVDec->m_cvCodecVersion == WMVA) {
			tWMVStatus = ParseStartCode(pWMVDec, pucBufferBits, uiBufferLengthFromCaller, &pucBufferBits, &uiBufferLengthFromCaller, &bNotEndOfFrame);
		    if (tWMVStatus != WMV_Succeeded) {
				return tWMVStatus;
			}
        };
		BS_reset (pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    
		if (pWMVDec->m_cvCodecVersion != WMVA) {
			tWMVStatus = WMVideoDecDecodeFrameHead2 (pWMVDec);
			if (tWMVStatus != WMV_Succeeded) {
				return tWMVStatus;
			}
		} 
		else {       
			WMVFrameType iFrameType;

			if (pWMVDec->m_bInterlacedSource) {
			 if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
				  iFrameType = PROGRESSIVE;     // Progressive
			else {
				if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
						iFrameType = INTERLACEFRAME; // Frame Interlace
					else    
						iFrameType = INTERLACEFIELD; // Field interlace
				}
			}
			else 
				iFrameType = PROGRESSIVE;    // Progressive       

			pWMVDec->m_bInterlaceV2 =  FALSE;
			if (iFrameType == PROGRESSIVE){// Progressive Frame
				DecodeVOPType_WMVA2 (pWMVDec);
			} 
			else if (iFrameType == INTERLACEFRAME){//Frame Interlace 

				DecodeVOPType_WMVA2 (pWMVDec);

			} 
		}
	}
	pWMVDec->m_iFrmWidthSrc = iFrmWidthSrc;
	pWMVDec->m_iFrmHeightSrc = iFrmHeightSrc; 

	return WMV_Succeeded;
}

VO_U32 VO_API voVC1DecInit(VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{ 
    tWMVDecInternalMember *gData = NULL_WMV; 
	VO_MEM_OPERATOR		  *memOp;

#if defined(CHKLCS_ON)
	VO_PTR pLicHandle;
	VO_U32 err;
	VO_LIB_OPERATOR *pLibOp = NULL;
	if(pUserData) {
		if(pUserData->memflag & 0xF0)
			pLibOp = pUserData->libOperator;
	}
	err = voCheckLibInit(&pLicHandle, VO_INDEX_DEC_VC1, pUserData?pUserData->memflag:0, g_VC1_hModule, pLibOp);
	if(err != VO_ERR_NONE)
	{
		*phDec = NULL;
		if(pLicHandle)
		{
			voCheckLibUninit(pLicHandle);
			pLicHandle = NULL;
		}
		return err;
	}
#endif

#if  defined(VOANDROID_DUMPVC1)
	int init_time =0;
#endif

#ifdef DISABLE_MEM_OPERATOR
	pUserData = NULL; //force NULL memory operator
#endif

	memOp = NULL;
	if(pUserData) {
		if((pUserData->memflag & 0x0F) == VO_IMF_USERMEMOPERATOR) {
			memOp = (VO_MEM_OPERATOR*)pUserData->memData;
		}
	}

    gData = (tWMVDecInternalMember*) wmvMallocHandle (memOp, sizeof (tWMVDecInternalMember), DHEAP_STRUCT);
    
    if (gData == NULL_WMV) {
        *phDec = NULL_WMV;  
        return VO_ERR_INVALID_ARG;
    }
    memset( gData, 0, sizeof (tWMVDecInternalMember));  // clear here simplies error handling in low memory case
    *phDec = (VO_HANDLE) gData;    // if an error occurs from here on, return this pointer so we can free what might have been malloced already

	memset( &gData->m_MemOperator, 0, sizeof(MemOpteratorInternal) );
	gData->m_bIsHeadDataParsed = FALSE_WMV;
	if(pUserData) 
		gData->m_MemOperator.Type = NORMAL_MEM_OP;
	else
		gData->m_MemOperator.Type = NONE_MEM_OP;

//#ifdef WIN32
//	memset( gData->m_SeqHeadInfoString, 0, 2048 );
//#endif

	gData->m_memOp = memOp ;
	gData->m_pBMPINFOHEADER = (VO_BITMAPINFOHEADER*) wmvMalloc (gData, sizeof (VO_BITMAPINFOHEADER)+200, DHEAP_STRUCT);
	if(gData->m_pBMPINFOHEADER == NULL)
		return WMV_BadMemory;

#ifdef CHKLCS_ON
	gData->m_phCheck = pLicHandle;
#endif

	//gData->intensitycomp_inited = 0;
	gData->m_frameNum = 0;
    gData->CpuNumber = 1;

	//memset(&GlobleFIFO,0,sizeof(voGlobleFIFOInfo));
#ifdef VOANDROID_DUMPVC1

	{
		FILE *fp0,*fp1;		
		fp0 =fopen("/mnt/sdcard/smoothstream/vc1init.txt","r");
		if(fp0)
		{
			fscanf(fp0,"%d",&init_time);
			fclose(fp0);
		}

		init_time += 1;
		
		fp1 =fopen("/mnt/sdcard/smoothstream/vc1init.txt","w");
		fprintf(fp1,"%d",init_time);
		fclose(fp1);
	}
	{
        FILE* fp;
		sprintf(vc1file,"%s_%d",dumpfile,init_time);
		fp =fopen(vc1file,"ab");
        fclose(fp);
    }
#endif

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"Init \n");
		fclose(fp);
	}
#endif

#ifdef SHAREMEMORY
	gData->CpuNumber = 1;
	gData->m_benableShareMemory = 1;
	gData->m_nShareMemoryExt = 1;

	//+3: to save post process
	//+1: to save ref0 process frame
	//ZOU TODO
#if 0
	gData->ntotal_frames= 2*gData->m_nShareMemoryExt // share memory
						+MAX_FRAMES_INTERNAL //interal
						+gData->CpuNumber*2 //*2 postprocess frame
						+gData->CpuNumber; // ref0 porcess frame

	gData->m_pfrmList = (tYUV420Frame_WMV**)wmvMalloc (gData,gData->ntotal_frames*sizeof(tYUV420Frame_WMV*), DHEAP_STRUCT);
	gData->m_memManageFIFO.img_seq = (tYUV420Frame_WMV**)wmvMalloc (gData,(gData->ntotal_frames+1)*sizeof(tYUV420Frame_WMV*), DHEAP_STRUCT);
#endif
#endif

	return VO_ERR_NONE;
}


VO_U32 VO_API voVC1DecSetParameter(VO_HANDLE hDec, VO_S32 nID, VO_PTR lValue)
{
	I32_WMV iInDataLen;
	tWMVDecInternalMember *gData = (tWMVDecInternalMember*)hDec;

	if( gData == NULL )
		return VO_ERR_INVALID_ARG;

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"voVC1DecSetParameter %x \n", nID);
		fclose(fp);
	}
#endif

	switch( nID ) {
	case VO_PID_COMMON_HEADDATA:
		{
			if( !gData->m_bIsHeadDataParsed ) {
				VO_CODECBUFFER *pInData = (VO_CODECBUFFER *)lValue;

				{
					gData->ntotal_frames= 2*gData->m_nShareMemoryExt // share memory
						+MAX_FRAMES_INTERNAL //interal
						+gData->CpuNumber*2    //*2 postprocess frame
						+gData->CpuNumber+1; // ref0 porcess frame

					if(gData->m_pfrmList)
						FREE_PTR(gData,gData->m_pfrmList);
					gData->m_pfrmList = (tYUV420Frame_WMV**)wmvMalloc (gData,gData->ntotal_frames*sizeof(tYUV420Frame_WMV*), DHEAP_STRUCT);	
					if(gData->m_pfrmList ==NULL)
						return WMV_BadMemory;
					if(gData->m_memManageFIFO.img_seq)
						FREE_PTR(gData,gData->m_memManageFIFO.img_seq);
					gData->m_memManageFIFO.img_seq = (tYUV420Frame_WMV**)wmvMalloc (gData,(gData->ntotal_frames+1)*sizeof(tYUV420Frame_WMV*), DHEAP_STRUCT);
					if(gData->m_memManageFIFO.img_seq ==NULL)
						return WMV_BadMemory;
				}

                {
#ifdef VOANDROID_DUMPVC1
					{
                        int startcode = 0xffffff0c;
                        FILE* fp =fopen(vc1file,"ab");
                        fwrite(&startcode,4,1,fp);
                        fwrite(&pInData->Length,4,1,fp);
                        fwrite(pInData->Buffer,pInData->Length,1,fp);
                        fclose(fp);
                    }
#endif

#ifdef TRACEVC1
	if(1){
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"dec_header  \n");
		fclose(fp);
	}
#endif
                    iInDataLen = dec_header( gData, pInData );
                }
				gData->m_bIsHeadDataParsed = TRUE_WMV;
				if ( -1 == iInDataLen ) {
					gData->m_bIsHeadDataParsed = FALSE_WMV;
					return VO_ERR_FAILED;
				}
			}
		}
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		{
			VO_MEM_VIDEO_OPERATOR *pMemShare     = (VO_MEM_VIDEO_OPERATOR *)lValue;
			gData->m_MemOperator.Type			 = VIDEO_MEM_SHARE_OP;
			gData->m_videoMemShare.GetBufByIndex = pMemShare->GetBufByIndex;
			gData->m_videoMemShare.Init          = pMemShare->Init;
			gData->m_videoMemShare.Uninit        = pMemShare->Uninit;
		}
		break;
	case VO_PID_COMMON_FLUSH:
		{
			if(gData->m_MemOperator.Type == VIDEO_MEM_SHARE_OP)
				reset_buf_seq(gData);
		}
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		{
			gData->m_outputMode = *(U32_WMV*)lValue; //0 or 1
			break;
		}
    case VO_PID_COMMON_CPUNUM:
        {
		    U32_WMV cpu_num = 	*(U32_WMV*)lValue;
#ifdef VOARMV6
			 gData->CpuNumber = 1;	
#else
#ifdef TRACEVC1
	#define TRACEVC1FILE "/data/local/tmp/VC1_DEC_LOG_42780.txt"
	if(1){
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"set cpu num = %d \n",cpu_num);
		fclose(fp);
	}
#endif
            if(cpu_num<1)
			{
                cpu_num = 1;
				*(U32_WMV*)lValue = 1;
			}
            if(cpu_num>MAX_THREAD_NUM)
			{
                cpu_num = MAX_THREAD_NUM;
				*(U32_WMV*)lValue = MAX_THREAD_NUM;
			}

            if(cpu_num != gData->CpuNumber)
            {
                gData->CpuNumber = cpu_num;	
				if(voVC1CreateThreads(gData))
					return VO_ERR_OUTOF_MEMORY;
            }
#endif
        }
        break;
	case VO_PID_COMMON_FRAME_BUF_EX:
#ifdef SHAREMEMORY
		if(*(U32_WMV*)lValue <= 0)
			return VO_ERR_WRONG_PARAM_ID;
		gData->m_nShareMemoryExt = *(U32_WMV*)lValue;
		gData->m_benableShareMemory = 1;
		break;
#endif
	case VO_PID_DEC_VC1_SET_LASTVIDEOFLAG:
#ifdef USE_FRAME_THREAD
		gData->nIsLastframe = *(U32_WMV*)lValue;
#endif
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

int OutputOneFrameInteral( VO_VIDEO_BUFFER *par, VO_VIDEO_OUTPUTINFO* outFormat, FILE* outFile)
{
	int i, width,height;
	unsigned char* out_src;
	if (!outFile) {
		return 0;
	}
   //if(outFormat->Format.Type == VO_VIDEO_FRAME_P)
   //     ;
   //else
   //    return 0;

	if (!par->Buffer[0] || !par->Buffer[1] || !par->Buffer[2]) {
		return 0;
	}
#ifdef TRACEVC1
	if(1){
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp," ----OutputOneFrameInteral   %d %d\n",outFormat->Format.Width,outFormat->Format.Height);
		fclose(fp);
	}
#endif

	width  = outFormat->Format.Width;
	height = outFormat->Format.Height;

	/* output decoded YUV420 frame */
	/* Y */
	out_src = par->Buffer[0];
	for( i = 0;i < height; i++ ) {
		fwrite(out_src, width, 1, outFile);
		out_src += par->Stride[0];
	}

	/* U */
	out_src = par->Buffer[1];
	for(i = 0; i< height/2; i++ ) {
		fwrite(out_src, width/2, 1, outFile);
		out_src += par->Stride[1];
	}

	/* V */
	out_src = par->Buffer[2];
	for( i = 0; i < height/2; i++ ) {
		fwrite(out_src, width/2, 1, outFile);
		out_src += par->Stride[2];
	}

	return 0;
}


#if 1
VO_U32 VO_API voVC1DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	tWMVDecInternalMember *gData = (tWMVDecInternalMember*)hDec;
	if( NULL == gData )
		return VO_ERR_INVALID_ARG;
	gData->m_pInputFrameData = (void *)pInput;
	gData->m_InBuffLenLeft   = pInput->Length;
	
	if( gData->m_pInputFrameData  && gData->m_InBuffLenLeft)
	{
		gData->nHasNewInput = 1;
		gData->m_input_header++;
#ifdef VOANDROID_DUMPVC1
	{
       int startcode = 0xffffff0d;
        FILE* fp =fopen(vc1file,"ab");
        fwrite(&startcode,4,1,fp);
        fwrite(&pInput->Length,4,1,fp);
        fwrite(pInput->Buffer,pInput->Length,1,fp);
        fclose(fp);
    }
#endif
	}

	//printf("[main]  voVC1DecSetInputData  start\n");
 	return VO_ERR_NONE;
}
VO_U32 VO_API voVC1DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	VO_S32 tWMVStatus = 0;
	int iInDataLen = 0,i=0,nthidx= 0;
	VO_CODECBUFFER		   *pInput;
	tWMVDecInternalMember*  gData = (tWMVDecInternalMember*)hDec;
	tYUV420Frame_WMV* pOutFrame = NULL;

	U8_WMV *pOutBuff[3] = {NULL};
	I32_WMV stride[3]   = {0};


	if( NULL == gData )
		return VO_ERR_INVALID_ARG;

	nthidx = gData->preThdsIdx;

	voprintf("=voVC1DecGetOutputData= %d \n",gData->m_input_header);

	if(gData->nHasNewInput)
	{
		pInput = (VO_CODECBUFFER *)gData->m_pInputFrameData;
		if( gData->m_bCodecIsWVC1 ) {
			if( pInput->Buffer[1] == 0x00 && pInput->Buffer[2] == 0x00 && pInput->Buffer[3] == 0x01 && pInput->Buffer[4] == 0x0F )
				return VO_ERR_INPUT_BUFFER_SMALL;
		}
		gData->m_inTimeStamp = pInput->Time;
		gData->m_inUserData = pInput->UserData;

#ifdef USE_FRAME_THREAD
		if(gData->CpuNumber > 1 && !gData->m_bYUV411)
		{
			volatile U32_WMV*status = NULL;
			VO_S32 n=0, nBusyThreads = 0;
			tWMVDecInternalMember* pWMVDec = gData;
			tWMVDecInternalMember* pThreadWMVDec = NULL;
			tWMVDecThdCtx* pThdCtx = NULL;
			I32_WMV indx_min=0, nth=0;
			
			if(gData->m_iFrmWidthSrc== 0  || gData->m_iFrmHeightSrc == 0)
			{
				pOutPutInfo->Flag = 0;
				gData->error_flag_main = 1;
				gData->m_input_header = 0;
				return -1;
			}

#if 0
			//set thread paramates TODO
			if(pWMVDec->bNewheader)
			{		
				for(n=0;n<pWMVDec->CpuNumber;n++)
				{
					tWMVDecInternalMember *pThreadWMVDecTmp = NULL;
					pThdCtx = (tWMVDecThdCtx*)pWMVDec->pThreadCtx[n];
					pThreadWMVDecTmp = pThdCtx->pThWMVDec;
					if(gData->m_input_header == 1)
					{
						if ( CopyParametes(pThreadWMVDecTmp,pWMVDec) != WMV_Succeeded )
							return -1;
					}
					else
					{
						pThreadWMVDecTmp->bNewheader = 1;
					}
				}
				pWMVDec->bNewheader = 0;
			}
#endif
			//wait pre thread/frame finishing header decode
			voprintf("------- \n");
			pThdCtx = (tWMVDecThdCtx*)pWMVDec->pThreadCtx[nthidx];
			pThreadWMVDec = pThdCtx->pThWMVDec;

			if(gData->m_input_header > 1)
			{
				if(pThreadWMVDec->pThreadStatus == THREAD_ERROR)
				{
					pOutPutInfo->Flag = 0;
					printf("main return -1 \n");
					gData->error_flag_main = 1;
					return -1;
				}
				voprintf("[main]  wait thread[%d] head finish \n",nthidx);
				sem_wait(pThreadWMVDec->pHasHeaderDecoded);
				voprintf("[main]  thread[%d] head finish \n",nthidx);
			}

			if(pWMVDec->bNewheader)
			{	
				for(n=0;n<pWMVDec->CpuNumber;n++)
				{
					tWMVDecInternalMember *pThreadWMVDecTmp = NULL;
					pThdCtx = (tWMVDecThdCtx*)pWMVDec->pThreadCtx[n];
					pThreadWMVDecTmp = pThdCtx->pThWMVDec;
					if(gData->m_input_header == 1)
					{
						if ( CopyParametes(pThreadWMVDecTmp,pWMVDec) != WMV_Succeeded )
							return -1;
					}
					else
					{
						pThreadWMVDecTmp->bNewheader = 1;
						if(pWMVDec->bNewSize)
							pThreadWMVDecTmp->bNewSize = 1;
					}
				}
				pWMVDec->bNewheader = 0;
				pWMVDec->bNewSize = 0;
			}

			if(pThreadWMVDec->pThreadStatus == THREAD_ERROR)
			{
				pOutPutInfo->Flag = 0;
				gData->error_flag_main = 1;
				return -1;
			}

			if(pWMVDec->preThdsIdx < pWMVDec->CpuNumber-1)
				nthidx++;
			else
				nthidx = 0;

			pThdCtx = (tWMVDecThdCtx*)pWMVDec->pThreadCtx[nthidx];
			pThreadWMVDec = pThdCtx->pThWMVDec;

			//wait current thread change to "free"
			printf("[main]  wait thread[%d] finish \n",nthidx);
			sem_wait(pThreadWMVDec->pHasFinishedThread);
			printf("[main]  thread[%d] is free  %d\n",nthidx, pWMVDec->bNewheader);


#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"------New frame  %d %d-------------\n",gData->nHasNewInput,gData->m_input_header);
		fclose(fp);
	}
#endif

#if 1
			if(pThreadWMVDec->bNewheader == 1)
			{
				//printf("thread %d \n",nthidx);
				CopySQHParametes(pThreadWMVDec,pWMVDec);
				CopyEntryPointParametes(pThreadWMVDec,pWMVDec);
				pThreadWMVDec->bNewheader = 0;
				if(pThreadWMVDec->bNewSize)
				{
					InitDataMembers_WMVA (pThreadWMVDec, pThreadWMVDec->m_iFrmWidthSrc, pThreadWMVDec->m_iFrmHeightSrc);
				}
			}
					

#endif

			pThreadWMVDec->preThdsIdx = pWMVDec->preThdsIdx;
			pThreadWMVDec->nthreads = nthidx;
			pThreadWMVDec->m_frameNum = pWMVDec->m_frameNum;			
			pThreadWMVDec->nInputFrameIndx = pWMVDec->m_input_header;
			pThreadWMVDec->m_inTimeStamp = pWMVDec->m_inTimeStamp;

			//set input bits for thread
			pThreadWMVDec->g_StrmModedataCtl.iBytesReadFrm = pInput->Length;
			pThreadWMVDec->g_StrmModedataCtl.iCounter = 0;			
			memcpy(pThreadWMVDec->g_StrmModedataCtl.pVideoBuffer,pInput->Buffer,pInput->Length);

			voprintf("[main]  notify thread[%d] to decode  \n",nthidx);
			sem_post(pThreadWMVDec->pThreadStart );
			pWMVDec->preThdsIdx = nthidx;

			//check whether there are output frames or not
#if 1
			//thread_sleep(0);

			{
				volatile U32_WMV *outstatus = &gData->nOutPutting;
				while(*outstatus==1)
					thread_sleep(0);
			}
			gData->nOutPutting = 1;
			if(gData->output_index > 0)
			{
				pOutFrame = gData->outputList[0];
				if(pOutFrame && pOutFrame->m_removeable)
				{					
					for(i = 0;i<gData->output_index-1;i++)
						gData->outputList[i]=gData->outputList[i+1];
					//gData->outputList[gData->output_index-1] = NULL;

					gData->output_index--;

					if(gData->output_index > 0 )
						pOutPutInfo->Flag  = 1;
					else
						pOutPutInfo->Flag  = 0;
				}
				else
					pOutFrame = NULL;
			}
			gData->nOutPutting = 0;
#endif
		}
		else
		{
			//single core
			if(gData->nHasNewInput)
			{
				gData->nInputFrameIndx = gData->m_input_header;
				iInDataLen = dec_frame ( gData, pInput->Buffer, pInput->Length, pOutput, pOutPutInfo );
				gData->m_InBuffLenLeft -= pInput->Length;
				if ( 0 == iInDataLen ) {
					gData->frame_errorflag = 1;
					gData->error_flag_main = 1;
					gData->m_uiNumFramesLeftForOutput = 0;
					gData->m_bPreProcRange = 0;
					gData->m_iRangeState = 0;
					gData->m_bRangeRedY_Flag = 0;
					gData->m_bRangeRedUV_Flag = 0;
					return VO_ERR_FAILED;
				}
				gData->frame_errorflag = 0;
				gData->m_frameNum ++;
				gData->nHasNewInput = 0;
			}
			return VO_ERR_NONE;
		}

		gData->nHasNewInput = 0;
#endif
	}
	else if(gData->CpuNumber > 1)
	{		
		//just check whether have output or not
#if 1
		//thread_sleep(0);
		if(gData->nIsLastframe && gData->m_input_header > 0 && gData->error_flag_main == 0)
		{
			//printf("====last frame flag=====\n");
			//IF NO INPUT any longer, we must wait and make sure all threads are "FINISHED"
			for(i=0;i<gData->CpuNumber;i++)
			{
				volatile tWMVDecThdCtx* pThdCtx = (tWMVDecThdCtx*)gData->pThreadCtx[i];
				volatile tWMVDecInternalMember* pThreadWMVDec = pThdCtx->pThWMVDec;
				volatile U32_WMV*status = &pThreadWMVDec->pThreadStatus;

				//printf("+++%d++++++++++++++++++%d-- %d\n",i,*status,pThreadWMVDec->nInputFrameIndx);
				//thread_sleep(1);				
				
				while(*status == THREAD_BUSY)
					;//thread_sleep(1);

				if(pThreadWMVDec->pThreadStatus == THREAD_ERROR)
				{
					pOutPutInfo->Flag = 0;
					printf("main return -1 \n");
					gData->error_flag_main = 1;
					return -1;
				}

				sem_wait(pThreadWMVDec->pHasFinishedThread);

				if(pThreadWMVDec->nInputFrameIndx == gData->m_input_header)
				{
					sem_wait(pThreadWMVDec->pHasHeaderDecoded);
					gData->m_pfrmPostQ = pThreadWMVDec->m_pfrmPostQ;
					gData->m_pfrmDisplayPre = pThreadWMVDec->m_pfrmDisplayPre;
					gData->m_pfrmCurrQ  = pThreadWMVDec->m_pfrmCurrQ;
				}

			}

			{
				volatile U32_WMV*outstatus = &gData->nOutPutting;
				while(*outstatus==1)
						thread_sleep(0);
			}

			gData->nOutPutting = 1;
			
			if(gData->output_index > 0)
			{
				pOutFrame = gData->outputList[0];
				if(pOutFrame && pOutFrame->m_removeable)
				{
					for(i = 0;i<gData->output_index-1;i++)
						gData->outputList[i]=gData->outputList[i+1];
					//gData->outputList[gData->output_index-1] = NULL;
					gData->output_index--;
				}
				if(pOutFrame && gData->output_index >  0)
					pOutPutInfo->Flag  = 1;
				else
					pOutPutInfo->Flag  = 0;
			}
			gData->nOutPutting = 0;

			gData->nIsLastframe = 0;
		}
		else
		{
			{
				volatile U32_WMV*outstatus = &gData->nOutPutting;
				while(*outstatus==1)
						thread_sleep(0);
			}

			gData->nOutPutting = 1;

			if(gData->output_index > 0)
			{
				pOutFrame = gData->outputList[0];
				if(pOutFrame && pOutFrame->m_removeable)
				{
					for(i = 0;i<gData->output_index-1;i++)
						gData->outputList[i]=gData->outputList[i+1];
					gData->output_index--;

					if(gData->output_index > 0)
						pOutPutInfo->Flag  = 1;
					else
						pOutPutInfo->Flag  = 0;
				}
				else
					pOutFrame = NULL;
			}

			gData->nOutPutting = 0;
		}
#endif
	}

	//printf(".");
	if(pOutFrame && pOutFrame->m_removeable)
	{
		I32_WMV  iSrcUVStride;
		I32_WMV  iSrcYOffset;
		I32_WMV  iSrcUOffset;
		I32_WMV  iSrcVOffset;
		I32_WMV  iFullSrcWidth;
		I32_WMV  iFullSrcHeight;
		I32_WMV  iOffsetX = 0, iOffsetY = 0;
		I32_WMV  iPadding = 64;
		I32_WMV iWidthSource =  pOutFrame->m_iFrmWidthSrc;
		I32_WMV iHeightSource =  pOutFrame->m_iFrmHeightSrc;

		I32_WMV  StrideY  = 0, StrideU  = 0, StrideV = 0;
		U8_WMV* ppxliRenderThisY = (U8_WMV*) (pOutFrame->m_pucYPlane);
		U8_WMV* ppxliRenderThisU = (U8_WMV*) (pOutFrame->m_pucUPlane);
		U8_WMV* ppxliRenderThisV = (U8_WMV*) (pOutFrame->m_pucVPlane);    
		iWidthSource = ( (iWidthSource + 15) >> 4 ) << 4;
		if (gData->m_bYUV411)
		{
			StrideY  = iWidthSource        + 64;
			StrideU  = (iWidthSource >> 2) + 32;
			StrideV  = StrideU;
			iOffsetX = 0;iOffsetY = 0;
			ppxliRenderThisY += StrideY * 32 + 32;
			ppxliRenderThisU += StrideU * 16 + 16;
			ppxliRenderThisV += StrideV * 16 + 16;
		}
		else {
			StrideY  = 0;StrideU  = 0;StrideV  = 0;
			iOffsetX = 32;iOffsetY = 32;
		}

		iFullSrcHeight = iHeightSource + iPadding;
		iFullSrcWidth  = iWidthSource  + iPadding;
		iSrcUVStride   = StrideU ? StrideU : (iFullSrcWidth >> 1);

		iSrcYOffset    = iOffsetY * iFullSrcWidth + iOffsetX;
		iSrcUOffset    = (iOffsetY >> 1) * iSrcUVStride  + (iOffsetX >> 1);
		iSrcVOffset    = iSrcUOffset;

		pOutput->Buffer[0] = ppxliRenderThisY + iSrcYOffset;
		pOutput->Buffer[1] = ppxliRenderThisU + iSrcUOffset;
		pOutput->Buffer[2] = ppxliRenderThisV + iSrcVOffset;
		pOutput->Stride[0]   = iFullSrcWidth;
		pOutput->Stride[1]   = iSrcUVStride;
		pOutput->Stride[2]   = iSrcUVStride;

		pOutput->ColorType = VO_COLOR_YUV_PLANAR420;
		pOutput->Time = pOutFrame->m_timeStamp;
		pOutPutInfo->Format.Width = pOutFrame->m_iFrmWidthSrc;
		pOutPutInfo->Format.Height = pOutFrame->m_iFrmHeightSrc; 

		pOutput->UserData = pOutFrame->m_UserData;
		pOutPutInfo->Format.Type = voFrameTypeSwap(pOutFrame->m_frameType);

#ifdef CHKLCS_ON
		gData->m_pPrevOutputBuff = pOutput;
		tWMVStatus = voCheckLibCheckVideo (gData->m_phCheck, pOutput, &pOutPutInfo->Format);
		//if(tWMVStatus != VO_ERR_NONE)
		//	return tWMVStatus;
#endif
	}
	else
	{
		pOutput->Buffer[0] = NULL; pOutput->Buffer[1] = NULL; pOutput->Buffer[2] = NULL;
		pOutput->Stride[0] = 0; pOutput->Stride[1] = 0; pOutput->Stride[2] = 0;
		pOutPutInfo->Format.Width = 0;
		pOutPutInfo->Format.Height = 0;
		pOutPutInfo->Flag = 0;
	}

	//printf("pOutPutInfo->Flag = %d \n",pOutPutInfo->Flag);

	return VO_ERR_NONE;
}
#else
#endif


VO_U32 VO_API voVC1DecGetParameter(VO_HANDLE hDec, VO_S32 nID, VO_PTR plValue)
{
	tWMVDecInternalMember*  gData  = (tWMVDecInternalMember*)hDec;
	VO_CODECBUFFER*			pInput = (VO_CODECBUFFER*)plValue;
	//I32_WMV ret =0;

	if( NULL == gData )
		return VO_ERR_INVALID_ARG;


	switch( nID ) {
	case VO_PID_VIDEO_FRAMETYPE:
		{
			tFrameType_WMV FrmTypeBackup  = gData->m_tFrmType;
			if(gData->m_bIsHeadDataParsed == 0)
				return VO_ERR_FAILED;

			if( WMV_Succeeded != WMVDecGetFrameType(gData, pInput) ) {

				return VO_ERR_FAILED;
			} 
			if(gData->m_tFrmType == BIVOP) {//APP just need to know if it is B frame,other frame type is not care.
				pInput->Time = (int)BVOP;
			}
			else 
            {
				pInput->Time = (int)gData->m_tFrmType; // I,P,B,
			}			
			gData->m_tFrmType = FrmTypeBackup;
		}
		break;
//#ifdef WIN32
//	case VO_PID_COMMON_HeadInfo:
//		{
//			VO_HEAD_INFO *pHeadInfo = (VO_HEAD_INFO *)plValue;
//			if( !gData->m_bIsHeadDataParsed ) {
//				sprintf(pHeadInfo->Description, "\n*** Error: It has not set VO_PID_COMMON_HEADDATA.Must set it before getting head info. ***");
//				pHeadInfo->Size = strlen(pHeadInfo->Description);
//				return VO_ERR_FAILED;
//			}
//			else {
//				strcpy(pHeadInfo->Description, gData->m_SeqHeadInfoString);
//				pHeadInfo->Size = strlen(gData->m_SeqHeadInfoString);
//			}
//		}
//		break;
//#endif
	case VO_PID_DEC_VC1_GET_LASTVIDEOINFO:
		{
			VO_VIDEO_OUTPUTINFO *pOutPutInfo = (VO_VIDEO_OUTPUTINFO *)plValue;

			if(gData->m_bIsHeadDataParsed == 0)
				return VO_ERR_FAILED;

			if(gData->m_pvopcRenderThis == NULL )
				return VO_ERR_FAILED;

			if(gData->frame_errorflag==0)
			{
				pOutPutInfo->Format.Width  = gData->m_iFrmWidthSrc;
				pOutPutInfo->Format.Height = gData->m_iFrmHeightSrc;
				pOutPutInfo->Format.Type = voFrameTypeSwap(gData->m_pvopcRenderThis->m_frameType);
			}
			else
			{
				pOutPutInfo->Format.Width  = 0;
				pOutPutInfo->Format.Height = 0;
				pOutPutInfo->Format.Type = VO_VIDEO_FRAME_NULL;
				return VO_ERR_FAILED;
			}
#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"VO_PID_DEC_VC1_GET_LASTVIDEOINFO %d %d\n",pOutPutInfo->Format.Width,pOutPutInfo->Format.Height);
		fclose(fp);
	}
#endif
		}
		break;
	case VO_PID_DEC_VC1_GET_LASTVIDEOBUFFER:
		{
			VO_VIDEO_BUFFER	*pOutBuf = (VO_VIDEO_BUFFER *)plValue;

			if(gData->m_bIsHeadDataParsed == 0)
				return VO_ERR_FAILED;

			if(gData->m_pfrmCurrQ == NULL  || gData->m_pfrmDisplayPre ==NULL)
				return VO_ERR_FAILED;
				
			if(gData->frame_errorflag ==0 && gData->m_outputMode == 0)
			{
				if (1)
				{
#ifdef ADJ_NEW
					if (gData->m_pfrmCurrQ->m_AdjustDecRangeNeed == 1 || gData->m_pfrmCurrQ->m_AdjustDecRangeAdvNeed ==1) 
					{
						if(gData->m_pfrmCurrQ->m_frameType == IVOP || gData->m_pfrmCurrQ->m_frameType == PVOP)
							gData->m_pvopcRenderThis = gData->m_pfrmPostQ;
						else
							gData->m_pvopcRenderThis = gData->m_pfrmDisplayPre;
					}
					else
#endif
					{
						if(gData->m_pfrmCurrQ->m_frameType == IVOP || gData->m_pfrmCurrQ->m_frameType == PVOP)
							gData->m_pvopcRenderThis = gData->m_pfrmCurrQ;
						else
							gData->m_pvopcRenderThis = gData->m_pfrmDisplayPre;
					}
				}
				else
				{
					if(gData->m_pfrmCurrQ->m_frameType == IVOP || gData->m_pfrmCurrQ->m_frameType == PVOP)
						gData->m_pvopcRenderThis = gData->m_pfrmCurrQ;
					else
						gData->m_pvopcRenderThis = gData->m_pfrmDisplayPre;

				}

				pOutBuf->Buffer[0] = gData->m_pvopcRenderThis->m_pucYPlane + gData->m_iWidthPrevYXExpPlusExp;
				pOutBuf->Buffer[1] = gData->m_pvopcRenderThis->m_pucUPlane + gData->m_iWidthPrevUVXExpPlusExp;
				pOutBuf->Buffer[2] = gData->m_pvopcRenderThis->m_pucVPlane + gData->m_iWidthPrevUVXExpPlusExp;

				pOutBuf->Stride[0] = gData->m_iWidthInternal;
				pOutBuf->Stride[1] = gData->m_iWidthInternalUV;
				pOutBuf->Stride[2] = gData->m_iWidthInternalUV;
				pOutBuf->ColorType = VO_COLOR_YUV_PLANAR420;
			}
			else
			{
				pOutBuf->Buffer[0] = NULL;
				pOutBuf->Buffer[1] = NULL;
				pOutBuf->Buffer[2] = NULL;
				pOutBuf->Stride[0] = 0;
				pOutBuf->Stride[1] = 0;
				pOutBuf->Stride[2] = 0;
				return VO_ERR_FAILED;
			}
		}
		break;
    case VO_PID_VIDEO_ASPECTRATIO:
		{
			if(gData->m_bIsHeadDataParsed == 0)
				return VO_ERR_FAILED;
			*((VO_S32*)plValue) = GetAspectRatio(gData);
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}
#ifdef MEMLEAK_CHECK
extern int mem_times;
extern voMemCheck  g_memcheck[4096];
#endif
VO_U32 VO_API voVC1DecUninit(VO_HANDLE hDec)
{
	tWMVDecInternalMember *gData = (tWMVDecInternalMember*)hDec;
	if( NULL == gData )
		return VO_ERR_INVALID_ARG;

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"UnInit  start \n");
		fclose(fp);
	}
#endif

#ifdef CHKLCS_ON
	voCheckLibUninit (gData->m_phCheck);
#endif

    if(gData->CpuNumber == 2 || gData->CpuNumber == 4)
        voVC1DestoryThreads(gData);

#ifdef SHAREMEMORY
	if(gData->m_pfrmList)
		FREE_PTR(gData,gData->m_pfrmList);
	if(gData->m_memManageFIFO.img_seq)
		FREE_PTR(gData,gData->m_memManageFIFO.img_seq);
#endif
	WMVideoDecClose ( gData );

#ifdef TRACEVC1
	{
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp,"UnInit  finishe \n");
		fclose(fp);
	}
#endif
#ifdef MEMLEAK_CHECK
	{
		int i=0;
		for(i=0;i<4094;i++)
		{
			if(g_memcheck[i].alloc_add != g_memcheck[i].free_add )
				printf("i=%d alloc %d size %d  free %d \n",i, g_memcheck[i].alloc_add,g_memcheck[i].alloc_size,g_memcheck[i].free_add );
		}
	}
#endif

	return VO_ERR_NONE;
}

I32_WMV YUV411_To_YUV420( U8_WMV **pSrcBuff, U8_WMV **pDstBuff, I32_WMV *iSrcStride, I32_WMV iW, I32_WMV iH )
{
	I32_WMV i,j,m,width,height2;
	U8_WMV *pSrc,*pDst;
	U8_WMV *pSrc2,*pDst2;
	I32_WMV	srcStrideUV,dstStrideUV;

	if(0 == (iW&15)) { // (iW % 16) == 0

		/* Y. Just copy */
		pSrc = pSrcBuff[0];
		pDst = pDstBuff[0];
		for(i = 0; i< iH; i++ ) {
			for(j = 0; j < iW; j += 16 ) {
				*(U32_WMV *)(pDst+j)    = *(U32_WMV *)(pSrc+j); 
				*(U32_WMV *)(pDst+j+4)  = *(U32_WMV *)(pSrc+j+4); 
				*(U32_WMV *)(pDst+j+8)  = *(U32_WMV *)(pSrc+j+8); 
				*(U32_WMV *)(pDst+j+12) = *(U32_WMV *)(pSrc+j+12); 
			}
			pSrc += iSrcStride[0];
			pDst += iW;
		}

		width   = iW>>2;
		height2 = iH>>1;
		srcStrideUV = iSrcStride[1];
		dstStrideUV = iW>>1;

		/* U,V */
		pSrc  = pSrcBuff[1];
		pDst  = pDstBuff[1];
		pSrc2 = pSrcBuff[2];
		pDst2 = pDstBuff[2];
		for(i = 0; i< height2; i++ ) {
			for(j = 0,m = 0; j< width; j += 4,m += 8  ) {
				pDst[m+0]  = pDst[m+1]  = (pSrc[j+0]  + pSrc[j+0+srcStrideUV]  + 1) >> 1;
				pDst[m+2]  = pDst[m+3]  = (pSrc[j+1]  + pSrc[j+1+srcStrideUV]  + 1) >> 1;
				pDst[m+4]  = pDst[m+5]  = (pSrc[j+2]  + pSrc[j+2+srcStrideUV]  + 1) >> 1;
				pDst[m+6]  = pDst[m+7]  = (pSrc[j+3]  + pSrc[j+3+srcStrideUV]  + 1) >> 1;
				pDst2[m+0] = pDst2[m+1] = (pSrc2[j+0] + pSrc2[j+0+srcStrideUV] + 1) >> 1;
				pDst2[m+2] = pDst2[m+3] = (pSrc2[j+1] + pSrc2[j+1+srcStrideUV] + 1) >> 1;
				pDst2[m+4] = pDst2[m+5] = (pSrc2[j+2] + pSrc2[j+2+srcStrideUV] + 1) >> 1;
				pDst2[m+6] = pDst2[m+7] = (pSrc2[j+3] + pSrc2[j+3+srcStrideUV] + 1) >> 1;
			}
			pSrc  += (srcStrideUV<<1);
			pDst  += dstStrideUV;
			pSrc2 += (srcStrideUV<<1);
			pDst2 += dstStrideUV;
		}
	}
	else { // (iW % 8) == 0

		/* Y. Just copy */
		pSrc = pSrcBuff[0];
		pDst = pDstBuff[0];
		for(i = 0; i< iH; i++ ) {
			for(j = 0; j < iW; j += 8 ) {
				*(U32_WMV *)(pDst+j)   = *(U32_WMV *)(pSrc+j); 
				*(U32_WMV *)(pDst+j+4) = *(U32_WMV *)(pSrc+j+4); 
			}
			pSrc += iSrcStride[0];
			pDst += iW;
		}

		width   = iW>>2;
		height2 = iH>>1;
		srcStrideUV = iSrcStride[1];
		dstStrideUV = iW>>1;

		/* U,V */
		pSrc  = pSrcBuff[1];
		pDst  = pDstBuff[1];
		pSrc2 = pSrcBuff[2];
		pDst2 = pDstBuff[2];
		for(i = 0; i< height2; i++ ) {
			for(j = 0,m = 0; j< width; j += 2,m += 4  ) {
				pDst[m+0]  = pDst[m+1]  = (pSrc[j+0]  + pSrc[j+0+srcStrideUV]  + 1) >> 1;
				pDst[m+2]  = pDst[m+3]  = (pSrc[j+1]  + pSrc[j+1+srcStrideUV]  + 1) >> 1;
				pDst2[m+0] = pDst2[m+1] = (pSrc2[j+0] + pSrc2[j+0+srcStrideUV] + 1) >> 1;
				pDst2[m+2] = pDst2[m+3] = (pSrc2[j+1] + pSrc2[j+1+srcStrideUV] + 1) >> 1;
			}
			pSrc  += (srcStrideUV<<1);
			pDst  += dstStrideUV;
			pSrc2 += (srcStrideUV<<1);
			pDst2 += dstStrideUV;
		}
	}

	return 0;
}

tWMVDecodeStatus WMVideoDecClose (HWMVDecoder phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose 1");
#endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    tWMVStatus = WMVInternalMemberDestroy (pWMVDec);

	wmvFree (pWMVDec, pWMVDec);
    pWMVDec = NULL_WMV;  

    return tWMVStatus;
}

/* This is for WMVP/WVP2 format file. */
tWMVDecodeStatus WMVideoDecClose2 (HWMVDecoder phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose2 1");
#endif

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    tWMVStatus = WMVInternalMemberDestroy (pWMVDec);

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVideoDecClose2 2");
#endif

    return tWMVStatus;
}



// ASF Binding enum
#define ASFBINDING_SIZE                   1   // size of ASFBINDING is 1 byte     
#define ASFBINDING_RESERVED               1   // bit 0 - reserved
#define ASFBINDING_NO_BFRAME              2   // bit 1 - when 0, b frame might be present
#define ASFBINDING_NO_SLICECODE           4   // bit 2 - when 0, slice might be present    
#define ASFBINDING_NO_MULTIPLEENTRY       8   // bit 3 - when 0, mult. entry headers might be present    
#define ASFBINDING_NO_MULTIPLESEQ        16   // bit 4 - when 0, mult. sequence headers might be present    
#define ASFBINDING_NO_INTERLACESOURCE    32   // bit 5 - when 0, interlace seq might be present

tWMVDecodeStatus voVC1_WMVideoDecDecodeSequenceHeader (HWMVDecoder* phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    U32_WMV uiBufferLengthFromCaller;
    U8_WMV* pucBufferBits = NULL;
    Bool_WMV bNotEndOfFrame = TRUE;
    tWMVDecInternalMember *pWMVDec;

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (*phWMVDecoder);

    WMVDecCBGetData (pWMVDec,0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame){
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL){
            return WMV_BrokenFrame;
        }
    }


    if (pWMVDec->m_cvCodecVersion == WMVA) 
	{             
        uiBufferLengthFromCaller = uiBufferLengthFromCaller - ASFBINDING_SIZE;
        
        if (uiBufferLengthFromCaller == 0) {
            WMVDecCBGetData (pWMVDec, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
            BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, 
                uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
        } else {
            BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits + ASFBINDING_SIZE, 
                uiBufferLengthFromCaller, bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
        }
    } else {
        BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, 
            bNotEndOfFrame, pWMVDec->m_cvCodecVersion == WMVA);
    }
    
    pWMVDec->m_pInputBitstream->m_bNotEndOfFrame = bNotEndOfFrame;
    tWMVStatus = decodeVOLHeadVC1 (pWMVDec, uiBufferLengthFromCaller, 0, 0, 0, 0);

    return tWMVStatus;
}

I32_WMV WMVideoDecGetFrameRate (HWMVDecoder hWMVDecoder) {
    //tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

    if (hWMVDecoder == NULL_WMV)
        return 0;

    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    return pWMVDec->m_iFrameRate;
}

// ****************************************************************************************************

#ifdef _SPRITE_DUMPFRAME_
FILE * g_hDump;
#endif

Void_WMV checkEndOfFrame(tWMVDecInternalMember *pWMVDec)
{   
    if(pWMVDec->m_pInputBitstream->m_bNotEndOfFrame)
    {
        U8_WMV* pucBufferBits;
        Bool_WMV bNotEndOfFrame;
        U32_WMV uSize;

        bNotEndOfFrame = pWMVDec->m_pInputBitstream->m_bNotEndOfFrame;
        while(bNotEndOfFrame)
        {
            CInputBitStream_WMV * pThis = pWMVDec->m_pInputBitstream;
            WMVDecCBGetData (pWMVDec,0, &pucBufferBits, 4, &uSize, &bNotEndOfFrame);
            pThis->m_bNotEndOfFrame = bNotEndOfFrame;
            if (pThis->m_pWMVDec->m_cvCodecVersion == WMVA) {
                U8_WMV * p = pucBufferBits;
                ParseStartCodeChunk(pThis->m_pWMVDec, p, uSize, &p, &uSize, bNotEndOfFrame);
            }
        }
    }
}

I32_WMV SwitchEntryPoint (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV result;
    I32_WMV iPrevEncWidth;
    I32_WMV iPrevEncHeight;

    pWMVDec->m_bSwitchingEntryPoint = TRUE;
    
    ResetConditionalVariablesForSequenceSwitch (pWMVDec);  // Rename this function to ResetConditionalVariablesForEntrySwitch
                                                    // once we switch over to new entry header
     iPrevEncWidth = pWMVDec->m_iFrmWidthSrc;
     iPrevEncHeight = pWMVDec->m_iFrmHeightSrc;

    result = DecodeEntryPointHeader(pWMVDec, NULL, NULL);
    if (ICERR_OK != result) {
        return result;
    }
    //Only after entry-point header can we know the real encoding size.
    //Now we can adjust pointer and variables.
    if (pWMVDec->m_iFrmWidthSrc != iPrevEncWidth || pWMVDec->m_iFrmHeightSrc != iPrevEncHeight) {
        pWMVDec->m_bSeqFrameWidthOrHeightChanged = TRUE;
    } else {
        pWMVDec->m_bSeqFrameWidthOrHeightChanged = FALSE;
    }

    result = InitDataMembers_WMVA (pWMVDec, pWMVDec->m_iFrmWidthSrc, pWMVDec->m_iFrmHeightSrc);    
    pWMVDec->m_bSwitchingEntryPoint = TRUE;
	//zou 66
	if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc > pWMVDec->m_iMaxPrevSeqFrameArea)
    {
        pWMVDec->m_iMaxPrevSeqFrameArea = pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc;
        FreeFrameAreaDependentMemory(pWMVDec);
        result = initFrameAreaDependentMemory(pWMVDec, pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
        if (ICERR_OK != result)
            return result;
    } 
    
    return result;
}

tWMVDecodeStatus voVC1ParpareBufferIVOP(tWMVDecInternalMember *pWMVDec)
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

	if ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP )&& pWMVDec->frame_errorflag == 0)
    {
        pWMVDec->m_fDecodedI = TRUE_WMV;        
        if (pWMVDec->m_bBFrameOn) {
            if (pWMVDec->m_iNumBFrames > 0) {
                if (pWMVDec->m_iBState == -3) {
                    //assert (pWMVDec->m_tFrmType != BVOP); // otherwise we are screwed!
					if(pWMVDec->m_tFrmType == BVOP)
						return WMV_Failed;
                    pWMVDec->m_iBState = 1;
                }
                else if (pWMVDec->m_tFrmType == BIVOP) {
                    // b frame coded as I
                    if (pWMVDec->m_iBState == 1) {
                        if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422,fix I B I issue.
                        {
							pWMVDec->m_uiNumFramesLeftForOutput = 1;
						}
						else
						{
							pWMVDec->m_bSkipThisFrame = TRUE;
							pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
							checkEndOfFrame(pWMVDec);
							pWMVDec->m_bRenderDirect = TRUE_WMV;
							//pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
							return WMV_Succeeded;
						}
                    }
                    
                    if (pWMVDec->m_bPrevAnchor) {
                        swapCurrRef1Pointers (pWMVDec);
                    }
                }
                else {
                    if (pWMVDec->m_iBState == 1) {               
						if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422
						{
							 pWMVDec->m_iBState=0;
							 swapRef0Ref1Pointers(pWMVDec);
						}
						else
						{
							pWMVDec->m_iBState=0;
							swapCurrRef1Pointers (pWMVDec);
							swapRef0Ref1Pointers(pWMVDec);
						}
                    }
                    else {
                        if (pWMVDec->m_bPrevAnchor) {
                            swapCurrRef1Pointers (pWMVDec);
                        }
                        swapRef0Ref1Pointers(pWMVDec);
                    }

                    if(pWMVDec->m_cvCodecVersion != WMVA)
                        pWMVDec->m_iPrevIFrame = 1;
                    pWMVDec->m_iDroppedPFrame = 0;
                }
            }   // if (pWMVDec->m_iNumBFrames > 0)
            else
                WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);                        
        }
    }
    else if (!pWMVDec->m_fDecodedI) {
        checkEndOfFrame(pWMVDec);         
        return WMV_NoKeyFrameDecoded;
    }

#ifdef SHAREMEMORY 
	if(voVC1GetFreeBufferForCur(pWMVDec)!=WMV_Succeeded)
		return WMV_Failed;
#endif
    pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
    pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData; 
	pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;

	return WMV_Succeeded;
}

tWMVDecodeStatus voVC1ParpareBufferIVOPNew(tWMVDecInternalMember *pGlobleWMVDec,tWMVDecInternalMember *pWMVDec)
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

	if ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP )&& pWMVDec->frame_errorflag == 0)
    {
        pWMVDec->m_fDecodedI = TRUE_WMV;        
        if (pWMVDec->m_bBFrameOn) {
            if (pWMVDec->m_iNumBFrames > 0) {
                if (pWMVDec->m_iBState == -3) {
                    //assert (pWMVDec->m_tFrmType != BVOP); // otherwise we are screwed!
					if(pWMVDec->m_tFrmType == BVOP)
						return WMV_Failed;
                    pWMVDec->m_iBState = 1;
                }
                else if (pWMVDec->m_tFrmType == BIVOP) {
                    // b frame coded as I
                    if (pWMVDec->m_iBState == 1) {
                        if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422,fix I B I issue.
                        {
							pWMVDec->m_uiNumFramesLeftForOutput = 1;
						}
						else
						{
							pWMVDec->m_bSkipThisFrame = TRUE;
							pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
							checkEndOfFrame(pWMVDec);
							pWMVDec->m_bRenderDirect = TRUE_WMV;
							//pWMVDec->m_eFrameWasOutput = FrameDecOut_OutputOptional;
							return WMV_Succeeded;
						}
                    }
                    
                    if (pWMVDec->m_bPrevAnchor) {
                        swapCurrRef1Pointers (pWMVDec);
                    }
                }
                else {
                    if (pWMVDec->m_iBState == 1) {               
						if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1)//zou 422
						{
							 pWMVDec->m_iBState=0;
							 swapRef0Ref1Pointers(pWMVDec);
						}
						else
						{
							pWMVDec->m_iBState=0;
							swapCurrRef1Pointers (pWMVDec);
							swapRef0Ref1Pointers(pWMVDec);
						}
                    }
                    else {
                        if (pWMVDec->m_bPrevAnchor) {
                            swapCurrRef1Pointers (pWMVDec);
                        }
                        swapRef0Ref1Pointers(pWMVDec);
                    }

                    if(pWMVDec->m_cvCodecVersion != WMVA)
                        pWMVDec->m_iPrevIFrame = 1;
                    pWMVDec->m_iDroppedPFrame = 0;
                }
            }   // if (pWMVDec->m_iNumBFrames > 0)
            else
                WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);                        
        }
    }
    else if (!pWMVDec->m_fDecodedI) {
        checkEndOfFrame(pWMVDec);         
        return WMV_NoKeyFrameDecoded;
    }

#ifdef SHAREMEMORY 
	if(voVC1GetFreeBufferForCur(pWMVDec)!=WMV_Succeeded)
		return WMV_Failed;
#endif
    pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
    pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;  
	pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;

	pWMVDec->m_tFrmTypePrevious = pWMVDec->m_tFrmType;
	pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);

	return WMV_Succeeded;
}

// ****************************************************************************************************
tWMVDecodeStatus voVC1DecHandleIVOP(  tWMVDecInternalMember *pWMVDec )
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
	
    if (pWMVDec->m_bMultiresEnabled)
    {
		//printf("%d %d \n",pWMVDec->m_iResIndexPrev,pWMVDec->m_iResIndex);
        if(HandleResolutionChange (pWMVDec))
            return WMV_Failed;
    }
    
    if(pWMVDec->m_pfrmRef0Q == pWMVDec->m_pfrmCurrQ && pWMVDec->nInputFrameIndx > 1)
    	return -1;

#ifdef USE_FRAME_THREAD
	if (pWMVDec->m_bInterlaceV2 || pWMVDec->m_bYUV411) 
	{
		//printf("pWMVDec->m_bInterlaceV2 = %d I\n ",pWMVDec->m_bInterlaceV2);
		if(pWMVDec->CpuNumber > 1 && pWMVDec->nInputFrameIndx > 1)
		{
			volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
			while(*ref_decodeprocess < (I32_WMV)pWMVDec->m_uintNumMBY+2)
			{
				thread_sleep(0);
				bkprintf("inter 411 I \n");
			}

			ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
			while(*ref_decodeprocess < (I32_WMV)pWMVDec->m_uintNumMBY+2)
			{
				thread_sleep(0);
				bkprintf("inter 411 I \n");
			}
		}
	}
#endif

    if (pWMVDec->m_bInterlaceV2) 
    {
#ifdef STABILITY
        if(pWMVDec->m_pFieldMvX_Pbuf == NULL)
            return WMV_Failed;
#endif
        tWMVStatus = decodeIInterlaceV2 (pWMVDec);
		Repeatpad (pWMVDec);
    }
    else if (pWMVDec->m_bXintra8) //for WMV3 WVC1 should be no this flag;
        //refto: Table 263: Sequence Header Data Structure STRUCT_C for Simple and Main Profiles
        tWMVStatus = WMV_Failed;//tWMVStatus = WMVideoDecDecodeI_X8 (pWMVDec);
    else if (pWMVDec->m_bYUV411)
	{
        tWMVStatus = decodeIInterlace411 (pWMVDec);
		Repeatpad (pWMVDec);
	}
    else
	{
        tWMVStatus = WMVideoDecDecodeI (pWMVDec);	
		if(pWMVDec->CpuNumber <= 1)
			Repeatpad (pWMVDec);
	}

	if (tWMVStatus != WMV_Succeeded) 
	{
        return tWMVStatus; 
	}
       
    if (pWMVDec->m_tFrmType == IVOP)
        pWMVDec->m_PredTypePrev = IVOP;
 
#if 1
    if (pWMVDec->m_cvCodecVersion >= WMVA && pWMVDec->m_tFrmType != BIVOP && pWMVDec->m_iNumBFrames > 0 && (pWMVDec->m_bYUV411 || pWMVDec->m_bInterlaceV2)) 
    {
        U32_WMV imbY, imbX;
        for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
            for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
            {
                I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
                I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
                pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
                pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
                pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
                pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
                pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
                pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
                pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
                pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
            }
    }


#if 0 //zou m_vctTrueHalfPel will used for B frame,the will be calculated by P frame, here we set to Zero.
	   //I move this operation to process of B frame.
  if (pWMVDec->m_cvCodecVersion >= WMVA && pWMVDec->m_tFrmType != BIVOP && pWMVDec->m_iNumBFrames > 0)
  {
		U32_WMV imbY, imbX;
        for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
            for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
            {
                pWMVDec->m_rgmv1_EMB[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.I16[X_INDEX] = 0;
                pWMVDec->m_rgmv1_EMB[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.I16[Y_INDEX] = 0;
            }
    }
#endif
#endif

   voprintf("[thread%d]  voVC1DecHandleIVOP  Finished  0x%x  %d\n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ,pWMVDec->nInputFrameIndx);
    return WMV_Succeeded;
}

tWMVDecodeStatus voVC1ParpareBufferPVOP(  tWMVDecInternalMember *pWMVDec )
{
	 if (pWMVDec->m_iNumBFrames == 0) {
        WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);
        pWMVDec->m_PredTypePrev = PVOP;
    }
    else {
        if (pWMVDec->m_iBState == 1) {
			if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1) {
                pWMVDec->m_iBState=0;
				swapRef0Ref1Pointers(pWMVDec);
			}
			else {
				pWMVDec->m_iBState=0;
				swapCurrRef1Pointers (pWMVDec);
				swapRef0Ref1Pointers(pWMVDec);
			}
        } else {
            if (pWMVDec->m_bPrevAnchor) {
                swapCurrRef1Pointers (pWMVDec);
            }
            swapRef0Ref1Pointers(pWMVDec);            
        }
    }   // if not pWMVDec->m_iNumBFrames == 0
            
    pWMVDec->m_iDroppedPFrame = 0;
    pWMVDec->m_PredTypePrev = PVOP;

#ifdef SHAREMEMORY 
	if(voVC1GetFreeBufferForCur(pWMVDec)!=WMV_Succeeded)
		return WMV_Failed;
#endif
            
    pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
    pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
	pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;

	if (pWMVDec->m_bPreProcRange)
	{
		if (pWMVDec->m_tFrmPredType == IVOP || pWMVDec->m_tFrmPredType == BIVOP)
			pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
	}

	//m_scaling: 01 scale down; m_scaling: 10 scale up;
	pWMVDec->m_scaling = (pWMVDec->m_iReconRangeState<<1)|pWMVDec->m_iRangeState;
	if(pWMVDec->m_scaling == 3)
		pWMVDec->m_scaling = 0;

	if(pWMVDec->m_scaling == 1 || pWMVDec->m_scaling == 2)
		pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;

#ifdef USE_FRAME_THREAD
	//Set BUFFER_PARPARED of this frame
	if ( pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp)
		pWMVDec->bUseRef0Process = 1;
	else
		pWMVDec->bUseRef0Process = 0;
	//printf("[thread%d]   P  VOP buffer parared  %d Reference = 0x%x \n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ->m_decodebufferparpared,pWMVDec->m_pfrmRef0Q);
#endif

	return WMV_Succeeded;
}

tWMVDecodeStatus voVC1ParpareBufferPVOPNew(tWMVDecInternalMember *pGlobleWMVDec, tWMVDecInternalMember *pWMVDec )
{
	 if (pWMVDec->m_iNumBFrames == 0) {
        WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);
        pWMVDec->m_PredTypePrev = PVOP;
    }
    else {
        if (pWMVDec->m_iBState == 1) {
			if(pWMVDec->m_bCodecIsWVC1 && pWMVDec->m_IBPFlag ==1) {
                pWMVDec->m_iBState=0;
				swapRef0Ref1Pointers(pWMVDec);
			}
			else {
				pWMVDec->m_iBState=0;
				swapCurrRef1Pointers (pWMVDec);
				swapRef0Ref1Pointers(pWMVDec);
			}
        } else {
            if (pWMVDec->m_bPrevAnchor) {
                swapCurrRef1Pointers (pWMVDec);
            }
            swapRef0Ref1Pointers(pWMVDec);            
        }
    }   // if not pWMVDec->m_iNumBFrames == 0
            
    pWMVDec->m_iDroppedPFrame = 0;
    pWMVDec->m_PredTypePrev = PVOP;

#ifdef SHAREMEMORY 
	if(voVC1GetFreeBufferForCur(pWMVDec)!=WMV_Succeeded)
		return WMV_Failed;
#endif
            
    pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
    pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
	pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;

	if (pWMVDec->m_bPreProcRange)
	{
		if (pWMVDec->m_tFrmPredType == IVOP || pWMVDec->m_tFrmPredType == BIVOP)
			pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
	}

	//m_scaling: 01 scale down; m_scaling: 10 scale up;
	pWMVDec->m_scaling = (pWMVDec->m_iReconRangeState<<1)|pWMVDec->m_iRangeState;
	if(pWMVDec->m_scaling == 3)
		pWMVDec->m_scaling = 0;

	if(pWMVDec->m_scaling == 1 || pWMVDec->m_scaling == 2)
		pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;

	pWMVDec->m_tFrmTypePrevious = pWMVDec->m_tFrmType;
	pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);

	//201318
	if ( pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp)
	{
		pWMVDec->m_pfrmRef0Process = pWMVDec->m_pfrmRef0Buffer[pWMVDec->m_ref0Indx];
		pWMVDec->m_ref0Indx++;
		pWMVDec->m_ref0Indx = pWMVDec->m_ref0Indx%pWMVDec->CpuNumber;
		pWMVDec->bUseRef0Process = 1;
	}
	else
	{
		//pWMVDec->m_pfrmRef0Process = pWMVDec->m_pfrmRef0Buffer[0];
		pWMVDec->bUseRef0Process = 0;
	}

	//printf("p  pWMVDec->bUseRef0Process = %d \n",pWMVDec->bUseRef0Process);

	pWMVDec->m_iPrevIFrame = 0;  
	
	return WMV_Succeeded;
}

tWMVDecodeStatus voVC1DecHandlePVOP(  tWMVDecInternalMember *pWMVDec )
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
	U8_WMV LUT[256];
	U8_WMV LUTUV[256];
 	
    if (pWMVDec->m_cvCodecVersion >= WMV3) 
	{
        if (pWMVDec->m_bMultiresEnabled)
        {
            if(HandleResolutionChange (pWMVDec))
                return WMV_Failed;
        }

		if(pWMVDec->m_pfrmRef0Q == pWMVDec->m_pfrmCurrQ)
			return -1;
		//printf("pWMVDec->m_bLuminanceWarp = %d \n",pWMVDec->m_bLuminanceWarp);

		if (pWMVDec->m_bLuminanceWarp) 	
		{
			pWMVDec->m_pLUT = LUT;
			pWMVDec->m_pLUTUV = LUTUV;
			IntensityCompensation_Parpare(pWMVDec);
		}

		//printf("pWMVDec->m_bInterlaceV2 = %d \n",pWMVDec->m_bInterlaceV2);

		if (pWMVDec->m_bInterlaceV2 || pWMVDec->m_bYUV411) 
		{
			//printf("pWMVDec->m_bInterlaceV2 = %d P\n ",pWMVDec->m_bInterlaceV2);
			if(pWMVDec->CpuNumber > 1)
			{
				volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
				while(*ref_decodeprocess < (I32_WMV)pWMVDec->m_uintNumMBY+2)
				{
					thread_sleep(0);
					bkprintf("inter 411 p \n");
				}
			}

			if ( pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp)
			{
				U32_WMV mb_row = 0;
				pWMVDec->m_EMB_PMainLoop.PB[0].m_ppxliRefBlkBase[0] = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
				pWMVDec->m_EMB_PMainLoop.PB[0].m_ppxliRefBlkBase[4] = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
				pWMVDec->m_EMB_PMainLoop.PB[0].m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

				for(mb_row = 0;mb_row<2+pWMVDec->m_uintNumMBY+2;mb_row++)//pad 4 mb row 
				{	
					AdjustReconRange_mbrow(pWMVDec, mb_row);
					IntensityCompensation_mbrow (pWMVDec, mb_row);
				}
				pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Process->m_pucYPlane;
				pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Process->m_pucUPlane;
				pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Process->m_pucVPlane;
				pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
			}
		}
	
        if (pWMVDec->m_bInterlaceV2) 
        {
            if(pWMVDec->m_pFieldMvX_Pbuf== NULL)
                return WMV_Failed;
            tWMVStatus = decodePInterlaceV2 (pWMVDec);
			Repeatpad (pWMVDec);
        }
        else if (pWMVDec->m_bYUV411) 
		{
            tWMVStatus = decodePInterlace411 (pWMVDec);
			Repeatpad (pWMVDec);
		}
        else  // WMVideoDecDecodeP_X9_EMB
		{
            tWMVStatus = WMVideoDecDecodeP_X9_EMB(pWMVDec);

			if(pWMVDec->CpuNumber <= 1)
				Repeatpad (pWMVDec);
		}
    }

	voprintf("[thread%d]   voVC1DecHandlePVOP finished  0x%x %d\n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ,pWMVDec->nInputFrameIndx);
	
	return tWMVStatus;
}


tWMVDecodeStatus voVC1ParpareBufferBVOPNew(tWMVDecInternalMember *pGlobleWMVDec,tWMVDecInternalMember *pWMVDec )
{
	if (pWMVDec->m_iBState == 1) 
	 {
         if(pWMVDec->m_bCodecIsWVC1) 
		 {
             pWMVDec->m_uiNumFramesLeftForOutput = 1;
             pWMVDec->m_IBPFlag = 1;
         }
        else
        {
            pWMVDec->m_bSkipThisFrame = TRUE;
            pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
            // ?? can I do this ??? pWMVDec->m_bRenderDirect == TRUE_WMV
            checkEndOfFrame(pWMVDec);
            pWMVDec->m_bRenderDirect = TRUE_WMV;
            return WMV_Succeeded;
        }
    }
            
    if (pWMVDec->m_iDroppedPFrame) {
        // risky!! this means we MUST be in VBR and the frame was "skipped" not "dropped"
        pWMVDec->m_iDroppedPFrame = 0;
        if (pWMVDec->m_bPrevAnchor) {
            // even more caution!!! means we have a long skip sequence
            swapCurrRef1Pointers (pWMVDec);
        }
        swapRef0Ref1Pointers(pWMVDec);                
        // propagate ref to current -- because these 2 were very similar which is why we VBR skipped
		pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmCurrQ;
		pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
		pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
		pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;	
        // whether or not this was true before, it must be true now ...
        pWMVDec->m_bPrevAnchor = TRUE;
    }
            
    if (pWMVDec->m_bPrevAnchor) //I P
        swapCurrRef1Pointers (pWMVDec);

#ifdef SHAREMEMORY 
	if(voVC1GetFreeBufferForCur(pWMVDec)!=WMV_Succeeded)
		return WMV_Failed;
#endif
	pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
	pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
	pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;

	pWMVDec->m_PredTypePrev = PVOP;
	pWMVDec->m_bPrevAnchor = (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP);

	return WMV_Succeeded;
}

tWMVDecodeStatus voVC1ParpareBufferBVOP(  tWMVDecInternalMember *pWMVDec )
{
	if (pWMVDec->m_iBState == 1) 
	 {
         if(pWMVDec->m_bCodecIsWVC1) 
		 {
             pWMVDec->m_uiNumFramesLeftForOutput = 1;
             pWMVDec->m_IBPFlag = 1;
         }
        else
        {
            pWMVDec->m_bSkipThisFrame = TRUE;
            pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
            // ?? can I do this ??? pWMVDec->m_bRenderDirect == TRUE_WMV
            checkEndOfFrame(pWMVDec);
            pWMVDec->m_bRenderDirect = TRUE_WMV;
            return WMV_Succeeded;
        }
    }
            
    if (pWMVDec->m_iDroppedPFrame) {
        // risky!! this means we MUST be in VBR and the frame was "skipped" not "dropped"
        pWMVDec->m_iDroppedPFrame = 0;
        if (pWMVDec->m_bPrevAnchor) {
            // even more caution!!! means we have a long skip sequence
            swapCurrRef1Pointers (pWMVDec);
        }
        swapRef0Ref1Pointers(pWMVDec);                
        // propagate ref to current -- because these 2 were very similar which is why we VBR skipped
		pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmCurrQ;
		pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
		pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
		pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;	
        // whether or not this was true before, it must be true now ...
        pWMVDec->m_bPrevAnchor = TRUE;
    }
            
    if (pWMVDec->m_bPrevAnchor) //I P
        swapCurrRef1Pointers (pWMVDec);

#ifdef SHAREMEMORY 
	if(voVC1GetFreeBufferForCur(pWMVDec)!=WMV_Succeeded)
		return WMV_Failed;
#endif
	pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
	pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
    pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
	pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
	pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;

	pWMVDec->m_PredTypePrev = PVOP;

	return WMV_Succeeded;
}

tWMVDecodeStatus voVC1DecHandleBVOP(  tWMVDecInternalMember *pWMVDec )
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

	if(pWMVDec->m_bSkipThisFrame)
		return WMV_Succeeded;

	if(pWMVDec->m_pfrmRef0Q == pWMVDec->m_pfrmCurrQ)
		return -1;

	if (pWMVDec->m_bYUV411 || pWMVDec->m_bInterlaceV2) 
    {
		//printf("pWMVDec->m_bInterlaceV2 = %d B\n ",pWMVDec->m_bInterlaceV2);
		if(pWMVDec->CpuNumber > 1)
		{
			volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmRef0Q->m_decodeprocess;
			while(*ref_decodeprocess < (I32_WMV)pWMVDec->m_uintNumMBY+2)
			{
				thread_sleep(0);
				bkprintf("inter 411 B\n");
			}
			ref_decodeprocess = &pWMVDec->m_pfrmRef1Q->m_decodeprocess;
			while(*ref_decodeprocess < (I32_WMV)pWMVDec->m_uintNumMBY+2)
			{
				thread_sleep(0);
				bkprintf("inter 411 B \n");
			}

			if (pWMVDec->bUseRef0Process)
			{
				ref_decodeprocess = &pWMVDec->m_pfrmRef0Process->m_decodeprocess;
				while(*ref_decodeprocess < (I32_WMV)pWMVDec->m_uintNumMBY+2)
				{
					thread_sleep(0);
					bkprintf("inter 411 B\n");
				}
			}
			
			//use pFieldMvX_Pbuf of last frame
			//pWMVDec->m_pFieldMvX_Pbuf = pWMVDec->m_pfrmRef0Q->pFieldMvX_Pbuf ;
			//pWMVDec->m_pFieldMvY_Pbuf = pWMVDec->m_pfrmRef0Q->pFieldMvY_Pbuf ;

		}

        if (pWMVDec->m_bInterlaceV2) 
        {
#ifdef STABILITY
            if(pWMVDec->m_pFieldMvX_Pbuf== NULL || pWMVDec->m_pFieldMvX_FPred == NULL 
                || pWMVDec->m_pFieldMvY_FPred == NULL || pWMVDec->m_pFieldMvX_BPred == NULL 
                ||  pWMVDec->m_pFieldMvY_BPred == NULL)
                return WMV_Failed;
#endif
            tWMVStatus = decodeBInterlaceV2 (pWMVDec);
        }
		else
        {
			tWMVStatus = decodeBInterlace411 (pWMVDec);
        }
    }
	else {
		tWMVStatus = decodeB_EMB(pWMVDec);
    }

	voprintf("[thread%d]   voVC1DecHandleBVOP finished  0x%x %d\n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ,pWMVDec->nInputFrameIndx);

    return tWMVStatus;
}

int OutputOneFrame_Test( U8_WMV* out_srcy, U8_WMV* out_srcu, U8_WMV* out_srcv,
						I32_WMV  y_stride,I32_WMV  uv_stride,I32_WMV width, I32_WMV height,
						FILE*outFile)
{
	int i;
	/* output decoded YUV420 frame */
	/* Y */
	for( i = 0;i < height; i++ ) {
		fwrite(out_srcy, width, 1, outFile);
		out_srcy += y_stride;
	}

	/* U */
	for(i = 0; i< height/2; i++ ) {
		fwrite(out_srcu, width/2, 1, outFile);
		out_srcu += uv_stride;
	}

	/* V */
	for( i = 0; i < height/2; i++ ) {
		fwrite(out_srcv, width/2, 1, outFile);
		out_srcv += uv_stride;
	}

	return 0;
}


tWMVDecodeStatus WMVideoDecVOP (tWMVDecInternalMember *pWMVDec)
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
	pWMVDec->m_nframes++;

	//printf("framtype = %d  Cur = 0x%x   ref0 = 0x%x, ref1 = 0x%x \n",pWMVDec->m_tFrmType,pWMVDec->m_pfrmCurrQ,pWMVDec->m_pfrmRef0Q,pWMVDec->m_pfrmRef1Q);
	//printf("%d m_bInterlaceV2 =%d m_bLuminanceWarp = %d m_tFrmType = %d \n\n",pWMVDec->m_nframes,pWMVDec->m_bInterlaceV2,pWMVDec->m_bLuminanceWarp,pWMVDec->m_tFrmType);
	//printf("pWMVDec->m_tFrmType  = %d   %d %d %d\n",pWMVDec->m_tFrmType, pWMVDec->m_nIframes,pWMVDec->m_nPframes,pWMVDec->m_nBframes);
	//printf("framtype = %d  Cur = 0x%x   pre = 0x%x \n",pWMVDec->m_tFrmType,pWMVDec->m_pfrmCurrQ,pWMVDec->m_pfrmPrevious);

	if(!pWMVDec->m_fDecodedI && ((pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != BIVOP)))
		return -1;

	if(pWMVDec->nInputFrameIndx ==1 && (pWMVDec->m_bRenderDirect ==1 || (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != BIVOP) ))
	{
		return -1;
	}

	if(pWMVDec->m_bRenderDirect)
	{
		if(pWMVDec->CpuNumber > 1 )
		{
			volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmCurrQ->m_decodeprocess;	
			while(*ref_decodeprocess <  (I32_WMV)(pWMVDec->m_uintNumMBY+2))
			{	
				thread_sleep(0);
				bkprintf("RenderDirect");//printf("^%d",*ref_decodeprocess);
			}
		}
		return WMV_Succeeded;
	}
	else
	if (pWMVDec->m_tFrmType == SKIPFRAME) {
#ifdef USE_FRAME_THREAD
		//ZOU tongbu
		if(pWMVDec->CpuNumber > 1 )
		{
			volatile I32_WMV *ref_decodeprocess = NULL;
			ref_decodeprocess= &pWMVDec->m_pfrmCurrQ->m_decodeprocess;	
			voprintf("[thread%d] start SKIP dec*ref_decodeprocess = %d  0x%x\n",pWMVDec->nthreads,*ref_decodeprocess,pWMVDec->m_pfrmCurrQ);
			while(*ref_decodeprocess <  (I32_WMV)(pWMVDec->m_uintNumMBY+2))
			{	
				thread_sleep(0);
				bkprintf("skip");//printf("^%d",*ref_decodeprocess);
			}
		}
#endif
//20130110
		pWMVDec->m_pfrmCurrQ->m_decodeprocess = -5;

        //Repeatpad (pWMVDec);
        checkEndOfFrame(pWMVDec);
        pWMVDec->m_bRenderDirect = TRUE_WMV;
        pWMVDec->m_bRenderDirectWithPrerender = TRUE_WMV;
		if(pWMVDec->m_pfrmCurrQ == NULL)
			return WMV_BadMemory;
	    pWMVDec->m_pfrmCurrQ->m_timeStamp = pWMVDec->m_inTimeStamp;
	    pWMVDec->m_pfrmCurrQ->m_frameType = pWMVDec->m_tFrmType;
        pWMVDec->m_pfrmCurrQ->m_UserData = pWMVDec->m_inUserData;
		pWMVDec->m_pfrmCurrQ->m_iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
		pWMVDec->m_pfrmCurrQ->m_iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;
#ifdef USE_FRAME_THREAD
		pWMVDec->m_pfrmCurrQ->m_decodeprocess = MAX_MB_HEIGHT;
#endif
		tbprintf("[thread%d]  voVC1DecHandleSkip  Finished   0x%x   %d\n",pWMVDec->nthreads,pWMVDec->m_pfrmCurrQ,pWMVDec->nInputFrameIndx);

#ifdef OUTPUTFRMAE
		
#ifdef OUTPUTFRMAE
		if(1){
			FILE * fp = NULL;
			char filename[255];
			static int num =0;
			U8_WMV* ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
			U8_WMV* ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			U8_WMV* ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			sprintf(filename,"F:/new/SA00099_SKP_%d.yuv ",num++); 
			fp =fopen(filename,"ab");
			OutputOneFrame_Test( ppxliCurrQY, ppxliCurrQU,ppxliCurrQV ,
			pWMVDec->m_iWidthInternal, pWMVDec->m_iWidthInternalUV, pWMVDec-> m_iWidthSource,pWMVDec->m_iHeightSource,fp);
			fclose(fp);
		}
#endif
#endif

        return WMV_Succeeded;
    }
	else 
	if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
    {
        pWMVDec->error_flag_main = 0;
        pWMVDec->error_flag_thread = 0;
		pWMVDec->frame_errorflag = 0;

        tWMVStatus = voVC1DecHandleIVOP(pWMVDec);
#ifdef OUTPUTFRMAE
		if(1){
			FILE * fp = NULL;
			char filename[255];
			static int num =0;
			U8_WMV* ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
			U8_WMV* ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			U8_WMV* ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;

			if(pWMVDec->m_tFrmType == BIVOP)
			{
				sprintf(filename,"F:/new/SA00099_BI%d.yuv ",num++); 
				fp =fopen(filename,"ab");
			}
			else
			{
				sprintf(filename,"F:/new/SA00099_I%d.yuv ",num++); 
				fp =fopen(filename,"ab");
			}
			
			OutputOneFrame_Test( ppxliCurrQY, ppxliCurrQU,ppxliCurrQV ,
				pWMVDec->m_iWidthInternal, pWMVDec->m_iWidthInternalUV, pWMVDec-> m_iWidthSource,pWMVDec->m_iHeightSource,fp);
			fclose(fp);
		}
#endif

        if(tWMVStatus)
        {
            pWMVDec->error_flag_main = 1;
            pWMVDec->error_flag_thread = 1;
			return -1;
        }

    }
    else
    { //PVOP BVOP  
        if (!pWMVDec->m_bBFrameOn || pWMVDec->m_tFrmType == PVOP)
        { //PVOP
            pWMVDec->m_nPframes++;
            if(pWMVDec->error_flag_main || pWMVDec->error_flag_thread)
                return -1;

            tWMVStatus= voVC1DecHandlePVOP(pWMVDec); 

			if(tWMVStatus)
			{
				 pWMVDec->error_flag_main = 1;
				 pWMVDec->error_flag_thread = 1;
				 return -1;
			}

        }
        else if (pWMVDec->m_tFrmType == BVOP) 
        { //BVOP
            pWMVDec->m_nBframes++;
            if(pWMVDec->error_flag_main || pWMVDec->error_flag_thread)
                return -1;

			if(pWMVDec->m_nBframes == 13)
				pWMVDec->m_nBframes = pWMVDec->m_nBframes;

			//printf("pWMVDec->m_nBframes = %d ,\n",pWMVDec->m_nBframes);

           
			tWMVStatus=voVC1DecHandleBVOP(pWMVDec);   

        } // if BVOP
    } 
        
    if (pWMVDec->m_tFrmType == PVOP)
        pWMVDec->m_iPrevIFrame = 0;    

    if (WMV_Succeeded != tWMVStatus) 
        return tWMVStatus;     

    pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.

	//do the copy: from Cur to Post,and do the scaling
#ifdef ADJ_NEW
	//if(pWMVDec->CpuNumber >1 && !pWMVDec->m_bInterlacedSource)
	//if(pWMVDec->CpuNumber >1 && !pWMVDec->m_bInterlaceV2 && !pWMVDec->m_bYUV411)
	if(pWMVDec->CpuNumber >1 && !pWMVDec->m_bYUV411)
	{
		if (!pWMVDec->m_iDroppedPFrame && pWMVDec->m_bRenderDirect != TRUE_WMV)
		{
			if (pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag)) 
			{//  
				if(CopyCurrToPostAfter_new (pWMVDec) != WMV_Succeeded)
					return -1;
				AdjustDecRangeWMVA(pWMVDec);
			}

			 if (pWMVDec->m_iRangeState)
			{//AdjustDecRange
				if(CopyCurrToPostAfter_new (pWMVDec) != WMV_Succeeded)
					return -1;
				AdjustDecRange(pWMVDec);
			}
		}
	}
	else
	{
		if (!pWMVDec->m_iDroppedPFrame && pWMVDec->m_bRenderDirect != TRUE_WMV)
		{
			pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeAdvNeed = 0;
			pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeNeed = 0;

			if (pWMVDec->m_cvCodecVersion == WMVA && (pWMVDec->m_bRangeRedY_Flag || pWMVDec->m_bRangeRedUV_Flag)) 
			{//  
				if(CopyCurrToPost_new (pWMVDec) != WMV_Succeeded)
					return -1;
				AdjustDecRangeWMVA(pWMVDec);
				pWMVDec->m_pfrmPostQ->m_AdjustDecRangeAdvNeed = 1;
				pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeAdvNeed = 1;
			}

			 if (pWMVDec->m_iRangeState)
			{//AdjustDecRange
				if(CopyCurrToPost_new (pWMVDec) != WMV_Succeeded)
					return -1;
				AdjustDecRange(pWMVDec);
				pWMVDec->m_pfrmPostQ->m_AdjustDecRangeNeed = 1;
				pWMVDec->m_pfrmCurrQ->m_AdjustDecRangeNeed = 1;
			}
		}
	}
#endif
	//printf("Finished \n");
    checkEndOfFrame(pWMVDec);

#ifdef USE_FRAME_THREAD	
	if(pWMVDec->CpuNumber > 1 )
	{
		if(pWMVDec->nInputFrameIndx > 1)
		{
			volatile I32_WMV *ref_decodeprocess = &pWMVDec->m_pfrmPrevious->m_decodeprocess;
			while(*ref_decodeprocess < (I32_WMV)(pWMVDec->m_uintNumMBY+2)) //control the finish order
				thread_sleep(0);
		}

		if ( pWMVDec->m_tFrmType == PVOP && pWMVDec->m_iNumBFrames > 0 && (pWMVDec->m_scaling || pWMVDec->m_bLuminanceWarp))
				pWMVDec->m_pfrmRef0Process->m_decodeprocess = MAX_MB_HEIGHT;

		pWMVDec->m_pfrmCurrQ->m_decodeprocess = MAX_MB_HEIGHT;//+2: pad
		
	}
#endif
	return WMV_Succeeded;
}

tWMVDecodeStatus WMVideoDecDecodeDataInternal (HWMVDecoder hWMVDecoder)
{
	tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    
    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);  
	
	if((tWMVStatus = WMVVideoDecVOPHeader(pWMVDec))!=WMV_Succeeded)
		return tWMVStatus;  
	
	//printf("%d frame type=%d lum %d scale %d \n",pWMVDec->m_nframes,pWMVDec->m_tFrmType,pWMVDec->m_bLuminanceWarp, pWMVDec->m_scaling );
    
	//printf(" threads%d----frame type = %d \n",pWMVDec->nthreads,pWMVDec->m_tFrmType);
	if((tWMVStatus = WMVideoDecVOP(pWMVDec)) != WMV_Succeeded)
		return tWMVStatus;

    return WMV_Succeeded;
}


tWMVDecodeStatus WMVideoDecReset (HWMVDecoder hWMVDecoder)
{
    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;

    return WMVideoDecResetBState(hWMVDecoder);
}


tWMVDecodeStatus  WMVideoDecResetBState( HWMVDecoder hWMVDecoder) 
{ 
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    
    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
    
    pWMVDec->m_fDecodedI = 0;
    pWMVDec->m_uiNumFramesLeftForOutput = 0;
   //pWMVDec->m_eFrameWasOutput = FrameDecOut_None;
    
    if (pWMVDec->m_cvCodecVersion == WMV3)
    {
        pWMVDec->m_t = 0;
        pWMVDec->m_tFrmType = IVOP; pWMVDec->m_iBState = -3; pWMVDec->m_iPrevIFrame = 0;
        pWMVDec->m_bPrevAnchor = FALSE; pWMVDec->m_bBFrameOn = TRUE;
        pWMVDec->m_iDroppedPFrame = 0; pWMVDec->m_bIsBChangedToI = FALSE;

        pWMVDec->m_bOmittedBFrame = FALSE;
    }
    return tWMVStatus;
}

extern  const I32_WMV g_iBInverse[];

I32_WMV DecodeEntryPointHeader (tWMVDecInternalMember *pWMVDec, I32_WMV *piPIC_HORIZ_SIZE, I32_WMV *piPIC_VERT_SIZE)
{
    I32_WMV i;
    I32_WMV ihrd_fullness_N;
    U8_WMV pucHrdFullness[32];
	//Bool_WMV coded_size_flag =0;

    pWMVDec->m_bBrokenLink = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bClosedEntryPoint = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bPanScanPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bRefDistPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bLoopFilter = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bUVHpelBilinear  = BS_getBits(pWMVDec->m_pbitstrmIn, 1);    //FASTUVMC
    pWMVDec->m_bExtendedMvMode = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //EXTENDED_MV
    pWMVDec->m_iDQuantCodingOn = BS_getBits(pWMVDec->m_pbitstrmIn, 2);   //DQUANT
    pWMVDec->m_bXformSwitch = BS_getBits(pWMVDec->m_pbitstrmIn, 1);       //VSTRANSFORM
    pWMVDec->m_bSequenceOverlap = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //OVERLAP
    /*if(pWMVDec->m_bCodecIsWVC1){
		pWMVDec->vo_OverLapFilter = pWMVDec->m_bSequenceOverlap;
    }*/
    pWMVDec->m_bExplicitSeqQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //QUANTIZER

    if (pWMVDec->m_bExplicitSeqQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    else 
        pWMVDec->m_bExplicitFrameQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

    if (pWMVDec->m_bHRD_PARAM_FLAG) {
        for (i = 0; i < pWMVDec->m_ihrd_num_leaky_buckets; i++){
            ihrd_fullness_N = BS_getBits(pWMVDec->m_pbitstrmIn, 8) + 1;
            pucHrdFullness[i] = (U8_WMV)(ihrd_fullness_N - 1);  //HRD_FULLNESS
        }
    }


#ifdef STABILITY  //1254 6.2.13
   if(pWMVDec->m_bClosedEntryPoint==1 && pWMVDec->m_bBrokenLink!=0)
        return -11;
//    if(coded_size_flag==1 && (pWMVDec->m_bBrokenLink !=1 && pWMVDec->m_bClosedEntryPoint !=1))
//        return -11; //no Use
#endif
   pWMVDec->m_iCodedSizeFlag = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

    if (pWMVDec->m_iCodedSizeFlag) {
        // New coded size 
        pWMVDec->m_iFrmWidthSrc  = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
        pWMVDec->m_iFrmHeightSrc   = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;

#ifdef TRACEVC1
	if(1){
		FILE*fp =fopen(TRACEVC1FILE,"a");
		fprintf(fp," DecodeEntryPointHeader %d %d \n",pWMVDec->m_iFrmWidthSrc,pWMVDec->m_iFrmHeightSrc);
		fclose(fp);
	}
#endif

#ifdef STABILITY //zou 66
    if(pWMVDec->m_iMaxFrmWidthSrc < pWMVDec->m_iFrmWidthSrc ||  pWMVDec->m_iMaxFrmHeightSrc < pWMVDec->m_iFrmHeightSrc)
	{
		pWMVDec->m_iFrmWidthSrc = 0; 
		pWMVDec->m_iFrmHeightSrc = 0;
        return -11;
	}
#endif
#ifdef STABILITY
        if(pWMVDec->m_iFrmWidthSrc > 2500 || pWMVDec->m_iFrmHeightSrc>2500 )
        {
            pWMVDec->m_iFrmWidthSrc = 0;
            pWMVDec->m_iFrmHeightSrc = 0;
            return ICERR_ERROR ;
        }

         if(pWMVDec->m_iFrmWidthSrc <16 || pWMVDec->m_iFrmHeightSrc<16 )
         {
             pWMVDec->m_iFrmWidthSrc = 0;
             pWMVDec->m_iFrmHeightSrc = 0;
             return ICERR_ERROR ;
         }
#endif
        if (piPIC_HORIZ_SIZE != NULL && piPIC_VERT_SIZE != NULL) {
            *piPIC_HORIZ_SIZE  = pWMVDec->m_iFrmWidthSrc;
            *piPIC_VERT_SIZE   = pWMVDec->m_iFrmHeightSrc;
        }
    }
    else {
        // Use max. coded size 
        pWMVDec->m_iFrmWidthSrc  = pWMVDec->m_iMaxFrmWidthSrc;
        pWMVDec->m_iFrmHeightSrc   = pWMVDec->m_iMaxFrmHeightSrc;
#ifdef STABILITY
        if(pWMVDec->m_iFrmWidthSrc > 2500 || pWMVDec->m_iFrmHeightSrc>2500 )
        {
            pWMVDec->m_iFrmWidthSrc = 0;
            pWMVDec->m_iFrmHeightSrc = 0;
            return ICERR_ERROR ;
        }

         if(pWMVDec->m_iFrmWidthSrc <16 || pWMVDec->m_iFrmHeightSrc<16 )
         {
             pWMVDec->m_iFrmWidthSrc = 0;
             pWMVDec->m_iFrmHeightSrc = 0;
             return ICERR_ERROR ;
         }
#endif
    }

//#ifdef STABILITY //1254  zou 66
//    if(pWMVDec->m_iMaxFrmWidthSrc < pWMVDec->m_iFrmWidthSrc ||  pWMVDec->m_iMaxFrmHeightSrc < pWMVDec->m_iFrmHeightSrc)
//	{
//		pWMVDec->m_iFrmWidthSrc = 0;
//		pWMVDec->m_iFrmHeightSrc = 0;
//        return -11;
//	}
//#endif

    if (pWMVDec->m_bExtendedMvMode) //EXTENDED_DMV
        pWMVDec->m_bExtendedDeltaMvMode = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

    pWMVDec->m_bRangeRedY_Flag = BS_getBits(pWMVDec->m_pbitstrmIn, 1);//RANGE_MAPY_FLAG
    if (pWMVDec->m_bRangeRedY_Flag) {
        pWMVDec->m_iRangeRedY = BS_getBits(pWMVDec->m_pbitstrmIn, 3) + 1; //RANGE_MAPY
    } else {
        pWMVDec->m_iRangeRedY = 0;
    }
    pWMVDec->m_bRangeRedUV_Flag = BS_getBits(pWMVDec->m_pbitstrmIn, 1);//RANGE_MAPUV_FLAG
    if (pWMVDec->m_bRangeRedUV_Flag) {
        pWMVDec->m_iRangeRedUV = BS_getBits(pWMVDec->m_pbitstrmIn, 3) + 1;//RANGE_MAPUV
    } else {
        pWMVDec->m_iRangeRedUV = 0;
    }
    if (BS_invalid (pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }
#ifdef STABILITY
    if(pWMVDec->m_iNumBFrames > 7)
        return -1;
#endif
    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iNumBFrames];
    pWMVDec->m_bExplicitQuantizer = pWMVDec->m_bExplicitSeqQuantizer || pWMVDec->m_bExplicitFrameQuantizer;
    SetMVRangeFlag (pWMVDec, 0);
    return ICERR_OK;
}

I32_WMV SetSliceWMVA (tWMVDecInternalMember *pWMVDec, Bool_WMV bSliceWMVA, U32_WMV uiFirstMBRow, U32_WMV uiNumBytesSlice, Bool_WMV bSecondField)
{
    pWMVDec->m_bSliceWMVA = bSliceWMVA;
    if (pWMVDec->m_bSliceWMVA == FALSE)
    {
        memset (pWMVDec->m_pbStartOfSliceRowOrig, 0, sizeof(Bool_WMV)* pWMVDec->m_iSliceBufSize);
        pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
        pWMVDec->m_uiCurrSlice = 0;
        return 0;
    }
    
    if(((I32_WMV)uiFirstMBRow) >= pWMVDec->m_iSliceBufSize)
        return -1;

    pWMVDec->m_pbStartOfSliceRowOrig[uiFirstMBRow] = 1;
    return 0;
}

tWMVDecodeStatus WMVideoDecGetSrcResolution(HWMVDecoder hWMVDecoder, 
            I32_WMV * piEncodedDisplaySizeX, I32_WMV * piEncodedDisplaySizeY)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    I32_WMV iWidth, iHeight;


    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

	iWidth  = pWMVDec->m_iFrmWidthSrc;
	iHeight = pWMVDec->m_iFrmHeightSrc;

    if(piEncodedDisplaySizeX != NULL)
    {
        *piEncodedDisplaySizeX = iWidth;
    }

    if(piEncodedDisplaySizeY != NULL)
    {
        *piEncodedDisplaySizeY = iHeight;
    }

    return tWMVStatus;

}

// ===========================================================================
// private functions, inits
// ===========================================================================

tWMVDecodeStatus WMVInternalMemberInit ( tWMVDecInternalMember *pWMVDec,I32_WMV iWidthSource,I32_WMV iHeightSource ) 
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVStatus = VodecConstruct (pWMVDec, iWidthSource, iHeightSource );
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus;

    pWMVDec->m_uiNumFramesLeftForOutput = 0;
    // size info
    pWMVDec->m_iWidthSource = pWMVDec->m_iFrmWidthSrc;
    pWMVDec->m_iHeightSource = pWMVDec->m_iFrmHeightSrc;
    pWMVDec->m_iWidthInternal = pWMVDec->m_iWidthPrevY;
    pWMVDec->m_iWidthInternalUV = pWMVDec->m_iWidthPrevUV; 

    return tWMVStatus;
}


tWMVDecodeStatus WMVInternalMemberDestroy (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO WMVInternalMemberDestroy 1");
#endif

    tWMVStatus = VodecDestruct(pWMVDec);
    return tWMVStatus;
}
/* After flush*/
void  reset_buf_seq(tWMVDecInternalMember *pWMVDec)
{
	FIFOTYPE *priv = &pWMVDec->m_memManageFIFO;  

	priv->r_idx = 0;
	priv->w_idx = pWMVDec->m_frameBufNum;
}

VO_S32 VO_API voGetVC1DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pWMV9Dec = pDecHandle;

	if(!pWMV9Dec)
		return VO_ERR_INVALID_ARG;

	pWMV9Dec->Init          = voVC1DecInit;
	pWMV9Dec->Uninit        = voVC1DecUninit;
	pWMV9Dec->SetParam      = voVC1DecSetParameter;
	pWMV9Dec->GetParam      = voVC1DecGetParameter;
	pWMV9Dec->SetInputData  = voVC1DecSetInputData;
	pWMV9Dec->GetOutputData = voVC1DecGetOutputData;

	return VO_ERR_NONE;
}