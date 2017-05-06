#ifndef _GLOBAL_H_ 
#define _GLOBAL_H_ 

#include "voType.h"
#include "portab.h"

#ifdef LC_ONLY_ARMv4
#define ONLY_LC		1
#endif//LC_ONLY_ARMv4
#if !ONLY_LC
#define MAIN_DEC
#define LTP_DEC
#define SBR_DEC		//SBR
#define PS_DEC		//Parametric Stereo
#ifndef RVDS
#define BSAC_DEC		//BSAC
#endif//RVDS
#endif//!ONLY_LC
#define ERROR_CHECK	1


#define CHECK_PROFILE	5

#define SUPPORT_MUL_CHANNEL	1

#if SUPPORT_MUL_CHANNEL
#define MAX_CHANNELS		8
#define DOWNTO2CHS			1
#else//SUPPORT_MUL_CHANNEL
#define MAX_CHANNELS		2
#endif//SUPPORT_MUL_CHANNEL
#define MAX_SAMPLES			1024
#define MAX_SYNTAX_ELEMENTS		2	

#define NUM_SAMPLE_RATES	12

#define MAX_WINDOW_GROUPS		8
#define MAX_SFB_SHORT		15
#define MAX_SFB				128
#define MAX_LTP_SFB         40
#define MAX_MS_MASK_BYTES	((MAX_SFB + 7) >> 3)
#define MAX_SFB_PRED		64
#define MAX_TNS_FILTERS		8
#define MAX_TNS_COEFS		60
#define MAX_TNS_ORDER		20
#define MAX_PULSES			4
#define MAX_GAIN_BANDS		3
#define MAX_GAIN_WIN		8
#define MAX_GAIN_ADJUST		7

#define SIZE_LONG_WIN		1024
#define SIZE_SHORT_WIN		128

#define NUM_SYN_ID_BITS		3
#define NUM_INST_TAG_BITS	4

#define SBR_EXTENSION		0x0d
#define SBR_EXTENSION_CRC	0x0e

#define EXTENSION_ID_PS 2

#define VO_INT_MAX 0x7FFFFFFF

#define GET_ELE_ID(p)	((AACElementID)(*(p) >> (8-NUM_SYN_ID_BITS)))

//Table 4.3 ¨C Syntax of top level payload for audio object types AAC Main, SSR, LC, and LTP
typedef enum
{
	ID_SCE =0,
	ID_CPE =1,
	ID_CCE =2,
	ID_LFE =3,
	ID_DSE =4,
	ID_PCE =5,
	ID_FIL =6,
	ID_END =7,
	ELM_ID_SIZE = 8
}ELEMENT_ID;

#define ZERO_HCB       0
#define FIRST_PAIR_HCB 5
#define ESC_HCB        11
#define QUAD_LEN       4
#define PAIR_LEN       2
#define NOISE_HCB      13
#define INTENSITY_HCB2 14
#define INTENSITY_HCB  15

#ifndef MAX
#define MAX(a,b)        ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)        ((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)			((x)>=0?(x):-(x))
#endif


#ifndef NULL
#define NULL 0
#endif

/* do y <<= n, clipping to range [-2^30, 2^30 - 1] (i.e. output has one guard bit) */
#define CLIP_2N_SHIFT(y, n) {                   \
	int sign = (y) >> 31;                   \
	if (sign != (y) >> (30 - (n)))  {       \
	(y) = sign ^ (0x3fffffff);          \
	} else {                                \
	(y) = (y) << (n);                   \
}                                       \
}

/* clip to [-2^n, 2^n-1], valid range of n = [1, 30] */
#define CLIP_2N(x, n) {                               \
	if ((x) >> 31 != (x) >> (n))                \
	(x) = ((x) >> 31) ^ ((1 << (n)) - 1);   \
}
//#define CLIPTOSHORT(x)  ((((x) >> 31) == (x >> 15))?(x):((x) >> 31) ^ ((1 << 15) - 1))
#define CLIPTOSHORT(x)  ((((x) >> 31) == (x >> 15))?(x):((x) >> 31) ^ 0x7fff)
#define MAC32(C,A,B)   (VO_S64)((VO_S64)(A)*(VO_S64)(B)+(VO_S64)(C))
static __inline int CLZ(VO_S32 x)
{
	int numZeros;
	
	if (!x)
		return 32;
	
	/* count leading zeros with binary search */
	numZeros = 1;
	if (!((VO_U32)x >> 16))	{ numZeros += 16; x <<= 16; }
	if (!((VO_U32)x >> 24))	{ numZeros +=  8; x <<=  8; }
	if (!((VO_U32)x >> 28))	{ numZeros +=  4; x <<=  4; }
	if (!((VO_U32)x >> 30))	{ numZeros +=  2; x <<=  2; }
	
	numZeros -= ((VO_U32)x >> 31);
	
	return numZeros;
}

#define MULHIGH(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 32)

#define Q4(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<4)+0.5)) : ((VO_S32)((A)*(1<<4)-0.5)))
#define Q5(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<5)+0.5)) : ((VO_S32)((A)*(1<<5)-0.5)))
#define Q6(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<6)+0.5)) : ((VO_S32)((A)*(1<<6)-0.5)))
#define Q7(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<7)+0.5)) : ((VO_S32)((A)*(1<<7)-0.5)))
#define Q8(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<8)+0.5)) : ((VO_S32)((A)*(1<<8)-0.5)))
#define Q9(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<9)+0.5)) : ((VO_S32)((A)*(1<<9)-0.5)))
#define Q10(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<10)+0.5)) : ((VO_S32)((A)*(1<<10)-0.5)))
#define Q11(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<11)+0.5)) : ((VO_S32)((A)*(1<<11)-0.5)))
#define Q12(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<12)+0.5)) : ((VO_S32)((A)*(1<<12)-0.5)))
#define Q13(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<13)+0.5)) : ((VO_S32)((A)*(1<<13)-0.5)))

#define Q14(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<14)+0.5)) : ((VO_S32)((A)*(1<<14)-0.5)))
#define Q15(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<15)+0.5)) : ((VO_S32)((A)*(1<<15)-0.5)))
#define Q16(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<16)+0.5)) : ((VO_S32)((A)*(1<<16)-0.5)))
#define Q17(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<17)+0.5)) : ((VO_S32)((A)*(1<<17)-0.5)))
#define Q18(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<18)+0.5)) : ((VO_S32)((A)*(1<<18)-0.5)))
#define Q19(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<19)+0.5)) : ((VO_S32)((A)*(1<<19)-0.5)))
#define Q20(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<20)+0.5)) : ((VO_S32)((A)*(1<<20)-0.5)))
#define Q21(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<21)+0.5)) : ((VO_S32)((A)*(1<<21)-0.5)))
#define Q22(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<22)+0.5)) : ((VO_S32)((A)*(1<<22)-0.5)))
#define Q23(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<23)+0.5)) : ((VO_S32)((A)*(1<<23)-0.5)))
#define Q24(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<24)+0.5)) : ((VO_S32)((A)*(1<<24)-0.5)))
#define Q25(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<25)+0.5)) : ((VO_S32)((A)*(1<<25)-0.5)))
#define Q26(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<26)+0.5)) : ((VO_S32)((A)*(1<<26)-0.5)))
#define Q27(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<27)+0.5)) : ((VO_S32)((A)*(1<<27)-0.5)))
#define Q28(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<28)+0.5)) : ((VO_S32)((A)*(1<<28)-0.5)))
#define Q29(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<29)+0.5)) : ((VO_S32)((A)*(1<<29)-0.5)))
#define Q30(A) (((A) >= 0) ? ((VO_S32)((A)*(1<<30)+0.5)) : ((VO_S32)((A)*(1<<30)-0.5)))
//#define Q31(A) (((1.00 - (A))<0.00000001)? ((VO_S32)0x7FFFFFFF) : (((A) >= 0) ? ((VO_S32)((A)*(1<<31)+0.5)) : ((VO_S32)((A)*(1<<31)-0.5))))
#define Q31(A) (((1.00 - (A))<0.00000001) ? ((VO_S32)0x7FFFFFFF) : (((A) >= 0) ? ((VO_S32)((A)*((VO_U32)(1<<31))+0.5)) : ((VO_S32)((A)*((VO_U32)(1<<31))-0.5))))
#define MUL_14(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1<<13)) >> 14)
#define MUL_15(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1<<14)) >> 15)
#define MUL_16(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1<<15)) >> 16)
#define MUL_17(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1<<16)) >> 17)
#define MUL_18(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1<<17)) >> 18)
#define MUL_19(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1<<18)) >> 19)
#define MUL_20(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 19)) >> 20)
#define MUL_21(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 20)) >> 21)
#define MUL_22(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 21)) >> 22)
#define MUL_23(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 22)) >> 23)
#define MUL_24(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 23)) >> 24)
#define MUL_25(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 24)) >> 25)
#define MUL_26(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 25)) >> 26)
#define MUL_27(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 26)) >> 27)
#define MUL_28(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 27)) >> 28)
#define MUL_29(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 28)) >> 29)
#define MUL_30(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 29)) >> 30)
#define MUL_31(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 30)) >> 31)
#define MUL_32(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 32)
#define MUL_33(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 33)
#define MUL_34(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 34)
#define MUL_35(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 35)
#define MUL_36(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 36)

#endif//_GLOBAL_H_ 



















