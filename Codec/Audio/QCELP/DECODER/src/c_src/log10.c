#ifdef DOC_FILE
/******************************************************************************
 File:          log10.cpp

 Version:       DSPSE Release   1.30    05 NOV 96

 Description:   This file contains a log10 routine in both
                fixed-point (FACT) and floating point.

 Revisions:     05 NOV 96   John Riley

 Copyright:     (c) Copyright 1996  DSP Software Engineering, Inc.
                All Rights Reserved. Duplication Strictly Prohibited.
******************************************************************************/
#else

/***************************************************************************/
/* Include files                                                           */
/***************************************************************************/
//#include <math.h>
#include"qcelp.h"
#include"qcelp13.h"

/***************************************************************************/
/* Constant definitions                                                    */
/***************************************************************************/
#define LOGE_2_OVER_LOGE_10             0.3010300
#define LOGE_2_OVER_LOGE_10_16         (Int16) 9864  //Q15

extern Int16 logtbl_16[256];
/***************************************************************************/
/* Macro definitions                                                       */
/***************************************************************************/

/****************************************************************************
Integer Subroutines
****************************************************************************/

/* Q15 mantissa betw 0.5 and 1. and integer shift in and Q11,16 out */
//I16  QC13_log10(I16 mant, Int16 shift)
Int16  QC13_log10(Int16 mant, Int16 shift)
{
    Int16 index;
    Int16 output;
    Int32 arA;
   
//    frct = 1;
//    ovm= 0;
//    sxm = 1;

    if (mant <= 0L)
    {
        return 0;
    }

    arA = (Int32)(mant<<(Int16)16);
    arA -= 0x40000000l;
    arA>>=6;
    arA+=0x00008000L;
    arA>>=16;
    index=(Int16)arA;
    if (index > (Int16)255)
        index = (Int16)255;
    output = logtbl_16[index];

    arA = ((Int32)shift * (Int32)LOGE_2_OVER_LOGE_10_16) << 1; //Q15/2=Q14
    arA += (Int32)output;


    arA >>= 5;  // Q11
    if(arA>(Int32)0x7fffl)
        arA=(Int32)0x7fffl;

    return (Int16)arA;
}

#endif

