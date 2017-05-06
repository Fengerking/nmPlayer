//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//#define VERBOSE    // VERBOSE doesn't work with multiproc!!
/***********************************************************************************/

#include "voWmvPort.h"
#include <math.h>
#if !defined( __arm) && !defined(_APOGEE_) && !defined(_TI_C55X_)
#   include <memory.h>
#endif
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_api.h"
#include "wmvdec_function.h"
#include "spatialpredictor_wmv.h"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_

/****************************************************************************************
  Array declarations
****************************************************************************************/

#ifndef WMV9_SIMPLE_ONLY
//  Bidirectional prediction weights
U16_WMV aNullPredWts_WMV[] = {
    640, 640, 669, 480, 708, 354, 748, 257, 792, 198, 760, 143, 808, 101, 772,  72,
    480, 669, 537, 537, 598, 416, 661, 316, 719, 250, 707, 185, 768, 134, 745,  97,
    354, 708, 416, 598, 488, 488, 564, 388, 634, 317, 642, 241, 716, 179, 706, 132,
    257, 748, 316, 661, 388, 564, 469, 469, 543, 395, 571, 311, 655, 238, 660, 180,
    198, 792, 250, 719, 317, 634, 395, 543, 469, 469, 507, 380, 597, 299, 616, 231,
    161, 855, 206, 788, 266, 710, 340, 623, 411, 548, 455, 455, 548, 366, 576, 288,
    122, 972, 159, 914, 211, 842, 276, 758, 341, 682, 389, 584, 483, 483, 520, 390,
    110, 1172, 144, 1107, 193, 1028, 254, 932, 317, 846, 366, 731, 458, 611, 499, 499
};

/****************************************************************************************
  Constructor for CSpatialPredictor
****************************************************************************************/
t_CSpatialPredictor *t_SpatialPredictorConstruct (tWMVDecInternalMember * pWMVDec)
{
#ifdef XDM
	t_CSpatialPredictor  *pSp = (t_CSpatialPredictor *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof( t_CSpatialPredictor);
#else
    t_CSpatialPredictor  *pSp = (t_CSpatialPredictor *) wmvMalloc (pWMVDec, sizeof(t_CSpatialPredictor), DHEAP_STRUCT);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_SpatialPredictorConstruct);
    if (!pSp)
        goto ERROR_EXIT;
#endif
    memset( pSp, 0, sizeof(t_CSpatialPredictor) );  // protect from low memory when one of the following mallocs fails.

#ifdef XDM
	pSp->m_pNeighbors               = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 16 * 4;
	pSp->m_pBuffer16                = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 24 * 2;
#else
    pSp->m_pNeighbors = (U8_WMV *) (wmvMalloc (pWMVDec, 16 * 4, DHEAP_LOCALHUFF)); // align to 4 byte (U32_WMV) and cast
    if (!pSp->m_pNeighbors)
        goto ERROR_EXIT;

    pSp->m_pBuffer16 = (I16_WMV *) wmvMalloc (pWMVDec, 24 * 2, DHEAP_LOCALHUFF);
    if (!pSp->m_pBuffer16)
        goto ERROR_EXIT;
#endif

    // tune accesses
    pSp->m_pLeft = pSp->m_pNeighbors + 31;
    pSp->m_pTop = pSp->m_pNeighbors + 32;

    // Set up sum arrays
#ifdef XDM
	pSp->m_pSums = (U16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += 32 * 4;
	pSp->m_pTopSum = pSp->m_pSums + 4;
#else
    pSp->m_pTopSum = (pSp->m_pSums = (U16_WMV *) wmvMalloc (pWMVDec, 32 * 4, DHEAP_LOCALHUFF)) + 4;  // extra space for MMX setupZero
    if (!pSp->m_pSums)
        goto ERROR_EXIT;
#endif
    pSp->m_pLeftSum = pSp->m_pTopSum + 12;

//    DEBUG_HEAP_ADD(3, sizeof(t_CSpatialPredictor) + (16 * 4) + (24 * 2) + (32 * 4));

    return pSp;

ERROR_EXIT:
    t_SpatialPredictorDestruct (pWMVDec, pSp);
    return NULL_WMV;
}

/****************************************************************************************
  Destructor for CSpatialPredictor
****************************************************************************************/
Void_WMV t_SpatialPredictorDestruct (tWMVDecInternalMember *pWMVDec, t_CSpatialPredictor *pSp)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_SpatialPredictorDestruct);
    if ( pSp != NULL ) {
		if ( pSp->m_pSums != NULL ){
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pSums :%x",(U32_WMV)pSp->m_pSums );
#endif
            wmvFree (pWMVDec, pSp->m_pSums);
		}

		if ( pSp->m_pBuffer16 != NULL ){
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pBuffer16 :%x",(U32_WMV)pSp->m_pBuffer16 );
#endif
            wmvFree (pWMVDec, pSp->m_pBuffer16);
		}

		if ( pSp->m_pNeighbors != NULL ){
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_pNeighbors :%x",(U32_WMV)pSp->m_pNeighbors );
#endif
            wmvFree (pWMVDec, pSp->m_pNeighbors);
		}

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO pSp :%x",(U32_WMV)pSp );
#endif
        wmvFree (pWMVDec, pSp);
    }
}


/****************************************************************************************
  Spatial prediction function for decoder
****************************************************************************************/
Bool_WMV bMin_Max_LE_2QP (I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV g,I32_WMV h, I32_WMV i2Qp)
{
    I32_WMV min, max;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(bMin_Max_LE_2QP);
    if (a >= h){ 
        min = h; max = a;
    }
    else{
        min = a; max = h;
    }
    if (min > c) min  = c;
    else if (c > max) max = c;
    if (min > e) min  = e;
    else if (e > max) max = e;

////////////// Run Experiement to see if it benefit.
    if (max - min >= i2Qp) return FALSE_WMV;

    if (min > b) min  = b;
    else if (b > max) max = b;
    if (min > d) min  = d;
    else if (d > max) max = d;
    if (min > f) min  = f;
    else if (f > max) max = f;
    if (min > g) min  = g;
    else if (g > max) max = g;

    return (Bool_WMV) (max - min < i2Qp);
}


Bool_WMV bMin_Max_LE_2QP_Short (I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV i2Qp)
{
    I32_WMV min, max;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(bMin_Max_LE_2QP_Short);
    if (a >= f){ 
        min = f; max = a;
    }
    else{
        min = a; max = f;
    }
    if (min > c) min  = c;
    else if (c > max) max = c;
    if (min > e) min  = e;
    else if (e > max) max = e;

////////////// Run Experiement to see if it benefit.
    //if (max - min >= i2Qp) return FALSE_WMV;

    if (min > b) min  = b;
    else if (b > max) max = b;
    if (min > d) min  = d;
    else if (d > max) max = d;

    return (Bool_WMV) (max - min < i2Qp);
}


#if !defined(WMV_OPT_X8_ARM )&& !defined(_MIPS_ASM_X8_OPT_) && (((defined(_SHX_ASM_X8_OPT_)) && (_SHX_ASM_X8_OPT_==0)) || (!defined(_SHX_ASM_X8_OPT_))) 

#define phi1(a) ((U32_WMV)(a+iThr1) <= uThr2)

static Void_WMV FilterHorzEdgeX8 (U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize)
{
    const I32_WMV iThr1 = ((iStepSize + 10) >> 3);
    const U32_WMV uThr2 = iThr1<<1;
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterHorzEdgeX8);
    
    for (i = 0; i < 8; ++i) {
        I32_WMV  v1, v2, v3, v4, v5, v6, v7, v8;
        U8_WMV* pVtmpR = ppxlcCenter;
        U8_WMV* pVtmpL = ppxlcCenter - iPixelDistance;
        I32_WMV eq_cnt;
        Bool_WMV bStrongFilter;

        v4 = *pVtmpL;
        v5 = *pVtmpR;
        pVtmpL -= iPixelDistance;
        pVtmpR += iPixelDistance;
        v3 = *pVtmpL;
        v6 = *pVtmpR;
        pVtmpL -= iPixelDistance;
        pVtmpR += iPixelDistance;
        v2 = *pVtmpL;
        v7 = *pVtmpR;
        pVtmpL -= iPixelDistance;
        pVtmpR += iPixelDistance;
        v1 = *pVtmpL;
        v8 = *pVtmpR;

        eq_cnt = phi1(v1 - v2) + phi1(v2 - v3) + phi1(v3 - v4) + phi1(v4 - v5);
        if (eq_cnt != 0)
            eq_cnt += phi1(pVtmpL[-iPixelDistance] - v1) + phi1(v5 - v6) + phi1(v6 - v7) + phi1(v7 - v8) + phi1(v8 - pVtmpR[iPixelDistance]);

        if (eq_cnt >= 6) {
            bStrongFilter = bMin_Max_LE_2QP (v1, v2, v3, v4, v5, v6, v7, v8, iStepSize*2);        
        } else {
            bStrongFilter = FALSE;
        }
        
        if (bStrongFilter) {
            I32_WMV  v2plus7 = v2 + v7;            
            *(ppxlcCenter - 2*iPixelDistance) = (3 * (v2 + v3) + v2plus7 + 4) >> 3;                                                        
            *(ppxlcCenter + iPixelDistance) = (3 * (v7 + v6) + v2plus7 + 4) >> 3;
            v2plus7 <<= 1;
            *(ppxlcCenter - iPixelDistance) = (v2 + 3 * v4 + v2plus7 + 4) >> 3;                                                        
            *(ppxlcCenter) = (v7 + 3 * v5 + v2plus7 + 4) >> 3;                                                        
        }
        else {

            I32_WMV v4_v5 = v4 - v5;
            I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
            I32_WMV absA30 = abs(a30);
            I32_WMV iMina31_a32;
            if (absA30 < iStepSize) {
                I32_WMV a31;                                 
                I32_WMV a32;                
                v2 -= v3;
                v6 -= v7;
                a31 = (2 * (v1-v4) - 5 * v2 + 4) >> 3;                                 
                a32 = (2 * (v5-v8) - 5 * v6 + 4) >> 3;
                
                iMina31_a32 = min(abs(a31),abs(a32)); 
                absA30 -= iMina31_a32;
                
                if ((absA30 > 0) && ((v4_v5 ^ a30) < 0)){

                    I32_WMV iSign = v4_v5 >> 31;
                    v4_v5 = abs(v4_v5) >> 1;

                    absA30 *= 5;
                    absA30 >>= 3;

                    if (absA30 > v4_v5)
                        absA30 = v4_v5;

                    absA30 ^= iSign;
                    absA30 -= iSign;

                    *(ppxlcCenter - iPixelDistance) = v4 - absA30;                                                                                         
                    *(ppxlcCenter) = v5 + absA30;
                }
            }        
        }        
        ppxlcCenter++;
    }
}

static Void_WMV FilterVertEdgeX8 (U8_WMV* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize)
{
    const I32_WMV iThr1 = ((iStepSize + 10) >> 3);// + 1;
    const U32_WMV uThr2 = iThr1<<1;
    U8_WMV* pVh = ppxlcCenter - 5;
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterVertEdgeX8);
                
    for (i = 0; i < 8; ++i) {
        
        //I32_WMV v0 = pVh[0];                                                                    
        I32_WMV v1 = pVh[1];                                                                    
        I32_WMV v2 = pVh[2];                                                                    
        I32_WMV v3 = pVh[3];                                                                    
        I32_WMV v4 = pVh[4];                                                                    
        I32_WMV v5 = pVh[5];                                                                    
        I32_WMV v6 = pVh[6];                                                                    
        I32_WMV v7 = pVh[7];                                                                    
        I32_WMV v8 = pVh[8];                                                                    
        Bool_WMV bStrongFilter;
        //I32_WMV v9 = pVh[9];                                                                    
                
        I32_WMV eq_cnt = phi1(v1 - v2) + phi1(v2 - v3) + phi1(v3 - v4) + phi1(v4 - v5);        
        if (eq_cnt != 0)
            eq_cnt += phi1(pVh[0] - v1) + phi1(v5 - v6) + phi1(v6 - v7) + phi1(v7 - v8) + phi1(v8 - pVh[9]);

        if (eq_cnt >= 6) {
            bStrongFilter = bMin_Max_LE_2QP (v1, v2, v3, v4, v5, v6, v7, v8, iStepSize*2); 
            
        } else {
            bStrongFilter = FALSE;
        }
        
        if (bStrongFilter) {            
            I32_WMV  v2plus7 = v2 + v7;
            pVh[3] = (3 * (v2 + v3) + v2plus7 + 4) >> 3;                                                        
            pVh[6] = (3 * (v7 + v6) + v2plus7 + 4) >> 3;
            v2plus7 <<= 1;
            pVh[4] = (v2 + 3 * v4 + v2plus7 + 4) >> 3;                                                        
            pVh[5] = (v7 + 3 * v5 + v2plus7 + 4) >> 3;       
        }
        else {
            I32_WMV v4_v5 = v4 - v5;
            I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
            I32_WMV absA30 = abs(a30);
            if (absA30 < iStepSize) {
                I32_WMV a31, a32, iMina31_a32; 
                
                v2 -= v3;
                v6 -= v7;
                a31 = (2 * (v1-v4) - 5 * v2 + 4) >> 3;                                 
                a32 = (2 * (v5-v8) - 5 * v6 + 4) >> 3;                                 

                iMina31_a32 = min(abs(a31),abs(a32)); 
                absA30 -= iMina31_a32;
                
                if ((absA30 > 0) && ((v4_v5 ^ a30) < 0)){
                    I32_WMV iSign = v4_v5 >> 31;
                    v4_v5 = abs(v4_v5) >> 1;

                    absA30 *= 5;
                    absA30 >>= 3;

                    if (absA30 > v4_v5)
                        absA30 = v4_v5;

                    absA30 ^= iSign;
                    absA30 -= iSign;

                    pVh[4] = v4 - absA30;                                                                                         
                    pVh[5] = v5 + absA30;
                }
            }   
        }
        
        pVh += iPixelIncrement;
    }
}

#else   // WMV_OPT_X8_ARM

// defined in ASM
extern  Void_WMV FilterHorzEdgeX8 (U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize);
extern  Void_WMV FilterVertEdgeX8 (U8_WMV* ppxlcCenter, I32_WMV iPixelIncrement, I32_WMV iStepSize);

#   if defined(DEBUG) 
        // a easy breakpoint mechanism from within asm
        extern  void WMVAsmBreak( int r0, int r1 ) {
            volatile int i = r0 != r1;
        }
#   endif

#endif  // WMV_OPT_X8_ARM
/***************************************************************************
  Deblocking function
***************************************************************************/
Void_WMV X8Deblock(U8_WMV *pCenter, I32_WMV iStride, I32_WMV iStepSize,
                   I32_WMV blockX, I32_WMV blockY, Bool_WMV bResidual, I32_WMV iOrient)
{
	Bool_WMV  bFlag;
    FUNCTION_PROFILE_DECL_START(fp,DECODEIX8LOOPFILTER_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(X8Deblock);
 
	bFlag = (blockY > 0) && (bResidual || (iOrient != 0 && iOrient != 4));
	if (bFlag)
        FilterHorzEdgeX8(pCenter, iStride, iStepSize);
    
	bFlag = (blockX > 0) && (bResidual || (iOrient != 0 && iOrient != 8));
    if (bFlag)
        FilterVertEdgeX8(pCenter, iStride, iStepSize);

    FUNCTION_PROFILE_STOP(&fp);
}

/***************************************************************************
  Context class global functions
***************************************************************************/
static U8_WMV fnMin(U8_WMV v1, U8_WMV v2, U8_WMV v3)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(fnMin);
    return (v1<v2)?((v1<v3)?v1:v3):((v2<v3)?v2:v3);
}

/***************************************************************************
  Context class constructor and destructor
***************************************************************************/
t_CContextWMV *t_ContextWMVConstruct (tWMVDecInternalMember *pWMVDec, I32_WMV iCol, I32_WMV iRow)
{
#ifdef XDM
	t_CContextWMV *pContext = (t_CContextWMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof(t_CContextWMV);
#else
    t_CContextWMV *pContext = (t_CContextWMV *) wmvMalloc (pWMVDec, sizeof(t_CContextWMV), DHEAP_STRUCT);
    if ( pContext == NULL_WMV )
        return NULL_WMV;
#endif
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_ContextWMVConstruct);

    memset( pContext, 0, sizeof(t_CContextWMV) );  // protect from low memory 
    pContext->m_iRow = iRow;
    pContext->m_iCol = iCol;

#ifdef XDM
	pContext->m_pData = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += iRow * iCol * sizeof(U8_WMV);
#else
    pContext->m_pData = (U8_WMV *) wmvMalloc (pWMVDec, iRow * iCol * sizeof(U8_WMV), DHEAP_LOCALHUFF);
    if (pContext->m_pData == NULL_WMV) {
        wmvFree(pWMVDec, pContext);
        return NULL_WMV;
    }
#endif

    pContext->m_iColGt2 = (pContext->m_iCol > 2) ? 2 : 1;
    return pContext;
}

Void_WMV t_ContextWMVDestruct (tWMVDecInternalMember *pWMVDec, t_CContextWMV *pContext)
{
    if ( pContext != NULL ) {
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_ContextWMVDestruct);
        if ( pContext->m_pData != NULL )
            wmvFree (pWMVDec, pContext->m_pData);
        wmvFree (pWMVDec,  pContext);
    }
}

/***************************************************************************
  Context class joint functions (only decoder side is safe)
***************************************************************************/
Void_WMV t_ContextGetDec (t_CContextWMV *pContext, I32_WMV iX, I32_WMV iY,
                          I32_WMV iStepSize, I32_WMV *iPredOrient, I32_WMV *iEstRun)
{
    U8_WMV  *pData1 = pContext->m_pData + ((iY - 1) & 1) * pContext->m_iCol;
    U8_WMV  *pData0 = pContext->m_pData + (iY & 1) * pContext->m_iCol;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_ContextGetDec);

// predict orientation
    if ((iX | iY) == 0) {
        *iPredOrient = 0;
    }
    else if (iX == 0) {  // left edge (vertical)
// check if top far right orientation is checked in (at least 1 MB wide)
        *iPredOrient = 1;
        // following check not required at decoder
        // iPredOrient = ((pData1[m_iColGt2] & 0x3) == 0x3) ? 0xff : 1;
    }
    else if (iY == 0) {  // top edge  (horizontal)
// no check needed
        *iPredOrient = 2;
    }
    else {
// check top far right
        I32_WMV  iXfr = iX+2;
        I32_WMV iT, iL;
        if (iXfr >= pContext->m_iCol)  iXfr = pContext->m_iCol - 1;
        // following check not required at decoder
        //if ((pData1[iXfr] & 0x3) == 0x3) {
        //    iPredOrient = 0xff;
        //    return;
        //}
// all the rules
        
        iT = pData1[iX] & 0x3;
        iL = pData0[iX - 1] & 0x3;

        if (iT==iL)
            *iPredOrient =  iT;
        else if (iL==2 && iT==0)
            *iPredOrient =  iL;
        else if (iL==0 && iT==1)
            *iPredOrient =  iT;
        else if (iL==2 && iT==1)
            *iPredOrient =  iL;
        else if (iL==1 && iT==2) {
            I32_WMV iTL = pData1[iX - 1] & 0x3;
            if (iTL == iL)            *iPredOrient =  iT;
            else {
                if (iStepSize > 12)   *iPredOrient =  iT;
                else {
                    if (iTL == iT)    *iPredOrient =  iL;
                    else              *iPredOrient =  iTL;
                }
            }
        }
        else
            *iPredOrient =  0;
    }

    {
        U8_WMV v1, v2, v3;
        if (iX == 0) {
            v2 = v1 = (iY == 0) ?
                /*m_uDefault*/ 16 : (pData1[0] >> 2);
        }
        else {
            v1 = (pData0[iX - 1] >> 2);
            v2 = (iY == 0)?
                v1 : (pData1[iX] >> 2);
        }
        v3 = ((iY & iX) == 0) ? v2 : (pData1[iX - 1] >> 2);

        *iEstRun = fnMin (v1, v2, v3);
    }
    return;
}

/***************************************************************************
  Context class joint put (only decoder side is safe)
***************************************************************************/
static I8_WMV orientRemap[]={0,0,0,0,1,0,0,0,2,0,0,0};

Void_WMV t_ContextPutDec (t_CContextWMV *pContext, I32_WMV iX, I32_WMV iY,
                          I32_WMV iOrientContext, I32_WMV iRunContext)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_ContextPutDec);

    iOrientContext = orientRemap[iOrientContext] & 0x3;
    pContext->m_pData[(iY & 1) * pContext->m_iCol + iX]
        = iOrientContext | (iRunContext << 2);
}

/***************************************************************************
  Orientation predictor (joint, for Chroma)
***************************************************************************/
I32_WMV  t_ContextGetChromaDec (t_CContextWMV *pContext, I32_WMV iX, I32_WMV iY)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(t_ContextGetChromaDec);

    if ((iX > 1) && (iY > 1))
        return pContext->m_pData[iX - 1] & 0x3;
    else if (iY > 1)
        return 1;   // vertical predictor
    else
        return 2;   // horizontal predictor (also for origin block)
}

/***************************************************************************
  END
***************************************************************************/
#endif // WMV9_SIMPLE_ONLY
