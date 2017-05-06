//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//+-------------------------------------------------------------------------
//
//  File:       arm_stub.c
//
//  The common function used by ARM optimization in C
//
//--------------------------------------------------------------------------

#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "msaudio.h"
#include "msaudiodec.h"
#include "macros.h"

//#ifdef WMA_TARGET_ARM

static const double dPI = PI;       // 3.1415926535897932384626433832795;

#define CHECK_POWER_OF_TWO(x) ((((x)-1) & (x)) == 0)


void prvDctIV_ARM(CoefType* rgiCoef, int nLog2cSB, 
                  CoefType CR, CoefType CI, CoefType CR1, 
                  CoefType CI1, CoefType STEP, CoefType CR2 );

//some C stub functions used by ARM optimization
#if WMA_OPT_FFT_ARM 
BP2Type FFT4DCT16_STEP(Int nLog2np, BP2Type* pCR)
{
    BP2Type STEP;
    I32 np = (1<<nLog2np);

    *pCR = BP2_FROM_FLOAT(cos((float)(dPI/np)));
    STEP = BP2_FROM_FLOAT(2*sin(-dPI/np));
    return STEP;
}


#if 0
void prvDctIV_ARM( CoefType* rgiCoef, int nLog2cSB, 
                   CoefType CR, CoefType CI, CoefType CR1, CoefType CI1, CoefType STEP, CoefType CR2 )
{
    int iFFTSize = 1<<(nLog2cSB-1);
    CoefType iTr, iTi, iBr, iBi;
    BP1Type UR, UI;
    BP1Type CI2  = -DIV2(STEP);  
    CoefType *piCoefTop, *piCoefBottom;
    Int i;

    piCoefTop          = rgiCoef;               
    piCoefBottom       = rgiCoef + (1<<nLog2cSB) - 1;

    for (i = iFFTSize>>1; i > 0; i--) {
        // The following instructions have be re-rodered for the EVC 3.0
        // for the MIPS platform.  Due to a compiler error, THEY WILL NOT
        // WORK in some other, still valid configurations.  Ryan White 7/13/01
        
        iBi = piCoefBottom[0];
        iTr = piCoefTop[0];
        piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iBi);
        piCoefBottom[0] = piCoefTop[1];

        piCoefTop[1] = MULT_BP1(CR,iBi) + MULT_BP1(CI,iTr);

        // rotate angle by -b = -pi/cSubband
        // recursion: cos(a-b) = cos(a+b) - 2*sin(b)*sin(a)
        // and:       sin(a-b) = sin(a+b) + 2*sin(b)*cos(a)
        UR = CR1 - MULT_BP1(STEP,CI);
        UI = CI1 + MULT_BP1(STEP,CR);
        CR1 = CR;  CR = UR;
        CI1 = CI;  CI = UI;

        piCoefTop += 2;
        piCoefBottom -= 2;
    }

    for (i = iFFTSize>>1; i > 0; i--) {
        iTr = piCoefTop[0];
        iTi = piCoefTop[1];
        piCoefTop[0] = MULT_BP1(CR,iTr) - MULT_BP1(CI,iTi);
        piCoefTop[1] = MULT_BP1(CR,iTi) + MULT_BP1(CI,iTr);

        // rotate angle by b = -pi/cSubband
        UR = CR1 - MULT_BP1(STEP,CI);
        UI = CI1 + MULT_BP1(STEP,CR);
        CR1 = CR;  CR = UR;
        CI1 = CI;  CI = UI;

        piCoefTop += 2;
    }

    //Call the FFT explicitly 
    //We can assume the frame size is always power of 2 here
    //prvFFT4DCT(NULL, rgiCoef, (fPowerOfTwo ? nLog2cSB - 1 : iFFTSize), FFT_FORWARD);
    prvFFT4DCT(NULL, rgiCoef, nLog2cSB - 1, FFT_FORWARD);

    // post FFT demodulation 
    // using R[i], I[i] as real and imaginary parts of complex point i
    // and C(i), S(i) are cos(i*pi/cSubband) and sin(i*pi/cSubband), and N = cSubband/2
    // R[0] = C(0) R[0] - S(0) I[0]
    // I[0] = -S(-(N-1)) R[N-1] - C(-(N-1)) I[N-1]
    // R[1] = C(1) R[1] - S(-1) I[N-1]
    // I[1] = -S(-(N-2)) R[N-2] - C(-(N-2)) I[N-2]
    // ...
    // R[N-2] = C(-(N-2)) R[N-2] - S(-(N-2)) I[N-2]
    // I[N-2] = -S(-1) R[1] - C(-1) I[1]
    // R[N-1] = C(-(N-1)) R[N-1] - S(-(N-1)) I[N-1]
    // R[N-1] = -S(0) R[0] - C(0) I[0]
    // and where 90 - angle trig formulas reveal:
    // C(-(N-i)) == -S(-i) and S(-(N-i)) = -C(-i)

    piCoefTop      = rgiCoef;            //reuse this pointer; start from head; represent the real part
    piCoefBottom   = rgiCoef + (1<<nLog2cSB)  - 2; //reuse this pointer; start from tail; represent the imag part

    CR = BP1_FROM_FLOAT(1);             //one
    CI = 0;                             //zero

    for (i = iFFTSize>>1; i > 0; i--) {
        iTr = piCoefTop[0];
        iTi = piCoefTop[1];
        iBr = piCoefBottom[0];
        iBi = piCoefBottom[1];

        piCoefTop[0] =  MULT_BP1(CR,iTr) -  MULT_BP1(CI,iTi);
        piCoefBottom[1] =  MULT_BP1(-CI,iTr) - MULT_BP1(CR,iTi);
        // INTEGER_ONLY( iMagnitude |= abs(piCoefTop[0]) );
        // INTEGER_ONLY( iMagnitude |= abs(piCoefBottom[1]) );

        // rotate angle by -b = -pi/cSubband
        // recursion: cos(a-b) = cos(a+b) - 2*sin(b)*sin(a)
        // and:       sin(a-b) = sin(a+b) + 2*sin(b)*cos(a)
        UR = CR2 - MULT_BP1(STEP,CI);
        UI = CI2 + MULT_BP1(STEP,CR);
        CR2 = CR;  CR = UR;
        CI2 = CI;  CI = UI;

        // note that cos(-(cSubband/2 - i)*pi/cSubband ) = -sin( -i*pi/cSubband )
        piCoefTop[1] = MULT_BP1(CR,iBr) + MULT_BP1(CI,iBi);
        piCoefBottom[0] = MULT_BP1(-CI,iBr) +  MULT_BP1(CR,iBi);
        
        // INTEGER_ONLY( iMagnitude |= abs(piCoefTop[1]) );
        // INTEGER_ONLY( iMagnitude |= abs(piCoefBottom[0]) );

        piCoefTop += 2;
        piCoefBottom -= 2;
    }

}
#endif



//WMARESULT auDctIV(CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude,
WMARESULT auDctIV(CoefType* rgiCoef, BP2Type fltAfterScaleFactor, U32 *piMagnitude,
                  const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted)
{
    
    BP1Type CR, CI, STEP, CR1, CI1, CR2, CI2;
    Int iFFTSize, cSB, nLog2SB;
    BP1Type  iFac;
    Bool fPowerOfTwo;
    const SinCosTable* pSinCosTable; 
    double fac;
    const double dPI3Q = 3*PI/4;                        // PI (-1/4 + 1)
    const double dPIby4 = PI/4;
#if defined(BUILD_INTEGER)
    CoefType *piCoefTop;
    const double d1p0   = 1.0 * NF2BP1;
    const double d2p0   = 2.0 * NF2BP1;
    //const double d1Hp0  = 0.5 * NF2BP1;
    const double d1Qp0  = 0.25* NF2BP1;
    Int nFacExponent, i;
    U32 iMagnitude = 0;
#else  // must be BUILD_INT_FLOAT
    const double d1p0   = 1.0;
    const double d2p0   = 2.0;
    //const double d1Hp0  = 0.5;
    const double d1Qp0  = 0.25;
#endif  // BUILD_INTEGER or BUILD_INT_FLOAT

#ifdef INTEGER_ENCODER
    Int iScaleFactor;
#endif

    // m_cSubbandAdjusted below deals with the need to scale transform results to compensate the fact 
    // that we're inv transforming coefficients from a transform that was twice or half our size

    cSB = cSubbandAdjusted;
    iFFTSize = cSB/2;
    nLog2SB = LOG2( cSB );
    fPowerOfTwo = CHECK_POWER_OF_TWO(iFFTSize);


    // fltAfterScaleFactor:  V2: 2/cSB == 1/iFFTSize and V1: sqrt(2/cSB)
#if defined(BUILD_INTEGER)
//    fac = fltAfterScaleFactor * cSB * d1Qp0;
    fac = FLOAT_FROM_BP2(fltAfterScaleFactor) * cSB * d1Qp0;	
    for( nFacExponent = 0; fabs(fac) > d1p0; nFacExponent++ )
        fac /= 2.0f;        // SH warning CBE4717 here is ignorable
#else   // must be BUILD_INT_FLOAT
    fac = fltAfterScaleFactor;
#endif

    // initialize sin/cos recursion
    // note this style of recurrsion is more accurate than Numerical Recipies 5.5.6
    if ( 64 <= cSB && cSB <= 2048 && fPowerOfTwo )
    {
        pSinCosTable = rgSinCosTables[cSB>>7];
#       if defined(BUILD_INTEGER)
            iFac = (I32)ROUNDD( fac );
#       else  // must be BUILD_INT_FLOAT
            iFac = (BP1Type)fac;
#       endif // BUILD_INTEGER or BUILD_INT_FLOAT
        // initial cosine/sine values
        CR =  MULT_BP1(iFac,pSinCosTable->cos_PIby4cSB);        // CR = (I32)(fac*cos(-PI/(4*m_cSubband)) * NF2BP1)
        CI = -MULT_BP1(iFac,pSinCosTable->sin_PIby4cSB);        // CI = (I32)(fac*sin(-PI/(4*m_cSubband)) * NF2BP1)
        // prior cosine/sine values to init Pre-FFT recurrsion trig( -PI/(4*M) - (-PI/M ) = trig( 3*PI/(4*M) )
        CR1 =  MULT_BP1(iFac,pSinCosTable->cos_3PIby4cSB);  // CR = (I32)(fac*cos(+3*PI/(4*m_cSubband)) * NF2BP1)
        CI1 =  MULT_BP1(iFac,pSinCosTable->sin_3PIby4cSB);  // CI = (I32)(fac*sin(+3*PI/(4*m_cSubband)) * NF2BP1)
        // rotation step for both recurrsions
        STEP = -pSinCosTable->two_sin_PIbycSB;              // STEP = 2*sin(-PI/m_cSubband) 
        // prior cosine/sine values to init Post-FFT recurrsion
        CR2 =  pSinCosTable->cos_PIbycSB;                   // CR = (I32)(cos( PI/m_cSubband) * NF2BP1)
        CI2 =  pSinCosTable->sin_PIbycSB;                   // CI = (I32)(sin( PI/m_cSubband) * NF2BP1)
    }
    else
    {   // not normally needed in decoder, here for generality
        double dA, dSB;

        dA = -dPIby4 / (dSB=cSB);
        CR = (BP1Type)( fac * cos( dA ) );
        CI = (BP1Type)( fac * sin( dA ) );
        dA = dPI3Q / dSB;
        CR1 = (BP1Type)( fac * cos( dA ) );
        CI1 = (BP1Type)( fac * sin( dA ) );
        dA = -dPI / dSB;
        STEP = (BP1Type)( d2p0 * sin( dA ) );
        CR2  = (BP1Type)( d1p0 * cos( dA ) );
        CI2  = -STEP/2;  
    }

//	printf("rgiCoef, 0x%x, 0x%x, 0x%x, 0x%x,\n ", rgiCoef[0], rgiCoef[1], rgiCoef[2], rgiCoef[3]);
    prvDctIV_ARM( rgiCoef, nLog2SB, CR, CI, CR1, CI1, STEP, CR2 );
//    printf("prvDctIV_ARM() success!\n");

#if defined(BUILD_INTEGER)
    if ( nFacExponent > 0 )
    {   // This scaling needed in v1 bit-streams
        piCoefTop      = rgiCoef; 
        iMagnitude <<= nFacExponent;
        for( i = cSB; i > 0; i-- )
        {
            *piCoefTop++ <<= nFacExponent;
        }
    }
#endif

    if (NULL != piMagnitude)
        *piMagnitude = INTEGER_OR_INT_FLOAT( iMagnitude, 0 );
#ifdef COEF64BIT
    assert(piMagnitude == NULL);
    if (NULL != piMagnitude) return WMA_E_NOTSUPPORTED;
#endif

#if defined(DCT_OUT_PRINT) && defined(_DEBUG)
    if (iFrameNumber == DCT_OUT_PRINT)
    { DEBUG_BREAK(); }
#endif

    return WMA_OK;
}
#endif // WMA_OPT_FFT_ARM


#if !WMA_OPT_AURECON_ARM
WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
{
    WMARESULT hr = WMA_OK;
    I16 i, iCh;
    CoefType *pcfPCMBuf;
    PCMSAMPLE iPCMData;
    I16 *p16Output = (I16 *)piOutput;
    for( i = 0; i < cSamples; i++ )
    {
        for( iCh = 0; iCh < pau->m_cChannel; iCh++ )
        {
            PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iCh;

            pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;

            iPCMData = (PCMSAMPLE)pcfPCMBuf[i];

            if (iPCMData > (I32)0X00007FFF)
                iPCMData = (I16)0X7FFF;
            else if (iPCMData < (I32)0XFFFF8000)
                iPCMData = (I16)0X8000;        

//            *(((I16*)piOutput)++) = (I16)iPCMData;
            *p16Output++ = (I16)iPCMData;
        }
    }

    return hr;
}


WMARESULT auReconSample24_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples)
{
    WMARESULT hr = WMA_OK;
    I16 i, iCh;
    CoefType *pcfPCMBuf;
    PCMSAMPLE iPCMData;
    I8 *p8Output = (I8 *)piOutput;
    for( i = 0; i < cSamples; i++ )
    {
        for( iCh = 0; iCh < pau->m_cChannel; iCh++ )
        {
            PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iCh;

            pcfPCMBuf = (CoefType *)ppcinfo->m_rgiPCMBuffer;

            iPCMData = (PCMSAMPLE)pcfPCMBuf[i];

            if (iPCMData > (I32)0X007FFFFF)
                iPCMData = 0X007FFFFF;
            else if (iPCMData < (I32)0XFF800000)
                iPCMData = 0XFF800000;        

//            *(((I8*)piOutput)++) = *(((I8*)(&iPCMData)));
//            *(((I8*)piOutput)++) = *(((I8*)(&iPCMData)+1));
//            *(((I8*)piOutput)++) = *(((I8*)(&iPCMData)+2));
            *p8Output++ = *(((I8*)(&iPCMData)));
            *p8Output++ = *(((I8*)(&iPCMData)+1));
            *p8Output++ = *(((I8*)(&iPCMData)+2));
        }
    }

    return hr;
}
#endif //WMA_OPT_AURECON_ARM

#if WMA_OPT_INVERSQUAN_LOWRATE_ARM
void qstCalcQuantStep_ARM(QuantStepType *pqstQuantStep, Int iQSS) 
{
#if defined(BUILD_INTEGER)
    if ( iQSS < DBPOWER_TABLE_OFFSET )
    {   // This happens when iPower==0, not an important case, but return 10^(1/20) with 28 FractBits
        // It can also happen with a small NoisePower (-13 has been seen)
        if ( iQSS < 0 )
        {   // negative values of iQSS are being generated in the V5 encoder (LowRate in particular)
            pqstQuantStep->iFraction = (I32)(0.382943866392*(1<<QUANTSTEP_FRACT_BITS)),      // Average Fraction
            pqstQuantStep->iFracBits = QUANTSTEP_FRACT_BITS - ((-iQSS>>3)+1);                // Approximate Exponent
        }
        else
        {
            pqstQuantStep->iFraction = (I32)(0.869439785679*(1<<QUANTSTEP_FRACT_BITS));     // Average Fraction
            pqstQuantStep->iFracBits = QUANTSTEP_FRACT_BITS - ((iQSS>>3)+1);                // Approximate Exponent
        }
    }
    else if ( iQSS < (DBPOWER_TABLE_OFFSET+DBPOWER_TABLE_SIZE) )
    {   
        // *** normal case ***
        pqstQuantStep->iFraction = rgDBPower10[ iQSS - DBPOWER_TABLE_OFFSET ];
        pqstQuantStep->iFracBits = QUANTSTEP_FRACT_BITS - ((iQSS>>3)+4);    // implied FractBit scale for rgiDBPower table
        NormUInt( (UInt*)(&pqstQuantStep->iFraction), &pqstQuantStep->iFracBits, 0x3FFFFFFF );
    }
    else
    {   // This branch can handle out-of-range cases. 
        // rare - but used in some cases by encoder - e.g. Tough_16m_16, Tough_22m_22(?).
//		const double    c_dblTenExpToTwo = 3.3219280948873623478703194294894; // ln(10)/ln(2);
//		const double    c_dblPointNineNine = (1.0 - DBL_EPSILON);
//		double  dblQuantStep,  dblExponent;
//		Int     iNumWholeBits,  iNumFractBits;
//		dblExponent = (double)(iQSS/(Float)QUANTSTEP_DENOMINATOR);
//		dblQuantStep = pow (10, dblExponent);
//		iNumWholeBits = (Int)(dblExponent * c_dblTenExpToTwo + c_dblPointNineNine); // Round UP
//		iNumWholeBits = max(iNumWholeBits, 5);
//		iNumFractBits = max(31 - iNumWholeBits, TRANSFORM_FRACT_BITS);
//		if (dblQuantStep * (1 << iNumFractBits) <= UINT_MAX)
//			pqstQuantStep->iFraction = (I32)(dblQuantStep * (1 << iNumFractBits));
//		else
//			pqstQuantStep->iFraction = UINT_MAX;      // iQSS of 218 seen in encoding Tough 16kHz Stereo 16kbps
//		pqstQuantStep->iFracBits = iNumFractBits;
//		NormUInt( (UInt*)(&pqstQuantStep->iFraction), &pqstQuantStep->iFracBits, 0x3FFFFFFF );

		int dblExponent, iNumWholeBits, iNumFractBits;
		double dblQuantStep;
		dblExponent = (iQSS*85+1)>>1; //(iQSS)*850/QUANTSTEP_DENOMINATOR, 850 = ln(10)/ln(2)*256
		dblQuantStep = voWMAPow2(dblExponent);
		iNumWholeBits = (dblExponent>>8) + 1;
		iNumWholeBits = max(iNumWholeBits, 5);
		iNumFractBits = max(31 - iNumWholeBits, TRANSFORM_FRACT_BITS);
		if (dblQuantStep * (1 << iNumFractBits) <= UINT_MAX)
			pqstQuantStep->iFraction = (I32)(dblQuantStep * (1 << iNumFractBits));
		else
			pqstQuantStep->iFraction = UINT_MAX;      // iQSS of 218 seen in encoding Tough 16kHz Stereo 16kbps
		pqstQuantStep->iFracBits = iNumFractBits;
		NormUInt( (UInt*)(&pqstQuantStep->iFraction), &pqstQuantStep->iFracBits, 0x3FFFFFFF );
    }
    MONITOR_RANGE( gMR_qstQuantStep, (pqstQuantStep->iFraction)/(Double)(1 << pqstQuantStep->iFracBits) );    

#else   // must be BUILD_INT_FLOAT or float encoder

    if ( iQSS < DBPOWER_TABLE_OFFSET )
    {   // This happens when iPower==0, not an important case, but return 10^(1/20) with 28 FractBits
        // It can also happen with a small NoisePower (-13 has been seen)
        if ( iQSS < 0 )
        {   // negative values of iQSS are being generated in the V5 encoder (LowRate in particular)
            *pqstQuantStep = 0.382943866392f,      // Average Fraction
            *pqstQuantStep /= (Float)(1<<((-iQSS>>3)+1));                // Approximate Exponent
        }
        else
        {
            *pqstQuantStep = 0.869439785679f;             // Average Fraction
            *pqstQuantStep *= (Float)(1<<((iQSS>>3)+1));      // Approximate Exponent
        }
    }
    else if ( iQSS < (DBPOWER_TABLE_OFFSET+DBPOWER_TABLE_SIZE) )
    {   // *** normal case ***
        // implied FractBit scale for rgiDBPower table
        *pqstQuantStep = rgDBPower10[ iQSS - DBPOWER_TABLE_OFFSET ] * rgfltDBPowerExponentScale[iQSS>>3];
    }
    else
    {   // This branch can handle out-of-range cases. 
        // rare - but used in some cases by encoder - e.g. Tough_16m_16.
        // iQSS of 218 seen in encoding Tough 16kHz Stereo 16kbps
        *pqstQuantStep = (QuantStepType)pow (10, (double)(iQSS/(Float)QUANTSTEP_DENOMINATOR) );
    }
    MONITOR_RANGE( gMR_qstQuantStep, *pqstQuantStep );

#endif
}

#if 0
void prvInverseQuantizeLowRate_LowCutOff(CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor, CBT *rgiCoefRecon, Int* piRecon)
{
    // not integerized since cLowCutOff is typically 0, so this is here for compatability with V1 
    Double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);

    while (*piRecon < pau->m_cLowCutOff){
        Float fltCoefRecon;
        Float   fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);//rgfltNoise [iRecon];
        Float   fltWeightFactor = ((float)rgiWeightFactor [pau->m_cLowCutOff])/(1<<WEIGHTFACTOR_FRACT_BITS);
#if defined(_SH4_)
        fltCoefRecon  = (Float) (fltNoise * fltWeightFactor * (Float) dblQuantStep * (float)(1 << WEIGHTFACTOR_FRACT_BITS)/((float)ppcinfo->m_wtMaxWeight) );
#else
        fltCoefRecon  = (Float) (fltNoise * fltWeightFactor * dblQuantStep * (float)(1<<WEIGHTFACTOR_FRACT_BITS)/((float)ppcinfo->m_wtMaxWeight) );
#endif
        rgiCoefRecon [*piRecon] = (Int)(fltCoefRecon * (1<<TRANSFORM_FRACT_BITS));
        MONITOR_RANGE(gMR_CoefRecon,rgiCoefRecon[iRecon]/32.0f);
        MONITOR_RANGE(gMR_WeightRatio,(fltWeightFactor*(float)(1<<WEIGHTFACTOR_FRACT_BITS)/((float)ppcinfo->m_wtMaxWeight)));
        MONITOR_COUNT(gMC_IQ_Float,9);
        *piRecon++;
    }
}
#endif

#endif //WMA_OPT_INVERSQUAN_LOWRATE_ARM


#if WMA_OPT_SUBFRAMERECON_ARM

WMARESULT auSubframeRecon_Std ( CAudioObject* pau, PerChannelInfo* ppcinfo, PerChannelInfo* ppcinfo2);
WMARESULT auSubframeRecon_MonoPro ( CAudioObject* pau );

#if 0
WMARESULT auSubframeRecon_Std ( CAudioObject* pau, PerChannelInfo* ppcinfo, PerChannelInfo* ppcinfo2)
{
    I16 iSizePrev, iSizeCurr;
    Int i, iOverlapSize;
    BP2Type bp2Sin;
    BP2Type bp2Cos;
    BP2Type bp2Sin1;
    BP2Type bp2Cos1;
    BP2Type bp2Step;
    BP2Type bp2SinT;     /* temp sin value within recurrsion */
    BP2Type bp2CosT;     /* temp cos value within recurrsion */

    CoefType *piPrevCoef, *piCurrCoef;
    CoefType cfPrevData, cfCurrData;

    CoefType *piPrevCoef2, *piCurrCoef2;

    // get previous subframe size
    iSizeCurr = ppcinfo->m_iSizeCurr;
    iSizePrev = ppcinfo->m_iSizePrev;
    
    //==================================================================
    // ==== In-place Reconstruction between prev and curr subframes ====
    //==================================================================
    
    // ---- Now move unused coef to the end of current subframe coef buffer ----
    piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon;   // now reverse the coef buffer
    piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon;   // now reverse the coef buffer
    for(i = 0; i < iSizeCurr >> 1; i++) {
        cfCurrData = piCurrCoef[i];
        piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i];
        piCurrCoef[iSizeCurr - 1 - i] = cfCurrData;
        
        cfCurrData = piCurrCoef2[i];
        piCurrCoef2[i] = piCurrCoef2[iSizeCurr - 1 - i];
        piCurrCoef2[iSizeCurr - 1 - i] = cfCurrData;
    }

    //in this case, iCurrCoefRecurQ2 will equal iCurrCoefRecurQ1
    //iOverlapSize = (iCurrCoefRecurQ2 - iCurrCoefRecurQ1) >> 1 = 0;
    if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
        return WMA_OK; 

    // ---- Setup the coef buffer pointer ----
    piPrevCoef = (CoefType *)ppcinfo->m_rgiCoefRecon - (iSizePrev >> 1);       // go forward
    piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon + (iSizeCurr >> 1) - 1;   // go backward
    
    piPrevCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon - (iSizePrev >> 1);       // go forward
    piCurrCoef2 = (CoefType *)ppcinfo2->m_rgiCoefRecon + (iSizeCurr >> 1) - 1;   // go backward

    // ---- Test which subframe is larger ----
    if( iSizeCurr > iSizePrev ){

        //prvCalcQ3Q4(pau, WMAB_FALSE, iSizePrev, iSizeCurr, iSizePrev, &iPrevCoefRecurQ3, &iPrevCoefRecurQ4);
        //prvCalcQ1Q2(pau, WMAB_TRUE, iSizePrev, iSizeCurr, &iCurrCoefRecurQ1, &iCurrCoefRecurQ2);

        // Go from smaller subframe to larger subframe
        piCurrCoef -= (iSizeCurr - iSizePrev) >> 1;
        piCurrCoef2 -= (iSizeCurr - iSizePrev) >> 1;

        // iOverlapSize = iSizePrev / 2;
        iOverlapSize = iSizePrev >> 1;

    }else{

        //prvCalcQ3Q4(pau, WMAB_FALSE, iSizePrev, iSizeCurr, iSizePrev, &iPrevCoefRecurQ3, &iPrevCoefRecurQ4);
        //prvCalcQ1Q2(pau, WMAB_TRUE, iSizePrev, iSizeCurr, &iCurrCoefRecurQ1, &iCurrCoefRecurQ2);

        if (iSizeCurr < iSizePrev)
        {
            // Go from smaller subframe to larger subframe
            piPrevCoef += ((iSizePrev - iSizeCurr) >> 1);
            piPrevCoef2 += ((iSizePrev - iSizeCurr) >> 1);
        }

        iOverlapSize = iSizeCurr >> 1;
    }
    
    // Reset trig recursion
    bp2Sin  = ppcinfo->m_fiSinRampUpStart;
    bp2Cos  = ppcinfo->m_fiCosRampUpStart;
    bp2Sin1 = ppcinfo->m_fiSinRampUpPrior;
    bp2Cos1 = ppcinfo->m_fiCosRampUpPrior;
    bp2Step = ppcinfo->m_fiSinRampUpStep;
    
    // ---- Now we do overlap and add here ----
    for(i = 0; i < iOverlapSize; i++ ){
        
        cfPrevData = *piPrevCoef;
        cfCurrData = *piCurrCoef;
        
        *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + 
            MULT_BP2(bp2Cos, cfPrevData) );
        
        *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + 
            MULT_BP2(bp2Cos, cfCurrData) );
        
        cfPrevData = *piPrevCoef2;
        cfCurrData = *piCurrCoef2;
        
        *piPrevCoef2++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + 
            MULT_BP2(bp2Cos, cfPrevData) );
        
        *piCurrCoef2-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + 
            MULT_BP2(bp2Cos, cfCurrData) );
        
        /* sin(a+b) = sin(a-b) + 2*sin(b)*cos(a) */
        /* cos(a+b) = cos(a-b) - 2*sin(b)*sin(a) */
        bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
        bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
        bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
        bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
    }

    return WMA_OK;
}


WMARESULT auSubframeRecon_MonoPro ( CAudioObject* pau )
{
    I16 iCh, iChSrc;
    I16 iSizePrev, iSizeCurr;
    BP2Type bp2Sin;
    BP2Type bp2Cos;
    BP2Type bp2Sin1;
    BP2Type bp2Cos1;
    BP2Type bp2Step;
    BP2Type bp2SinT;     /* temp sin value within recurrsion */
    BP2Type bp2CosT;     /* temp cos value within recurrsion */
    CoefType *piPrevCoef, *piCurrCoef;
    CoefType cfPrevData, cfCurrData;
    Int i, iOverlapSize;
    PerChannelInfo* ppcinfo;

    // mono or version 3 and above
    for (iCh = 0; iCh < pau->m_cChInTile; iCh++) {
        
        // ======== Setup the init condition for PCM reconstruction ========
        iChSrc = pau->m_rgiChInTile [iCh];
        ppcinfo = pau->m_rgpcinfo + iChSrc;
        
        // get previous subframe size
        iSizePrev = ppcinfo->m_iSizePrev;
        iSizeCurr = ppcinfo->m_iSizeCurr;
        
        //==================================================================
        // ==== In-place Reconstruction between prev and curr subframes ====
        //==================================================================
        
        // ---- Now move unused coef to the end of current subframe coef buffer ----
        piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon;   // now reverse the coef buffer
        for(i = 0; i < iSizeCurr >> 1; i++) {
            cfCurrData = piCurrCoef[i];
            piCurrCoef[i] = piCurrCoef[iSizeCurr - 1 - i];
            piCurrCoef[iSizeCurr - 1 - i] = cfCurrData;
        }
        
        //in this case, iCurrCoefRecurQ2 will equal iCurrCoefRecurQ1
        //iOverlapSize = (iCurrCoefRecurQ2 - iCurrCoefRecurQ1) >> 1 = 0;
        if ((pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE) && (pau->m_bFirstUnifiedPureLLMFrm == WMAB_FALSE))
            continue; 

        // ---- Setup the coef buffer pointer ----
        piPrevCoef = (CoefType *)ppcinfo->m_rgiCoefRecon - (iSizePrev >> 1);       // go forward
        piCurrCoef = (CoefType *)ppcinfo->m_rgiCoefRecon + (iSizeCurr >> 1) - 1;   // go backward
        
        // ---- Test which subframe is larger ----
        if( iSizeCurr > iSizePrev ){
            
            //prvCalcQ3Q4(pau, WMAB_FALSE, iSizePrev, iSizeCurr, iSizePrev, &iPrevCoefRecurQ3, &iPrevCoefRecurQ4);
            //prvCalcQ1Q2(pau, WMAB_TRUE, iSizePrev, iSizeCurr, &iCurrCoefRecurQ1, &iCurrCoefRecurQ2);
            
            // Go from smaller subframe to larger subframe
            piCurrCoef -= (iSizeCurr - iSizePrev) >> 1;
            
            // iOverlapSize = iSizePrev / 2;
            iOverlapSize = iSizePrev >> 1;
            
        }else{
            
            //prvCalcQ3Q4(pau, WMAB_FALSE, iSizePrev, iSizeCurr, iSizePrev, &iPrevCoefRecurQ3, &iPrevCoefRecurQ4);
            //prvCalcQ1Q2(pau, WMAB_TRUE, iSizePrev, iSizeCurr, &iCurrCoefRecurQ1, &iCurrCoefRecurQ2);
            
            if (iSizeCurr < iSizePrev)
            {
                // Go from smaller subframe to larger subframe
                piPrevCoef += ((iSizePrev - iSizeCurr) >> 1);
            }
            
            iOverlapSize = iSizeCurr >> 1;
        }
 
       // Reset trig recursion
        bp2Sin  = ppcinfo->m_fiSinRampUpStart;
        bp2Cos  = ppcinfo->m_fiCosRampUpStart;
        bp2Sin1 = ppcinfo->m_fiSinRampUpPrior;
        bp2Cos1 = ppcinfo->m_fiCosRampUpPrior;
        bp2Step = ppcinfo->m_fiSinRampUpStep;
        
        // ---- Now we do overlap and add here ----
        for(i = 0; i < iOverlapSize; i++ ){
            
            cfPrevData = *piPrevCoef;
            cfCurrData = *piCurrCoef;
            
            *piPrevCoef++ = INT_FROM_COEF( MULT_BP2(-bp2Sin, cfCurrData) + 
                MULT_BP2(bp2Cos, cfPrevData) );
            
            *piCurrCoef-- = INT_FROM_COEF( MULT_BP2(bp2Sin, cfPrevData) + 
                MULT_BP2(bp2Cos, cfCurrData) );
            
            /* sin(a+b) = sin(a-b) + 2*sin(b)*cos(a) */
            /* cos(a+b) = cos(a-b) - 2*sin(b)*sin(a) */
            bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
            bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
            bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
            bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
        }
    }

    return WMA_OK;
}
#endif


WMARESULT auSubframeRecon ( CAudioObjectDecoder *paudec, CAudioObject* pau )
{
    WMARESULT hr = WMA_OK;
    I16 iCh, iChSrc;
    I16 iSizePrev, iSizeCurr, iSizePrev2;
    PerChannelInfo *ppcinfo, *ppcinfo2;
    Int i, iOverlapSize;
    I16 iCurrCoefRecurQ1, iCurrCoefRecurQ2, iCurrCoefRecurQ3 = 0, iCurrCoefRecurQ4 = 0;
    CoefType *piCurrCoef;

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif

#if defined(BUILD_WMAPROLBRV2)
    if (WMAB_TRUE == pau->m_bReconProc)
    {
        TRACEWMA_EXIT(hr, chexSubframeRecon(paudec, pau));
        goto exit;
    }
#endif // BUILD_WMAPROLBRV2

#ifdef BUILD_INTEGER
    if (pau->m_iVersion >= 3) {
#ifdef SAVE_PCMBUFFER_IN_PLLM
        if (pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE 
            && pau->m_bLastUnifiedPureLLMFrm == WMAB_FALSE)
        {
            // If UPLLM (except last frm), nothing is done here.
            // the conversion (INT_FROM_COEF) will be done in 
            // next BUILD_INTEGER block due to different PCM
            // recon scheme.
        }
        else
        {
#endif //SAVE_PCMBUFFER_IN_PLLM
            for (iCh = 0; iCh < pau->m_cChInTile; iCh++) {
                iChSrc = pau->m_rgiChInTile [iCh];
                ppcinfo = pau->m_rgpcinfo + iChSrc;
        
                iSizePrev = ppcinfo->m_iSizePrev;
                iSizeCurr = ppcinfo->m_iSizeCurr;

                // In Unified WMA, we must call prvCalcQ1Q2/Q3Q4 to get the window shape. 
                // iOverlapSize = min(iSizePrev, iSizeCurr) / 2;
                prvCalcQ1Q2(pau, WMAB_TRUE, iSizePrev, iSizeCurr, &iCurrCoefRecurQ1, &iCurrCoefRecurQ2);
                iOverlapSize = (iCurrCoefRecurQ2 - iCurrCoefRecurQ1) / 2;

            
                piCurrCoef = ppcinfo->m_rgiCoefRecon;
                for (i = -iSizePrev / 2; i < - iOverlapSize; i++) {
                    piCurrCoef[i] = INT_FROM_COEF(piCurrCoef[i]);
                }
            }
#ifdef SAVE_PCMBUFFER_IN_PLLM
        }
#endif //SAVE_PCMBUFFER_IN_PLLM
    }
#endif

    i = WMAB_FALSE;
    if( pau->m_cChInTile == 2 ){
        
        // ======== Setup the init condition for PCM reconstruction ========
        ppcinfo = &pau->m_rgpcinfo[pau->m_rgiChInTile[0]];
        ppcinfo2 = &pau->m_rgpcinfo[pau->m_rgiChInTile[1]];

        iSizePrev = ppcinfo->m_iSizePrev;
        iSizePrev2 = ppcinfo2->m_iSizePrev;

        if( (pau->m_iVersion <= 2) || (iSizePrev == iSizePrev2) )
            i = WMAB_TRUE;
    }
            
    // ********** Reconstruction for each channel in the tile **********
    if( i == WMAB_TRUE ){

        auSubframeRecon_Std ( pau, ppcinfo, ppcinfo2 );

    } else {

        // mono or version 3 and above
        auSubframeRecon_MonoPro( pau );
    }

#ifdef BUILD_INTEGER
    {
        I16 iSizeNext;
        Int iOverlapSizeNext, iEnd;
        for (iCh = 0; iCh < pau->m_cChInTile; iCh++) {
            iChSrc = pau->m_rgiChInTile [iCh];
            ppcinfo = pau->m_rgpcinfo + iChSrc;
    
            iSizePrev = ppcinfo->m_iSizePrev;
            iSizeCurr = ppcinfo->m_iSizeCurr;
            iSizeNext = ppcinfo->m_iSizeNext;

            // In Unified WMA, we must call prvCalcQ1Q2/Q3Q4 to get the window shape. 
            // iOverlapSize = min(iSizePrev, iSizeCurr) / 2;
            prvCalcQ1Q2(pau, WMAB_TRUE, iSizePrev, iSizeCurr, &iCurrCoefRecurQ1, &iCurrCoefRecurQ2);
            iOverlapSize = (iCurrCoefRecurQ2 - iCurrCoefRecurQ1) / 2;

            // iOverlapSizeNext = min(iSizeNext, iSizeCurr) / 2;
            prvCalcQ3Q4(pau, WMAB_TRUE, iSizeCurr, iSizeNext, iSizeCurr, &iCurrCoefRecurQ3, &iCurrCoefRecurQ4);
            iOverlapSizeNext = (iCurrCoefRecurQ4 - iCurrCoefRecurQ3) / 2;

            if (pau->m_iVersion < 3) {
                iEnd = iSizeCurr - iOverlapSizeNext;
            } else { // for v3, the other half is done during the next subframe (at the top)
                iEnd = iSizeCurr / 2;
#ifdef SAVE_PCMBUFFER_IN_PLLM
                if (pau->m_bUnifiedPureLLMCurrFrm == WMAB_TRUE 
                    && pau->m_bLastUnifiedPureLLMFrm == WMAB_FALSE)
                {
                    // In UPLLM (except last frm), we recon all samples instead
                    // of half.
                    iEnd = iSizeCurr;
                }
#endif //SAVE_PCMBUFFER_IN_PLLM
            }
        
            piCurrCoef = ppcinfo->m_rgiCoefRecon;
            for (i = iOverlapSize; i < iEnd; i++) {
               piCurrCoef[i] = INT_FROM_COEF(piCurrCoef[i]);
            }
        }
    }
#endif
#if defined(BUILD_WMAPROLBRV2)
exit:
#endif
#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
} // auSubframeRecon
#endif //WMA_OPT_SUBFRAMERECON_ARM

#if WMA_OPT_CHANNEL_DOWNMIX_ARM

#define CLIP_PCMRANGE(y) \
  if (y < pau->m_iPCMSampleMin) \
     y = pau->m_iPCMSampleMin; \
  else if (y > pau->m_iPCMSampleMax) \
     y = pau->m_iPCMSampleMax;

// For post processing done post PCM buffer
#define PPPOSTADVPTR(p, amt) { (p) += (amt)*pau->m_nBytePerSample; }
#define PPPOSTGETSAMPLE(p, ch) (PAUPFNGETSAMPLE((PCMSAMPLE*)(p), (ch)))
#define PPPOSTSETSAMPLE(v, p, ch) (PAUPFNSETSAMPLE(v, (PCMSAMPLE*)(p), (ch)))
#define PPPOSTCLIPRANGE(x) CLIP_PCMRANGE(x)

WMARESULT prvChannelUpMix (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, Int nBlocks,
                             U8 *piDestBuffer)
{
    CAudioObject* pau     = paudec->pau;
    Int cSrcChannel       = pau->m_cChannel;
    Int cDstChannel       = paudec->m_cDstChannel;
    //WMARESULT wmaResult   = WMA_OK;
    PCMSAMPLE* rgpcmsTemp = paudec->m_rgpcmsTemp;
    U8  *piSrc            = piSourceBuffer;
    U8  *piDst            = piDestBuffer;
    Int iBlock, iChSrc, iChDst;

    // up mixing done backwards so as to not overwrite needed memory
    // Block transform: channel up mixing, and if needed, requantization.
    PPPOSTADVPTR(piSrc, cSrcChannel * (nBlocks-1));
    PPPOSTADVPTR(piDst, cDstChannel * (nBlocks-1));
    
    for (iBlock = nBlocks-1; iBlock >= 0; iBlock--)
    {
        // Initialize
        memset(rgpcmsTemp, 0, sizeof(PCMSAMPLE) * cDstChannel);
        for (iChDst = 0; iChDst < cDstChannel; iChDst++)
        {
            for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)
            {
                PCMSAMPLE pcmsTemp = PPPOSTGETSAMPLE(piSrc, iChSrc);
                rgpcmsTemp[iChDst] += (PCMSAMPLE) MULT_CHDN(pcmsTemp,
                    paudec->m_rgrgfltChDnMixMtx[iChDst][iChSrc]);
            }
            PPPOSTCLIPRANGE(rgpcmsTemp[iChDst]);
        }
        // One block completed: copy back results
        for (iChDst = 0; iChDst < cDstChannel; iChDst++) 
        {
            PPPOSTSETSAMPLE(rgpcmsTemp[iChDst], piDst, iChDst);
        }
        
        //Stride back to the previous block
        PPPOSTADVPTR(piSrc, -cSrcChannel);
        PPPOSTADVPTR(piDst, -cDstChannel);
    }

    return WMA_OK;
}

#if 0
WMARESULT prvChannelDownMix (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, Int nBlocks,
                             U8 *piDestBuffer)
{
    CAudioObject* pau     = paudec->pau;
    WMARESULT wmaResult   = WMA_OK;
    U8  *piSrc            = piSourceBuffer;
    U8  *piDst            = piDestBuffer;
    Int cSrcChannel       = pau->m_cChannel;
    Int cDstChannel       = paudec->m_cDstChannel;
    Int iBlock, iChSrc, iChDst;
    U8 *tmpSrc, *tmpDst;
    PCMSAMPLE pcmsTemp, pcmsDst;
    PCMSAMPLE* pcmsMixMtx;

    if (!paudec->m_fChannelFoldDown)
    {
        return wmaResult;
    }

  #ifdef TEST_VRHEADPHONE

    TRACEWMA_EXIT(wmaResult, AVRHeadphoneProcess (paudec->m_pVRHPhone, piSourceBuffer, nBlocks, piDestBuffer));
        //(paudec, (U8*) pbDst, nBlocks, (U8*) pbDst));
exit:
    return wmaResult;

  #else

    if (cSrcChannel >= cDstChannel)
    {
        if( pau->m_iBitDepthSelector == BITDEPTH_16 )
        {
            // Block transform: channel down mixing, and if needed, requantization.
            for (iBlock = 0; iBlock < nBlocks; iBlock++)
            {
                for (iChDst = 0; iChDst < cDstChannel; iChDst++)
                {
                    pcmsDst = 0;
                    tmpSrc = piSrc;
                    pcmsMixMtx = paudec->m_rgrgfltChDnMixMtx[iChDst];

                    for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)
                    {
                        //PCMSAMPLE pcmsTemp = PPPOSTGETSAMPLE(piSrc, iChSrc);
                        pcmsTemp = *((I16*)tmpSrc);
                        pcmsDst += (PCMSAMPLE) MULT_CHDN(pcmsTemp, *pcmsMixMtx++);

                        tmpSrc += 2;
                    }
            
                    PPPOSTCLIPRANGE(pcmsDst);
                    /*if (pcmsDst > (I32)0X00007FFF)
                        pcmsDst = (I32)0X00007FFF;
                    else if (pcmsDst < (I32)0XFFFF8000)
                        pcmsDst = (I32)0XFFFF8000;*/        

                    *((I16*)piDst) = (I16)pcmsDst;
                    piDst += 2;
                }
                piSrc = tmpSrc;
            }
        }
        else if( pau->m_iBitDepthSelector == BITDEPTH_24 )
        {
            // Block transform: channel down mixing, and if needed, requantization.
            for (iBlock = 0; iBlock < nBlocks; iBlock++)
            {
                for (iChDst = 0; iChDst < cDstChannel; iChDst++)
                {
                    pcmsDst = 0;
                    tmpSrc = piSrc;
                    pcmsMixMtx = paudec->m_rgrgfltChDnMixMtx[iChDst];

                    for (iChSrc = 0; iChSrc < cSrcChannel; iChSrc++)
                    {
                        //PCMSAMPLE pcmsTemp = PPPOSTGETSAMPLE(piSrc, iChSrc);
                        pcmsTemp = (*((U8*)tmpSrc) | (*((U8*)(tmpSrc + 1)) << 8) | (*((I8*)(tmpSrc + 2)) << 16));
                        pcmsDst += (PCMSAMPLE) MULT_CHDN(pcmsTemp, *pcmsMixMtx++);

                        tmpSrc += 3;
                    }

                    PPPOSTCLIPRANGE(pcmsDst);
                    /*if (pcmsDst > (I32)0X007FFFFF)
                        pcmsDst = (I32)0X007FFFFF;
                    else if (pcmsDst < (I32)0XFF800000)
                        pcmsDst = (I32)0XFF800000;*/       

                    tmpDst = (U8*) &pcmsDst;
                    
                    *((I8*)piDst) = *((I8*)tmpDst);
                    *((I8*)(piDst + 1)) = *((I8*)(tmpDst + 1));
                    *((I8*)(piDst + 2)) = *((I8*)(tmpDst + 2));
                    piDst += 3;
                }
                piSrc = tmpSrc;
            }
        }
        else
        {
            //support only 16 and 24 bit output
            ASSERT(0);
        }
    }
    else
    {
        prvChannelUpMix(paudec, piSourceBuffer, nBlocks, piDestBuffer);
    }

    return wmaResult;

  #endif  // TEST_VRHEADPHONE

} // prvChannelDownMix
#endif
#endif //WMA_OPT_CHANNEL_DOWNMIX_ARM


#if 0
//#if WMA_OPT_REQUANTO16_ARM
WMARESULT prvRequantizeTo16(CAudioObjectDecoder *paudec, U8 *piSrc, Int nBlocks)
{
    WMARESULT wmaResult = WMA_OK;
    CAudioObject* pau = paudec->pau;
    Int iBlock;
    Int cDstChannel = paudec->m_cDstChannel;
    U8 *piDst = piSrc;
    I16 pcmsTemp;

    if (!paudec->m_fReQuantizeTo16)
        goto exit;

    // In the future if this condition is false, return WMA_E_FAIL or something
    assert(paudec->m_nDstBytePerSample == 2);
    //WinCE only support 16/24 bit input/output
    assert(pau->m_nValidBitsPerSample == 24);

    for (iBlock = 0; iBlock < nBlocks*cDstChannel; iBlock++) 
    {
        pcmsTemp = (*((U8*)(piSrc + 1))) | (*((I8*)(piSrc + 2)) << 8);
        *((I16*)piDst) = pcmsTemp;
        
        piSrc += 3;
        piDst += 2;
    }

exit:
    return wmaResult;
}
#endif //WMA_OPT_REQUANTO16_ARM

//#endif //WMA_TARGET_ARM

