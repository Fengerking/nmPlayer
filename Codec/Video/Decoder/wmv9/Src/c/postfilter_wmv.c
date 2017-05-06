//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1998
//
//  File:       postfilter.cpp
//
//--------------------------------------------------------------------------

#include "xplatform_wmv.h"
#include "limits.h"
#include "voWmvPort.h"
#include "wmvdec_member.h"
#include "typedef.h"
#include "postfilter_wmv.h"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif
#include "opcodes.h"
#include "tables_wmv.h"


//THR1 = the threshold before we consider neighboring pixels to be "diffrent"
#define THR1 2       
//THR2 = the total number of "diffrent" pixels under which we use stronger filter
#define THR2 6
#define INV_THR2 3

#ifndef _IOS
#ifndef _MAC_OS
Void_WMV (*g_pDeblockMB)(DEBLOCKMB_ARGS);
Void_WMV (*g_pApplySmoothing)(APPLYSMOOTHING_ARGS);
Void_WMV (*g_pDetermineThreshold)(DETERMTHR_ARGS);
Void_WMV (*g_pDeringMB) (DERINGMB_ARGS);
#endif
#endif

#ifdef _Embedded_x86
#include "postfilter_emb.h"
#endif

#ifdef macintosh
#include "postfilter_altivec.h"
extern Bool_WMV g_bSupportAltiVec_WMV;
#endif

Void_WMV g_InitPostFilter (Bool_WMV bFastDeblock, Bool_WMV bYUV411) 
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InitPostFilter);
    if (bYUV411 == FALSE_WMV) //progressive mode
    {
#       if !(defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
            // take out C-code for xbox
            if (bFastDeblock)
                g_pDeblockMB = DeblockMB_Short;
            else
                g_pDeblockMB = DeblockMB;
        
            g_pApplySmoothing = ApplySmoothing;
            g_pDetermineThreshold = DetermineThreshold;
            g_pDeringMB = DeringMB;
#       endif
        
#       if (defined(_WMV_TARGET_X86_) || defined(_Embedded_x86)) && defined (_SUPPORT_POST_FILTERS_)
            if (g_SupportMMX ()) {        
#               if !(defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
                    if (bFastDeblock)
                        g_pDeblockMB = DeblockMB_Short_MMX;
                    else
                        g_pDeblockMB = DeblockMB_MMX;
                    g_pDetermineThreshold = DetermineThreshold_MMX;
#					endif
                g_pApplySmoothing = ApplySmoothing_MMX;
            }
        
#           if defined(_WMV_TARGET_X86_) && defined (_SUPPORT_POST_FILTERS_)
                if (g_SupportSSE1()){
                    g_pDetermineThreshold = DetermineThreshold_KNI;
                    if (bFastDeblock)
                        g_pDeblockMB = DeblockMB_Short_KNI;
                    else
                        g_pDeblockMB = DeblockMB_KNI;
                    }
#           endif
#       endif
        
#       if defined(macintosh) && defined(_MAC_VEC_OPT)
            if (g_bSupportAltiVec_WMV)
            {
                if (bFastDeblock)
                    g_pDeblockMB = DeblockMB_Short_AltiVec;
                else
                    g_pDeblockMB = DeblockMB_AltiVec;

                g_pApplySmoothing = ApplySmoothing_AltiVec;
                g_pDetermineThreshold = DetermineThreshold_AltiVec;
            }
#       endif //macintosh
    }
#       if !defined(WMV9_SIMPLE_ONLY)
            else  //Interlace mode
            {
                if (bFastDeblock)
                    g_pDeblockMB = DeblockMBInterlace411_Short;
                else
                    g_pDeblockMB = DeblockMBInterlace411;
#				if (defined(_WMV_TARGET_X86_) || defined(_Embedded_x86)) && defined(_SUPPORT_POST_FILTERS_)
                if (g_bSupportMMX_WMV) {
                    if (bFastDeblock)
                        g_pDeblockMB = DeblockMBInterlace411_Short_MMX;
                    else
                        g_pDeblockMB = DeblockMBInterlace411_MMX;
                    if(g_SupportSSE1())
                        g_pDeblockMB = DeblockMBInterlace411_KNI;

                }
#				endif
            }
#       endif // !(defined(WMV9_SIMPLE_ONLY)

}

FORCEINLINE I32_WMV MIN (I32_WMV a,I32_WMV b,I32_WMV c) 
{
    if (a < b) {
        if (a < c) return a;
        return c;
    } else {
        if (b < c) return b;
        else return c;
    }
}

FORCEINLINE I32_WMV CLIP (I32_WMV a,I32_WMV b,I32_WMV c) 
{
    if (b < c) {
        if (a < b) a = b;
        if (a > c) a = c;
        return a;
    }
    if (a > b) a = b;
    if (a < c) a = c;
    return a;
}

I32_WMV MAX(int a,int b,int c,int d,int e,int f,int g,int h) 
{
    I32_WMV max = a;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MAX);
    if (max < b) max  = b;
    if (max < c) max  = c;
    if (max < d) max  = d;
    if (max < e) max  = e;
    if (max < f) max  = f;
    if (max < g) max  = g;
    if (max < h) max  = h;
    return max;
}

/*
I32_WMV MIN(int a,int b,int c,int d,int e,int f,int g,int h) 
{
    I32_WMV min = a;
    if (min > b) min  = b;
    if (min > c) min  = c;
    if (min > d) min  = d;
    if (min > e) min  = e;
    if (min > f) min  = f;
    if (min > g) min  = g;
    if (min > h) min  = h;
    return min;
}
*/
// this fn defined in spatialpredictor.cpp
Bool_WMV bMin_Max_LE_2QP(I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV g,I32_WMV h, I32_WMV i2Qp);
Bool_WMV bMin_Max_LE_2QP_Short (I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV i2Qp);

/*
#define P(m) (((m < 1) && (abs(v1-v0) < iStepSize)) ? v0 :   \
             (m <  1)                              ? v1 :    \
             (m == 1)                              ? v1 :    \
             (m == 2)                              ? v2 :    \
             (m == 3)                              ? v3 :    \
             (m == 4)                              ? v4 :    \
             (m == 5)                              ? v5 :    \
             (m == 6)                              ? v6 :    \
             (m == 7)                              ? v7 :    \
             (m == 8)                              ? v8 :    \
             (abs(v8-v9) < iStepSize)              ? v9 : v8)

#define filt(n) g_rgiClapTabDec[((P(n-4) + P(n-3) + 2*P(n-2) + 2*P(n-1) + 4*P(n) + 2*P(n+1) + 2*P(n+2) + P(n+3) + P(n+4) + 8)>>4)]
*/
#define P_filt1 rgiClapTab[((6*v0 + 4*v1 + 2*v2 + 2*v3 + v4 + v5 + 8)>>4)]
#define P_filt2 rgiClapTab[((4*v0 + 2*v1 + 4*v2 + 2*v3 + 2*v4 + v5 + v6 + 8)>>4)]
#define P_filt3 rgiClapTab[((2*v0 + 2*v1 + 2*v2 + 4*v3 + 2*v4 + 2*v5 + v6 + v7 + 8)>>4)]
#define P_filt4 rgiClapTab[((v0 + v1 + 2*v2 + 2*v3 + 4*v4 + 2*v5 + 2*v6 + v7 + v8 + 8)>>4)]
#define P_filt5 rgiClapTab[((v1 + v2 + 2*v3 + 2*v4 + 4*v5 + 2*v6 + 2*v7 + v8 + v9 + 8)>>4)]
#define P_filt6 rgiClapTab[((v2 + v3 + 2*v4 + 2*v5 + 4*v6 + 2*v7 + 2*v8 + 2*v9 + 8)>>4)]
#define P_filt7 rgiClapTab[((v3 + v4 + 2*v5 + 2*v6 + 4*v7 + 2*v8 + 4*v9 + 8)>>4)]
#define P_filt8 rgiClapTab[((v4 + v5 + 2*v6 + 2*v7 + 4*v8 + 6*v9 + 8)>>4)]


#define PFE_filt1_delta_Index		(6*v0+4*v1+2*v2+2*v3+v4+v5+8)
#define PFE_filt2_delta_Index		(2*v2 + v4 + v6 - 2*v0 - 2*v1)
#define PFE_filt3_delta_Index		(2*v3 + v5 + v7 - 2*v0 - 2*v2)
#define PFE_filt4_delta_Index		(2*v4 + v6 + v8 - v0 - v1 - 2*v3)
#define PFE_filt5_delta_Index		(2*v5 + v7 + v9 - v0 - v2 - 2*v4)
#define PFE_filt6_delta_Index		(2*v6 + v8 + v9 - v1 - v3 - 2*v5)
#define PFE_filt7_delta_Index		(2*v7 + 2*v9 - v2 - v4 - 2*v6)
#define PFE_filt8_delta_Index		(2*v8 + 2*v9 - v3 - v5 - 2*v7)



//7 tab LPF
//filt(n) g_rgiClapTabDec[((P(n-3) + P(n-2) + P(n-1) + 2*P(n) + P(n+1) + P(n+2) + P(n+3) + 4)>>3)]
#define P_filt2_Short rgiClapTab[((3*v1 + 2*v2 + v3 + v4 + v5 + 4)>>3)]
#define P_filt3_Short rgiClapTab[((2*v1 + v2 + 2*v3 + v4 + v5 + v6 + 4)>>3)]
#define P_filt4_Short rgiClapTab[((v1 + v2 + v3 + 2*v4 + v5 + v6 + v7 + 4)>>3)]
#define P_filt5_Short rgiClapTab[((v2 + v3 + v4 + 2*v5 + v6 + v7 + v8 + 4)>>3)]
#define P_filt6_Short rgiClapTab[((v3 + v4 + v5 + 2*v6 + v7 + 2*v8 + 4)>>3)]
#define P_filt7_Short rgiClapTab[((v4 + v5 + v6 + 2*v7 + 3*v8 + 4)>>3)]

#define P_filt2_delta_Index		(3*v1+2*v2+v3+v4+v5+4)
#define P_filt3_delta_Index		(v3+v6 - v1-v2)
#define P_filt4_delta_Index		(v4+v7 - v1-v3)
#define P_filt5_delta_Index		(v5+v8 - v1-v4)
#define P_filt6_delta_Index		(v6+v8 - v2-v5)
#define P_filt7_delta_Index		(v7+v8 - v3-v6)


#define phi(a) ((abs(a) <= THR1) ? 1 : 0)     
#define SIGN(a) ((a < 0) ? -1 : 1)
#define ABS(x)      (((x) >= 0) ? (x) : -(x))                           /** Absolute value of x */

#define inverse_phi(a) ((abs(a) > THR1) ? 1 : 0)     

#ifndef WMV9_SIMPLE_ONLY
#ifdef WMV_OPT_PPL_ARM


extern Void_WMV FilterEdge (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
);


/* 
 * Notes: Optimized C code from Ref code for ARM CPU without MMX
 * With MMX, this code won't necessary to accelerate, maybe even slow down because of mixing index calcuating.
 * Without MMX, it shows overall perf gain around 9.8% over v9 main profile.
 */

#if 0

FORCEINLINE Void_WMV FilterEdge (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
) {

    U8_WMV* pV5 = ppxlcCenter;
    I32_WMV i;
    I32_WMV eq_cnt;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

	I32_WMV v0;                                                                    
	I32_WMV v1;                                                                    
	I32_WMV v2;                                                                    
	I32_WMV v3;                                                                    
	I32_WMV v4;                                                                    
	I32_WMV v5;                                                                    
	I32_WMV v6;                                                                    
	I32_WMV v7;                                                                    
	I32_WMV v8;                                                                    
	I32_WMV v9;                                                                    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterEdge);


    for (i = 0; i < iEdgeLength; ++i) {
       
       
		v1 = *(pV5-4*iPixelDistance);                                                                    
		v2 = *(pV5-3*iPixelDistance);                                                                    
		v3 = *(pV5-2*iPixelDistance);                                                                    
		v4 = *(pV5-iPixelDistance);                                                                    
		v5 = *pV5;                                                                    
		v6 = *(pV5+iPixelDistance);                                                                    
		v7 = *(pV5+2*iPixelDistance);                                                                    
		v8 = *(pV5+3*iPixelDistance);                                                                    

        // check every four lines
		if ((i & 3) == 0)
		{
			eq_cnt = phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
					phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8);

			if (eq_cnt < 3) // 9 - THR2
				goto easy_deblock;

			v0 = *(pV5-5*iPixelDistance);                                                                    
			v9 = *(pV5+4*iPixelDistance);                                                                    
			eq_cnt +=phi(v1 - v2) + phi(v0 - v1) + phi(v8 - v9);

			if (eq_cnt < THR2) // 9 - THR2
				goto easy_deblock;
		}   
		else
		{
			if (eq_cnt < THR2) // 9 - THR2
				goto easy_deblock;

			v0 = *(pV5-5*iPixelDistance);                                                                    
			v9 = *(pV5+4*iPixelDistance);                                                                    
		}
       

		   {
			I32_WMV min, max;
			if (v2 >= v7){ 
				min = v7; max = v2;
			}
			else{
				min = v2; max = v7;
			}
			if (min > v4) min  = v4;
			else if (v4 > max) max = v4;

			if (min > v6) min  = v6;
			else if (v6 > max) max = v6;

			if (min > v3) min  = v3;
			else if (v3 > max) max = v3;

			if (min > v5) min  = v5;
			else if (v5 > max) max = v5;

			if (min > v1) min  = v1;
			else if (v1 > max) max = v1;

			if (min > v8) min  = v8;
			else if (v8 > max) max = v8;


			if (max - min < 2*iStepSize)
			{
			   if (abs(v1-v0) >= iStepSize) v0 = v1;
			   if (abs(v8-v9) >= iStepSize) v9 = v8;

			   min = PFE_filt1_delta_Index;
			   *(pV5 - 4*iPixelDistance) = rgiClapTab[ min >> 4]; 
			   min += PFE_filt2_delta_Index;
			   *(pV5 - 3*iPixelDistance) = rgiClapTab[ min >> 4];     
			   min += PFE_filt3_delta_Index;
			   *(pV5 - 2*iPixelDistance) = rgiClapTab[min >> 4];                                                        
			   min += PFE_filt4_delta_Index;
			   *(pV5 - iPixelDistance)   = rgiClapTab[min >> 4];                                                        
			   min += PFE_filt5_delta_Index;
			   *(pV5)                    = rgiClapTab[min >> 4];                                                        
			   min += PFE_filt6_delta_Index;
			   *(pV5 + iPixelDistance)   = rgiClapTab[min >> 4];                                                        
			   min += PFE_filt7_delta_Index;
			   *(pV5 + 2*iPixelDistance) = rgiClapTab[min >> 4];                                                        
			   min += PFE_filt8_delta_Index;
			   *(pV5 + 3*iPixelDistance) = rgiClapTab[min >> 4];  
			}                                                                          
		   }
		goto done_deblock;

easy_deblock:	
	 {
           I32_WMV v4_v5 = v4 - v5;
           I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) / 8;
           if (abs(a30) < iStepSize) 
		   {
               I32_WMV v2_v3 = v2 - v3;
               I32_WMV v6_v7 = v6 - v7;
               I32_WMV a31 = (2*(v1-v4) - 5*v2_v3 + 4) / 8;                                 
               I32_WMV a32 = (2*(v5-v8) - 5*v6_v7 + 4) / 8;                                 
               I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
               if (iMina31_a32 < abs(a30))
			   {
                   I32_WMV dA30 = SIGN(a30) * iMina31_a32 - a30;
                   I32_WMV d = CLIP(5*dA30/8,0,v4_v5/2);
                   *(pV5 - iPixelDistance) = rgiClapTab[v4 - d];                                                                     
                   *pV5 = rgiClapTab[v5 + d];
               }
           }
       }

done_deblock:      
       pV5 += iPixelIncrement;
    }
}
#endif // 0

#else

FORCEINLINE Void_WMV FilterEdge (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
) {

    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV4 = pV5 - iPixelDistance;
    U8_WMV* pV3 = pV4 - iPixelDistance;
    U8_WMV* pV2 = pV3 - iPixelDistance;
    U8_WMV* pV1 = pV2 - iPixelDistance;
    U8_WMV* pV0 = pV1 - iPixelDistance;
    U8_WMV* pV6 = pV5 + iPixelDistance;
    U8_WMV* pV7 = pV6 + iPixelDistance;
    U8_WMV* pV8 = pV7 + iPixelDistance;
    U8_WMV* pV9 = pV8 + iPixelDistance;
    I32_WMV i;
    I32_WMV eq_cnt;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterEdge);

    for (i = 0; i < iEdgeLength; ++i) {
       
       I32_WMV v0 = *pV0;                                                                    
       I32_WMV v1 = *pV1;                                                                    
       I32_WMV v2 = *pV2;                                                                    
       I32_WMV v3 = *pV3;                                                                    
       I32_WMV v4 = *pV4;                                                                    
       I32_WMV v5 = *pV5;                                                                    
       I32_WMV v6 = *pV6;                                                                    
       I32_WMV v7 = *pV7;                                                                    
       I32_WMV v8 = *pV8;                                                                    
       I32_WMV v9 = *pV9;      
       // Logical place to prefetch *(pV9+CACHE_LINE_SIZE) for one vertical edge (actually, pV5, pV6, pV7, pV8 or pV9 work as well
       // for a horizontal edge, want to prefetch *(ppxlcCenter + (i-5)*iPixelDistance + CACHE_LINE_SIZE) but only for 10 of the possible 16 loops
       // but by only doing the one based on pV9, you would get 16/21 cache lines prefetched at no cost in additional registers
       
       /* check every four lines */ 
       if ((i & 3) == 0)
       {
            eq_cnt = phi(v0 - v1) + phi(v1 - v2) + phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
                    phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8) + phi(v8 - v9);
       }   
       
////////////// Run Experiement to see if it benefit.
       /*
       I32_WMV eq_cnt = inverse_phi(v2 - v3) + inverse_phi(v3 - v4) + inverse_phi(v4 - v5) +                     
                    inverse_phi(v5 - v6) + inverse_phi(v6 - v7);    
       if (eq_cnt <= INV_THR2)                                                          
           eq_cnt = (5 - eq_cnt_inv) + phi(v0 - v1) + phi(v1 - v2) + phi(v7 - v8) + phi(v8 - v9);       
       */ 

       if (eq_cnt >= THR2) {                                                         
           //I32_WMV max = MAX(v1,v2,v3,v4,v5,v6,v7,v8);    //DC Offset mode                               
           //I32_WMV min = MIN(v1,v2,v3,v4,v5,v6,v7,v8);                                   
           //if (abs(max-min) < 2*iStepSize){
           if (bMin_Max_LE_2QP(v1,v2,v3,v4,v5,v6,v7,v8,2*iStepSize)){                                          
               if (abs(v1-v0) >= iStepSize) v0 = v1;
               if (abs(v8-v9) >= iStepSize) v9 = v8;
               *pV1 = P_filt1;                                                   
               *pV2 = P_filt2;                                                        
               *pV3 = P_filt3;                                                        
               *pV4 = P_filt4;                                                        
               *pV5 = P_filt5;                                                        
               *pV6 = P_filt6;                                                        
               *pV7 = P_filt7;                                                        
               *pV8 = P_filt8;                                                        
/*
               *pV1 = filt(1);                                                   
               *pV2 = filt(2);                                                        
               *pV3 = filt(3);                                                        
               *pV4 = filt(4);                                                        
               *pV5 = filt(5);                                                        
               *pV6 = filt(6);                                                        
               *pV7 = filt(7);                                                        
               *pV8 = filt(8);                                                        
*/
           }                                                                          
       } else {
           I32_WMV v4_v5 = v4 - v5;
           I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) / 8;
           if (abs(a30) < iStepSize) {
               I32_WMV v2_v3 = v2 - v3;
               I32_WMV v6_v7 = v6 - v7;
               I32_WMV a31 = (2*(v1-v4) - 5*v2_v3 + 4) / 8;                                 
               I32_WMV a32 = (2*(v5-v8) - 5*v6_v7 + 4) / 8;                                 
               I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
               if (iMina31_a32 < abs(a30)){
                   I32_WMV dA30 = SIGN(a30) * iMina31_a32 - a30;
                   I32_WMV d = CLIP(5*dA30/8,0,v4_v5/2);
                   *pV4 = rgiClapTab[v4 - d];                                                                     
                   *pV5 = rgiClapTab[v5 + d];
               }
           }
           /* 
           I32_WMV a30 = (2*v3 - 5*v4 + 5*v5 - 2*v6 + 4) >> 3;                                 
           I32_WMV a31 = (2*v1 - 5*v2 + 5*v3 - 2*v4 + 4) >> 3;                                 
           I32_WMV a32 = (2*v5 - 5*v6 + 5*v7 - 2*v8 + 4) >> 3;                                 
           I32_WMV A30 = SIGN(a30) * ( MIN(abs(a30),abs(a31),abs(a32))   );                
           I32_WMV d = CLIP( (5*(A30-a30)/8),0,((v4-v5)/2) * ((abs(a30) < iStepSize) ? 1 : 0)   );
           *pV4 = g_rgiClapTabDec[v4 - d];                                                                     
           *pV5 = g_rgiClapTabDec[v5 + d]; 
           */
       }
      
       pV0 += iPixelIncrement;
       pV1 += iPixelIncrement;
       pV2 += iPixelIncrement;
       pV3 += iPixelIncrement;
       pV4 += iPixelIncrement;
       pV5 += iPixelIncrement;
       pV6 += iPixelIncrement;
       pV7 += iPixelIncrement;
       pV8 += iPixelIncrement;
       pV9 += iPixelIncrement;
    }
}
#endif
#endif // WMV9_SIMPLE_ONLY

#ifdef WMV_OPT_PPL_ARM


extern Void_WMV FilterEdge_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
);


/* 
 * Notes: Optimized C code from Ref code for ARM CPU without MMX
 * With MMX, this code won't necessary to accelerate, maybe even slow down because of mixing index calcuating.
 * Without MMX, it shows overall perf gain around 4.1% over v9 main profile.
 */

#if 0

FORCEINLINE Void_WMV FilterEdge_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
) {

    U8_WMV* pV5 = ppxlcCenter;
    I32_WMV i;
    I32_WMV eq_cnt;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    for (i = 0; i < iEdgeLength; ++i)
    {
       
       I32_WMV v1;// = *pV1;                                                                    
       I32_WMV v2;// = *pV2;                                                                    
       I32_WMV v3;// = *pV3;                                                                    
       I32_WMV v4;// = *pV4;                                                                    
       I32_WMV v5;// = *pV5;                                                                    
       I32_WMV v6;// = *pV6;                                                                    
       I32_WMV v7;// = *pV7;                                                                    
       I32_WMV v8;// = *pV8;
       
       if ((i & 3) == 0)
       {
			v3 = *(pV5 - 2*iPixelDistance);
			v4 = *(pV5 - iPixelDistance);
			v5 = *(pV5);
			v6 = *(pV5 + iPixelDistance);
			v7 = *(pV5 + 2*iPixelDistance);

            eq_cnt = phi(v3 - v4) + phi(v5 - v4) +  phi(v5 - v6) + phi(v6 - v7);       
			if (eq_cnt < 2) //already 3 0's so eq_cnt < THR2-1=5
			{
				goto easy_deblock;
			}

			v2 = *(pV5 - 3*iPixelDistance);
			v1 = *(pV5 - 4*iPixelDistance);
			v8 = *(pV5 + 3*iPixelDistance);

            eq_cnt += phi(v1 - v2) + phi(v2 - v3) + phi(v7 - v8);
		    if (eq_cnt < THR2-1)
			   goto easy_deblock;

       }
	   else
	   {
		   if (eq_cnt < THR2-1)
		   {
				v4 = *(pV5 - iPixelDistance);
				v5 = *(pV5);
				goto easy_deblock;
		   }

		   
			v1 = *(pV5 - 4*iPixelDistance);
			v2 = *(pV5 - 3*iPixelDistance);
			v3 = *(pV5 - 2*iPixelDistance);
			v4 = *(pV5 - iPixelDistance);
			v5 = *(pV5);
			v6 = *(pV5 + iPixelDistance);
			v7 = *(pV5 + 2*iPixelDistance);
			v8 = *(pV5 + 3*iPixelDistance);
		}

	   {
			I32_WMV min, max;
			if (v2 >= v7){ 
				min = v7; max = v2;
			}
			else{
				min = v2; max = v7;
			}
			if (min > v4) min  = v4;
			else if (v4 > max) max = v4;
			if (min > v6) min  = v6;
			else if (v6 > max) max = v6;


			if (min > v3) min  = v3;
			else if (v3 > max) max = v3;
			if (min > v5) min  = v5;
			else if (v5 > max) max = v5;

//           if (bMin_Max_LE_2QP_Short(v2,v3,v4,v5,v6,v7,2*iStepSize))
			if (max - min < 2*iStepSize)
		   { 
			   if (abs(v2-v1) >= iStepSize) v1 = v2;
			   if (abs(v7-v8) >= iStepSize) v8 = v7;

			   min = P_filt2_delta_Index;
			   *(pV5 - 3*iPixelDistance) = rgiClapTab[min>>3];
			   min += P_filt3_delta_Index;
			   *(pV5 - 2*iPixelDistance) = rgiClapTab[min>>3];                                                        
			   min += P_filt4_delta_Index;
			   *(pV5 - iPixelDistance) = rgiClapTab[min>>3];                                                        
			   min += P_filt5_delta_Index;
			   *pV5 = rgiClapTab[min>>3];                                                        
			   min += P_filt6_delta_Index;
			   *(pV5 + iPixelDistance) = rgiClapTab[min>>3];                                                        
			   min += P_filt7_delta_Index;
			   *(pV5 + 2*iPixelDistance) = rgiClapTab[min>>3];                                                        

		   }
	   }
	   goto done_deblock;

easy_deblock:
	   {

           I32_WMV dx = v5 - v4;
           if ((dx !=0) && abs(dx) < iStepSize)
           {
                *(pV5 - iPixelDistance) = rgiClapTab[v4 + (dx / 4)];
                *pV5 = rgiClapTab[v5 - (dx / 4)];
           }
	   }
done_deblock:
	   
       pV5 += iPixelIncrement;
    }
}
#endif // 0

#else

#ifndef WMV9_SIMPLE_ONLY

//Original FilterEdge_Short Ref C Code
FORCEINLINE Void_WMV FilterEdge_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
) {


#if 0
    U8_WMV* pV5 = ppxlcCenter - g_PPLTopOffset;
    U8_WMV* pV4 = ppxlcCenter - iPixelDistance;
#else
    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV4 = pV5 - iPixelDistance;
#endif

    U8_WMV* pV3 = pV4 - iPixelDistance;
    U8_WMV* pV2 = pV3 - iPixelDistance;
    U8_WMV* pV1 = pV2 - iPixelDistance;
    U8_WMV* pV6 = pV5 + iPixelDistance;
    U8_WMV* pV7 = pV6 + iPixelDistance;
    U8_WMV* pV8 = pV7 + iPixelDistance;
    I32_WMV i;
    I32_WMV eq_cnt;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterEdge_Short);

	//Only be right when iEdgeLength is multiply of 4.
    for (i = 0; i < iEdgeLength; i++)
    {
       
       I32_WMV v1 = *pV1;                                                                    
       I32_WMV v2 = *pV2;                                                                    
       I32_WMV v3 = *pV3;                                                                    
       I32_WMV v4 = *pV4;                                                                    
       I32_WMV v5 = *pV5;                                                                    
       I32_WMV v6 = *pV6;                                                                    
       I32_WMV v7 = *pV7;                                                                    
       I32_WMV v8 = *pV8;                                                                    
       
       if ((i & 3) == 0)
       {
            eq_cnt = /*phi(v0 - v1) +*/ phi(v1 - v2) + phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
                    phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8)/* + phi(v8 - v9)*/;       
       }
       
       if (eq_cnt >= THR2-1)
       {                                                         
           if (bMin_Max_LE_2QP_Short(v2,v3,v4,v5,v6,v7,2*iStepSize))
           { 
               if (abs(v2-v1) >= iStepSize) v1 = v2;
               if (abs(v7-v8) >= iStepSize) v8 = v7;
               
               *pV2 = P_filt2_Short;                                                        
               *pV3 = P_filt3_Short;                                                        
               *pV4 = P_filt4_Short;                                                        
               *pV5 = P_filt5_Short;                                                        
               *pV6 = P_filt6_Short;                                                        
               *pV7 = P_filt7_Short;                                                        
           }                                                                          
       } 
       else
       {
           I32_WMV dx = v5 - v4;
           if ((dx !=0) && abs(dx) < iStepSize)
           {
                *pV4 = rgiClapTab[v4 + (dx / 4)];
                *pV5 = rgiClapTab[v5 - (dx / 4)];
           }
       }
      
       pV1 += iPixelIncrement;
       pV2 += iPixelIncrement;
       pV3 += iPixelIncrement;
       pV4 += iPixelIncrement;
       pV5 += iPixelIncrement;
       pV6 += iPixelIncrement;
       pV7 += iPixelIncrement;
       pV8 += iPixelIncrement;
    }
}

#endif //WMV_OPT_PPL_ARM
#endif //WMV9_SIMPLE_ONLY

// This function was introduced by Tung-chiang Yang (tcyang) in November, 2001.  It */
/* should be functionally equivalent to FilterEdge() and any difference should be   */
/* considered a bug.                                                                */

#if 0 // IW, not used
FORCEINLINE Void_WMV    FilterEdge_Improved(
    tWMVDecInternalMember   *pWMVDec,
    U8_WMV*     ppxlcCenter,
    I32_WMV     iPixelDistance,
    I32_WMV     iPixelIncrement,
    I32_WMV     iEdgeLength,
    I32_WMV     iStepSize)
{
    U8_WMV*     pV5 = ppxlcCenter;
    U8_WMV*     pV4 = pV5 - iPixelDistance;
    U8_WMV*     pV3 = pV4 - iPixelDistance;
    U8_WMV*     pV2 = pV3 - iPixelDistance;
    U8_WMV*     pV1 = pV2 - iPixelDistance;
    U8_WMV*     pV0 = pV1 - iPixelDistance;
    U8_WMV*     pV6 = pV5 + iPixelDistance;
    U8_WMV*     pV7 = pV6 + iPixelDistance;
    U8_WMV*     pV8 = pV7 + iPixelDistance;
    U8_WMV*     pV9 = pV8 + iPixelDistance;
    I32_WMV     i;
    const U8_WMV*   rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterEdge_Improved);

    for (i = 0; i < iEdgeLength; ++i){
        I16_WMV     v0 = *pV0;                                                                    
        I16_WMV     v1 = *pV1;                                                                    
        I16_WMV     v2 = *pV2;                                                                    
        I16_WMV     v3 = *pV3;                                                                    
        I16_WMV     v4 = *pV4;                                                                    
        I16_WMV     v5 = *pV5;                                                                    
        I16_WMV     v6 = *pV6;                                                                    
        I16_WMV     v7 = *pV7;                                                                    
        I16_WMV     v8 = *pV8;                                                                    
        I16_WMV     v9 = *pV9;
        I16_WMV     v0_v1 = v0 - v1;
        I16_WMV     v1_v2 = v1 - v2;
        I16_WMV     v2_v3 = v2 - v3;
        I16_WMV     v3_v4 = v3 - v4;
        I16_WMV     v4_v5 = v4 - v5;
        I16_WMV     v5_v6 = v5 - v6;
        I16_WMV     v6_v7 = v6 - v7;
        I16_WMV     v7_v8 = v7 - v8;
        I16_WMV     v8_v9 = v8 - v9;

//      I32_WMV eq_cnt = phi(v1 - v0) + phi(v2 - v1) + phi(v3 - v2) + phi(v4 - v3)
//                       + phi(v5 - v4) + phi(v6 - v5) + phi(v7 - v6) + phi(v8 - v7)
//                       + phi(v9 - v8);       

//      if (eq_cnt >= THR2){

        // In this branch we care only whether eq_cnt is larger than or
        // or equal to THR2 or not.  We can stop computing phi()'s if we
        // know for sure eq_cnt will or will not satisfy that criterion
        // (for consideration of the next short-circuit if expression, we
        // might need to keep computing eq_cnt even if we know it will be
        // at least THR2.

        // Note that in C we program this way.  With MMX or some platform with SIMD
        // features, computing the sum of phi() could be easier for the embedded parallelism.

        I32_WMV     eq_cnt, eq_cnt1;

        eq_cnt = phi(v1_v2) + phi(v2_v3) + phi(v3_v4) + phi(v4_v5);
        // If any of the following three if conditions fails below, we know eq_cnt >= THR2
        // cannot happen and we short-circuit out.  However, if none of them fails, we
        // might still need to evaluate eq_cnt completely -- other than phi(v1 - v0).
        if (eq_cnt >= (THR2 - 5)){
            eq_cnt += phi(v5_v6);
            if (eq_cnt >= (THR2 - 4)){
                eq_cnt += phi(v6_v7);
                if (eq_cnt >= (THR2 - 3)){
                    eq_cnt += phi(v7_v8);
                }
            }
        }

        // If phi(v2 - v1), phi(v3 - v2), ......, phi(v8 - v7) are all 1, then at this
        // point eq_cnt will be 7 and so is eq_cnt1.  Otherwise eq_cnt1 will be less than
        // 7.
        eq_cnt1 = eq_cnt;

        // We need to compute phi(v1 - v0) and phi(v9 - v8) only when they could break the
        // tie, when all the other phi()'s add to (THR2 - 2) or (THR2 - 1).
        if ((eq_cnt == (THR2 - 2)) || (eq_cnt == (THR2 - 1))){
            eq_cnt += phi(v0_v1);
            eq_cnt += phi(v8_v9);
        }

        // At this point eq_cnt is no longer the original definition of eq_cnt, but
        // whether eq_cnt >= THR2 or not does not change.

        if (eq_cnt >= THR2){
//          if (bMin_Max_LE_2QP(v1, v2, v3, v4, v5, v6, v7, v8, 2*iStepSize)){

            // At this point, 64.9% cases satisfy (eq_cnt1 == 7) and (iStepSize > 7), given
            // that eq_cnt >= THR2.  eq_cnt1 == 7 means all |v_{i+1} - v_i| are at most
            // 2 (so the corresponding phi() is 1), so v1, v2, ......, v8 at most span a
            // range of 14.  If iStepSize > 7 in this case, we are guaranteed the
            // bMin_Max_LE_2QP() will return true.

            // The logical expression (eq_cnt1 == 7) && (iStepSize > 7) is much cheaper
            // than the bMin_Max_LE_2QP() macro, so we short-circuit the if expression
            // if the AND result is true.

            // Among those failing (eq_cnt1 == 7) && (iStepSize > 7), (for the given sample
            // input iStepSize <= 7 only when it is 4; iStepSize can range from 4, 8, ...., 28)
            // 98.2% cases will go through bMin_Max_LE_2QP().  This triggers the idea for TCYANG6.

            // Calling bMin_Max_LE_2QP() is somewhat expensive, so we add another condition here
            // when eq_cnt1 == 6 and |v8 - v1| + 2*THR2 <= 2*iStepSize.  If eq_cnt1 == 6, it means
            // in the chain v1, v2, ....., v8 there is only one gap/difference larger than 2 and
            // all the other 6 gaps are 0, 1, or 2.  So if we take the difference between v1 and
            // v8, the largest possible difference between mininum and maximum of v1, v2, ..., v8
            // cannot exceed abs(v8 - v1) + 2 * THR2.  If this largest possible gap is smaller than
            // or equal to 2*iStepSize, we can short-circuit out.

            if (((eq_cnt1 == 7) && (iStepSize > 7)) ||
                ((eq_cnt1 == 6) && (abs(v8 - v1) + 2*THR2 <= 2*iStepSize)) ||
                bMin_Max_LE_2QP(v1, v2, v3, v4, v5, v6, v7, v8, 2*iStepSize)){

                // The original macro definitions for P_filt? requires 40 multiplications
                // (some of them can be shiftings, multiplications by 2 and 4) and 60 additions
                // of integers.  The countings above include the "+ 8" but do not include the
                // ">> 4" part.

                // Note that the original algorithm changes v9; if the algorithm uses v9 in
                // this function later, it needs to be updated here.  v0 is used so it is
                // updated here.
                if (abs(v0_v1) >= iStepSize){
                    v0 = v1;
                    v0_v1 = 0;
                }
                if (abs(v8_v9) >= iStepSize){
                    v8_v9 = 0;
                }

                // We use only 15 (integer) multiplications and 44 additions if we
                // compute delta_pfilt directly.  The scheme here uses 10 (integer)
                // multiplications and 55 additions, and 9 additions in computing delta_vi[]
                // can be saved if we reuse the computed v_{i+1} - v_i.
                {
                    I32_WMV     p_filt, delta_pfilt, delta2_pfilt;

                    // *pV? computation below no longer uses rgiClapTab[].  It can be shown that
                    // the computed (p_filt >> 4) will never be negative or exceed 256 because p_filt
                    // is essentially a linear combination of v_i's with corresponding coefficients
                    // between 0 and 1.  Therefore if all v_i's are in [0, 256], p_filt will also be
                    // in that range.  The addition of 8 will not make p_filt 257 even if all v_i's are
                    // 256.
                    p_filt = (6*v0 + 4*v1 + 2*(v2 + v3) + v4 + v5) + 8;
                    *pV1 = p_filt >> 4;
                    delta_pfilt = (v6 + v4 - 2*(v1_v2 + v0));
                    p_filt += delta_pfilt;
                    *pV2 = p_filt >> 4;
                    delta2_pfilt = v6_v7 + v4_v5 + 2*(v2_v3 - v1_v2);
                    delta_pfilt -= delta2_pfilt;
                    p_filt += delta_pfilt;
                    *pV3 = p_filt >> 4;
                    delta2_pfilt = v7_v8 + v5_v6 + 2*(v3_v4 - v2_v3) - v0_v1;
                    delta_pfilt -= delta2_pfilt;
                    p_filt += delta_pfilt;
                    *pV4 = p_filt >> 4;
                    delta2_pfilt = v8_v9 + v6_v7 + 2*(v4_v5 - v3_v4) - v1_v2;
                    delta_pfilt -= delta2_pfilt;
                    p_filt += delta_pfilt;
                    *pV5 = p_filt >> 4;
                    delta2_pfilt = v7_v8 + 2*(v5_v6 - v4_v5) - v2_v3 - v0_v1;
                    delta_pfilt -= delta2_pfilt;
                    p_filt += delta_pfilt;
                    *pV6 = p_filt >> 4;
                    delta2_pfilt = v8_v9 + 2*(v6_v7 - v5_v6) - v3_v4 - v1_v2;
                    delta_pfilt -= delta2_pfilt;
                    p_filt += delta_pfilt;
                    *pV7 = p_filt >> 4;
                    delta2_pfilt = 2*(v7_v8 - v6_v7) - v4_v5 - v2_v3;
                    delta_pfilt -= delta2_pfilt;
                    p_filt += delta_pfilt;
                    *pV8 = p_filt >> 4;
                }
            }
        } else {
            // Check FilterEdge() for the original algorithm.

            // If abs(v4_v5) == 0 or 1, d has to be 0 no matter what when d = CLIP(*, 0, v4_v5/2),
            // as (v4 - v5)/2 being 0 and we can bypass the other computations.

            if (abs(v4_v5) > 1){
                I32_WMV     a30 = (2*(v3-v6) - 5*v4_v5 + 4) / 8;

                // Here we explain why v4_v5 * a30 should be negative for us to keep
                // computing.  We have already excluded the case for v4_v5 == 0, so
                // v4_v5 * a30 == 0 means a30 == 0, and then iMina31_a32 has to be 0 for
                // us to change *pV4 and *pV5, but in that case dA30 and d will also be 0.

                // Now suppose v4_v5 * a30 > 0, meaning they are both positive or negative.
                // Without loss of generality, assume v4_v5 > 0 and a30 > 0.  In this case
                // if we compute dA30, it means iMina31_a32 < a30, and dA30 will be negative,
                // which leads to d being 0 after CLIP().  For similar arguments we find
                // d is 0 (if we evaluate d and dA30 at all) if v4_v5 < 0 and a30 < 0.

                // This short-circuit scheme should be the one intern Sachin found in the summer.

                if (v4_v5 * a30 < 0){
                    if (abs(a30) < iStepSize){
                        I32_WMV     a31 = (2*(v1 - v4) - 5*v2_v3 + 4) / 8;
                        I32_WMV     a32 = (2*(v5 - v8) - 5*v6_v7 + 4) / 8;
                        I32_WMV     iMina31_a32 = min(abs(a31), abs(a32));

                        // In the original algorithm the branch below compares iMina31_a32
                        // with abs(a30), and now we compare it with abs(a30) - 1.  This
                        // is because if iMina31_a32 == abs(a30) - 1, then dA30 is essentially
                        // -SIGN(a30)*(|a30| - iMina31_a32), and 5*dA30/8 = 0.

                        if (iMina31_a32 < abs(a30) - 1){
                            // Here we do not need the clipping at 0 (the calling of CLIP() involves
                            // the comparison of its two boundary points, and it does not look smart).
                            // Without loss of generality, assume v4_v5 > 0.  In this case a30 < 0
                            // and dA30 > 0 with d = 5*dA30/8 >= 0.  There is no need to try another
                            // clip at 0.  The same argument can be used if v4_v5 < 0.
                            I32_WMV     dA30, d;
                            I32_WMV     threshold = v4_v5/2;

                            if (a30 > 0){
                                dA30 = iMina31_a32 - a30;
                                d = 5*dA30/8;
                                if (d < threshold)
                                    d = threshold;
                            } else {    // a30 < 0
                                dA30 = iMina31_a32 + a30;
                                d = -5*dA30/8;
                                if (d > threshold)
                                    d = threshold;
                            }

                            // Using rgiClapTab[] is not necessary because v4 - d and
                            // v5 + d are guaranteed not to become negative or go over
                            // 256 because d is clamped between 0 and (v4_v5)/2.
                            *pV4 = v4 - d;                                                                     
                            *pV5 = v5 + d;
                        }
                    }
                }
            }
        }
      
        pV0 += iPixelIncrement;
        pV1 += iPixelIncrement;
        pV2 += iPixelIncrement;
        pV3 += iPixelIncrement;
        pV4 += iPixelIncrement;
        pV5 += iPixelIncrement;
        pV6 += iPixelIncrement;
        pV7 += iPixelIncrement;
        pV8 += iPixelIncrement;
        pV9 += iPixelIncrement;
    }
}
#endif // not used

#if !defined(WMV9_SIMPLE_ONLY)
FORCEINLINE Void_WMV FilterHalfEdge (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV4 = pV5 - iPixelDistance;
    U8_WMV* pV3 = pV4 - iPixelDistance;
    U8_WMV* pV2 = pV3 - iPixelDistance;
    U8_WMV* pV1 = pV2 - iPixelDistance;
    U8_WMV* pV0 = pV1 - iPixelDistance;
    U8_WMV* pV6 = pV5 + iPixelDistance;
    U8_WMV* pV7 = pV6 + iPixelDistance;
    U8_WMV* pV8 = pV7 + iPixelDistance;
    U8_WMV* pV9 = pV8 + iPixelDistance;
    I32_WMV i;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterHalfEdge);

    for (i = 0; i < iEdgeLength; ++i) {
       
       I32_WMV v0 = *pV0;                                                                    
       I32_WMV v1 = *pV1;                                                                    
       I32_WMV v2 = *pV2;                                                                    
       I32_WMV v3 = *pV3;                                                                    
       I32_WMV v4 = *pV4;                                                                    
       I32_WMV v5 = *pV5;                                                                    
       I32_WMV v6 = *pV6;                                                                    
       I32_WMV v7 = *pV7;                                                                    
       I32_WMV v8 = *pV8;                                                                    
       I32_WMV v9 = *pV9;                                                                    
                                                                                     
       I32_WMV eq_cnt = phi(v0 - v1) + phi(v1 - v2) + phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
                    phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8) + phi(v8 - v9);       
                                                                                            
       if (eq_cnt >= THR2) {                                                         
           //I32_WMV max = MAX(v1,v2,v3,v4,v5,v6,v7,v8);    //DC Offset mode                               
           //I32_WMV min = MIN(v1,v2,v3,v4,v5,v6,v7,v8);                                   
           //if (abs(max-min) < 2*iStepSize){                                          
           if (bMin_Max_LE_2QP(v1,v2,v3,v4,v5,v6,v7,v8,2*iStepSize)){                                          
               if (abs(v1-v0) >= iStepSize) v0 = v1;
               *pV1 = P_filt1;                                                   
               *pV2 = P_filt2;                                                        
               *pV3 = P_filt3;                                                        
               *pV4 = P_filt4;                                                        
               /* 
               *pV1 = filt(1);                                                   
               *pV2 = filt(2);                                                        
               *pV3 = filt(3);                                                        
               *pV4 = filt(4);                                                        
               */
           }                                                                          
       } else {
           I32_WMV v4_v5 = v4 - v5;
           I32_WMV a30 = (2*(v3-v6) - 5*v4_v5 + 4) / 8;
           if (abs(a30) < iStepSize) {
               I32_WMV v2_v3 = v2 - v3;
               I32_WMV v6_v7 = v6 - v7;
               I32_WMV a31 = (2*(v1-v4) - 5*v2_v3 + 4) / 8;                                 
               I32_WMV a32 = (2*(v5-v8) - 5*v6_v7 + 4) / 8;                                 
               I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
               if (iMina31_a32 < abs(a30)){
                   I32_WMV dA30 = SIGN(a30) * iMina31_a32 - a30;
                   I32_WMV d = CLIP(5*dA30/8,0,v4_v5/2);
                   *pV4 = rgiClapTab [v4 - d];                                                                     
               }
           }
           /*
           I32_WMV a30 = (2*v3 - 5*v4 + 5*v5 - 2*v6 + 4) >> 3;                                 
           I32_WMV a31 = (2*v1 - 5*v2 + 5*v3 - 2*v4 + 4) >> 3;                                 
           I32_WMV a32 = (2*v5 - 5*v6 + 5*v7 - 2*v8 + 4) >> 3;                                 
                                                                                      
           I32_WMV A30 = SIGN(a30) * ( MIN(abs(a30),abs(a31),abs(a32))   );                
           I32_WMV d = CLIP( (5*(A30-a30)/8),0,((v4-v5)/2) * ((abs(a30) < iStepSize) ? 1 : 0)   );
           *pV4 = g_rgiClapTabDec[v4 - d];                                                                     
           */
       }
      
       pV0 += iPixelIncrement;
       pV1 += iPixelIncrement;
       pV2 += iPixelIncrement;
       pV3 += iPixelIncrement;
       pV4 += iPixelIncrement;
       pV5 += iPixelIncrement;
       pV6 += iPixelIncrement;
       pV7 += iPixelIncrement;
       pV8 += iPixelIncrement;
       pV9 += iPixelIncrement;
    
    }
}

FORCEINLINE Void_WMV FilterHalfEdge_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV4 = pV5 - iPixelDistance;
    U8_WMV* pV3 = pV4 - iPixelDistance;
    U8_WMV* pV2 = pV3 - iPixelDistance;
    U8_WMV* pV1 = pV2 - iPixelDistance;
    U8_WMV* pV6 = pV5 + iPixelDistance;
    U8_WMV* pV7 = pV6 + iPixelDistance;
    U8_WMV* pV8 = pV7 + iPixelDistance;
    I32_WMV i;
    I32_WMV eq_cnt;
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterHalfEdge_Short);

    for (i = 0; i < iEdgeLength; ++i)
    {
       
       I32_WMV v1 = *pV1;                                                                    
       I32_WMV v2 = *pV2;                                                                    
       I32_WMV v3 = *pV3;                                                                    
       I32_WMV v4 = *pV4;                                                                    
       I32_WMV v5 = *pV5;                                                                    
       I32_WMV v6 = *pV6;                                                                    
       I32_WMV v7 = *pV7;                                                                    
       I32_WMV v8 = *pV8;                                                                    
       
       if ((i & 3) == 0)
       {
            eq_cnt = /*phi(v0 - v1) +*/ phi(v1 - v2) + phi(v2 - v3) + phi(v3 - v4) + phi(v4 - v5) +                     
                    phi(v5 - v6) + phi(v6 - v7) + phi(v7 - v8)/* + phi(v8 - v9)*/;       
       }
       
       if (eq_cnt >= THR2-1)
       {                                                         
           if (bMin_Max_LE_2QP_Short(v2,v3,v4,v5,v6,v7,2*iStepSize))
           { 
               if (abs(v2-v1) >= iStepSize) v1 = v2;
               
               *pV2 = P_filt2_Short;                                                        
               *pV3 = P_filt3_Short;                                                        
               *pV4 = P_filt4_Short;                                                        
           }                                                                          
       } 
       else
       {
           I32_WMV dx = v5 - v4;
           if ((dx !=0) && abs(dx) < iStepSize)
           {
                *pV4 = rgiClapTab[v4 + (dx / 4)];
           }
       }
      
       pV1 += iPixelIncrement;
       pV2 += iPixelIncrement;
       pV3 += iPixelIncrement;
       pV4 += iPixelIncrement;
       pV5 += iPixelIncrement;
       pV6 += iPixelIncrement;
       pV7 += iPixelIncrement;
       pV8 += iPixelIncrement;
    }
}


//Find maxumum and minimum values in a 10x10 block
Void_WMV DetermineThreshold(U8_WMV *ptr, I32_WMV *thr, I32_WMV *range, I32_WMV width) 
{
    I32_WMV max = 0;                                         
    I32_WMV min = 255;  
    I32_WMV x, y;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DetermineThreshold);

    for (x = -1; x < 9; x++){                    
        for (y = -1; y < 9; y++){                
            I32_WMV pixelValue = ptr[x+(y*width)];       
            if (max < pixelValue) max = pixelValue;  
            if (min > pixelValue) min = pixelValue;  
        }                                            
    }
    *thr = (max + min + 1) / 2;
    *range = max - min;
}    

Void_WMV ApplySmoothing(U8_WMV *pixel,I32_WMV width, I32_WMV max_diff,I32_WMV thr)
{

    Bool_WMV bin[10][10];
    I32_WMV  x, y;
    U8_WMV output[8][8];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ApplySmoothing);

    for (x = 0; x < 10; x++){
        for (y = 0; y < 10; y++){
            U8_WMV pixelValue =  pixel[(x-1)+((y-1)*width)];
            bin [x][y] = (pixelValue >= thr) ? 1 : 0;
        }
    }


    for (x = 0; x < 8; x++){
        for (y = 0; y < 8; y++){
            if ((bin[x][y] == bin[x+1][y])    //If All 9 values equel (all 0 or all 1)
             && (bin[x][y] == bin[x+2][y])
             && (bin[x][y] == bin[x]  [y+1])
             && (bin[x][y] == bin[x+1][y+1])
             && (bin[x][y] == bin[x+2][y+1])
             && (bin[x][y] == bin[x]  [y+2])
             && (bin[x][y] == bin[x+1][y+2])
             && (bin[x][y] == bin[x+2][y+2])) {

                U8_WMV *ppxlcFilt = pixel+x+(y*width);

                //Apply Spoothing Filter
                I32_WMV filt = (  ppxlcFilt[-1-width] +  2*ppxlcFilt[0 -width] +   ppxlcFilt[+1-width] +
                            2*ppxlcFilt[-1      ] +  4*ppxlcFilt[0       ] + 2*ppxlcFilt[+1      ] +      
                              ppxlcFilt[-1+width] +  2*ppxlcFilt[0 +width] +   ppxlcFilt[+1+width] +8) >> 4;
                
                if ((filt - *ppxlcFilt) > max_diff) filt = *ppxlcFilt + max_diff;
                else if ((filt - *ppxlcFilt) < -max_diff) filt = *ppxlcFilt - max_diff;
                CLIP(filt,0,255);
                output[x][y] = (U8_WMV)filt;

            } else output[x][y] = *(pixel+x+(y*width));
        }
    }
    for (x = 0; x < 8; x++){
        for (y = 0; y < 8; y++){
        *(pixel+x+(y*width)) = output[x][y];
           
        }
    }
}


Void_WMV    ApplySmoothing_Improved(
    U8_WMV      *pixel,
    I32_WMV     width,
    I32_WMV     max_diff,
    I32_WMV     thr)
{
    register I16_WMV    i, j;
    Bool_WMV    bin[10][10];
    U8_WMV      binRowSum[8][8], binSum[8][8], auxColumnLeft[8], auxColumnRight[8];
    U8_WMV      output[8][8];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ApplySmoothing_Improved);

    for (i = 0; i < 10; i++){
        for (j = 0; j < 10; j++){
            U8_WMV  pixelValue =  pixel[(j-1) + ((i-1)*width)];

            bin[i][j] = (pixelValue >= thr) ? 1 : 0;
        }
    }

    // The following big block of codes computes the sum of 9 bin[][]'s surrounding the
    // given pixel.  For instance, binSum[i][j] will be the sum of
    // bin[i][j], bin[i+1][j], bin[i+2][j], bin[i+1][j], bin[i+1][j+1],
    // bin[i+1][j+2], bin[i+2][j], bin[i+2][j+1], and bin[i+2][j+2].  Note that for
    // programming convenience, there is an offset of 1 on both indices for bin[][].

    // The loop below adds up rows so each row corresponds to the sum of three rows
    // of bin[*][].  We first add up two rows every other row, then we add the additional
    // bin[*][] row.

    // Note that here the computations of binRowSum[][] can be carried out in parallel
    // with SIMD.

    for (j = 0; j < 8; j++){
        binRowSum[0][j] = (U8_WMV) (bin[1][j+1] + bin[2][j+1]);
        binRowSum[2][j] = (U8_WMV) (bin[3][j+1] + bin[4][j+1]);
        binRowSum[4][j] = (U8_WMV) (bin[5][j+1] + bin[6][j+1]);
        binRowSum[6][j] = (U8_WMV) (bin[7][j+1] + bin[8][j+1]);
        binRowSum[7][j] = (U8_WMV) (binRowSum[6][j] + bin[9][j+1]);
        binRowSum[6][j] = (U8_WMV) (binRowSum[6][j] + bin[6][j+1]);
        binRowSum[5][j] = (U8_WMV) (binRowSum[4][j] + bin[7][j+1]);
        binRowSum[4][j] = (U8_WMV) (binRowSum[4][j] + bin[4][j+1]);
        binRowSum[3][j] = (U8_WMV) (binRowSum[2][j] + bin[5][j+1]);
        binRowSum[2][j] = (U8_WMV) (binRowSum[2][j] + bin[2][j+1]);
        binRowSum[1][j] = (U8_WMV) (binRowSum[0][j] + bin[3][j+1]);
        binRowSum[0][j] = (U8_WMV) (binRowSum[0][j] + bin[0][j+1]);
    }

    // Here we are ready to add up three columns of binRowSum[][] and keep the sums
    // in binSum[].  We need to prepare the boundaries auxColumnLeft[] and auxColumnRight[]
    // here.

    auxColumnLeft[0] = (U8_WMV) (bin[1][0] + bin[2][0]);
    auxColumnLeft[2] = (U8_WMV) (bin[3][0] + bin[4][0]);
    auxColumnLeft[4] = (U8_WMV) (bin[5][0] + bin[6][0]);
    auxColumnLeft[6] = (U8_WMV) (bin[7][0] + bin[8][0]);
    auxColumnLeft[7] = (U8_WMV) (auxColumnLeft[6] + bin[9][0]);
    auxColumnLeft[6] = (U8_WMV) (auxColumnLeft[6] + bin[6][0]);
    auxColumnLeft[5] = (U8_WMV) (auxColumnLeft[4] + bin[7][0]);
    auxColumnLeft[4] = (U8_WMV) (auxColumnLeft[4] + bin[4][0]);
    auxColumnLeft[3] = (U8_WMV) (auxColumnLeft[2] + bin[5][0]);
    auxColumnLeft[2] = (U8_WMV) (auxColumnLeft[2] + bin[2][0]);
    auxColumnLeft[1] = (U8_WMV) (auxColumnLeft[0] + bin[3][0]);
    auxColumnLeft[0] = (U8_WMV) (auxColumnLeft[0] + bin[0][0]);

    auxColumnRight[0] = (U8_WMV) (bin[1][9] + bin[2][9]);
    auxColumnRight[2] = (U8_WMV) (bin[3][9] + bin[4][9]);
    auxColumnRight[4] = (U8_WMV) (bin[5][9] + bin[6][9]);
    auxColumnRight[6] = (U8_WMV) (bin[7][9] + bin[8][9]);
    auxColumnRight[7] = (U8_WMV) (auxColumnRight[6] + bin[9][9]);
    auxColumnRight[6] = (U8_WMV) (auxColumnRight[6] + bin[6][9]);
    auxColumnRight[5] = (U8_WMV) (auxColumnRight[4] + bin[7][9]);
    auxColumnRight[4] = (U8_WMV) (auxColumnRight[4] + bin[4][9]);
    auxColumnRight[3] = (U8_WMV) (auxColumnRight[2] + bin[5][9]);
    auxColumnRight[2] = (U8_WMV) (auxColumnRight[2] + bin[2][9]);
    auxColumnRight[1] = (U8_WMV) (auxColumnRight[0] + bin[3][9]);
    auxColumnRight[0] = (U8_WMV) (auxColumnRight[0] + bin[0][9]);

    // Now we carry out the sum of columns.

    for (i = 0; i < 8; i++){
        binSum[i][0] = (U8_WMV) (binRowSum[i][0] + binRowSum[i][1]);
        binSum[i][2] = (U8_WMV) (binRowSum[i][2] + binRowSum[i][3]);
        binSum[i][4] = (U8_WMV) (binRowSum[i][4] + binRowSum[i][5]);
        binSum[i][6] = (U8_WMV) (binRowSum[i][6] + binRowSum[i][7]);
        binSum[i][7] = (U8_WMV) (binSum[i][6] + auxColumnRight[i]);
        binSum[i][6] += binRowSum[i][5];
        binSum[i][5] = (U8_WMV) (binSum[i][4] + binRowSum[i][6]);
        binSum[i][4] += binRowSum[i][3];
        binSum[i][3] = (U8_WMV) (binSum[i][2] + binRowSum[i][4]);
        binSum[i][2] += binRowSum[i][1];
        binSum[i][1] = (U8_WMV) (binSum[i][0] + binRowSum[i][2]);
        binSum[i][0] += auxColumnLeft[i];
    }

    for (i = 0; i < 8; i++){
        for (j = 0; j < 8; j++){
            // The original algorithm makes sure that the 9 bin[][] centered around the
            // pixel under consideration are the same (either 0 or 1), and apply the
            // low-pass filtering.  Jerry He (yamihe) uses the idea to add up all the
            // 9 bin[][] values.  If the sum is 0 or 9, then all the 9 bin[][] are the
            // same.
            if ((binSum[i][j] == 0) || (binSum[i][j] == 9)){
                U8_WMV  *ppxlcFilt = pixel + j + (i*width);

                // Apply Spoothing Filter
                I32_WMV     filt = (ppxlcFilt[-1-width] +  2*ppxlcFilt[0 -width] +   ppxlcFilt[+1-width] +
                            2*ppxlcFilt[-1      ] +  4*ppxlcFilt[0       ] + 2*ppxlcFilt[+1      ] +      
                              ppxlcFilt[-1+width] +  2*ppxlcFilt[0 +width] +   ppxlcFilt[+1+width] +8) >> 4;

                // Here we subtract *ppxlcFilt before the clamping.  We add it back
                // after the clamping.

                filt = filt - *ppxlcFilt;
                if (filt > max_diff)
                    filt = max_diff;
                else if (filt < -max_diff)
                    filt = -max_diff;
                filt = filt + *ppxlcFilt;

                // The CLIP() macro here is removed.  filt is obtained from applying a low-pass
                // mask to the ppxlcFilt[]'s.  Each of the ppxlcFilt[] is between 0 and 255 (inclusive),
                // and adding them together, adding 8 and then dividing by 16 is guaranteed to make filt
                // still within the range [0, 255].  Applying the clipping above with max_diff would not
                // change this property.
                output[i][j] = (U8_WMV) filt;
            } else {
                output[i][j] = *(pixel + i*width + j);
            }
        }
    }
    for (i = 0; i < 8; i++){
        // Copying a row.
        for (j = 0; j < 8; j++)
            *(pixel + i*width + j) = output[i][j];
    }
}


Void_WMV DeblockMB (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockMB);
    
    if (bDoTop) {
        // Filter the top Y, U and V edges.
        FilterEdge (pWMVDec, ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterEdge (pWMVDec, ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterEdge (pWMVDec, ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfEdge (pWMVDec, ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfEdge (pWMVDec, ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfEdge (pWMVDec, ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterEdge(pWMVDec, ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge(pWMVDec, ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterEdge(pWMVDec, ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterEdge(pWMVDec, ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfEdge(pWMVDec, ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfEdge(pWMVDec, ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfEdge(pWMVDec, ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
  } 
}

Void_WMV DeblockMB_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockMB_Short);
#if 0
	g_PPLTopOffset = 0; //clear
#endif
    
    if (bDoTop) {
        // Filter the top Y, U and V edges.
#if 0
		if (1 == g_PPLMBRow)
		{
			g_PPLTopOffset = iWidthPrevY << 5; //2*MB_SIZE*iWidthPrevY; two rows difference
		}
#endif

        FilterEdge_Short (pWMVDec, ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);

#if 0
		if (1 == g_PPLMBRow)
		{
			g_PPLTopOffset = iWidthPrevUV << 4; //2*BLOCK_SIZE*iWidthPrevUV; two rows difference
		}
#endif

        FilterEdge_Short (pWMVDec, ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterEdge_Short (pWMVDec, ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

#if 0
	g_PPLTopOffset = 0; //clear
#endif

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfEdge_Short (pWMVDec, ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfEdge_Short (pWMVDec, ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfEdge_Short (pWMVDec, ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterEdge_Short(pWMVDec, ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge_Short(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge_Short(pWMVDec, ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterEdge_Short(pWMVDec, ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterEdge_Short(pWMVDec, ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfEdge_Short(pWMVDec, ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfEdge_Short(pWMVDec, ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfEdge_Short(pWMVDec, ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
  } 
}

Void_WMV Full_DeblockMB (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoRight,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoBottom,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Full_DeblockMB);

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterEdge_Short(pWMVDec, ppxliY+iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge_Short(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

	// Do next middle
    if (bDoRight) {
        // Filter the middle horizontal Y edge
        FilterEdge_Short(pWMVDec, ppxliY+iWidthPrevY * 8+MB_SIZE,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge_Short(pWMVDec, ppxliY + 8 + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoRight) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge_Short(pWMVDec, ppxliY+MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterEdge_Short(pWMVDec, ppxliU+BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterEdge_Short(pWMVDec, ppxliV+BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottom) {
        // Filter the top Y, U and V edges.
#if 0
		if (1 == g_PPLMBRow)
		{
			g_PPLTopOffset = iWidthPrevY << 5; //2*MB_SIZE*iWidthPrevY; two rows difference
		}
#endif
        FilterEdge_Short (pWMVDec, ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);

#if 0
		if (1 == g_PPLMBRow)
		{
			g_PPLTopOffset = iWidthPrevUV << 4; //2*BLOCK_SIZE*iWidthPrevUV; two rows difference
		}
#endif

        FilterEdge_Short (pWMVDec, ppxliU + (iWidthPrevUV * BLOCK_SIZE), iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterEdge_Short (pWMVDec, ppxliV + (iWidthPrevUV * BLOCK_SIZE), iWidthPrevUV,1,BLOCK_SIZE,iStepSize);

#if 0
	g_PPLTopOffset = 0; //clear
#endif

    }
}
#endif //WMV9_SIMPLE_ONLY

#if 0 // not used, IW
Void_WMV DeblockMB_Improved(
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockMB_Improved);
    
    if (bDoTop) {
        // Filter the top Y, U and V edges.
        FilterEdge_Improved (pWMVDec, ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterEdge_Improved (pWMVDec, ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterEdge_Improved (pWMVDec, ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfEdge (pWMVDec, ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfEdge (pWMVDec, ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfEdge (pWMVDec, ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterEdge_Improved(pWMVDec, ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge_Improved(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge_Improved(pWMVDec, ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterEdge_Improved(pWMVDec, ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterEdge_Improved(pWMVDec, ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfEdge(pWMVDec, ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfEdge(pWMVDec, ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfEdge(pWMVDec, ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
  } 
}
#endif

#if !defined(WMV9_SIMPLE_ONLY) 
Void_WMV DeblockMBInterlace411 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockMBInterlace411);

    if (bDoMiddle) {
        // Filter the middle vertical Y edge
        FilterEdge(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge(pWMVDec, ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);

    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfEdge(pWMVDec, ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
    } 
}


Void_WMV DeblockMBInterlace411_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockMBInterlace411_Short);

    if (bDoMiddle) {
        // Filter the middle vertical Y edge
        FilterEdge_Short(pWMVDec, ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterEdge_Short(pWMVDec, ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);

    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfEdge_Short(pWMVDec, ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
    } 
}
#endif // !(defined(WMV9_SIMPLE_ONLY) 

Void_WMV DeringMB (
    U8_WMV        *ppxlcY,
    U8_WMV        *ppxlcU,
    U8_WMV        *ppxlcV,
    I32_WMV                  iStepSize,
    I32_WMV                  iWidthPrevY,
    I32_WMV                  iWidthPrevUV
)
{  
    I32_WMV thr[6];
    I32_WMV range[6];
    I32_WMV k_max;
    I32_WMV max_range;
    I32_WMV k;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeringMB);
    
    g_pDetermineThreshold(ppxlcY,                                    &thr[0],&range[0],iWidthPrevY);
    g_pDetermineThreshold(ppxlcY+ BLOCK_SIZE,                        &thr[1],&range[1],iWidthPrevY);
    g_pDetermineThreshold(ppxlcY+(BLOCK_SIZE*iWidthPrevY),           &thr[2],&range[2],iWidthPrevY);
    g_pDetermineThreshold(ppxlcY+(BLOCK_SIZE*iWidthPrevY)+BLOCK_SIZE,&thr[3],&range[3],iWidthPrevY);
    g_pDetermineThreshold(ppxlcU,                                    &thr[4],&range[4],iWidthPrevUV);
    g_pDetermineThreshold(ppxlcV,                                    &thr[5],&range[5],iWidthPrevUV);
    
    k_max = (range[0]     > range[1]) ? 0     : 1;
    k_max = (range[k_max] > range[2]) ? k_max : 2;
    k_max = (range[k_max] > range[3]) ? k_max : 3;
    max_range = range[k_max];
    for (k = 0; k < 4; k++){
        if ((range[k] < 32) && (max_range >= 64)) thr[k] = thr[k_max];
        if (max_range < 16) thr[k] = 0;
    }

    g_pApplySmoothing(ppxlcY                                    ,iWidthPrevY ,iStepSize*2,thr[0]);
    g_pApplySmoothing(ppxlcY+BLOCK_SIZE                         ,iWidthPrevY ,iStepSize*2,thr[1]);
    g_pApplySmoothing(ppxlcY+(BLOCK_SIZE*iWidthPrevY)           ,iWidthPrevY ,iStepSize*2,thr[2]);
    g_pApplySmoothing(ppxlcY+BLOCK_SIZE+(BLOCK_SIZE*iWidthPrevY),iWidthPrevY ,iStepSize*2,thr[3]);
    g_pApplySmoothing(ppxlcU                                    ,iWidthPrevUV,iStepSize*2,thr[4]);
    g_pApplySmoothing(ppxlcV                                    ,iWidthPrevUV,iStepSize*2,thr[5]);
}

//#endif // _SUPPORT_POST_FILTERS_

