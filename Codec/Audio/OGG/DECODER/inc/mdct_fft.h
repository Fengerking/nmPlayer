//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    mdct_fft.h

Abstract:

    mdct and fft transform header file. 

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/

#ifndef __MDCT_FFT_H_
#define	__MDCT_FFT_H_

#define DATA_TYPE OGG_S32
#define REG_TYPE  register OGG_S32

#ifdef _LOW_ACCURACY_
#define cPI3_8 (0x0062)
#define cPI2_8 (0x00b5)
#define cPI1_8 (0x00ed)
#else
#define cPI3_8 (0x30fbc54d)
#define cPI2_8 (0x5a82799a)
#define cPI1_8 (0x7641af3d)
#endif

//#define MDCTIV

#ifdef MDCTIV
extern int IMDCT(int n, int *in, int *out);
#else
extern	void mdct_butterfly_generic(DATA_TYPE *x,int points,int step);
extern	void mdct_butterfly_32(DATA_TYPE *x);
extern	void MDCTBackward(int n, DATA_TYPE *in, DATA_TYPE *out);	//mdct_backward
//extern	void mdct_bitreverse(DATA_TYPE *x,int n,int step,int shift);
extern	void mdct_preprocess(DATA_TYPE *in, DATA_TYPE *out, int n4, int step);
extern	void mdct_postprocess(DATA_TYPE *in, DATA_TYPE *out, int n4, int step);
#endif
#endif	//__MDCT_FFT_H_