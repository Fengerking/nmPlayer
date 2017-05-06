//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

  Copyright (c) 2001  Microsoft Corporation
  
    Module Name:
    
    interpolate.cpp
      
    Abstract:
        
    Multirate functions : downsampling, interpolation (half and
    quarter pel bicubic filters), multires motion estimation

    Author:

    Sridhar Srinivasan (sridhsri@microsoft.com) 19-February-2001

    Revision:

    Chuang Gu (chuanggu@microsoft.com) 1-Oct-2002: reorganize code 

    Revision History:
                  
*************************************************************************/
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#ifdef macintosh
#   include "cpudetect_mac.h"
#   include "motioncomp_altivec.h"
#endif
#include "tables_wmv.h"

#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h"
#include "postfilter_wmv.h"

#include "spatialpredictor_wmv.h"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"

#include <math.h>

extern const I16_WMV gaSubsampleBilinear[];
extern const I16_WMV gaSubsampleBicubic[];
extern const I16_WMV gaSubsampleLong[];

/***********************************************************************
C version of interpolation routines
***********************************************************************/
#if defined( _DEBUG ) || defined(DEBUG)
#   define DEBUG_STATEMENT(x)   x
#else
#   define DEBUG_STATEMENT(x)
#endif

//Please use these variables to verify the correctness of the implementation of the interpolate code. Note wmvdiff alone is NOT enough here.
//Generally, overflow counters should be much smaller than the corresponding non-overflow counts.

DEBUG_STATEMENT(int g_AddErrOverflow_cnt;)
DEBUG_STATEMENT(int g_AddErr_cnt;)
DEBUG_STATEMENT(int g_BiCubic_0x_cnt;)
DEBUG_STATEMENT(int g_BiCubic_0x_Overflow_cnt;)
DEBUG_STATEMENT(int g_BiCubic_x0_cnt;)
DEBUG_STATEMENT(int g_BiCubic_x0_Overflow_cnt;)
DEBUG_STATEMENT(int g_BiCubic_xx_cnt;)
DEBUG_STATEMENT(int g_BiCubic_xx_Overflow_cnt;)
DEBUG_STATEMENT(int g_BiCubic_all_Overflow_cnt;)


/* This flag controls to use the separation of x(1,3) and 2 in the g_NewHorzFilterX_Fun(), as  case 2 can replace the multiply with simple add/shift. However, for ARM, couting the number instructions show that for H filter it is more favorable to 
  use the original X. Therefore I am not defining it here. For V filter, due to the register pressure of the X version, I prefer this flag to be defined.

  For platforms with multiple cycle  multiplier, it is definitely advantegeous to define them both.
  */

//#define VUSE_2ANDx
//#define HUSE_2ANDx
#ifdef _EMB_WMV3_
#ifdef _EMB_SSIMD32_

static U32_WMV test_err_cliping(U32_WMV u);
static Void_WMV test_err_cliping2(U32_WMV *pt0, U32_WMV *pt1);

/******************************************************************************************
*
*  Here starts the section of functions to be written in assembly.
*
*
*******************************************************************************************
*/

// Here starts the so called "Group A" routines to be written in assembly in conjuction with the "group B" routines.
// Note these routines will be called in-loop by the group B routines which are thin wrappers. So it is best to do all the register save/restore 
//in the group B routines outside of the loop. These routines here should only perform a simple jump/return.

#ifdef WMV_OPT_MOTIONCOMP_ARM
    extern void g_Prefetch(const U8_WMV * pSrc, I32_WMV iWidth);
#   define G_PREFETCH(x,y)   // g_Prefetch(x + 32,y);
#else
#   define G_PREFETCH(x,y)
#endif

#if !defined(WMV_OPT_MOTIONCOMP_ARM) && !defined(WMV_OPT_MOTIONCOMP_SHX)

/*
I8_WMV  g_NewVertFilterStepTbl[] =
{
    1, 20, 3, -16, 1, 20, 3, -16, 1, 20, 3, -16,1, 20, 3, -16, 1, 20, 3, -16
};
*/
#if !defined(_MIPS_ASM_MOTIONCOMP_OPT_)

#if !defined(VUSE_2ANDx)
//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewVertFilterX(const U8_WMV  *pSrc,
                         const I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         const I32_WMV iShift, 
                         const I32_WMV iRound32, 
                         const I8_WMV * const pV, 
                         I32_WMV iNumHorzLoop, 
                         const U32_WMV uiMask,
                         Bool_WMV b1MV, 
                         U16_WMV * pTbl
                        )
{
    
    I32_WMV k;
    U32_WMV overflow = 0;
    I16_WMV v0, v1, v2, v3;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilterX);
    
    v0 = pV[0];
    v1 = pV[1];
    v2 = pV[2];
    v3 = pV[3];         DEBUG_CACHE_READ_BYTES(pV,4);
    
    
    for(k = 0; k < (iNumHorzLoop<<1); k++)
    {
        
        register U32_WMV t0;
        register U32_WMV o1, o2, o3;
        I32_WMV m;
   
#ifdef _BIG_ENDIAN_
        o1 = pSrc[2] | (pSrc[0]<<16);
#else     
        o1 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
        pSrc += iSrcStride;
        
#ifdef _BIG_ENDIAN_
        o2 = pSrc[2] | (pSrc[0]<<16);
#else
        o2 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
        pSrc += iSrcStride;
        
#ifdef _BIG_ENDIAN_
        o3 = pSrc[2] | (pSrc[0]<<16);
#else
        o3 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
        pSrc += iSrcStride;
        
        for(m = 0; m < iNumInnerLoop; m++)
        {
            t0 = o1*v0 + iRound32;
            t0 += o2*v1;
            t0 += o3*v2;
            o1 = o2;
            o2 = o3;
#ifdef _BIG_ENDIAN_
            o3 = pSrc[2] | (pSrc[0]<<16);
#else
            o3 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
            t0 += o3*v3;
            
            //t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 ;
            overflow |= t0;
            t0 >>= iShift;
            t0 &= uiMask;
            
            *(U32_WMV *)pDst = t0;              DEBUG_CACHE_WRITE_BYTES(pDst,4);
            pDst += 40;
            pSrc += iSrcStride;
        }
        

        pSrc -= pTbl[0 + ((k&1)<<1)] ;
        pDst -= pTbl[1 + ((k&1)<<1)];
        
        
    }
    
    return overflow;
    
}

#else // !defined(VUSE_2ANDx)

//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewVertFilterx(const U8_WMV  *pSrc,
                         const I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         const I32_WMV iShift, 
                         const I32_WMV iRound32, 
                         const I8_WMV * const pV, 
                         I32_WMV iNumHorzLoop, 
                         const U32_WMV uiMask,
                         Bool_WMV b1MV, 
                         U16_WMV * pTbl
                        )
{
    
    I32_WMV k;
    U32_WMV overflow = 0;
    I16_WMV v0, v1, v2, v3;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilterX);
    
    v0 = pV[0];
    v1 = pV[1];
    v2 = pV[2];
    v3 = pV[3];
    
    
    for(k = 0; k < (iNumHorzLoop<<1); k++)
    {
        
        register U32_WMV t0;
        register U32_WMV o1, o2, o3;
        I32_WMV m;
   
#ifdef _BIG_ENDIAN_
        o1 = pSrc[2] | (pSrc[0]<<16);
#else     
        o1 = pSrc[0] | (pSrc[2]<<16);
#endif
        pSrc += iSrcStride;
        
#ifdef _BIG_ENDIAN_
        o2 = pSrc[2] | (pSrc[0]<<16);
#else
        o2 = pSrc[0] | (pSrc[2]<<16);
#endif
        pSrc += iSrcStride;
        
#ifdef _BIG_ENDIAN_
        o3 = pSrc[2] | (pSrc[0]<<16);
#else
        o3 = pSrc[0] | (pSrc[2]<<16);
#endif
        pSrc += iSrcStride;
        
        for(m = 0; m < iNumInnerLoop; m++)
        {
            t0 = o1*v0 + iRound32;
            t0 += o2*v1;
            t0 += o3*v2;
            o1 = o2;
            o2 = o3;
#ifdef _BIG_ENDIAN_
            o3 = pSrc[2] | (pSrc[0]<<16);
#else
            o3 = pSrc[0] | (pSrc[2]<<16);
#endif
            t0 += o3*v3;
            
            //t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 ;
            overflow |= t0;
            t0 >>= iShift;
            t0 &= uiMask;
            
            *(U32_WMV *)pDst = t0;
            pDst += 40;
            pSrc += iSrcStride;
        }
        

        pSrc -= pTbl[0 + ((k&1)<<1)] ;
        pDst -= pTbl[1 + ((k&1)<<1)];
        
        
    }
    
    return overflow;
    
}



//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewVertFilter2(const U8_WMV  *pSrc,
                         const I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         const I32_WMV iShift, 
                         const I32_WMV iRound32, 
                         I32_WMV iNumHorzLoop, 
                         const U32_WMV uiMask,
                         Bool_WMV b1MV, 
                         U16_WMV * pTbl
                        )
{
    
    I32_WMV k;
    U32_WMV overflow = 0;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilterX);
    
    for(k = 0; k < (iNumHorzLoop<<1); k++)
    {
        
        register U32_WMV t0;
        register U32_WMV o1, o2, o3;
        I32_WMV m;
   
#ifdef _BIG_ENDIAN_
        o1 = pSrc[2] | (pSrc[0]<<16);
#else     
        o1 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
        pSrc += iSrcStride;
        
#ifdef _BIG_ENDIAN_
        o2 = pSrc[2] | (pSrc[0]<<16);
#else
        o2 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
        pSrc += iSrcStride;
        
#ifdef _BIG_ENDIAN_
        o3 = pSrc[2] | (pSrc[0]<<16);
#else
        o3 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
        pSrc += iSrcStride;
        
        for(m = 0; m < iNumInnerLoop; m++)
        {
            t0 = o2 + o3;
            t0 = (t0 << 3) + t0;
            t0 = t0 - o1;
            o1 = o2;
            o2 = o3;
#ifdef _BIG_ENDIAN_
            o3 = pSrc[2] | (pSrc[0]<<16);
#else
            o3 = pSrc[0] | (pSrc[2]<<16);       DEBUG_CACHE_READ_BYTES(pSrc,4);
#endif
           // t0 += o3*v3;

            t0 = t0 + iRound32;
            t0 = t0 - o3;
            
            //t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 ;
            overflow |= t0;
            t0 >>= iShift;
            t0 &= uiMask;
            
            *(U32_WMV *)pDst = t0;              DEBUG_CACHE_WRITE_BYTES(pDst,4);
            pDst += 40;
            pSrc += iSrcStride;
        }
        

        pSrc -= pTbl[0 + ((k&1)<<1)] ;
        pDst -= pTbl[1 + ((k&1)<<1)];
        
        
    }
    
    return overflow;
    
}

#endif // !defined(VUSE_2ANDx)


//#pragma code_seg (EMBSEC_PML)
Void_WMV g_NewVertFilter0Long(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV)
{
    
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilter0Long);
    
    if(b1MV == 0)
    {
        for(i = 0; i < 8; i++)
        {
#ifdef _BIG_ENDIAN_
            *(I32_WMV *)(pDst + 0)       = pSrc[0+2 ]|(pSrc[0  ]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+3 ]|(pSrc[0+1]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4+2 ]|(pSrc[4  ]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+3 ]|(pSrc[4+1]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8+2 ]|(pSrc[8  ]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+3 ]|(pSrc[8+1]<<16);
#else
            *(I32_WMV *)(pDst + 0)       = pSrc[0   ]|(pSrc[0+2]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ]|(pSrc[8+3]<<16); 
#endif

            DEBUG_CACHE_READ_BYTES(pSrc,12);
            DEBUG_CACHE_WRITE_BYTES(pDst,12);
            DEBUG_CACHE_WRITE_BYTES(pDst+20,12);

            pSrc += iSrcStride;
            pDst += 40;
        }
    }
    else
    {
        
        for(i = 0; i < 16; i++)
        {
#ifdef _BIG_ENDIAN_
            *(I32_WMV *)(pDst + 0)       = pSrc[0+2 ]|(pSrc[0  ]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+3 ]|(pSrc[0+1]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4+2 ]|(pSrc[4  ]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+3 ]|(pSrc[4+1]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8+2 ]|(pSrc[8  ]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+3 ]|(pSrc[8+1]<<16);
            *(I32_WMV *)(pDst + 12)      = pSrc[12+2]|(pSrc[12 ]<<16);
            *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+3]|(pSrc[12+1]<<16);
            *(I32_WMV *)(pDst + 16)      = pSrc[16+2]|(pSrc[16  ]<<16);
            *(I32_WMV *)(pDst + 16 + 20) = pSrc[16+3]|(pSrc[16+1]<<16);
#else
            *(I32_WMV *)(pDst + 0)       = pSrc[0   ]|(pSrc[0+2]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ]|(pSrc[8+3]<<16);
            *(I32_WMV *)(pDst + 12)      = pSrc[12  ]|(pSrc[12+2]<<16);
            *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+1]|(pSrc[12+3]<<16);
            *(I32_WMV *)(pDst + 16)      = pSrc[16  ]|(pSrc[16+2]<<16);
            *(I32_WMV *)(pDst + 16 + 20) = pSrc[16+1]|(pSrc[16+3]<<16);
#endif
            
            DEBUG_CACHE_READ_BYTES(pSrc,12);
            DEBUG_CACHE_WRITE_BYTES(pDst,40);

            pSrc += iSrcStride;
            pDst += 40;
        }
    }
}


#if !defined(HUSE_2ANDx)
//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewHorzFilterX(U8_WMV *pF, const I32_WMV iShift, const I32_WMV iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV)
{
    I32_WMV j, i;
    U32_WMV overflow = 0;
    register U32_WMV t0, t1; //, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    const I16_WMV h0 = pH[0];
    const I16_WMV h1 = pH[1];
    const I16_WMV h2 = pH[2];
    const I16_WMV h3 = pH[3];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewHorzFilterX);
    DEBUG_CACHE_READ_BYTES(pH,4);

    for (j = 0; j < iNumLoops; j += 4) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            register I32_WMV o0;
            register I32_WMV o1;
         
            t0 = *(I32_WMV *)pF;
          
            o0 = iRound2_32;
            o1 = iRound2_32;
            //t0 = *(I32_WMV *)pF;
            t1 = *(I32_WMV *)(pF + 20); 
            o0 += t0 * h0;
            
            o0 += t1 * h1;
            o1 += t1 * h0;
            t1 = (*(U32_WMV *)(pF+4));
            
#ifdef _BIG_ENDIAN_
            t1 = (t0<<16) | (t1>>16);
#else
            t0 = t0>>16;
            t1 = t0 | (t1<<16);
#endif
            //  t0 = *(U16_WMV *)(pF+2) |  (*(U16_WMV *)(pF+4)) <<16;
            o0 += t1 * h2;
            o1 += t1 * h1;
            t0 = *(U16_WMV *)(pF + 20 + 2);
            // t0 = *(U16_WMV *)(pF + 12 + 2) |  (*(U16_WMV *)(pF + 12 +4)) <<16;
            t1 = (*(U32_WMV *)(pF+20+4));               DEBUG_CACHE_READ_BYTES(pF,24);
#ifdef _BIG_ENDIAN_            
            t1 = (t1>>16) | (t0<<16);
#else
            t1 = t0 | (t1<<16);
#endif
            o0 += t1 * h3;
            o1 += t1 * h2;            
            t0 = *(I32_WMV *)(pF+4);
            o1 += t0 * h3;
            
            //o0 = t0 * h0  + t1 * h1 + t2 * h2 + t3 * h3 + iRound2_32;
            //o1 = t1 * h0  + t2 * h1 + t3 * h2 + t4 * h3 + iRound2_32;
            
            overflow |= o0;
            overflow |= o1;
            o0 >>= iShift;
            o1 >>= iShift;
            o0 &= 0x00ff00ff;           
            o1 &= 0x00ff00ff;
            
            *(U32_WMV *)pF = o0;
            *(U32_WMV *)(pF+20) = o1;                   DEBUG_CACHE_WRITE_BYTES(pF,24);
            pF += 40;
        }
        pF += -(40<<(3+b1MV)) + 4;
    }
    
    return overflow;
}

#else // !defined(HUSE_2ANDx)

//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewHorzFilterx(U8_WMV *pF, const I32_WMV iShift, const I32_WMV iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV)
{
    I32_WMV j, i;
    U32_WMV overflow = 0;
    register U32_WMV t0, t1; //, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    const I16_WMV h0 = pH[0];
    const I16_WMV h1 = pH[1];
    const I16_WMV h2 = pH[2];
    const I16_WMV h3 = pH[3];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewHorzFilterx);


    for (j = 0; j < iNumLoops; j += 4) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            register I32_WMV o0;
            register I32_WMV o1;
         
            t0 = *(I32_WMV *)pF;
          
            o0 = iRound2_32;
            o1 = iRound2_32;
            //t0 = *(I32_WMV *)pF;
            t1 = *(I32_WMV *)(pF + 20); 
            o0 += t0 * h0;
            
            o0 += t1 * h1;
            o1 += t1 * h0;
            t1 = (*(U32_WMV *)(pF+4));
            
#ifdef _BIG_ENDIAN_
            t1 = (t0<<16) | (t1>>16);
#else
            t0 = t0>>16;
            t1 = t0 | (t1<<16);
#endif
            //  t0 = *(U16_WMV *)(pF+2) |  (*(U16_WMV *)(pF+4)) <<16;
            o0 += t1 * h2;
            o1 += t1 * h1;
            t0 = *(U16_WMV *)(pF + 20 + 2);
            // t0 = *(U16_WMV *)(pF + 12 + 2) |  (*(U16_WMV *)(pF + 12 +4)) <<16;
            t1 = (*(U32_WMV *)(pF+20+4));
#ifdef _BIG_ENDIAN_            
            t1 = (t1>>16) | (t0<<16);
#else
            t1 = t0 | (t1<<16);
#endif
            o0 += t1 * h3;
            o1 += t1 * h2;            
            t0 = *(I32_WMV *)(pF+4);
            o1 += t0 * h3;
            
            //o0 = t0 * h0  + t1 * h1 + t2 * h2 + t3 * h3 + iRound2_32;
            //o1 = t1 * h0  + t2 * h1 + t3 * h2 + t4 * h3 + iRound2_32;
            
            overflow |= o0;
            overflow |= o1;
            o0 >>= iShift;
            o1 >>= iShift;
            o0 &= 0x00ff00ff;           
            o1 &= 0x00ff00ff;
            
            *(U32_WMV *)pF = o0;
            *(U32_WMV *)(pF+20) = o1;
            pF += 40;
        }
        pF += -(40<<(3+b1MV)) + 4;
    }
    
    return overflow;
}

#endif //!defined(_MIPS_ASM_MOTIONCOMP_OPT_)

//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewHorzFilter2(U8_WMV *pF, const I32_WMV iShift, const I32_WMV iRound2_32, Bool_WMV b1MV)
{
    I32_WMV j, i;
    U32_WMV overflow = 0;
    register U32_WMV t0, t1; //, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewHorzFilter2);

    for (j = 0; j < iNumLoops; j += 4) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            register I32_WMV o0;
            register I32_WMV o1;
            I32_WMV h1;
         
            t0 = *(I32_WMV *)pF;
          
            o0 = iRound2_32;
            o1 = iRound2_32;
            //t0 = *(I32_WMV *)pF;
            t1 = *(I32_WMV *)(pF + 20); 

         //   o0 += t0 * h0;
            o0 = iRound2_32 - t0;
            
        //    o0 += t1 * h1;

         //   o1 += t1 * h0;
            o1 = iRound2_32 - t1;

            h1 = (*(U32_WMV *)(pF+4));
            
#ifdef _BIG_ENDIAN_
            h1 = (t0<<16) | (h1>>16);
#else
            t0 = t0>>16;
            h1 = t0 | (h1<<16);
#endif
            t0 = h1 + t1;
            t0 = (t0 <<3) + t0;
            o0 = o0 + t0;

            t0 = *(U16_WMV *)(pF + 20 + 2);
            t1 = (*(U32_WMV *)(pF+20+4));
#ifdef _BIG_ENDIAN_            
            t1 = (t1>>16) | (t0<<16);
#else
            t1 = t0 | (t1<<16);
#endif
            h1 = t1 + h1;
            h1 = (h1 <<3) + h1;
            o1 = o1 + h1;

            o0 = o0 - t1;

            t0 = *(I32_WMV *)(pF+4);
            o1 = o1 - t0;
            
            overflow |= o0;
            overflow |= o1;
            o0 >>= iShift;
            o1 >>= iShift;
            o0 &= 0x00ff00ff;           
            o1 &= 0x00ff00ff;
            
            *(U32_WMV *)pF = o0;
            *(U32_WMV *)(pF+20) = o1;
            pF += 40;
        }
        pF += -(40<<(3+b1MV)) + 4;
    }
    
    return overflow;
    
}

#endif // !defined(HUSE_2ANDx)

#endif // !defined(WMV_OPT_MOTIONCOMP_ARM) && !defined(WMV_OPT_MOTIONCOMP_SHX)

#endif // _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewVertFilterX_C(const U8_WMV  *pSrc,
                         const I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         const I32_WMV iShift, 
                         const I32_WMV iRound32, 
                         const I8_WMV * const pV, 
                         I32_WMV iNumHorzLoop, 
                         Bool_WMV b1MV 
                        )
{
    
    I16_WMV k, m;
    I16_WMV v0, v1, v2, v3;
    I16_WMV o1, o2, o3;
    I16_WMV t0;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);
    U8_WMV *pIn, *pOut;
    I16_WMV iRound16 = (I16_WMV)iRound32;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilterX_C);
    
    v0 = pV[0];
    v1 = pV[1];
    v2 = pV[2];
    v3 = pV[3];                         DEBUG_CACHE_READ_BYTES(pV,4);
    
    for(k = 0; k < (iNumHorzLoop<<2); k++)
    {
        pIn = (U8_WMV*)pSrc++;
        pOut = pDst + 2*k;

        o1 = *pIn;                      DEBUG_CACHE_READ_BYTES(pIn,1);
        pIn += iSrcStride;
        
        o2 = *pIn;                      DEBUG_CACHE_READ_BYTES(pIn,1);
        pIn += iSrcStride;
        
        o3 = *pIn;                      DEBUG_CACHE_READ_BYTES(pIn,1);
        pIn += iSrcStride;
        
        for(m = 0; m < iNumInnerLoop; m++)
        {
            t0 = o1*v0 + iRound16;
            t0 += o2*v1;
            t0 += o3*v2;
            o1 = o2;
            o2 = o3;
            o3 = *pIn;                  DEBUG_CACHE_READ_BYTES(pIn,1);
            t0 += o3*v3;
            
            //t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 ;
            t0 >>= iShift;

            *(U16_WMV *)pOut = t0;      DEBUG_CACHE_WRITE_BYTES(pOut,1);
            pOut += 40;
            pIn += iSrcStride;
        }
    }
    
    return 0;
}


//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewVertFilterX_C_Clip(const U8_WMV  *pSrc,
                         const I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         const I32_WMV iShift, 
                         const I32_WMV iRound32, 
                         const I8_WMV * const pV, 
                         I32_WMV iNumHorzLoop, 
                         Bool_WMV b1MV 
                        )
{
    
    I16_WMV k, m;
    I16_WMV v0, v1, v2, v3;
    I16_WMV o1, o2, o3;
    I16_WMV t0;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);
    U8_WMV *pIn, *pOut;
    I16_WMV iRound16 = (I16_WMV)iRound32;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilterX_C);
   
    v0 = pV[0];
    v1 = pV[1];
    v2 = pV[2];
    v3 = pV[3];                         DEBUG_CACHE_READ_BYTES(pV,4);
    
    
    for(k = 0; k < (iNumHorzLoop<<2); k++)
    {
        pIn = (U8_WMV*)pSrc++;
        pOut = pDst + 2*k;

        o1 = *pIn;
        pIn += iSrcStride;                      DEBUG_CACHE_READ_BYTES(pIn,1);
        
        o2 = *pIn;
        pIn += iSrcStride;                      DEBUG_CACHE_READ_BYTES(pIn,1);
        
        o3 = *pIn;
        pIn += iSrcStride;                      DEBUG_CACHE_READ_BYTES(pIn,1);
        
        for(m = 0; m < iNumInnerLoop; m++)
        {
            t0 = o1*v0 + iRound16;
            t0 += o2*v1;
            t0 += o3*v2;
            o1 = o2;
            o2 = o3;
            o3 = *pIn;                          DEBUG_CACHE_READ_BYTES(pIn,1);
            t0 += o3*v3;
            
            //t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 ;
            t0 >>= iShift;

            if ( t0 & 0xFF00)
            {
                if (t0 < 0) t0 = 0;
                else if (t0 > 255) t0 = 255;
            }

            *(U16_WMV *)pOut = t0;              DEBUG_CACHE_WRITE_BYTES(pOut,1);
            pOut += 40;
            pIn += iSrcStride;
        }
    }
    
    return 0;
}


//#pragma code_seg (EMBSEC_PML)
Void_WMV g_NewVertFilter0Long_C(const U8_WMV  *pSrc,  I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV)
{
    
    I32_WMV i, j;
    U8_WMV* pOut;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewVertFilter0Long_C);
    
    if(b1MV == 0)
    {
        for(i = 0; i < 8; i++)
        {
            pOut = pDst;
            for (j = 0; j < 11; j++)
            {
                *(I16_WMV *)pOut = pSrc[j];
                pOut += 2;
            }
            DEBUG_CACHE_READ_BYTES(pSrc,11);
            DEBUG_CACHE_WRITE_BYTES(pOut-22,22);
            pSrc += iSrcStride;
            pDst += 40;
        }
    }
    else
    {
        for(i = 0; i < 16; i++)
        {
            pOut = pDst;
            for (j = 0; j < 19; j++)
            {
                *(I16_WMV *)pOut = pSrc[j];
                pOut += 2;
            }
            DEBUG_CACHE_READ_BYTES(pSrc,19);
            DEBUG_CACHE_WRITE_BYTES(pOut-38,38);
            pSrc += iSrcStride;
            pDst += 40;
        }
    }
}


//#pragma code_seg (EMBSEC_PML)
U_SIMD g_NewHorzFilterX_C(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV)
{
    I32_WMV j, i;
    I32_WMV t0, t1, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    U8_WMV* pOut;
    I16_WMV iRound16 = (I16_WMV)iRound2_32;
    
    const I16_WMV h0 = pH[0];
    const I16_WMV h1 = pH[1];
    const I16_WMV h2 = pH[2];
    const I16_WMV h3 = pH[3];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_NewHorzFilterX_C);
    DEBUG_CACHE_READ_BYTES(pH,4);

    for (j = 0; j < iNumLoops; j++) 
    {
        pOut = pF + 2*j;
        for(i = 0; i < iNumLoops; i++)
        {
            t0 = *(I16_WMV *)pOut;
            t1 = *(I16_WMV *)(pOut + 2); 
            t2 = *(I16_WMV *)(pOut + 4);
            t3 = *(I16_WMV *)(pOut + 6);    DEBUG_CACHE_READ_BYTES(pOut,8);

            t0 = t0 * h0;
            t0 += t1 * h1;

            t2 = t2 * h2;
            t2 += t3 * h3;

            //t0 = t0 * h0  + t1 * h1 + t2 * h2 + t3 * h3 + iRound2_32;
            t0 += iRound16;
            t0 += t2;
            
            t0 >>= iShift;
            if ( t0 & 0xFFFFFF00)
            {
                if (t0 < 0) t0 = 0;
                else if (t0 > 255) t0 = 255;
            }
            
            *(U16_WMV *)pOut = t0;          DEBUG_CACHE_WRITE_BYTES(pOut,1);
            pOut += 40;
        }
    }
    
    return 0;
}


#ifdef _EMB_SSIMD32_

DEBUG_STATEMENT(int g_AddErrBOverflow_cnt;)
//#pragma code_seg (EMBSEC_DEF)
Void_WMV g_AddErrorB_EMB_Overflow(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, I32_WMV iOffset)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1, y0,v0,v1;
    U32_WMV err_even, err_odd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddErrorB_EMB_Overflow);
    
    DEBUG_STATEMENT(g_AddErrBOverflow_cnt++;)
        
    pRef0 += iOffset;
    pRef1 += iOffset;

        for (iy = 0; iy < 16; iy++) 
        {
            
            
            u0 = pRef0[0];
            u1 = pRef0[0 + 5];
            v0 = pRef1[0];
            v1 = pRef1[0 + 5];
            
            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 16];

            u0 = (u0 + v0 + 0x00010001) >>1;
            u1 = (u1 + v1 + 0x00010001) >>1;

            u0 = u0 & ~0x8000;
            u1 = u1 & ~0x8000;
            
            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow  |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow  |= u1;
            
            
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping2(&u0,&u1);
            }
            
            pErrorBuf ++;
            pRef0 += (iy&1)*8 + 1;
            pRef1 += (iy&1)*8 + 1;
            
#ifdef _BIG_ENDIAN_            
            y0 = (u1) | ((u0) << 8);
#else
            y0 = (u0) | ((u1) << 8);
#endif
            
            
            *(U32_WMV *)ppxlcDst = y0;
            
            ppxlcDst += 4 + (iy&1)*(iPitch -8);
            
        }
        
        
}


#if !defined( WMV_OPT_MOTIONCOMP_ARM) 

//#pragma code_seg (EMBSEC_BML)


Void_WMV g_AddNullB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV u0,u1,u2,u3, y0,y1,v0,v1,v2,v3;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddNullB_SSIMD);
    
    pRef0 += iOffset;
    pRef1 += iOffset;

    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        
        u0 = pRef0[0];
        u1 = pRef0[0 + 5];
        u2 = pRef0[1];
        u3 = pRef0[1 + 5];

        v0 = pRef1[0];
        v1 = pRef1[0 + 5];
        v2 = pRef1[1];
        v3 = pRef1[1 + 5];

        u0 = (u0 + v0 + 0x00010001) >>1;
        u1 = (u1 + v1 + 0x00010001) >>1;
        u0 = u0 & ~0x8000;
        u1 = u1 & ~0x8000;

        u2 = (u2 + v2 + 0x00010001) >>1;
        u3 = (u3 + v3 + 0x00010001) >>1;
        u2 = u2 & ~0x8000;
        u3 = u3 & ~0x8000;
        
        DEBUG_CACHE_READ_BYTES(pRef0,6);
        DEBUG_CACHE_READ_BYTES(pRef1,6);

        pRef0 += 10;
        pRef1 += 10;
        
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
        y1 = (u3) | ((u2) << 8);
#else
        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);
#endif
        
        *(U32_WMV *)ppxlcDst = y0;
        *(U32_WMV *)(ppxlcDst + 4)= y1;
        
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,8);

        ppxlcDst += iPitch;
    }
    
}

#endif // !defined( WMV_OPT_MOTIONCOMP_ARM) 

#if !defined( WMV_OPT_MOTIONCOMP_ARM) 

//#pragma code_seg (EMBSEC_BML)
Void_WMV g_AddErrorB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1,u2,u3, y0,y1, v0,v1,v2,v3;
    U32_WMV err_even, err_odd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddErrorB_SSIMD);

    pRef0 += iOffset;
    pRef1 += iOffset;
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        
        u0 = pRef0[0];
        u1 = pRef0[0 + 5];

        v0 = pRef1[0];
        v1 = pRef1[0 + 5];
        
        err_even = pErrorBuf[0];
        err_odd  = pErrorBuf[0 + 16];

        u0 = (u0 + v0 + 0x00010001) >>1;
        u1 = (u1 + v1 + 0x00010001) >>1;

        u0 = u0 & ~0x8000;
        u1 = u1 & ~0x8000;
        
        u0 = u0 + err_even-((err_even & 0x8000) << 1);
        err_overflow  |= u0;
        u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
        err_overflow  |= u1;
        
        err_even = pErrorBuf[1];
        err_odd  = pErrorBuf[1 + 16];

        u2 = pRef0[1];
        u3 = pRef0[1 + 5];
        v2 = pRef1[1];
        v3 = pRef1[1 + 5];

        u2 = (u2 + v2 + 0x00010001) >>1;
        u3 = (u3 + v3 + 0x00010001) >>1;
        u2 = u2 & ~0x8000;
        u3 = u3 & ~0x8000;
        
        u2 = u2 + err_even-((err_even & 0x8000) << 1);
        err_overflow  |= u2;
        u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
        err_overflow  |= u3;
        
        DEBUG_CACHE_READ_BYTES(pRef0,6);
        DEBUG_CACHE_READ_BYTES(pRef1,6);
        DEBUG_CACHE_READ_BYTES(pErrorBuf,24);
        
        pErrorBuf += 2;
        pRef0 += 10;
        pRef1 += 10;
        
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
        y1 = (u3) | ((u2) << 8);
#else    
        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);
#endif
        
        *(U32_WMV *)ppxlcDst = y0;
        *(U32_WMV *)(ppxlcDst + 4)= y1;
        
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,8);

        ppxlcDst += iPitch;
        
    }
    
    DEBUG_STATEMENT(g_AddErr_cnt++;)
        
    if(err_overflow & 0xff00ff00) {
        ppxlcDst -= 8*iPitch;
        pRef0 -= 10*8+iOffset;
        pRef1 -= 10*8+iOffset;
        pErrorBuf -= 16;
        g_AddErrorB_EMB_Overflow(ppxlcDst, pRef0 , pRef1,  pErrorBuf, iPitch, iOffset);
    }
        
}

#endif // !defined( WMV_OPT_MOTIONCOMP_ARM) 
//Here starts the so called "Group B" routines. These routines should be written in assembly conjunction with the group A routines. See the notes before that.


//#pragma code_seg (EMBSEC_DEF)
Void_WMV g_AddError_EMB_Overflow(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1, y0;
    U32_WMV err_even, err_odd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddError_EMB_Overflow);
    DEBUG_STATEMENT(g_AddErrOverflow_cnt++;)
        
        for (iy = 0; iy < 16; iy++) 
        {
            
            
            u0 = pRef[0];
            u1 = pRef[0 + 5];
            
            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 16];
            
            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow  |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow  |= u1;
            
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping2(&u0,&u1);
            }

            pErrorBuf ++;
            pRef += (iy&1)*8 + 1;
            
#ifdef _BIG_ENDIAN_            
            y0 = (u1) | ((u0) << 8);
#else
            y0 = (u0) | ((u1) << 8);
#endif
            
            
            *(U32_WMV *)ppxlcDst = y0;
            
            ppxlcDst += 4 + (iy&1)*(iPitch -8);
            
        }
        
        
}

#if !defined( WMV_OPT_MOTIONCOMP_ARM) && !defined(WMV_OPT_MOTIONCOMP_SHX)

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV u0,u1,u2,u3, y0,y1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddNull_SSIMD);
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        
        u0 = pRef[0];
        u1 = pRef[0 + 5];
        u2 = pRef[1];
        u3 = pRef[1 + 5];
        
        DEBUG_CACHE_WRITE_BYTES(pRef,6);

        pRef += 10;
        
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
        y1 = (u3) | ((u2) << 8);
#else
        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);
#endif
        
        *(U32_WMV *)ppxlcDst = y0;
        *(U32_WMV *)(ppxlcDst + 4)= y1;
        
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,8);

        ppxlcDst += iPitch;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1,u2,u3, y0,y1;
    U32_WMV err_even, err_odd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddError_SSIMD);
    DEBUG_CACHE_READ_BYTES(pErrorBuf,32*4);
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) {
        
        u0 = pRef[0];
        u1 = pRef[0 + 5];
        
        err_even = pErrorBuf[0];
        err_odd  = pErrorBuf[0 + 16];
        
        u0 = u0 + err_even-((err_even & 0x8000) << 1);
        err_overflow  |= u0;
        u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
        err_overflow  |= u1;
        
        err_even = pErrorBuf[1];
        err_odd  = pErrorBuf[1 + 16];
        
        u2 = pRef[1];
        u3 = pRef[1 + 5];
        
        u2 = u2 + err_even-((err_even & 0x8000) << 1);
        err_overflow  |= u2;
        u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
        err_overflow  |= u3;
        
        DEBUG_CACHE_READ_BYTES(pRef,6);

        pErrorBuf += 2;
        pRef += 10;
        
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
        y1 = (u3) | ((u2) << 8);
#else    
        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);
#endif
        
        *(U32_WMV *)ppxlcDst = y0;
        *(U32_WMV *)(ppxlcDst + 4)= y1;

        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,8);
        
        ppxlcDst += iPitch;
        
    }
    
    DEBUG_STATEMENT(g_AddErr_cnt++;)
        
    if(err_overflow & 0xff00ff00) {

        ppxlcDst -= 8*iPitch;
        pRef -= 10*8;
        pErrorBuf -= 16;
        g_AddError_EMB_Overflow(ppxlcDst, pRef , pErrorBuf, iPitch);
    }
        
}

Void_WMV  g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
    //iXFrac == 2, iYFrac == 2.
	I32_WMV i, j, k0,k1;
	I32_WMV iNumLoops = 8<<b1MV;

	iRndCtrl = 8 - ( iRndCtrl&0xff);

    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
		I16_WMV PF0, PF1, PF2, PF3, PF4;
             
        PF0 = pSrc[0] + pSrc[iSrcStride];
        
        for (j = 0; j < iNumLoops; j += 4) {

            pT = pSrc + j;
			PF1 = pT[1] + pT[iSrcStride+1];
			PF2 = pT[2] + pT[iSrcStride+2];
			PF3 = pT[3] + pT[iSrcStride+3];
			PF4 = pT[4] + pT[iSrcStride+4];

            k0 = (((PF0 + PF1) << 2) + iRndCtrl) >> 4;
            k1 = (((PF1 + PF2) << 2) + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

            k0 = (((PF2 + PF3) << 2) + iRndCtrl) >> 4;
            k1 = (((PF3 + PF4) << 2) + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;

			PF0 = PF4;
        }
        
        pSrc += iSrcStride;
        pDst += 40;
    }
}

Void_WMV  g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
    //iXFrac == 2, iYFrac == 0.
    I32_WMV i, j, k0,k1;
    I32_WMV iNumLoops = 8<<b1MV;
    
    iRndCtrl = 8 - ( iRndCtrl&0xff);
    
    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
		I16_WMV PF0, PF1, PF2, PF3, PF4;

        PF0 = pSrc[0];

        for (j = 0; j < iNumLoops; j += 4) {

            pT = pSrc + j;
            PF1 = pT[1];
            PF2 = pT[2];
            PF3 = pT[3];
            PF4 = pT[4];

            k0 = (((PF0 + PF1 ) << 3) + iRndCtrl) >> 4;
            k1 = (((PF1 + PF2 ) << 3) + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

            k0 = (((PF2 + PF3 ) << 3) + iRndCtrl) >> 4;
            k1 = (((PF3 + PF4 ) << 3) + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;

			PF0 = PF4;
        }
        
        pSrc += iSrcStride;
        pDst += 40;
    }
}

Void_WMV  g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
    //iXFrac == 0, iYFrac == 2.
   
    I32_WMV i, j, k0,k1;
    I32_WMV iNumLoops = 8<<b1MV;
    
    iRndCtrl = 8 - ( iRndCtrl&0xff);
    
    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
		I16_WMV PF0, PF1, PF2, PF3;
        
        for (j = 0; j < iNumLoops; j += 4) {

			pT = pSrc + j;
            PF0 = pT[0] + pT[iSrcStride+0];
            PF1 = pT[1] + pT[iSrcStride+1];
            PF2 = pT[2] + pT[iSrcStride+2];
            PF3 = pT[3] + pT[iSrcStride+3];

            k0 = ((PF0 << 3) + iRndCtrl) >> 4;
            k1 = ((PF1 << 3) + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

            k0 = ((PF2 << 3) + iRndCtrl) >> 4;
            k1 = ((PF3 << 3) + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;
        }

        pSrc += iSrcStride;
        pDst += 40;
    }    
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV  g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
#if 1
	I32_WMV i, j, k0,k1;
	I32_WMV iNumLoops = 8<<b1MV;

	iRndCtrl = 8 - ( iRndCtrl&0xff);

    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
		I16_WMV PF0, PF1, PF2, PF3, PF4;
      
        PF0 = (pSrc[0]<<2) + (pSrc[iSrcStride] - pSrc[0]) * iYFrac;
        
        for (j = 0; j < iNumLoops; j += 4) {

            pT = pSrc + j;
			PF1 = (pT[1] << 2) + (pT[iSrcStride+1] - pT[1]) * iYFrac;
			PF2 = (pT[2] << 2) + (pT[iSrcStride+2] - pT[2]) * iYFrac;
			PF3 = (pT[3] << 2) + (pT[iSrcStride+3] - pT[3]) * iYFrac;
			PF4 = (pT[4] << 2) + (pT[iSrcStride+4] - pT[4]) * iYFrac;

            k0 = ((PF0 << 2) + (PF1 - PF0) * iXFrac + iRndCtrl) >> 4;
            k1 = ((PF1 << 2) + (PF2 - PF1) * iXFrac + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;

            k0 = ((PF2 << 2) + (PF3 - PF2) * iXFrac + iRndCtrl) >> 4;
            k1 = ((PF3 << 2) + (PF4 - PF3) * iXFrac + iRndCtrl) >> 4;
            *(I16_WMV *)(pDst + j + 2) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 22) = (U8_WMV) k1;

			PF0 = PF4;
        }
        
        pSrc += iSrcStride;
        pDst += 40;
    }

#else    
    I32_WMV i, j, k0,k1;
    I32_WMV iNumLoops = 8<<b1MV;
    I16_WMV pFilter[16 + 1];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBilinear_SSIMD);
    
    iRndCtrl = 8 - ( iRndCtrl&0xff);
    
    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
        for (j = 0; j < iNumLoops; j+=4) 
        {
            pT = pSrc + j;
            pFilter[j] = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
            pFilter[j+1] = (pT[0+1]<<2)+  ( pT[iSrcStride+1] - pT[0+1])* iYFrac;
            pFilter[j+2] = (pT[0+2]<<2)+  ( pT[iSrcStride+2] - pT[0+2])* iYFrac;
            pFilter[j+3] = (pT[0+3]<<2)+  ( pT[iSrcStride+3] - pT[0+3])* iYFrac;
        }
        
        pT = pSrc + j;
        pFilter[j] = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
        
        for (j = 0; j < iNumLoops; j += 2) {
            k0 = ((pFilter[j] <<2) + (pFilter[1 + j] - pFilter[j]) * iXFrac +  iRndCtrl) >> 4;
            k1 = ((pFilter[j+1] <<2) + (pFilter[1 + j+1] - pFilter[j+1]) * iXFrac + iRndCtrl) >> 4;
            assert(k0>=0);
            assert(k0<256);
            assert(k1>=0);
            assert(k1<256);
            *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 20) = (U8_WMV) k1;
        }

        DEBUG_CACHE_READ_BYTES(pSrc,iNumLoops+4);
        DEBUG_CACHE_READ_BYTES(pSrc+iSrcStride,iNumLoops+4);
        DEBUG_CACHE_READ_BYTES(pSrc+2*iSrcStride,iNumLoops+4);
        DEBUG_CACHE_READ_BYTES(pSrc+3*iSrcStride,iNumLoops+4);
        DEBUG_CACHE_READ_BYTES(pFilter,2*(iNumLoops+4));
        DEBUG_CACHE_WRITE_BYTES(pDst,20+iNumLoops);
        
        pSrc += iSrcStride;
        pDst += 40;
    }
#endif 
}
#endif // !defined( WMV_OPT_MOTIONCOMP_ARM) && !defined(WMV_OPT_MOTIONCOMP_SHX)

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_AddNull_EMB_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
{
    I32_WMV ix, iy;
    U16_WMV* pRef16 = (U16_WMV*)pRef;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddNull_EMB_C);
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        for (ix = 0; ix < BLOCK_SIZE; ix++)
        {
            *(ppxlcDst+ix) = (U8_WMV) pRef16[ix];
        }

        DEBUG_CACHE_WRITE_BYTES(pRef16,BLOCK_SIZE);
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,BLOCK_SIZE*2);

        pRef16 += 20;
        ppxlcDst += iPitch;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_AddError_EMB_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I32_WMV ix, iy;
    U16_WMV u0;
    I16_WMV e0;

    U16_WMV* pRef16 = (U16_WMV*)pRef;
    I16_WMV* pErrorBuf16 = (I16_WMV*)pErrorBuf;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_AddError_EMB_C);
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        for (ix = 0; ix < BLOCK_SIZE; ix++)
        {
            u0 = pRef16[ix];
            e0 = pErrorBuf16[ix];
            e0 += u0;
            if (e0 < 0) e0 = 0;
            else if (e0 > 255) e0 = 255;
            *(ppxlcDst+ix) = (U8_WMV) e0;
        }

        DEBUG_CACHE_READ_BYTES(pRef16,BLOCK_SIZE);
        DEBUG_CACHE_READ_BYTES(pErrorBuf16,BLOCK_SIZE);
        DEBUG_CACHE_WRITE_BYTES(ppxlcDst,BLOCK_SIZE*2);

        pErrorBuf16 += 8;
        pRef16 += 20;
        ppxlcDst += iPitch;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV  g_InterpolateBlockBilinear_EMB_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
    
    I32_WMV i, j, k0,k1;
    I32_WMV iNumLoops = 8<<b1MV;
    I16_WMV pFilter[16 + 1];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBilinear_EMB_C);
    
    iRndCtrl = 8 - ( iRndCtrl&0xff);
    
    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
        for (j = 0; j < iNumLoops; j+=4) 
        {
            pT = pSrc + j;
            pFilter[j]   = (pT[0]<<2)   + ( pT[iSrcStride]   - pT[0]  )* iYFrac;
            pFilter[j+1] = (pT[0+1]<<2) + ( pT[iSrcStride+1] - pT[0+1])* iYFrac;
            pFilter[j+2] = (pT[0+2]<<2) + ( pT[iSrcStride+2] - pT[0+2])* iYFrac;
            pFilter[j+3] = (pT[0+3]<<2) + ( pT[iSrcStride+3] - pT[0+3])* iYFrac;
        }
        
        pT = pSrc + j;
        pFilter[j] = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;
        
        for (j = 0; j < iNumLoops; j += 2) {
            k0 = ((pFilter[j] <<2) + (pFilter[1 + j] - pFilter[j]) * iXFrac +  iRndCtrl) >> 4;
            k1 = ((pFilter[j+1] <<2) + (pFilter[1 + j+1] - pFilter[j+1]) * iXFrac + iRndCtrl) >> 4;
            assert(k0>=0);
            assert(k0<256);
            assert(k1>=0);
            assert(k1<256);
            *(I16_WMV *)(pDst + 2*j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + 2*j + 2) = (U8_WMV) k1;
        }

        DEBUG_CACHE_READ_BYTES(pSrc,iNumLoops+1);
        DEBUG_CACHE_READ_BYTES(pSrc+iSrcStride,iNumLoops+1);
        DEBUG_CACHE_WRITE_BYTES(pFilter,iNumLoops);
        DEBUG_CACHE_READ_BYTES(pFilter,iNumLoops);
        DEBUG_CACHE_WRITE_BYTES(pDst,2*iNumLoops);

        pSrc += iSrcStride;
        pDst += 40;
    }
    
    //dbg_cnt_1++;
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV g_InterpolateBlockBicubicOverflow_EMB(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
    I32_WMV i, j, k;
    const I16_WMV *pH;
    const I16_WMV *pV;
    I16_WMV pFilter[32+3];
    I32_WMV iRound1;
    I32_WMV iRound2;
    I32_WMV iShift1, iShift2;
    I32_WMV jloop, m;
    I32_WMV iNumLoops = 1<<(3+b1MV);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubicOverflow_EMB);
    
    iRndCtrl &= 0xff;

    DEBUG_STATEMENT( g_BiCubic_all_Overflow_cnt++;)
    
    pH = gaSubsampleBicubic + 4 * iXFrac;
    pV = gaSubsampleBicubic + 4 * iYFrac;
    
    if(iXFrac == 0)
    {
        iShift1 = (iYFrac == 2) ? 4 : 6;
        iShift2 = 0;
        iRound1 = (1 << (iShift1 - 1)) - 1 + iRndCtrl;
        iRound2 = 0;
        jloop = iNumLoops + 1;
        
    }
    else if(iYFrac == 0)
    {
        iShift1 = 0;
        iShift2 = (iXFrac == 2) ? 4 : 6;
        iRound2 = (1 << (iShift2 - 1)) - iRndCtrl;
        iRound1 = 0;
        jloop = iNumLoops + 3;
    }
    else
    {
        iShift1 = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift2 = 7;
        iShift1 -= iShift2;
        iRound1 = (1 << (iShift1 - 1)) - 1 + iRndCtrl;
        iRound2 = 64 - iRndCtrl;
        jloop = iNumLoops + 3;
    }
        
    for (i = 0; i < iNumLoops; i++) {

        for (j = 0; j < jloop; j++) {
            const U8_WMV  *pT = pSrc - iSrcStride - 1 + j;
            pFilter[j] = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift1;
        }
        
        for (j = 0; j < iNumLoops; j++) {
            k = (pFilter[j] * pH[0] + pFilter[1 + j] * pH[1] + pFilter[2 + j] * pH[2]
                + pFilter[3 + j] * pH[3] + iRound2) >> iShift2;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            m = ((j&1)*20) +j - (j&1);
            *(I16_WMV *)(pDst + m) = (U8_WMV) k;
        }
        
        pSrc += iSrcStride;
        pDst += 40;
    }
}

#if !defined( WMV_OPT_MOTIONCOMP_ARM) 

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlock_00_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV,U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlock_00_SSIMD);
    
    if(b1MV == 0)
    {
        for (i = 0; i < 8; i++) 
        {
#ifdef _BIG_ENDIAN_
            *(I32_WMV *)(pDst + 0)       = pSrc[0+2]|(pSrc[0  ]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+3]|(pSrc[0+1]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4+2]|(pSrc[4  ]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+3]|(pSrc[4+1]<<16);
#else
            *(I32_WMV *)(pDst + 0)       = pSrc[0  ]|(pSrc[0+2]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1]|(pSrc[0+3]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4  ]|(pSrc[4+2]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1]|(pSrc[4+3]<<16);
#endif
            
            DEBUG_CACHE_READ_BYTES(pSrc,8);
            DEBUG_CACHE_WRITE_BYTES(pDst,24);

            pSrc += iSrcStride;
            pDst += 40;
        }
        
    }
    else
    {
        
        for (i = 0; i < 16; i++) 
        {
#ifdef _BIG_ENDIAN_
            *(I32_WMV *)(pDst + 0)       = pSrc[0+2 ]|(pSrc[0   ]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+3 ]|(pSrc[0+1 ]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4+2 ]|(pSrc[4   ]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+3 ]|(pSrc[4+1 ]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8+2 ]|(pSrc[8   ]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+3 ]|(pSrc[8+1 ]<<16);
            *(I32_WMV *)(pDst + 12)      = pSrc[12+2]|(pSrc[12  ]<<16);
            *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+3]|(pSrc[12+1]<<16);
#else
            *(I32_WMV *)(pDst + 0)       = pSrc[0   ]|(pSrc[0+2]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ]|(pSrc[8+3]<<16);
            *(I32_WMV *)(pDst + 12)      = pSrc[12  ]|(pSrc[12+2]<<16);
            *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+1]|(pSrc[12+3]<<16);
#endif
            
            DEBUG_CACHE_READ_BYTES(pSrc,16);
            DEBUG_CACHE_WRITE_BYTES(pDst,32);

            pSrc += iSrcStride;
            pDst += 40;
        }
    }

    return 0;
    
}

#if 0
//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockB_00_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_BMainLoop * pBMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV,U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockB_00_SSIMD);
    
    if(b1MV == 0)
    {
        for (i = 0; i < 8; i++) 
        {
#ifdef _BIG_ENDIAN_
            *(I32_WMV *)(pDst + 0)       = pSrc[0+2]|(pSrc[0  ]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+3]|(pSrc[0+1]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4+2]|(pSrc[4  ]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+3]|(pSrc[4+1]<<16);
#else
            *(I32_WMV *)(pDst + 0)       = pSrc[0  ]|(pSrc[0+2]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1]|(pSrc[0+3]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4  ]|(pSrc[4+2]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1]|(pSrc[4+3]<<16);
#endif
            
            pSrc += iSrcStride;
            pDst += 40;
        }
        
    }
    else
    {
        
        for (i = 0; i < 16; i++) 
        {
#ifdef _BIG_ENDIAN_
            *(I32_WMV *)(pDst + 0)       = pSrc[0+2 ]|(pSrc[0   ]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+3 ]|(pSrc[0+1 ]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4+2 ]|(pSrc[4   ]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+3 ]|(pSrc[4+1 ]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8+2 ]|(pSrc[8   ]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+3 ]|(pSrc[8+1 ]<<16);
            *(I32_WMV *)(pDst + 12)      = pSrc[12+2]|(pSrc[12  ]<<16);
            *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+3]|(pSrc[12+1]<<16);
#else
            *(I32_WMV *)(pDst + 0)       = pSrc[0   ]|(pSrc[0+2]<<16);
            *(I32_WMV *)(pDst + 0 + 20)  = pSrc[0+1 ]|(pSrc[0+3]<<16);
            *(I32_WMV *)(pDst + 4)       = pSrc[4   ]|(pSrc[4+2]<<16);
            *(I32_WMV *)(pDst + 4 + 20)  = pSrc[4+1 ]|(pSrc[4+3]<<16);
            *(I32_WMV *)(pDst + 8)       = pSrc[8   ]|(pSrc[8+2]<<16);
            *(I32_WMV *)(pDst + 8 + 20)  = pSrc[8+1 ]|(pSrc[8+3]<<16);
            *(I32_WMV *)(pDst + 12)      = pSrc[12  ]|(pSrc[12+2]<<16);
            *(I32_WMV *)(pDst + 12 + 20) = pSrc[12+1]|(pSrc[12+3]<<16);
#endif
            
            pSrc += iSrcStride;
            pDst += 40;
        }
    }

    return 0;
    
}
#endif // 0
#endif // WMV_OPT_MOTIONCOMP_ARM

#endif // _EMB_SSIMD32_


//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlock_00_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV,U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    I32_WMV i, j;
    U8_WMV *pOut;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlock_00_C);

    if(b1MV == 0)
    {
        for (i = 0; i < 8; i++) 
        {
            pOut = pDst;
            for (j = 0; j < 8; j++)
            {
                *(I16_WMV *)pOut = pSrc[j];
                pOut += 2;
            }

            DEBUG_CACHE_READ_BYTES(pSrc,8);
            DEBUG_CACHE_READ_BYTES(pOut-16,16);

            pSrc += iSrcStride;
            pDst += 40;
        }
        
    }
    else
    {
        
        for (i = 0; i < 16; i++) 
        {
            pOut = pDst;
            for (j = 0; j < 16; j++)
            {
                *(I16_WMV *)pOut = pSrc[j];
                pOut += 2;
            }

            DEBUG_CACHE_READ_BYTES(pSrc,16);
            DEBUG_CACHE_READ_BYTES(pOut-32,32);
            
            pSrc += iSrcStride;
            pDst += 40;
        }
    }

    return 0;
}


#ifdef _EMB_SSIMD32_

//Void_WMV g_NewVertFilter0LongNoGlblTbl(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV, I32_WMV * pTbl);
Void_WMV g_NewVertFilter0LongNoGlblTbl(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
#define m_ui80008000  0x80008000//( *(U32_WMV *)pMainLoop->m_rgiSubsampleBicubic)


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_0x_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    //    I32_WMV i;
    U32_WMV overflow = 0;
    
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_0x_SSIMD);
    
    assert(iYFrac!= 0);
    assert(iXFrac == 0);
    
    pSrc -= iSrcStride;

#ifdef VUSE_2ANDx
    overflow = g_NewVertFilterx(pSrc,  iSrcStride, pDst, 6, pMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 6, pMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
#endif

    G_PREFETCH(pSrc, iSrcStride);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)


    return overflow & pMainLoop->m_uic000c000;

}

#endif // _EMB_SSIMD32_

I_SIMD g_InterpolateBlockBicubic_0x_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pV = pPMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_0x_C);
    
    assert(iYFrac!= 0);
    assert(iXFrac == 0);
    
    pSrc -= iSrcStride;

    g_NewVertFilterX_C_Clip(pSrc,  iSrcStride, pDst, 6, pPMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, b1MV);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)


    return 0;

}

#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_x0_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_x0_SSIMD);

    assert(iXFrac!= 0);
    assert(iYFrac == 0);
    pSrc --;
    
#ifdef WMV_OPT_MOTIONCOMP_ARM
//    g_NewVertFilter0LongNoGlblTbl(pSrc, iSrcStride, pDst, b1MV +2, pMainLoop->m_rgiSSIMD_00Tbl);
    g_NewVertFilter0LongNoGlblTbl(pSrc, iSrcStride, pDst, b1MV);
#else
    g_NewVertFilter0Long(pSrc, iSrcStride, pDst, b1MV +2);
#endif

     G_PREFETCH(pSrc, iSrcStride);

#ifdef HUSE_2ANDx
    overflow = g_NewHorzFilterx(pDst, 6 , pMainLoop->m_ui00200020MiRndCtl, pH, b1MV);
#else
    overflow = g_NewHorzFilterX_Fun(pDst, 6 , pMainLoop->m_ui00200020MiRndCtl, pH, b1MV);
#endif
    
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)
        
    return overflow & pMainLoop->m_uic000c000;
}

#endif // _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_x0_C(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pPMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_x0_C);

    assert(iXFrac!= 0);
    assert(iYFrac == 0);
    pSrc --;
    
    g_NewVertFilter0Long_C(pSrc, iSrcStride, pDst, b1MV +2);
    g_NewHorzFilterX_C(pDst, 6 , pPMainLoop->m_ui00200020MiRndCtl, pH, b1MV);
    
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)

    return 0;
}

#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_xx_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow ;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_xx_SSIMD);
    
    assert(iYFrac!= 0);
    assert(iXFrac!= 0);
    
    pSrc -= (iSrcStride + 1);
     
#ifdef VUSE_2ANDx
    overflow = g_NewVertFilterx(pSrc, iSrcStride, (U8_WMV *)pDst, 5, pMainLoop->m_ui000f000fPiRndCtl,  pV, (2<<b1MV)+1,  pMainLoop->m_uiffff07ff, b1MV, pTbl);
#else
    overflow = g_NewVertFilterX_Fun(pSrc, iSrcStride, (U8_WMV *)pDst, 5, pMainLoop->m_ui000f000fPiRndCtl,  pV, (2<<b1MV)+1,  pMainLoop->m_uiffff07ff, b1MV, pTbl);
#endif

    G_PREFETCH(pSrc, iSrcStride);

#ifdef HUSE_2ANDx
    overflow |= g_NewHorzFilterx(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
#else
   overflow |= g_NewHorzFilterX_Fun(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
#endif
 
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return overflow & 0x80008000;
}

#endif // _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_xx_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pPMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pPMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_xx_C);
    
    assert(iYFrac!= 0);
    assert(iXFrac!= 0);
    
    pSrc -= (iSrcStride + 1);
    
    g_NewVertFilterX_C(pSrc, iSrcStride, (U8_WMV *)pDst, 5, pPMainLoop->m_ui000f000fPiRndCtl,  pV, (2<<b1MV)+1, b1MV);
    g_NewHorzFilterX_C(pDst, 7, pPMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
    
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
        
    return 0;
}

#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_02_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_02_SSIMD);
    
    assert((iYFrac)==2);
    assert(iXFrac == 0);
    
    pSrc -= iSrcStride;
    
#ifdef VUSE_2ANDx
    overflow = g_NewVertFilter2(pSrc,  iSrcStride, pDst, 4, pMainLoop->m_ui00070007PiRndCtl,   2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 4, pMainLoop->m_ui00070007PiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
#endif

    G_PREFETCH(pSrc, iSrcStride);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)

    return overflow & pMainLoop->m_uif000f000;

}

#endif // _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_02_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pV = pPMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_02_C);
    
    assert((iYFrac)==2);
    assert(iXFrac == 0);
    
    pSrc -= iSrcStride;
    
    g_NewVertFilterX_C_Clip(pSrc,  iSrcStride, pDst, 4, pPMainLoop->m_ui00070007PiRndCtl,  pV, 2<<b1MV, b1MV);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)
    return 0;
}

#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_20_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_20_SSIMD);
    
    assert((iXFrac)==2);
    assert(iYFrac == 0);
    
    pSrc --;
   
#ifdef WMV_OPT_MOTIONCOMP_ARM
//    g_NewVertFilter0LongNoGlblTbl(pSrc,  iSrcStride, pDst, b1MV + 2,  pMainLoop->m_rgiSSIMD_00Tbl);
    g_NewVertFilter0LongNoGlblTbl(pSrc,  iSrcStride, pDst, b1MV);
#else
    g_NewVertFilter0Long(pSrc,  iSrcStride, pDst, b1MV + 2);
#endif

     G_PREFETCH(pSrc, iSrcStride);
    
#ifdef HUSE_2ANDx
    overflow = g_NewHorzFilter2(pDst, 4, pMainLoop->m_ui00080008MiRndCtl,  b1MV);
#else
    overflow = g_NewHorzFilterX_Fun(pDst, 4, pMainLoop->m_ui00080008MiRndCtl, pH, b1MV);
#endif
    
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)
        
    return overflow & pMainLoop->m_uif000f000;

}

#endif // _EMB_SSIMD32_


//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_20_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pPMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_20_C);
    
    assert((iXFrac)==2);
    assert(iYFrac == 0);
    
    pSrc --;
   
    g_NewVertFilter0Long_C(pSrc,  iSrcStride, pDst, b1MV + 2);
    g_NewHorzFilterX_C(pDst, 4, pPMainLoop->m_ui00080008MiRndCtl, pH, b1MV);
    
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)
        
    return 0;
}


#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_x2_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_x2_SSIMD);
    
    assert((iYFrac)==2);
    assert((iXFrac&1)==1);
    
    pSrc -= (iSrcStride + 1);
 
#ifdef VUSE_2ANDx
    overflow = g_NewVertFilter2(pSrc,  iSrcStride, pDst, 3, pMainLoop->m_ui00030003PiRndCtl,  (2<<b1MV)+1, pMainLoop->m_uiffff1fff, b1MV, pTbl);
#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 3, pMainLoop->m_ui00030003PiRndCtl, pV, (2<<b1MV)+1, pMainLoop->m_uiffff1fff, b1MV, pTbl);
#endif

     G_PREFETCH(pSrc, iSrcStride);

#ifdef HUSE_2ANDx
    overflow |= g_NewHorzFilterx(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
#else
    overflow |= g_NewHorzFilterX_Fun(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
#endif

    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return overflow & m_ui80008000;

}

#endif // _EMB_SSIMD32_


//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_x2_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pPMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pPMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_x2_C);
    
    assert((iYFrac)==2);
    assert((iXFrac&1)==1);
    
    pSrc -= (iSrcStride + 1);
    
    g_NewVertFilterX_C(pSrc,  iSrcStride, pDst, 3, pPMainLoop->m_ui00030003PiRndCtl, pV, (2<<b1MV)+1, b1MV);
    g_NewHorzFilterX_C(pDst, 7, pPMainLoop->m_ui00400040MiRndCtl, pH, b1MV);

    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return 0;

}

#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_2x_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_2x_SSIMD);
    
    assert((iYFrac&1)==1);
    assert((iXFrac)==2);
    
    pSrc -= (iSrcStride + 1);
    
#ifdef VUSE_2ANDx
    overflow = g_NewVertFilterx(pSrc,  iSrcStride, (U8_WMV *)pDst, 3,  pMainLoop->m_ui00030003PiRndCtl,  pV, (2<<b1MV) +1, pMainLoop->m_uiffff1fff, b1MV, pTbl);
#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, (U8_WMV *)pDst, 3,  pMainLoop->m_ui00030003PiRndCtl,  pV, (2<<b1MV) +1, pMainLoop->m_uiffff1fff, b1MV, pTbl);
#endif

     G_PREFETCH(pSrc, iSrcStride);

#ifdef HUSE_2ANDx
    overflow |= g_NewHorzFilter2(pDst, 7, pMainLoop->m_ui00400040MiRndCtl,  b1MV);
#else
    overflow |= g_NewHorzFilterX_Fun(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
#endif
   
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return overflow & m_ui80008000;

}

#endif // _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_2x_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pPMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pPMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_2x_C);
    
    assert((iYFrac&1)==1);
    assert((iXFrac)==2);
    
    pSrc -= (iSrcStride + 1);
    g_NewVertFilterX_C(pSrc,  iSrcStride, (U8_WMV *)pDst, 3,  pPMainLoop->m_ui00030003PiRndCtl,  pV, (2<<b1MV) +1, b1MV);
    g_NewHorzFilterX_C(pDst, 7, pPMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
   
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return 0;

}

#ifdef _EMB_SSIMD32_

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_22_SSIMD ( const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow ;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_2x_SSIMD);
    
    assert((iYFrac)==2);
    assert((iXFrac)==2);
    
    pSrc -= (iSrcStride + 1);
    
    //either one working
    //overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 1, iRndCtrl,  pV,  (2<<b1MV)+1, 0xffff7fff, b1MV);
    //overflow |= g_NewHorzFilterX_Fun(pDst, 7,  0x00400040 - iRndCtrl, pH, b1MV);
        
#ifdef VUSE_2ANDx
    overflow = g_NewVertFilter2(pSrc,  iSrcStride, pDst, 1, pMainLoop->m_iRndCtrl,   (2<<b1MV)+1, pMainLoop->m_uiffff7fff, b1MV, pTbl);
#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 1, pMainLoop->m_iRndCtrl,  pV,  (2<<b1MV)+1, pMainLoop->m_uiffff7fff, b1MV, pTbl);
#endif

     G_PREFETCH(pSrc, iSrcStride);

#ifdef HUSE_2ANDx
    overflow |= g_NewHorzFilter2(pDst, 7,  pMainLoop->m_ui00400040MiRndCtl, b1MV);
#else
    overflow |= g_NewHorzFilterX_Fun(pDst, 7,  pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
#endif
  
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return overflow & m_ui80008000;

}

#endif // _EMB_SSIMD32_


//#pragma code_seg (EMBSEC_PML)
I_SIMD g_InterpolateBlockBicubic_22_C ( const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pPMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pPMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InterpolateBlockBicubic_22_C);
    
    assert((iYFrac)==2);
    assert((iXFrac)==2);
    
    pSrc -= (iSrcStride + 1);
    
    g_NewVertFilterX_C(pSrc,  iSrcStride, pDst, 1, pPMainLoop->m_iRndCtrl,  pV,  (2<<b1MV)+1, b1MV);
    g_NewHorzFilterX_C(pDst, 7,  pPMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
  
    
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
    return 0;

}

/****************************************************************************************
*
*  End of the section for assembly conversion.
*
*****************************************************************************************
*/

//#pragma code_seg (EMBSEC_DEF)
Void_WMV InitBlockInterpolation_EMB (tWMVDecInternalMember * pWMVDec)
{
#ifdef _EMB_SSIMD_MC_
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitBlockInterpolation_EMB);

    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_SSIMD_Fun;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_SSIMD;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_SSIMD;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_SSIMD;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_SSIMD;

    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_SSIMD_Fun;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_SSIMD;
    
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_SSIMD;
    
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_SSIMD;
    
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_SSIMD;

#else // _EMB_SSIMD_MC_

#ifdef _EMB_WMV3_

    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_C;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_C;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_C;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_C;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_C;
#endif //_EMB_WMV3_

#endif // _EMB_SSIMD_MC_

#if defined(macintosh) && defined(_MAC_VEC_OPT)
    
#endif // defined(macintosh) && defined(_MAC_VEC_OPT)

}


#ifdef _EMB_SSIMD32_

static U32_WMV test_err_cliping(U32_WMV u)
{
    U32_WMV v;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(test_err_cliping);
    
    if(u&0x8000)
    {
        u=u+0x10000;
    }
    
    if((v=(u&0xff000000)))
    {
        u&=0xffff;
        
        if(!(v&0x80000000))
            u|=0x00ff0000;
    }
    
    if((v=(u&0x0000ff00)))
    {
        u&=0xffff0000;
        if(!(v&0x00008000))
            u|=0x000000ff;
    }
    
    
    return u;
    
}

static Void_WMV test_err_cliping2(U32_WMV *pt0, U32_WMV *pt1)
{
    
    U32_WMV t0=*pt0;
    U32_WMV t1=*pt1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(test_err_cliping2);
    
    if(t0&0xff00ff00)
    {
        t0=test_err_cliping(t0);
    }
    if(t1&0xff00ff00)
    {
        t1=test_err_cliping(t1);
    }
    
    
    *pt0=t0;
    *pt1=t1;
}


#endif // _EMB_SSIMD32_




#ifdef _EMB_SSIMD64_

static U64_WMV test_err_cliping(U64_WMV u);
static Void_WMV test_err_cliping2(I64_WMV *pt0, I64_WMV *pt1);

Void_WMV  g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);

/******************************************************************************************
*
*  Here starts the section of functions to be written in assembly.
*
*
*******************************************************************************************
*/

// Here starts the so called "Group A" routines to be written in assembly in conjuction with the "group B" routines.
// Note these routines will be called in-loop by the group B routines which are thin wrappers. So it is best to do all the register save/restore 
//in the group B routines outside of the loop. These routines here should only perform a simple jump/return.

/*
I8_WMV  g_NewVertFilterStepTbl[] =
{
    1, 24, 7, -16, 1, 24, 7, -16, 1, 24, 7, -16, 1, 24, 7, -16, 1, 24, 7, -16
};
*/

/*
 * iRound64 = iRound32 | iRound32<<32, Need Pass correct 64 Round
 * uiMask64 = uiMask32 & (uiMask32 << 16) & (uiMask32 << 32)
 * pDst must be 8 bytes aligned.
 */

#ifndef _MIPS_ASM_MOTIONCOMP_OPT_

U64_WMV g_NewVertFilterX(const U8_WMV  *pSrc,
                             const I32_WMV iSrcStride, 
                             U8_WMV * pDst, 
                             const I32_WMV iShift, 
                             const I64_WMV iRound64, 
                             const I8_WMV * const pV, 
                             I32_WMV iNumHorzLoop, 
                             const U64_WMV uiMask,
                             Bool_WMV b1MV, 
                             U16_WMV * pTbl
                             )
{

    I64_WMV k;
    U64_WMV overflow = 0;
    I64_WMV v0, v1, v2, v3;
    I64_WMV iNumInnerLoop = 1<<(3+b1MV);

    /*must be even number of horzloop. This will slow down on iNumHorzLoop=1, comparing SSIMD32, SSIMD64*/
    assert((iNumHorzLoop & 1)==0);

    v0 = pV[0];
    v1 = pV[1];
    v2 = pV[2];
    v3 = pV[3];

     for(k = 0; k < (iNumHorzLoop); k++)
     {
            register U64_WMV t0;
            register U64_WMV o1, o2, o3;
            I64_WMV m;

            o1 = (U64_WMV)pSrc[0] | ((U64_WMV)pSrc[2]<<16) | ((U64_WMV)pSrc[4]<<32) | ((U64_WMV)pSrc[6]<<48);
            pSrc += iSrcStride;
            o2 = (U64_WMV)pSrc[0] | ((U64_WMV)pSrc[2]<<16) | ((U64_WMV)pSrc[4]<<32) | ((U64_WMV)pSrc[6]<<48);
            pSrc += iSrcStride;
            o3 = (U64_WMV)pSrc[0] | ((U64_WMV)pSrc[2]<<16) | ((U64_WMV)pSrc[4]<<32) | ((U64_WMV)pSrc[6]<<48);
            pSrc += iSrcStride;

            for(m = 0; m < iNumInnerLoop; m++)
            {
                t0 = o1*v0 + iRound64;
                t0 += o2*v1;
                t0 += o3*v2;
                o1 = o2;
                o2 = o3;
                o3 = (U64_WMV)pSrc[0] | ((U64_WMV)pSrc[2]<<16) | ((U64_WMV)pSrc[4]<<32) | ((U64_WMV)pSrc[6]<<48);
                t0 += o3*v3;

                //t0 = o0*v0 + o1*v1 + o2*v2 +o3*v3 ;
                overflow |= t0;
                t0 >>= iShift;
                t0 &= uiMask;

                *(U64_WMV *)pDst = t0;
                pDst += 48;
                pSrc += iSrcStride;
            }

            pSrc -= pTbl[0 + ((k&1)<<1)];
            pDst -= pTbl[1 + ((k&1)<<1)];
     }

     return overflow;
}
#endif // _MIPS_ASM_MOTIONCOMP_OPT_


/*
 * pDst must be 8 bytes aligned.
 */
//good
Void_WMV g_NewVertFilter0Long(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV)
{
 
    I64_WMV i;

    if(b1MV == 0)
    {
        for(i = 0; i < 8; i++)
        {
            *(I64_WMV *)(pDst + 0)       = (U64_WMV)pSrc[0   ] | ((U64_WMV)pSrc[0+2]<<16) | ((U64_WMV)pSrc[4]<<32)   | ((U64_WMV)pSrc[6]<<48);
            *(I64_WMV *)(pDst + 0 + 24)  = (U64_WMV)pSrc[1   ] | ((U64_WMV)pSrc[0+3]<<16) | ((U64_WMV)pSrc[4+1]<<32) | ((U64_WMV)pSrc[4+3]<<48);
            *(I32_WMV *)(pDst + 8)       = (U32_WMV)pSrc[8   ] | ((U32_WMV)pSrc[8+2]<<16);
            *(I32_WMV *)(pDst + 8 + 24)  = (U32_WMV)pSrc[8+1 ] | ((U32_WMV)pSrc[8+3]<<16);

            pSrc += iSrcStride;
            pDst += 48;
        }
    }
    else
    {

        for(i = 0; i < 16; i++)
        {
            *(I64_WMV *)(pDst + 0)       = (U64_WMV)pSrc[0   ] | ((U64_WMV)pSrc[0+2]<<16) | ((U64_WMV)pSrc[4]<<32) | ((U64_WMV)pSrc[4+2]<<48);
            *(I64_WMV *)(pDst + 0 + 24)  = (U64_WMV)pSrc[0+1 ] | ((U64_WMV)pSrc[0+3]<<16) | ((U64_WMV)pSrc[4+1 ]<<32) | ((U64_WMV)pSrc[4+3]<<48);
            *(I64_WMV *)(pDst + 8)       = (U64_WMV)pSrc[8   ] | ((U64_WMV)pSrc[8+2]<<16) | ((U64_WMV)pSrc[12]<<32) | ((U64_WMV)pSrc[12+2]<<48);
            *(I64_WMV *)(pDst + 8 + 24)  = (U64_WMV)pSrc[8+1 ] | ((U64_WMV)pSrc[8+3]<<16) | ((U64_WMV)pSrc[12+1 ]<<32) | ((U64_WMV)pSrc[12+3]<<48);
            *(I32_WMV *)(pDst + 16)       = (U32_WMV)pSrc[16   ] | ((U32_WMV)pSrc[16+2]<<16);
            *(I32_WMV *)(pDst + 16 + 24)  = (U32_WMV)pSrc[16+1 ] | ((U32_WMV)pSrc[16+3]<<16);

            pSrc += iSrcStride;
            pDst += 48;
        }
    }
}

#if !defined(_MIPS_ASM_MOTIONCOMP_OPT_)

/*
 * pF must be 8 bytes aligned.
 * iRound2_64 = iRound2_32 | iRound2_32 << 32;
 */

U64_WMV g_NewHorzFilterX(U8_WMV *pF, const I32_WMV iShift, const I64_WMV iRound2_64, const I8_WMV * const pH, Bool_WMV b1MV)
{
    I64_WMV j, i;
    U64_WMV overflow = 0;
    register U64_WMV t0, t1, t2, t3;
    I64_WMV  iNumLoops = 1<<(3+b1MV);

    const I64_WMV h0 = pH[0];
    const I64_WMV h1 = pH[1];
    const I64_WMV h2 = pH[2];
    const I64_WMV h3 = pH[3];

#ifndef USE_INNERLOOP
    for (j = 0; j < iNumLoops; j += 8) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            register I64_WMV o0;
            register I64_WMV o1;

            t0 = *(I64_WMV *)pF; //v0246
  
            o0 = iRound2_64;
            o1 = iRound2_64;

            t1 = *(U64_WMV *)(pF + 24);  //v1357

            o0 += t0 * h0;
            o0 += t1 * h1;

            o1 += t1 * h0;

            t2 = (*(U32_WMV *)(pF+8)); //v8I

            t0 = (t0>>16) | (t2<<48); //v2468

            o0 += t0 * h2;
            o1 += t0 * h1;

            t3 = *(U16_WMV *)(pF + 24 + 8); //v9
            t1 = (t1>>16) | (t3<<48); //v3579

            o0 += t1 * h3;
            o1 += t1 * h2;

            t0 = (t0>>16) | (t2<<32); //v468I
            o1 += t0 * h3;

            overflow |= o0;
            overflow |= o1;

            o0 >>= iShift;
            o1 >>= iShift;

            o0 &= 0x00ff00ff00ff00ff;           
            o1 &= 0x00ff00ff00ff00ff;

            *(U64_WMV *)pF = o0; //v0
            *(U64_WMV *)(pF+24) = o1; //v1

            pF += 48;
        }
        pF += -(48<<(3+b1MV)) + 8;
    }

#else // USE_INNERLOOP

   //Need Change if this is wanted. 
    for(i = 0; i < 8; i++)
    {
        t0 = *(I32_WMV *)pF;

         for (j = 0; j < 8; j += 4) 
        {
            register I32_WMV o0;
            register I32_WMV o1;
  
            o0 = iRound2_32;
            o1 = iRound2_32;
            //t0 = *(I32_WMV *)pF;
            t1 = *(I32_WMV *)(pF + 12); 
            o0 += t0 * h0;
            
            o0 += t1 * h1;
            o1 += t1 * h0;
            t1 = (*(U32_WMV *)(pF+4));
            t0 = t0>>16;
            t1 = t0 | (t1<<16);
          //  t0 = *(U16_WMV *)(pF+2) |  (*(U16_WMV *)(pF+4)) <<16;
            o0 += t1 * h2;
            o1 += t1 * h1;
            t0 = *(U16_WMV *)(pF + 12 + 2);
           // t0 = *(U16_WMV *)(pF + 12 + 2) |  (*(U16_WMV *)(pF + 12 +4)) <<16;
            t1 = (*(U32_WMV *)(pF+12+4));
            t1 = t0 | (t1<<16);
            o0 += t1 * h3;
            o1 += t1 * h2;            
            t0 = *(I32_WMV *)(pF+4);
            o1 += t0 * h3;

            //o0 = t0 * h0  + t1 * h1 + t2 * h2 + t3 * h3 + iRound2_32;
            //o1 = t1 * h0  + t2 * h1 + t3 * h2 + t4 * h3 + iRound2_32;

            overflow |= o0;
            overflow |= o1;
            o0 >>= iShift;
            o1 >>= iShift;
            o0 &= 0x00ff00ff;           
            o1 &= 0x00ff00ff;

            *(U32_WMV *)pF = o0;
            *(U32_WMV *)(pF+12) = o1;
            pF += 4;
        }
        pF += 16;
    }

#endif // USE_INNERLOOP

    return overflow;

}

#endif //_MIPS_ASM_MOTIONCOMP_OPT_



//#pragma code_seg (EMBSEC_BML)
Void_WMV g_AddErrorB_EMB_Overflow(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, I32_WMV iOffset)
{
	I64_WMV iy;
    register U64_WMV err_overflow = 0;
    U64_WMV u0,u1, y0, v0,v1;
    register U64_WMV err_even, err_odd;
    I64_WMV* tErrorBuf = (I64_WMV*)pErrorBuf;
	U64_WMV *tRef0;
	U64_WMV *tRef1;

	pRef0 += iOffset;
	pRef1 += iOffset;

	tRef0 = (U64_WMV *)pRef0;
	tRef1 = (U64_WMV *)pRef1;

    DEBUG_STATEMENT(g_AddErrOverflow_cnt++;)
    

    for (iy = 0; iy < 8; iy++) 
    {
        u0 = tRef0[0];
        u1 = tRef0[0 + 3]; //3*8=24bytes
        
		v0 = tRef1[0];
		v1 = tRef1[0 + 3];

        err_even = tErrorBuf[0];
        err_odd  = tErrorBuf[0 + 8]; //8*8=64

		// average the two frames
		u0 = (u0 + v0 + 0x0001000100010001) >>1;
		u1 = (u1 + v1 + 0x0001000100010001) >>1;
		u0 = u0 & ~0x800080008000; //mask off the improperly shifted values.
		u1 = u1 & ~0x800080008000;


        u0 = u0 + err_even-((err_even & 0x800080008000) << 1);
        err_overflow  |= u0;
        u1 = u1 + err_odd -((err_odd  & 0x800080008000) << 1);
        err_overflow  |= u1;

        if(err_overflow & 0xff00ff00ff00ff00)
        {
            test_err_cliping2(&u0,&u1);
            err_overflow = 0;
        }
            
        tErrorBuf++;
        tRef0 += 6;
		tRef1 += 6;

		// Re-Pack the bytes
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
#else
        y0 = (u0) | ((u1) << 8);
#endif

        *(U32_WMV *)ppxlcDst = (U32_WMV)y0;// & 0xffffffff;
        *(U32_WMV *)(ppxlcDst+4) = (U32_WMV)(y0>>32);// & 0xffffffff;

        ppxlcDst += iPitch;
    }
}


//#pragma code_seg (EMBSEC_BML)
Void_WMV g_AddErrorB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset)
{
    I64_WMV iy;
    U64_WMV err_overflow = 0;
    register U64_WMV u0,u1, y0, v0, v1;
    register U64_WMV err_even, err_odd;
    I64_WMV *tErrorBuf = (I64_WMV *)pErrorBuf;
	U64_WMV *tRef0;
	U64_WMV *tRef1;

	pRef0 += iOffset;
	pRef1 += iOffset;

	tRef0 = (U64_WMV *)pRef0;
	tRef1 = (U64_WMV *)pRef1;

    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        u0 = tRef0[0];
        u1 = tRef0[0 + 3];
        
		v0 = tRef1[0];
		v1 = tRef1[0 + 3];

        err_even = tErrorBuf[0];
        err_odd  = tErrorBuf[8]; //8*8=64bytes

		// average the two frames
		u0 = (u0 + v0 + 0x0001000100010001) >>1;
		u1 = (u1 + v1 + 0x0001000100010001) >>1;
		u0 = u0 & ~0x800080008000; //mask off the improperly shifted values.
		u1 = u1 & ~0x800080008000;


		// Add in the error
        u0 = u0 + err_even-((err_even & 0x800080008000) << 1);
        err_overflow  |= u0;
        u1 = u1 + err_odd -((err_odd  & 0x800080008000) << 1);
        err_overflow  |= u1;
   
        tErrorBuf += 1;
        tRef0 += 6;
		tRef1 += 6;

		// Re-Pack the bytes
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
#else
        y0 = (u0) | ((u1) << 8);
#endif

        *(U32_WMV *)ppxlcDst = (U32_WMV)y0;// & 0xffffffff;
        *(U32_WMV *)(ppxlcDst+4) = (U32_WMV)(y0>>32);// & 0xffffffff;

        ppxlcDst += iPitch;
    }

    DEBUG_STATEMENT(g_AddErr_cnt++;)

    if(err_overflow & 0xff00ff00ff00ff00)
    {
        ppxlcDst -= 8*iPitch;
        g_AddErrorB_EMB_Overflow(ppxlcDst, pRef0-iOffset, pRef1-iOffset, pErrorBuf, iPitch, iOffset);
    }
}


//#pragma code_seg (EMBSEC_BML)
Void_WMV g_AddNullB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch)
{

    I64_WMV iy;
    register U64_WMV u0,u1, y0, v0,v1;

	U64_WMV *tRef0;
	U64_WMV *tRef1;

	pRef0 += iOffset;
	pRef1 += iOffset;

	tRef0 = (U64_WMV *)pRef0;
	tRef1 = (U64_WMV *)pRef1;


    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {

        u0 = tRef0[0];
        u1 = tRef0[0 + 3];

		v0 = tRef1[0];
		v1 = tRef1[0 + 3];

		// average the two frames
		u0 = (u0 + v0 + 0x0001000100010001) >>1;
		u1 = (u1 + v1 + 0x0001000100010001) >>1;
		u0 = u0 & ~0x800080008000; //mask off the improperly shifted values.
		u1 = u1 & ~0x800080008000;

        tRef0 += 6;
		tRef1 += 6;

		// Re-Pack the bytes
#ifdef _BIG_ENDIAN_
        y0 = (u1) | ((u0) << 8);
#else
        y0 = (u0) | ((u1) << 8);
#endif

		// Store the results
        *(U32_WMV *)ppxlcDst = (U32_WMV)y0;
        *(U32_WMV *)(ppxlcDst+4) = (U32_WMV)(y0>>32);

        ppxlcDst += iPitch;
    }
}





//Here starts the so called "Group B" routines. These routines should be written in assembly conjunction with the group A routines. See the notes before that.

/*
 * ppxlcDst must be 8 bytes aligned.
 * pRef, pErrorBuf must be 8 bytes aligned. iPitch must be multiple of 8.
 */
Void_WMV g_AddError_EMB_Overflow(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I64_WMV iy;
    register U64_WMV err_overflow = 0;
    U64_WMV u0,u1, y0;
    register U64_WMV err_even, err_odd;
    U64_WMV* tRef;
    I64_WMV* tErrorBuf;

    tRef = (U64_WMV*)pRef;
    tErrorBuf = (I64_WMV*)pErrorBuf;


    DEBUG_STATEMENT(g_AddErrOverflow_cnt++;)
    

    for (iy = 0; iy < 8; iy++) 
    {
        u0 = tRef[0];
        u1 = tRef[0 + 3]; //3*8=24bytes
        
        err_even = tErrorBuf[0];
        err_odd  = tErrorBuf[0 + 8]; //8*8=64

        u0 = u0 + err_even-((err_even & 0x800080008000) << 1);
        err_overflow  |= u0;
        u1 = u1 + err_odd -((err_odd  & 0x800080008000) << 1);
        err_overflow  |= u1;

        if(err_overflow & 0xff00ff00ff00ff00)
        {
            test_err_cliping2(&u0,&u1);
            err_overflow = 0;
        }
            
        tErrorBuf++;
        tRef += 6;

        y0 = (u0) | ((u1) << 8);

        *(U32_WMV *)ppxlcDst = (U32_WMV)y0;// & 0xffffffff;
        *(U32_WMV *)(ppxlcDst+4) = (U32_WMV)(y0>>32);// & 0xffffffff;

        ppxlcDst += iPitch;
    }
}


/*
 * pRef must be 8 bytes aligned
 * iPitch must be multiple of 8
 */

Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
{
    I64_WMV iy;
    register U64_WMV u0,u1, y0;

    U64_WMV* tRef = (U64_WMV*) pRef;

    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {

        u0 = tRef[0];
        u1 = tRef[0 + 3];

        tRef += 6;

        y0 = (u0) | ((u1) << 8);

        *(U32_WMV *)ppxlcDst = (U32_WMV)y0;// & 0xffffffff;
        *(U32_WMV *)(ppxlcDst+4) = (U32_WMV)(y0>>32);// & 0xffffffff;

        ppxlcDst += iPitch;
    }
}


/*
 * ppxlcDst must be 8 bytes aligned.
 * pRef must be 8 bytes aligned.
 * pErrorBuf must be 8 bytes aligned.
 * iPitch must be multiple of 8
 */

//good
Void_WMV g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I64_WMV iy;
    U64_WMV err_overflow = 0;
    register U64_WMV u0,u1, y0;
    register U64_WMV err_even, err_odd;

    I64_WMV *tErrorBuf = (I64_WMV *)pErrorBuf;
    U64_WMV *tRef = (U64_WMV *)pRef;

    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        u0 = tRef[0];
        u1 = tRef[0 + 3];
        
        err_even = tErrorBuf[0];
        err_odd  = tErrorBuf[8]; //8*8=64bytes

        u0 = u0 + err_even-((err_even & 0x800080008000) << 1);
        err_overflow  |= u0;
        u1 = u1 + err_odd -((err_odd  & 0x800080008000) << 1);
        err_overflow  |= u1;
   
        tErrorBuf += 1;
        tRef += 6;

        y0 = (u0) | ((u1) << 8);

        *(U32_WMV *)ppxlcDst = (U32_WMV)y0;// & 0xffffffff;
        *(U32_WMV *)(ppxlcDst+4) = (U32_WMV)(y0>>32);// & 0xffffffff;

        ppxlcDst += iPitch;
    }

    DEBUG_STATEMENT(g_AddErr_cnt++;)

    if(err_overflow & 0xff00ff00ff00ff00)
    {
        ppxlcDst -= 8*iPitch;
        g_AddError_EMB_Overflow(ppxlcDst, pRef , pErrorBuf, iPitch);
    }
}


/*
 * No 32 SSIMD applied in this function. Changes are only complying 32/64 bit immediate calc result.
 */

Void_WMV  g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{

    I64_WMV i, j, k0,k1;
    I64_WMV iNumLoops = 8<<b1MV;
    I16_WMV pFilter[16 + 1];
    
    iRndCtrl = 8 - ( iRndCtrl&0xff);
       
    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
        for (j = 0; j < iNumLoops; j+=4) 
        {
            //Vertical Interpolation: PF = PT * (4-iYFrac) + PT[NextLine] * iYFrac; iYrac = {0 - 3}
            pT = pSrc + j;
            pFilter[j]   = (pT[0]<<2)   +  ( pT[iSrcStride]   - pT[0]  ) * iYFrac;
            pFilter[j+1] = (pT[0+1]<<2) +  ( pT[iSrcStride+1] - pT[0+1]) * iYFrac;
            pFilter[j+2] = (pT[0+2]<<2) +  ( pT[iSrcStride+2] - pT[0+2]) * iYFrac;
            pFilter[j+3] = (pT[0+3]<<2) +  ( pT[iSrcStride+3] - pT[0+3]) * iYFrac;
        }

        pT = pSrc + j;
        pFilter[j] = (pT[0]<<2)+  ( pT[iSrcStride] - pT[0])* iYFrac;

        for (j = 0; j < iNumLoops; j += 2) 
        {
            //Horizontal Interpolation: PD = PF * (4-iXFrac) + PF[1] * iXFrac;
            k0 = ((pFilter[j] <<2) + (pFilter[1 + j] - pFilter[j]) * iXFrac +  iRndCtrl) >> 4;
            k1 = ((pFilter[j+1] <<2) + (pFilter[1 + j+1] - pFilter[j+1]) * iXFrac + iRndCtrl) >> 4;
            assert(k0>=0);
            assert(k0<256);
            assert(k1>=0);
            assert(k1<256);

            *(I16_WMV *)(pDst + j) = (U8_WMV) k0;
            *(I16_WMV *)(pDst + j + 24) = (U8_WMV) k1;
        }
        pSrc += iSrcStride;
        pDst += 48;
    }
}


/*
 * When Bicubic Overflow happens, this function is called to recalc the whole block. It only saves when overflow happens rarely.
 * No SSIMD 32/64 is applied in this function because it is rarely used.
 */

Void_WMV g_InterpolateBlockBicubicOverflow_EMB(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
{
    I32_WMV i, j, k;
    const I16_WMV *pH;
    const I16_WMV *pV;
    I16_WMV pFilter[32+3];
    I32_WMV iRound1;
    I32_WMV iRound2;
    I32_WMV iShift1, iShift2;
    I32_WMV jloop, m;
    I32_WMV iNumLoops = 1<<(3+b1MV);

    iRndCtrl &= 0xff;

    //assert (iWidth < 32); // Due to pFilter size, we can only handle widths of <= 32

    pH = gaSubsampleBicubic + 4 * iXFrac;
    pV = gaSubsampleBicubic + 4 * iYFrac;

    if(iXFrac == 0)
    {
        iShift1 = (iYFrac == 2) ? 4 : 6;
        iShift2 = 0;

        iRound1 = (1 << (iShift1 - 1)) - 1 + iRndCtrl;
        iRound2 = 0;
        jloop = iNumLoops + 1;
    
    }
    else if(iYFrac == 0)
    {
        iShift1 = 0;
        iShift2 = (iXFrac == 2) ? 4 : 6;
        iRound2 = (1 << (iShift2 - 1)) - iRndCtrl;
        iRound1 = 0;
        jloop = iNumLoops + 3;
    }
    else
    {
        iShift1 = ((iXFrac == 2) ? 4 : 6) + ((iYFrac == 2) ? 4 : 6);
        iShift2 = 7;
        iShift1 -= iShift2;

        iRound1 = (1 << (iShift1 - 1)) - 1 + iRndCtrl;
        iRound2 = 64 - iRndCtrl;
        jloop = iNumLoops + 3;
    }
   
    for (i = 0; i < iNumLoops; i++) 
    {
        for (j = 0; j < jloop; j++) 
        {
            const U8_WMV  *pT = pSrc - iSrcStride - 1 + j;
            pFilter[j] = (pT[0] * pV[0] + pT[iSrcStride] * pV[1] + pT[2 * iSrcStride] * pV[2]
                + pT[3 * iSrcStride] * pV[3] + iRound1) >> iShift1;

        }

        for (j = 0; j < iNumLoops; j++) 
        {
            k = (pFilter[j] * pH[0] + pFilter[1 + j] * pH[1] + pFilter[2 + j] * pH[2]
                + pFilter[3 + j] * pH[3] + iRound2) >> iShift2;
            if (k < 0)
                k = 0;
            else if (k > 255)
                k = 255;
            m = ((j&1)*24) +j - (j&1);
            *(I16_WMV *)(pDst + m) = (U8_WMV) k;
        }

        pSrc += iSrcStride;
        pDst += 48;
    }
}


/*
 * pDst must 8 bytes aligned.
 */
//good
I64_WMV g_InterpolateBlock_00_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    I64_WMV i;

    if(b1MV == 0)
    {
        for (i = 0; i < 8; i++) 
        {
            *(I64_WMV *)(pDst + 0)       = (U64_WMV)pSrc[0   ] | ((U64_WMV)pSrc[0+2]<<16) | ((U64_WMV)pSrc[4]<<32)   | ((U64_WMV)pSrc[4+2]<<48);
            *(I64_WMV *)(pDst + 0 + 24)  = (U64_WMV)pSrc[0+1 ] | ((U64_WMV)pSrc[0+3]<<16) | ((U64_WMV)pSrc[4+1]<<32) | ((U64_WMV)pSrc[4+3]<<48);

            pSrc += iSrcStride;
            pDst += 48;
        }
    }
    else
    {

        for (i = 0; i < 16; i++) 
        {
            *(I64_WMV *)(pDst + 0)       = (U64_WMV)pSrc[0   ] | ((U64_WMV)pSrc[0+2]<<16) | ((U64_WMV)pSrc[4   ]<<32) | ((U64_WMV)pSrc[4+2]<<48);
            *(I64_WMV *)(pDst + 0 + 24)  = (U64_WMV)pSrc[0+1 ] | ((U64_WMV)pSrc[0+3]<<16) | ((U64_WMV)pSrc[4+1 ]<<32) | ((U64_WMV)pSrc[4+3]<<48);
            *(I64_WMV *)(pDst + 8)       = (U64_WMV)pSrc[8   ] | ((U64_WMV)pSrc[8+2]<<16) | ((U64_WMV)pSrc[12  ]<<32) | ((U64_WMV)pSrc[12+2]<<48);
            *(I64_WMV *)(pDst + 8 + 24)  = (U64_WMV)pSrc[8+1 ] | ((U64_WMV)pSrc[8+3]<<16) | ((U64_WMV)pSrc[12+1]<<32) | ((U64_WMV)pSrc[12+3]<<48);

            pSrc += iSrcStride;
            pDst += 48;
        }
    }
    return 0;
}


/*
 * pDst must be 8 bytes aligned.
 */

I64_WMV g_InterpolateBlockBicubic_0x_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
//    I32_WMV i;
    U64_WMV overflow = 0;
    I64_WMV rndctrl;

    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    assert((iYFrac&1)==1);
    assert(iXFrac == 0);
    

    rndctrl = pMainLoop->m_ui001f001fPiRndCtl | ((U64_WMV)pMainLoop->m_ui001f001fPiRndCtl << 32);

    pSrc -= iSrcStride;
    overflow = g_NewVertFilterX(pSrc,  iSrcStride, pDst, 6, rndctrl,  pV, 2<<b1MV, 0x00ff00ff00ff00ff, b1MV, pTbl);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)
    return (overflow & 0xc000c000c000c000);
}


/*
 * pDst must be 8 bytes aligned.
 */

I64_WMV g_InterpolateBlockBicubic_x0_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    I64_WMV rndctrl;

    assert((iXFrac&1)==1);
    assert(iYFrac == 0);
    pSrc --;
        
    g_NewVertFilter0Long(pSrc, iSrcStride, pDst, b1MV +2);

    rndctrl = pMainLoop->m_ui00200020MiRndCtl | ((U64_WMV)pMainLoop->m_ui00200020MiRndCtl << 32);

    overflow = g_NewHorzFilterX(pDst, 6 , rndctrl, pH, b1MV);

    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)

    return (overflow & 0xc000c000c000c000);
}


/*
 * pDst must be 8 bytes aligned
 */

I64_WMV g_InterpolateBlockBicubic_xx_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow ;
    I64_WMV rndctrl;
    
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    assert((iYFrac&1)==1);
    assert((iXFrac&1)==1);

    pSrc -= (iSrcStride + 1);

    rndctrl = pMainLoop->m_ui000f000fPiRndCtl | ((U64_WMV)pMainLoop->m_ui000f000fPiRndCtl << 32);

    overflow = g_NewVertFilterX(pSrc, iSrcStride, (U8_WMV *)pDst, 5, rndctrl,  pV, (2<<b1MV)+2,  0xffff07ff07ff07ff, b1MV, pTbl);

    rndctrl = pMainLoop->m_ui00400040MiRndCtl | ((U64_WMV)pMainLoop->m_ui00400040MiRndCtl << 32);
    overflow |= g_NewHorzFilterX(pDst, 7, rndctrl, pH, b1MV);


    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
    return (overflow & 0x8000800080008000);
}


/*
 * pDst must be 8 bytes aligned
 */

I64_WMV g_InterpolateBlockBicubic_02_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow = 0;
    I64_WMV rndctrl;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    assert((iYFrac)==2);
    assert(iXFrac == 0);

    pSrc -= iSrcStride;

    rndctrl = pMainLoop->m_ui00070007PiRndCtl | ((U64_WMV)pMainLoop->m_ui00070007PiRndCtl << 32);

    overflow = g_NewVertFilterX(pSrc,  iSrcStride, pDst, 4, rndctrl,  pV, 2<<b1MV, 0x00ff00ff00ff00ff, b1MV, pTbl);

    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)
    return (overflow & 0xf000f000f000f000);
}


/*
 * pDst must be 8 bytes aligned
 */

I64_WMV g_InterpolateBlockBicubic_20_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    I64_WMV rndctrl;

    assert((iXFrac)==2);
    assert(iYFrac == 0);

    pSrc --;

    rndctrl = pMainLoop->m_ui00080008MiRndCtl | ((U64_WMV)pMainLoop->m_ui00080008MiRndCtl << 32);
   
    g_NewVertFilter0Long(pSrc,  iSrcStride, pDst, b1MV + 2);
    overflow = g_NewHorzFilterX(pDst, 4, rndctrl, pH, b1MV);
  
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)

    return (overflow & 0xf000f000f000f000);
}


/*
 * pDst must be 8 bytes aligned
 */

I64_WMV g_InterpolateBlockBicubic_x2_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow = 0;
    I64_WMV rndctrl;
    
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    assert((iYFrac)==2);
    assert((iXFrac&1)==1);

    pSrc -= (iSrcStride + 1);

    rndctrl = pMainLoop->m_ui00030003PiRndCtl | ((U64_WMV)pMainLoop->m_ui00030003PiRndCtl << 32);

    //either one working!
    overflow = g_NewVertFilterX(pSrc,  iSrcStride, pDst, 3, rndctrl, pV, (2<<b1MV)+2, 0xffff1fff1fff1fff, b1MV, pTbl);

    rndctrl = pMainLoop->m_ui00400040MiRndCtl | ((U64_WMV)pMainLoop->m_ui00400040MiRndCtl << 32);

    overflow |= g_NewHorzFilterX(pDst, 7, rndctrl, pH, b1MV);

    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
    return (overflow & 0x8000800080008000);
}


/*
 * pDst must be 8 bytes aligned
 */

I64_WMV g_InterpolateBlockBicubic_2x_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow = 0;
    I64_WMV rndctrl;
    
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    assert((iYFrac&1)==1);
    assert((iXFrac)==2);

    pSrc -= (iSrcStride + 1);
        
    rndctrl = pMainLoop->m_ui00030003PiRndCtl | ((U64_WMV)pMainLoop->m_ui00030003PiRndCtl << 32);
    overflow = g_NewVertFilterX(pSrc,  iSrcStride, (U8_WMV *)pDst, 3,  rndctrl,  pV, (2<<b1MV) +2, 0xffff1fff1fff1fff, b1MV, pTbl);

        //either one working!
    rndctrl = pMainLoop->m_ui00400040MiRndCtl | ((U64_WMV)pMainLoop->m_ui00400040MiRndCtl << 32);
    overflow |= g_NewHorzFilterX(pDst, 7, rndctrl, pH, b1MV);

    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
    return (overflow & 0x8000800080008000);
}


/*
 * pDst must be 8 bytes aligned
 */

I64_WMV g_InterpolateBlockBicubic_22_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U64_WMV overflow = 0;
    I64_WMV rndctrl;
  
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    assert((iYFrac)==2);
    assert((iXFrac)==2);

    pSrc -= (iSrcStride + 1);

        //either one working
    rndctrl = pMainLoop->m_iRndCtrl | ((U64_WMV)pMainLoop->m_iRndCtrl << 32);

    overflow = g_NewVertFilterX(pSrc,  iSrcStride, pDst, 1, rndctrl,  pV,  (2<<b1MV)+2, 0xffff7fff7fff7fff, b1MV, pTbl);

    rndctrl = pMainLoop->m_ui00400040MiRndCtl | ((U64_WMV)pMainLoop->m_ui00400040MiRndCtl << 32);
    overflow |= g_NewHorzFilterX(pDst, 7,  rndctrl, pH, b1MV);

    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)
    return (overflow & 0x8000800080008000);
}


/****************************************************************************************
*
*  End of the section for assembly conversion.
*
*****************************************************************************************
*/
/*
//#pragma code_seg (EMBSEC_DEF)
Void_WMV InitBlockInterpolation_EMB (tWMVDecInternalMember * pWMVDec)
{
#ifdef _EMB_SSIMD_MC_
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitBlockInterpolation_EMB);
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_SSIMD_Fun;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_SSIMD;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_SSIMD;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_SSIMD;
    
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_PMainLoop.PB.m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_SSIMD;

    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_SSIMD_Fun;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_SSIMD;
    
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_SSIMD;
    
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_SSIMD;
    
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_SSIMD;
    pWMVDec->m_EMB_BMainLoop.PB.m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_SSIMD;
#else // _EMB_SSIMD_MC_
    //TBD
    assert(0);
#endif // _EMB_SSIMD_MC_
}
*/

/*****************************************************************************
    -------------------------------------------------------------------------
    |11111111|00000000|11111111|00000000|11111111|00000000|11111111|00000000|
    -------------------------------------------------------------------------
    64               48                32                16                 0
    -------------------------------------------------------------------------
    |        V3       |        V2       |        V1       |        V0       |
    -------------------------------------------------------------------------

    Vi(0-3) = min(255, max(Vi, 0));

  test _err_clipping is to convert U to Ui.
*******************************************************************************/

#define RESTORE16VAL(val) \
    if ((val) & 0x800080008000){ \
        (val) += ((val) & 0x8000) << 1; \
        (val) += ((val) & 0x80000000) << 1; \
        (val) += ((val) & 0x800000000000) << 1; \
    }


static U64_WMV test_err_cliping(U64_WMV u)
{
    U64_WMV v;

    v = 0x800080008000;
    if(u & v)
    {
        RESTORE16VAL(u);
    }

    //most significant 8 bits
    if (v=(u & 0xff00000000000000))
    {
        u &= 0xffffffffffff;            // clear 0
        if (!(v & 0x8000000000000000))  // >255
            u |= 0xff000000000000;      //set 255
    }

    if (v=(u & 0xff0000000000))
    {
        u &= 0xffff0000ffffffff;    // clear 0
        if (!(v & 0x800000000000))
            u |= 0xff00000000;//set 255
    }

    if (v=(u & 0xff000000))
    {
        u &= 0xffffffff0000ffff; // clear 0

        if (!(v & 0x80000000))
            u |= 0xff0000;
    }

    if (v = (u & 0xff00))
    {
        u &= 0xffffffffffff0000; // clear 0

        if (!(v & 0x8000))  // <0
            u |= 0xff; //10 ff
    }

    return u;
}

static Void_WMV test_err_cliping2(I64_WMV *pt0, I64_WMV *pt1)
{

    I64_WMV t0=*pt0;
    I64_WMV t1=*pt1;

    
    if(t0&0xff00ff00ff00ff00)
    {
        t0=test_err_cliping(t0);
    }

    if(t1&0xff00ff00ff00ff00)
    {
        t1=test_err_cliping(t1);
    }
    *pt0=t0;
    *pt1=t1;
}

#endif //_EMB_SSIMD64_

#endif //_EMB_WMV3_
