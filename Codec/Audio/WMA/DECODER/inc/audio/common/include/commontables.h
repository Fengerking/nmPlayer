/****************************************************************
*
* Filename:	commontables.h
*
* Function:	The fixed realize of math functions.
*
* Author:	
*			Witten.Wen		2008-06-18
*
* Copyrights: 
*
*		VisualOn SoftWare Co., Ltd. All Rights are reserved
*
* Revision History:	
*
*		Witten.Wen added 
*						lookup_sine
*						rgrgfltChDnMixMtx6_2
*						rgrgfltChDnMixMtx8_2
*						rgrgfltChDnMixMtx8_6
*						voWMAMemset
*						voWMAMemcpy
*							in 2009-05-31
*
*		Witten.Wen added
*						voWMAPow2
*						voWMASqrt512
*						voWMACeil
*							in 2009-10-01
*		Witten.Wen added
*						voWMAlog2
*						voWMAlog
*						voWMAlog10
*						ROUNDFLOG2
*							in 2010-03-17
*
*		Witten.Wen deleted
*						ROUNDFLOG2
*							in 2010-08-11
**************************************************************/
#ifndef __COMMONTABLES_H_
#define __COMMONTABLES_H_

//#include "sdkbase.h"
#include "voType.h"

#ifndef int64
typedef long long int64;
#endif

#define POWSHIFTBITS 15


#define MULTI32(x, y) (VO_U32)(((VO_U64)x * y)>>32)

extern VO_U16 pow2_256[];
extern VO_U16 sqrt512[];
extern VO_U16 sine_table[];

extern VO_U16 log2_256_table[256];
extern int voWMAlog2(VO_U32 x);
extern int voWMAlog(VO_U32 x);
extern int voWMAlog10(VO_U32 x);

extern double voWMAPow2(int x);
//pow2(x) = voWMAPow2(x*256)
//850 == log2(10)*256
//#define voWMAPow10(x) (voWMAPow2(x*850))
extern double voWMASqrt512(int x);

extern int voWMACeil(double x);

extern int lookup_sine(int x);

extern const int rgrgfltChDnMixMtx6_2[2][6];
extern const int rgrgfltChDnMixMtx8_2[2][8];
extern const int rgrgfltChDnMixMtx8_6[6][8];

extern void *voWMAMemset(void *pSrc, int n, unsigned int size);
extern void *voWMAMemcpy(void *pDest, const void *pSrc, unsigned int size);
//extern void *voWMAMemmove(void *pDest, const void *pSrc, size_t size);
#endif

