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

#   define DEBUG_STATEMENT(x)

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
#   define G_PREFETCH(x,y) 
#else
#   define G_PREFETCH(x,y)
#endif

//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewVertFilterX_C(const U8_WMV  *pSrc,
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

U32_WMV g_NewVertFilterX_Only_C(U8_WMV  *pSrc,
                         I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         I32_WMV iDstStride, 
                         I32_WMV iShift, 
                         I32_WMV iRound32, 
                         I8_WMV * pV, 
                         I32_WMV iNumHorzLoop, 
                         U32_WMV uiMask,
                         Bool_WMV b1MV)
{    
    I32_WMV k,m;
    //U32_WMV overflow = 0;
    I16_WMV v0, v1, v2, v3;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);

    U8_WMV* pSrcTmp =  pSrc;
    U8_WMV* pDstTmp =  pDst;
    U32_WMV round = (iRound32>>16);
    U32_WMV shift  = iShift;
    
    v0 = pV[0];    v1 = pV[1];
    v2 = pV[2];    v3 = pV[3];  

    for(k = 0; k < (iNumHorzLoop<<2); k++)
    {  
        pSrcTmp =  pSrc;
        pDstTmp =  pDst;

        for(m = 0; m < iNumInnerLoop; m++)
        {
            U32_WMV s0 = pSrcTmp[k];
            U32_WMV s1 = pSrcTmp[k+1*iSrcStride];
            U32_WMV s2 = pSrcTmp[k+2*iSrcStride];
            U32_WMV s3 = pSrcTmp[k+3*iSrcStride];
            I32_WMV t0 = s0*v0 + s1*v1 + s2*v2 + s3*v3 + round;

            t0 >>= shift;
            pDstTmp[k] = CLIP(t0);

            pSrcTmp += iSrcStride;
            pDstTmp += (iDstStride);
        }       
    }

    //ReOrgnaizeCompBuffer((U16_WMV*)pDst);
    return 0;    
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_NewVertFilter0Long_C(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV)
{
    
    I32_WMV i;
    
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
           

            pSrc += iSrcStride;
            pDst += 40;
        }
    }
}

//#pragma code_seg (EMBSEC_PML)
U32_WMV g_NewHorzFilterX_C(U8_WMV *pF, const I32_WMV iShift, const I32_WMV iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV)
{
    I32_WMV j, i;
    U32_WMV overflow = 0;
    register U32_WMV t0, t1; //, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    const I16_WMV h0 = pH[0];
    const I16_WMV h1 = pH[1];
    const I16_WMV h2 = pH[2];
    const I16_WMV h3 = pH[3];

    for (j = 0; j < iNumLoops; j += 4) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            register I32_WMV o0;
            register I32_WMV o1;
         
            t0 = *(I32_WMV *)pF;          
            o0 = iRound2_32;
            o1 = iRound2_32;
            t1 = *(I32_WMV *)(pF + 20); 
            o0 += t0 * h0;
            
            o0 += t1 * h1;
            o1 += t1 * h0;
            t1 = (*(U32_WMV *)(pF+4));
            
            t0 = t0>>16;
            t1 = t0 | (t1<<16);

            o0 += t1 * h2;
            o1 += t1 * h1;
            t0 = *(U16_WMV *)(pF + 20 + 2);
            t1 = (*(U32_WMV *)(pF+20+4)); 
            t1 = t0 | (t1<<16);

            o0 += t1 * h3;
            o1 += t1 * h2;            
            t0 = *(I32_WMV *)(pF+4);
            o1 += t0 * h3;

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

U32_WMV g_NewVertFilterX_C_new( U8_WMV  *pSrc,
                          I32_WMV iSrcStride, 
                          I16_WMV * pDst, 
                          I32_WMV iDstStride, 
                          I32_WMV iShift, 
                          I32_WMV iRound32, 
                          I8_WMV *  pV, 
                         I32_WMV iNumHorzLoop, 
                          U32_WMV uiMask,
                         Bool_WMV b1MV)
{    
    I32_WMV k,m;
    //U32_WMV overflow = 0;
    I16_WMV v0, v1, v2, v3;
    I32_WMV iNumInnerLoop = 1<<(3+b1MV);

    U8_WMV* pSrcTmp =  pSrc;
    I16_WMV* pDstTmp =  (I16_WMV*)pDst;
    U32_WMV round = (iRound32>>16);
    U32_WMV shift  = iShift;
    //U32_WMV mask = (uiMask>>16);
    
    v0 = pV[0];    v1 = pV[1];
    v2 = pV[2];    v3 = pV[3];  

    for(k = 0; k < (iNumHorzLoop<<2); k++)
    {  
        pSrcTmp =  pSrc;
        pDstTmp =  (I16_WMV*)pDst;

        for(m = 0; m < iNumInnerLoop; m++)
        {
            U32_WMV s0 = pSrcTmp[k];
            U32_WMV s1 = pSrcTmp[k+1*iSrcStride];
            U32_WMV s2 = pSrcTmp[k+2*iSrcStride];
            U32_WMV s3 = pSrcTmp[k+3*iSrcStride];
            I32_WMV t0 = s0*v0 + s1*v1 + s2*v2 + s3*v3 + round;

            //overflow |= t0;

            t0 >>= shift;
            pDstTmp[k] = t0;

            pSrcTmp += iSrcStride;
            pDstTmp += iDstStride;
        }       
    }
    return 0;//overflow;    
}

U32_WMV g_NewHorzFilterX_C_new(I16_WMV *pSrc, 
                                I32_WMV iSrcStride,
                                U8_WMV *pDst, 
                                I32_WMV iDstStride,
                               const I32_WMV iShift,
                               const I32_WMV iRound2_32, 
                               const I8_WMV * const pH, 
                               Bool_WMV b1MV)
{
    I32_WMV j, i;
    //U32_WMV overflow = 0;
    //register U32_WMV t0, t1; //, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    const I16_WMV h0 = pH[0];
    const I16_WMV h1 = pH[1];
    const I16_WMV h2 = pH[2];
    const I16_WMV h3 = pH[3];

    I16_WMV* pSrcTmp =  (I16_WMV*)pSrc;
    U8_WMV* pDstTmp =  (U8_WMV*)pDst;
    U32_WMV round = (iRound2_32>>16);
    //U32_WMV shift  = iShift;

    //ReOrgnaizeCompBuffer_2normal(pSrcTmp);

    for (j = 0; j < iNumLoops; j ++) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            I32_WMV s0 = pSrcTmp[i+j*iSrcStride];
            I32_WMV s1 = pSrcTmp[i+1+j*iSrcStride];
            I32_WMV s2 = pSrcTmp[i+2+j*iSrcStride];
            I32_WMV s3 = pSrcTmp[i+3+j*iSrcStride];
            I32_WMV t0 = s0*h0 + s1*h1 + s2*h2 +s3*h3 + round;

            //overflow |= t0;          
            t0 >>= iShift;
            pDstTmp[i+j*iDstStride] = CLIP(t0);
        }
     }

     //ReOrgnaizeCompBuffer(pDstTmp);
     return 0;//overflow;
}

U32_WMV g_NewHorzFilterX_Only_C(U8_WMV *pSrc, 
                                I32_WMV iSrcStride,
                                U8_WMV *pDst, 
                                I32_WMV iDstStride,
                               const I32_WMV iShift,
                               const I32_WMV iRound2_32, 
                               const I8_WMV * const pH, 
                               Bool_WMV b1MV)
{
    I32_WMV j, i;
    //U32_WMV overflow = 0;
    //register U32_WMV t0, t1; //, t2, t3;
    I32_WMV  iNumLoops = 1<<(3+b1MV);
    
    const I16_WMV h0 = pH[0];
    const I16_WMV h1 = pH[1];
    const I16_WMV h2 = pH[2];
    const I16_WMV h3 = pH[3];

    U8_WMV* pSrcTmp =  pSrc;
    U8_WMV* pDstTmp =  (U8_WMV*)pDst;
    U32_WMV round = (iRound2_32>>16);
    //U32_WMV shift  = iShift;

    //ReOrgnaizeCompBuffer_2normal(pSrcTmp);

    for (j = 0; j < iNumLoops; j ++) 
    {
        for(i = 0; i < iNumLoops; i++)
        {
            U32_WMV s0 = pSrcTmp[i+j*iSrcStride];
            U32_WMV s1 = pSrcTmp[i+1+j*iSrcStride];
            U32_WMV s2 = pSrcTmp[i+2+j*iSrcStride];
            U32_WMV s3 = pSrcTmp[i+3+j*iSrcStride];
            I32_WMV t0 = s0*h0 + s1*h1 + s2*h2 +s3*h3 + round;

            //overflow |= t0;
            t0 >>= iShift;
            pDstTmp[i+j*(iDstStride)] = CLIP(t0);
        }
     }

     //ReOrgnaizeCompBuffer(pDstTmp);
     return 0;
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV g_AddErrorB_EMB_Overflow_C(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, I32_WMV iOffset)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1, y0,v0,v1;
    U32_WMV err_even, err_odd;
    
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

//#pragma code_seg (EMBSEC_BML)
Void_WMV g_AddNullB_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV u0,u1,u2,u3, y0,y1,v0,v1,v2,v3;
    
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
       

        ppxlcDst += iPitch;
    }
}

Void_WMV g_AddNullB_SSIMD_C_new(U8_WMV* ppxlcDst,  
                                                I32_WMV iDststride,
                                                U8_WMV* pRef0 , 
                                                U8_WMV* pRef1,
                                                I32_WMV iOffset,
                                                I32_WMV iRefstride)
{
    int i,j;
    pRef0 += iOffset*4;
    pRef1 += iOffset*4;
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            ppxlcDst[i*iDststride+j] = (pRef0[i*iRefstride+j] + pRef1[i*iRefstride+j] +1)>>1;
        }
    }
}

//#pragma code_seg (EMBSEC_BML)
Void_WMV g_AddErrorB_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1,u2,u3, y0,y1, v0,v1,v2,v3;
    U32_WMV err_even, err_odd;

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
 

        ppxlcDst += iPitch;
        
    }
   
        
    if(err_overflow & 0xff00ff00) {
        ppxlcDst -= 8*iPitch;
        pRef0 -= 10*8+iOffset;
        pRef1 -= 10*8+iOffset;
        pErrorBuf -= 16;
        g_AddErrorB_EMB_Overflow_C(ppxlcDst, pRef0 , pRef1,  pErrorBuf, iPitch, iOffset);
    }
        
}


Void_WMV g_AddErrorB_SSIMD_C_new(U8_WMV* ppxlcDst, 
                                 I32_WMV iDststride,
                                 U8_WMV* pRef0, 
                                 U8_WMV* pRef1, 
                                 I32_WMV iRefstride,
                                 I16_WMV* pErrorBlock, 
                                 U32_WMV iOffset)
{
    I32_WMV i, j;
    pRef0 += iOffset*4;
    pRef1 += iOffset*4;
    
    for (i = 0; i < 8; i++) 
    {
        for (j = 0; j< 8; j++) 
        {
            int ref_ave = (pRef0[i*iRefstride+j]+pRef1[i*iRefstride+j]+1)>>1;
            ppxlcDst[i*iDststride+j] = CLIP(ref_ave + pErrorBlock[i*8+j]);
        }        
    }
        
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV g_AddError_EMB_Overflow_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1, y0;
    U32_WMV err_even, err_odd;
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

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_AddNull_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV u0,u1,u2,u3, y0,y1;
    
    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        
        u0 = pRef[0];
        u1 = pRef[0 + 5];
        u2 = pRef[1];
        u3 = pRef[1 + 5];
        

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

        ppxlcDst += iPitch;
    }
}

Void_WMV g_AddNull_SSIMD_C_new(U8_WMV* ppxlcDst,I32_WMV iDststride, U8_WMV* pRef , I32_WMV iRefstride)
{
    int i,j;
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            ppxlcDst[i*iDststride+j] = pRef[i*iRefstride+j];
        }
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV g_AddError_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I32_WMV iy;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1,u2,u3, y0,y1;
    U32_WMV err_even, err_odd;
    
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

        
        ppxlcDst += iPitch;
        
    }
    
    DEBUG_STATEMENT(g_AddErr_cnt++;)
        
    if(err_overflow & 0xff00ff00) {

        ppxlcDst -= 8*iPitch;
        pRef -= 10*8;
        pErrorBuf -= 16;
        g_AddError_EMB_Overflow_C(ppxlcDst, pRef , pErrorBuf, iPitch);
    }
        
}

Void_WMV g_AddError_SSIMD_C_new(U8_WMV* ppxlcDst, 
                                I32_WMV iDststride,
                                U8_WMV* pRef ,
                                I32_WMV iRefstride,
                                I16_WMV* pErrorBlock)
{
    I32_WMV iy,ix;

    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        for(ix = 0; ix < BLOCK_SIZE; ix++) 
        {
            ppxlcDst[iy*iDststride+ix] = CLIP(pErrorBlock[iy*8+ix] + pRef[iy*iRefstride+ix]);
        }
    }    
}
#if 0
Void_WMV g_AddError_SSIMD_C_new2(U8_WMV* ppxlcDst, U8_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch)
{
    I32_WMV iy,ix;
    U32_WMV err_overflow = 0;
    U32_WMV u0,u1,u2,u3, y0,y1;
    U32_WMV err_even, err_odd;

    U8_WMV *pRefTmp = (U8_WMV *)pRef;
    I16_WMV *pErrTmp = (I16_WMV *)pErrorBuf;
    I32_WMV ref,err,sum=0;

    for (iy = 0; iy < BLOCK_SIZE; iy++) 
    {
        for(ix = 0; ix < BLOCK_SIZE; ix++) 
        {
            ref = pRefTmp[ix];
            err = pErrTmp [ix];
            sum = ref + err;
            ppxlcDst[ix] = CLIP(sum);
        }
        pRefTmp += 40;
        pErrTmp += 8;
        ppxlcDst += iPitch;
    }    
}
#endif

Void_WMV  g_InterpolateBlockBilinear_SSIMD_11_C (const U8_WMV *pSrc,
                                                 I32_WMV iSrcStride, 
                                                 U8_WMV *pDst, 
                                                 I32_WMV iXFrac, 
                                                 I32_WMV iYFrac,
                                                 I32_WMV iRndCtrl, 
                                                 Bool_WMV b1MV)
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

Void_WMV  g_InterpolateBlockBilinear_SSIMD_10_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
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

Void_WMV  g_InterpolateBlockBilinear_SSIMD_01_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
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
Void_WMV  g_InterpolateBlockBilinear_SSIMD_C (const U8_WMV *pSrc, 
                                              I32_WMV iSrcStride, 
                                              U8_WMV *pDst, 
                                              I32_WMV iXFrac, 
                                              I32_WMV iYFrac, 
                                              I32_WMV iRndCtrl, 
                                              Bool_WMV b1MV)
{
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
}

static const I8_WMV voSubsampleBilinear[] = {
    4, 0,       3, 1,
    2, 2,      1, 3,
};


Void_WMV  g_InterpolateBlockBilinear_SSIMD_C_new (const U8_WMV *pSrc, 
                                                  I32_WMV iSrcStride, 
                                                  U8_WMV *pDst, 
                                                  I32_WMV iDstStride, 
                                                  I32_WMV iXFrac, 
                                                  I32_WMV iYFrac, 
                                                  I32_WMV iRndCtrl, 
                                                  Bool_WMV b1MV)
{
    I32_WMV i, j, k0,k1;
	I32_WMV iNumLoops = 8<<b1MV;
    U8_WMV* pDstTmp =  (U8_WMV*)pDst;
    //I32_WMV pFilter[32 + 1];
    //I8_WMV* pH = voSubsampleBilinear + 2 * (iXFrac&3);
    //I8_WMV* pV = voSubsampleBilinear + 2 * (iYFrac&3);
    if(b1MV)
        b1MV = b1MV;

	iRndCtrl = 8 - ( iRndCtrl&0xff);    


    for (i = 0; i < iNumLoops; i++) 
    {
        const U8_WMV  *pT ;
		I16_WMV PF0, PF1, PF2, PF3, PF4;
      
        PF0 = (pSrc[0]<<2) + (pSrc[iSrcStride] - pSrc[0]) * iYFrac;

        for (j = 0; j < iNumLoops; j += 4)
        {
            pT = pSrc + j;
			PF1 = (pT[1] << 2) + (pT[iSrcStride+1] - pT[1]) * iYFrac;
			PF2 = (pT[2] << 2) + (pT[iSrcStride+2] - pT[2]) * iYFrac;
			PF3 = (pT[3] << 2) + (pT[iSrcStride+3] - pT[3]) * iYFrac;
			PF4 = (pT[4] << 2) + (pT[iSrcStride+4] - pT[4]) * iYFrac;

            k0 = ((PF0 << 2) + (PF1 - PF0) * iXFrac + iRndCtrl) >> 4;
            k1 = ((PF1 << 2) + (PF2 - PF1) * iXFrac + iRndCtrl) >> 4;

            pDstTmp[j] = (U8_WMV)k0;
            pDstTmp[j+1] = (U8_WMV)k1;

            k0 = ((PF2 << 2) + (PF3 - PF2) * iXFrac + iRndCtrl) >> 4;
            k1 = ((PF3 << 2) + (PF4 - PF3) * iXFrac + iRndCtrl) >> 4;

            pDstTmp[j+2] = (U8_WMV)k0;
            pDstTmp[j+3] = (U8_WMV)k1;

            PF0 = PF4;
        }
        pSrc += iSrcStride;
        pDstTmp += iDstStride;
    }
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

Void_WMV g_InterpolateBlockBicubicOverflow_EMB_new(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV)
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


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlock_00_SSIMD_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV,U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    I32_WMV i;
    
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

I32_WMV g_InterpolateBlock_00_SSIMD_C_new (U8_WMV *pSrc, 
                                           I32_WMV iSrcStride, 
                                           U8_WMV *pDst,
                                           I32_WMV iDstStride,
                                           EMB_PBMainLoop * pMainLoop, 
                                           I32_WMV iXFrac, 
                                           I32_WMV iYFrac, 
                                           Bool_WMV b1MV) // iXFrac == 0; iYFrac == 0
{
    U8_WMV* pDstTmp = (U8_WMV*)pDst;
#ifdef VOARMV7
    if(b1MV == 0)
        voMemoryCpy_8x8_ARMV7(pSrc,iSrcStride,pDstTmp,iDstStride);
    else
        voMemoryCpy_16x16_ARMV7(pSrc,iSrcStride,pDstTmp,iDstStride);
    
#else
    {
         I32_WMV i,j;
        if(b1MV == 0)
        {
            for (i = 0; i < 8; i++) 
            {
                for (j = 0; j< 8; j++) 
                {
                    pDstTmp[j] = pSrc[j];
                }        

                pSrc += iSrcStride;
                pDstTmp += (iDstStride);
            }        
        }
        else
        {        
            for (i = 0; i < 16; i++) 
            {
                for (j = 0; j< 16; j++) 
                {
                    pDstTmp[j] = pSrc[j];
                }        

                pSrc += iSrcStride;
                pDstTmp +=  (iDstStride);
            } 
        }
    }
#endif

    //ReOrgnaizeCompBuffer((U16_WMV *)pDst);

    return 0;    
}


//#pragma code_seg (EMBSEC_PML)
Void_WMV g_NewVertFilter0LongNoGlblTbl_ARMV4(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
#define m_ui80008000  0x80008000//( *(U32_WMV *)pMainLoop->m_rgiSubsampleBicubic)


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_0x_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    //    I32_WMV i;
    U32_WMV overflow = 0;
    
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    
    pSrc -= iSrcStride;

//#ifdef VOARMV7
//    overflow = ARMV7_g_NewVertFilterX_Only(pSrc,  iSrcStride, pDst, 6, pMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
//#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 6, pMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
//#endif

    G_PREFETCH(pSrc, iSrcStride);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)


    return overflow & pMainLoop->m_uic000c000;

}
I32_WMV g_InterpolateBlockBicubic_0x_SSIMD_new (U8_WMV *pSrc, 
                                                I32_WMV iSrcStride, 
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop, 
                                                I32_WMV iXFrac, 
                                                I32_WMV iYFrac,  
                                                Bool_WMV b1MV ) // iXFrac == 0; iYFrac == 0
{
    //    I32_WMV i;
    //U32_WMV overflow = 0;
    
    I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
   
    
    pSrc -= iSrcStride;

#ifdef VOARMV7
    g_NewVertFilterX_Only_6_ARMV7(pSrc,  iSrcStride, pDst,iDstStride, 6, pMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV);
#else
    g_NewVertFilterX_Only_C(pSrc,  iSrcStride, pDst,iDstStride, 6, pMainLoop->m_ui001f001fPiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV);
#endif

    return 0;//overflow & pMainLoop->m_uic000c000;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_x0_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;

    pSrc --;

    g_NewVertFilter0LongNoGlblTbl_Fun(pSrc, iSrcStride, pDst, b1MV);
    

     G_PREFETCH(pSrc, iSrcStride);

    overflow = g_NewHorzFilterX_Fun(pDst, 6 , pMainLoop->m_ui00200020MiRndCtl, pH, b1MV);
    
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)
        
    return overflow & pMainLoop->m_uic000c000;
}

I32_WMV g_InterpolateBlockBicubic_x0_SSIMD_new (U8_WMV *pSrc,
                                                I32_WMV iSrcStride, 
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop, 
                                                I32_WMV iXFrac,
                                                I32_WMV iYFrac, 
                                                Bool_WMV b1MV ) // iXFrac == 0; iYFrac == 0
{
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;

    pSrc --;

#ifdef VOARMV7
    g_NewHorzFilterX_Only_6_ARMV7(pSrc, iSrcStride,pDst, iDstStride,6 , pMainLoop->m_ui00200020MiRndCtl, pH, b1MV);
#else
    g_NewHorzFilterX_Only_C(pSrc, iSrcStride,pDst, iDstStride,6 , pMainLoop->m_ui00200020MiRndCtl, pH, b1MV);
#endif    
        
    return 0;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_xx_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow ;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
   
    
    pSrc -= (iSrcStride + 1);
     
    overflow = g_NewVertFilterX_Fun(pSrc, iSrcStride, (U8_WMV *)pDst, 5, pMainLoop->m_ui000f000fPiRndCtl,  pV, (2<<b1MV)+1,  pMainLoop->m_uiffff07ff, b1MV, pTbl);

    G_PREFETCH(pSrc, iSrcStride);

   overflow |= g_NewHorzFilterX_Fun(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
 
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return (overflow & m_ui80008000);
}

I32_WMV g_InterpolateBlockBicubic_xx_SSIMD_new (U8_WMV *pSrc, 
                                                I32_WMV iSrcStride,
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop,
                                                I32_WMV iXFrac,
                                                I32_WMV iYFrac,
                                                Bool_WMV b1MV) // iXFrac == 0; iYFrac == 0
{
    I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
   
    
    pSrc -= (iSrcStride + 1);

#ifdef VOARMV7
    voInterpolateBlockBicubic_ARMV7(pSrc, iSrcStride, pDst, iDstStride,5,
        pMainLoop->m_ui000f000fPiRndCtl,
        pMainLoop->m_ui00400040MiRndCtl,
        pV,
        pH,
        b1MV);
#else    
    {
        I16_WMV *pMCBufferTmp = (I16_WMV*)pMainLoop->prgMotionCompBuffer_EMB;
        g_NewVertFilterX_Fun_new(pSrc, iSrcStride, pMCBufferTmp, 20, 5, pMainLoop->m_ui000f000fPiRndCtl,  pV, (2<<b1MV)+1,  pMainLoop->m_uiffff07ff, b1MV);
        g_NewHorzFilterX_Fun_new(pMCBufferTmp,20, pDst, iDstStride, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
    }
#endif
    return 0;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_02_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    
    pSrc -= iSrcStride;
//#ifdef VOARMV7
//    overflow = ARMV7_g_NewVertFilterX_Only(pSrc,  iSrcStride, pDst, 4, pMainLoop->m_ui00070007PiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
//#else
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 4, pMainLoop->m_ui00070007PiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV, pTbl);
//#endif
    G_PREFETCH(pSrc, iSrcStride);
    
    DEBUG_STATEMENT(g_BiCubic_0x_cnt++;)

    return overflow & pMainLoop->m_uif000f000;

}
I32_WMV g_InterpolateBlockBicubic_02_SSIMD_new (U8_WMV *pSrc, 
                                                I32_WMV iSrcStride, 
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop, 
                                                I32_WMV iXFrac, 
                                                I32_WMV iYFrac, 
                                                Bool_WMV b1MV) // iXFrac == 0; iYFrac == 0
{
    I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    //unsigned char tmp[24*24];
    //unsigned char tmp2[24*24];
   
    pSrc -= iSrcStride;

#ifdef VOARMV7
    g_NewVertFilterX_Only_4_ARMV7(pSrc,  iSrcStride, pDst, iDstStride, 4, pMainLoop->m_ui00070007PiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV);
#else
    g_NewVertFilterX_Only_C(pSrc,  iSrcStride, pDst, iDstStride, 4, pMainLoop->m_ui00070007PiRndCtl,  pV, 2<<b1MV, pMainLoop->m_ui00ff00ff, b1MV);
#endif
    return 0;
}
//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_20_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    
    pSrc --;

     g_NewVertFilter0LongNoGlblTbl_Fun(pSrc, iSrcStride, pDst, b1MV);
   

     G_PREFETCH(pSrc, iSrcStride);
    
    overflow = g_NewHorzFilterX_Fun(pDst, 4, pMainLoop->m_ui00080008MiRndCtl, pH, b1MV);
    
    DEBUG_STATEMENT( g_BiCubic_x0_cnt++;)
        
    return overflow & pMainLoop->m_uif000f000;

}

I32_WMV g_InterpolateBlockBicubic_20_SSIMD_new (U8_WMV *pSrc, 
                                                I32_WMV iSrcStride, 
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop,
                                                I32_WMV iXFrac,
                                                I32_WMV iYFrac,  
                                                Bool_WMV b1MV)
{
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;

    pSrc --;

#ifdef VOARMV7
    g_NewHorzFilterX_Only_4_ARMV7(pSrc, iSrcStride,pDst,iDstStride, 4, pMainLoop->m_ui00080008MiRndCtl, pH, b1MV);
#else    
    g_NewHorzFilterX_Only_C(pSrc, iSrcStride,pDst,iDstStride, 4, pMainLoop->m_ui00080008MiRndCtl, pH, b1MV); 
#endif

        
    return 0;

}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_x2_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    
    pSrc -= (iSrcStride + 1);
 
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 3, pMainLoop->m_ui00030003PiRndCtl, pV, (2<<b1MV)+1, pMainLoop->m_uiffff1fff, b1MV, pTbl);

     G_PREFETCH(pSrc, iSrcStride);

    overflow |= g_NewHorzFilterX_Fun(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);

    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return (overflow & m_ui80008000);

}

I32_WMV g_InterpolateBlockBicubic_x2_SSIMD_new (U8_WMV *pSrc, 
                                                I32_WMV iSrcStride,
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop, 
                                                I32_WMV iXFrac, 
                                                I32_WMV iYFrac, 
                                                Bool_WMV b1MV)
{
    I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;

    
    pSrc -= (iSrcStride + 1);
#ifdef VOARMV7
    voInterpolateBlockBicubic_ARMV7(pSrc, iSrcStride, pDst, iDstStride,3,
        pMainLoop->m_ui00030003PiRndCtl,
        pMainLoop->m_ui00400040MiRndCtl,
        pV,
        pH,
        b1MV);
#else 
    {
        I16_WMV *pMCBufferTmp = (I16_WMV*)pMainLoop->prgMotionCompBuffer_EMB;    
        g_NewVertFilterX_Fun_new(pSrc,  iSrcStride, pMCBufferTmp,20, 3, pMainLoop->m_ui00030003PiRndCtl, pV, (2<<b1MV)+1, pMainLoop->m_uiffff1fff, b1MV);
        g_NewHorzFilterX_Fun_new(pMCBufferTmp,20,pDst,iDstStride, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
    }
#endif

    return 0;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_2x_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow = 0;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    
    pSrc -= (iSrcStride + 1);
    
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, (U8_WMV *)pDst, 3,  pMainLoop->m_ui00030003PiRndCtl,  pV, (2<<b1MV) +1, pMainLoop->m_uiffff1fff, b1MV, pTbl);

     G_PREFETCH(pSrc, iSrcStride);

    overflow |= g_NewHorzFilterX_Fun(pDst, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
   
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return (overflow & m_ui80008000);

}

I32_WMV g_InterpolateBlockBicubic_2x_SSIMD_new (U8_WMV *pSrc,
                                                I32_WMV iSrcStride,
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop, 
                                                I32_WMV iXFrac, 
                                                I32_WMV iYFrac,
                                                Bool_WMV b1MV)
{
    I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
      
    pSrc -= (iSrcStride + 1);
#ifdef VOARMV7
    voInterpolateBlockBicubic_ARMV7(pSrc, iSrcStride, pDst, iDstStride,3,
        pMainLoop->m_ui00030003PiRndCtl,
        pMainLoop->m_ui00400040MiRndCtl,
        pV,
        pH,
        b1MV);
#else   
    {
        I16_WMV *pMCBufferTmp = (I16_WMV*)pMainLoop->prgMotionCompBuffer_EMB;
        g_NewVertFilterX_Fun_new(pSrc,  iSrcStride, pMCBufferTmp, 20, 3,  pMainLoop->m_ui00030003PiRndCtl,  pV, (2<<b1MV) +1, pMainLoop->m_uiffff1fff, b1MV);
        g_NewHorzFilterX_Fun_new(pMCBufferTmp,20,pDst,iDstStride, 7, pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
    }
#endif

    return 0;

}

//#pragma code_seg (EMBSEC_PML)
I32_WMV g_InterpolateBlockBicubic_22_SSIMD ( const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,  Bool_WMV b1MV, U16_WMV *pTbl) // iXFrac == 0; iYFrac == 0
{
    U32_WMV overflow ;
    const I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    const I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    
    pSrc -= (iSrcStride + 1);
    
    //either one working        
    overflow = g_NewVertFilterX_Fun(pSrc,  iSrcStride, pDst, 1, pMainLoop->m_iRndCtrl,  pV,  (2<<b1MV)+1, pMainLoop->m_uiffff7fff, b1MV, pTbl);

     G_PREFETCH(pSrc, iSrcStride);

    overflow |= g_NewHorzFilterX_Fun(pDst, 7,  pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
  
    DEBUG_STATEMENT(g_BiCubic_xx_cnt++;)

    return (overflow & m_ui80008000);

}

I32_WMV g_InterpolateBlockBicubic_22_SSIMD_new ( U8_WMV *pSrc, 
                                                I32_WMV iSrcStride, 
                                                U8_WMV *pDst, 
                                                I32_WMV iDstStride,
                                                EMB_PBMainLoop * pMainLoop, 
                                                I32_WMV iXFrac, 
                                                I32_WMV iYFrac,  
                                                Bool_WMV b1MV) 
{
    I8_WMV *pH = pMainLoop->m_rgiSubsampleBicubic + 4 * iXFrac;
    I8_WMV *pV = pMainLoop->m_rgiSubsampleBicubic + 4 * iYFrac;
    
    pSrc -= (iSrcStride + 1);
    
 #ifdef VOARMV7
    voInterpolateBlockBicubic_ARMV7(pSrc, iSrcStride, pDst, iDstStride,1,
        pMainLoop->m_iRndCtrl,
        pMainLoop->m_ui00400040MiRndCtl,
        pV,
        pH,
        b1MV);
#else    
    {
        I16_WMV *pMCBufferTmp = (I16_WMV*)pMainLoop->prgMotionCompBuffer_EMB;
        g_NewVertFilterX_Fun_new(pSrc,  iSrcStride, pMCBufferTmp, 20,1, pMainLoop->m_iRndCtrl,  pV,  (2<<b1MV)+1, pMainLoop->m_uiffff7fff, b1MV);
        g_NewHorzFilterX_Fun_new(pMCBufferTmp, 20, pDst, iDstStride, 7,  pMainLoop->m_ui00400040MiRndCtl, pH, b1MV);
    }
#endif
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

    int i=0;
    for(i=0;i<2;i++)
    {
        EMB_PBMainLoop *pPB = &pWMVDec->m_EMB_PMainLoop.PB[i];
        EMB_PBMainLoop *pPBB = &pWMVDec->m_EMB_BMainLoop.PB[i];

        pPB->m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_SSIMD_Fun;
        pPB->m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_SSIMD;
        pPB->m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_SSIMD;
        pPB->m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_SSIMD;
        
        pPB->m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_SSIMD;
        pPB->m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_SSIMD;
        pPB->m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_SSIMD;
        pPB->m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_SSIMD;
        
        pPB->m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_SSIMD;
        pPB->m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_SSIMD;
        pPB->m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_SSIMD;
        pPB->m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_SSIMD;       
     
        pPB->m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_SSIMD;
        pPB->m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_SSIMD;
        pPB->m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_SSIMD;
        pPB->m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_SSIMD;

        //new MC
        pPB->m_pInterpolateBicubic_new[0][0] = g_InterpolateBlock_00_SSIMD_Fun_new;
        pPB->m_pInterpolateBicubic_new[0][1] = g_InterpolateBlockBicubic_0x_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[0][2] = g_InterpolateBlockBicubic_02_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[0][3] = g_InterpolateBlockBicubic_0x_SSIMD_new;
        
        pPB->m_pInterpolateBicubic_new[1][0] = g_InterpolateBlockBicubic_x0_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[1][1] = g_InterpolateBlockBicubic_xx_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[1][2] = g_InterpolateBlockBicubic_x2_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[1][3] = g_InterpolateBlockBicubic_xx_SSIMD_new;
        
        pPB->m_pInterpolateBicubic_new[2][0] = g_InterpolateBlockBicubic_20_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[2][1] = g_InterpolateBlockBicubic_2x_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[2][2] = g_InterpolateBlockBicubic_22_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[2][3] = g_InterpolateBlockBicubic_2x_SSIMD_new;
        
        pPB->m_pInterpolateBicubic_new[3][0] = g_InterpolateBlockBicubic_x0_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[3][1] = g_InterpolateBlockBicubic_xx_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[3][2] = g_InterpolateBlockBicubic_x2_SSIMD_new;
        pPB->m_pInterpolateBicubic_new[3][3] = g_InterpolateBlockBicubic_xx_SSIMD_new;

        //for B frame

        pPBB->m_pInterpolateBicubic[0][0] = g_InterpolateBlock_00_SSIMD_Fun;
        pPBB->m_pInterpolateBicubic[0][1] = g_InterpolateBlockBicubic_0x_SSIMD;
        pPBB->m_pInterpolateBicubic[0][2] = g_InterpolateBlockBicubic_02_SSIMD;
        pPBB->m_pInterpolateBicubic[0][3] = g_InterpolateBlockBicubic_0x_SSIMD;
        
        pPBB->m_pInterpolateBicubic[1][0] = g_InterpolateBlockBicubic_x0_SSIMD;
        pPBB->m_pInterpolateBicubic[1][1] = g_InterpolateBlockBicubic_xx_SSIMD;
        pPBB->m_pInterpolateBicubic[1][2] = g_InterpolateBlockBicubic_x2_SSIMD;
        pPBB->m_pInterpolateBicubic[1][3] = g_InterpolateBlockBicubic_xx_SSIMD;
        
        pPBB->m_pInterpolateBicubic[2][0] = g_InterpolateBlockBicubic_20_SSIMD;
        pPBB->m_pInterpolateBicubic[2][1] = g_InterpolateBlockBicubic_2x_SSIMD;
        pPBB->m_pInterpolateBicubic[2][2] = g_InterpolateBlockBicubic_22_SSIMD;
        pPBB->m_pInterpolateBicubic[2][3] = g_InterpolateBlockBicubic_2x_SSIMD;
        
        pPBB->m_pInterpolateBicubic[3][0] = g_InterpolateBlockBicubic_x0_SSIMD;
        pPBB->m_pInterpolateBicubic[3][1] = g_InterpolateBlockBicubic_xx_SSIMD;
        pPBB->m_pInterpolateBicubic[3][2] = g_InterpolateBlockBicubic_x2_SSIMD;
        pPBB->m_pInterpolateBicubic[3][3] = g_InterpolateBlockBicubic_xx_SSIMD;

        pPBB->m_pInterpolateBicubic_new[0][0] = g_InterpolateBlock_00_SSIMD_Fun_new;
        pPBB->m_pInterpolateBicubic_new[0][1] = g_InterpolateBlockBicubic_0x_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[0][2] = g_InterpolateBlockBicubic_02_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[0][3] = g_InterpolateBlockBicubic_0x_SSIMD_new;
        
        pPBB->m_pInterpolateBicubic_new[1][0] = g_InterpolateBlockBicubic_x0_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[1][1] = g_InterpolateBlockBicubic_xx_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[1][2] = g_InterpolateBlockBicubic_x2_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[1][3] = g_InterpolateBlockBicubic_xx_SSIMD_new;
        
        pPBB->m_pInterpolateBicubic_new[2][0] = g_InterpolateBlockBicubic_20_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[2][1] = g_InterpolateBlockBicubic_2x_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[2][2] = g_InterpolateBlockBicubic_22_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[2][3] = g_InterpolateBlockBicubic_2x_SSIMD_new;
        
        pPBB->m_pInterpolateBicubic_new[3][0] = g_InterpolateBlockBicubic_x0_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[3][1] = g_InterpolateBlockBicubic_xx_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[3][2] = g_InterpolateBlockBicubic_x2_SSIMD_new;
        pPBB->m_pInterpolateBicubic_new[3][3] = g_InterpolateBlockBicubic_xx_SSIMD_new;
    }


}



static U32_WMV test_err_cliping(U32_WMV u)
{
    U32_WMV v;
    
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





