	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2004				*
	*																		*
	************************************************************************/
#ifndef _PORTAB_H
#define _PORTAB_H  
typedef signed long int32_t;
typedef unsigned long uint32_t;  
typedef signed short int16_t; 
typedef unsigned short uint16_t; 
typedef signed char int8_t; 
typedef unsigned char uint8_t; 
typedef signed __int64 int64_t;
typedef signed long nativeInt;
typedef unsigned long nativeUInt;  

#define RSHIFT_ROUND(v,n)	(((v)+(1<<(n-1)))>>(n))

// general fixed point range (-16..16)
#define FIX_FRACBITS	27
// additional fast acc output range limitation (-2..2)
#define ACCFAST_ADJ		3
// additional fast mul b range limitation (-16..16)
#define FIXFAST_ADJ		0
// very bad approx when there is no support for fix_mul (32x32=64)
#define FIX_MUL_ADJ		3

#define FIX_SHIFT_A		(FIX_FRACBITS-FIX_FRACBITS/2)
#define FIX_SHIFT_B		(FIX_FRACBITS/2)

typedef int32_t fix_t;
typedef fix_t fixint_t;

#define FIXC(v)				((fix_t)((v)*(1<<FIX_FRACBITS)+((v)>=0?0.5:-0.5)))
#define FIXI(v)				(v)
#if FIX_FRACBITS<28
#define FIX28(v)			RSHIFT_ROUND(v,28-FIX_FRACBITS)
#else
#define FIX28(v)			(v)
#endif

#define ACCFAST_ASHIFT(a,n)	RSHIFT_ROUND(a,FIX_SHIFT_A-ACCFAST_ADJ+(n))
#define ACCFAST_BSHIFT(b)	RSHIFT_ROUND(b,FIX_SHIFT_B)

#define accfast_var(name)		int32_t_t name
#define accfast_mul(name,a,b)	(name) = (a)*(b)
#define accfast_mla(name,a,b)	(name) += (a)*(b)
#define accfast_neg(name)		(name) = -(name)
#define accfast_get(name,n)		((fix_t)((name)>>(ACCFAST_ADJ-(n))))

#define INLINE __inline
#if defined(VC_PC)

#define fix_mul(a,b) _fix_mul(a,b)
#define fix_mul64(a,b) _fix_mul(a,b)
static INLINE fix_t _fix_mul(fix_t a,fix_t b)
{
	__asm 
	{
		mov  eax,a
		imul b
		shrd eax, edx, FIX_FRACBITS
	}
}
/*
static __inline fix_t fix_mul2(fix_t a,fix_t b)
{

	_asm {
		mov  eax,a
		imul b
		mov  eax,edx
	}
	
}*/

#endif//IA32
//#ifdef ARM
//static INLINE fix_t fix_mul(fix_t a,fix_t b)
//{
//	return (fix_t)(((int64_t)(a)*(int64_t)(b)) >> FIX_FRACBITS);
//}
//#endif//ARM

#define fix_mul2(a,b) ((fix_t)(((int64_t)(a)*(int64_t)(b))))
#ifndef fix_mul64
#define fix_mul64(a,b)		((fix_t)(((int64_t)(a)*(int64_t)(b))>>FIX_FRACBITS))
#endif//fix_mul64

#ifndef fix_mul
#define fix_mul fix_mul64
#endif//fix_mul
#define Q4(A) (((A) >= 0) ? ((int32_t)((A)*(1<<4)+0.5)) : ((int32_t)((A)*(1<<4)-0.5)))
#define Q5(A) (((A) >= 0) ? ((int32_t)((A)*(1<<5)+0.5)) : ((int32_t)((A)*(1<<5)-0.5)))
#define Q6(A) (((A) >= 0) ? ((int32_t)((A)*(1<<6)+0.5)) : ((int32_t)((A)*(1<<6)-0.5)))
#define Q7(A) (((A) >= 0) ? ((int32_t)((A)*(1<<7)+0.5)) : ((int32_t)((A)*(1<<7)-0.5)))
#define Q8(A) (((A) >= 0) ? ((int32_t)((A)*(1<<8)+0.5)) : ((int32_t)((A)*(1<<8)-0.5)))
#define Q9(A) (((A) >= 0) ? ((int32_t)((A)*(1<<9)+0.5)) : ((int32_t)((A)*(1<<9)-0.5)))
#define Q10(A) (((A) >= 0) ? ((int32_t)((A)*(1<<10)+0.5)) : ((int32_t)((A)*(1<<10)-0.5)))
#define Q11(A) (((A) >= 0) ? ((int32_t)((A)*(1<<11)+0.5)) : ((int32_t)((A)*(1<<11)-0.5)))
#define Q12(A) (((A) >= 0) ? ((int32_t)((A)*(1<<12)+0.5)) : ((int32_t)((A)*(1<<12)-0.5)))
#define Q13(A) (((A) >= 0) ? ((int32_t)((A)*(1<<13)+0.5)) : ((int32_t)((A)*(1<<13)-0.5)))

#define Q14(A) (((A) >= 0) ? ((int32_t)((A)*(1<<14)+0.5)) : ((int32_t)((A)*(1<<14)-0.5)))
#define Q15(A) (((A) >= 0) ? ((int32_t)((A)*(1<<15)+0.5)) : ((int32_t)((A)*(1<<15)-0.5)))
#define Q16(A) (((A) >= 0) ? ((int32_t)((A)*(1<<16)+0.5)) : ((int32_t)((A)*(1<<16)-0.5)))
#define Q17(A) (((A) >= 0) ? ((int32_t)((A)*(1<<17)+0.5)) : ((int32_t)((A)*(1<<17)-0.5)))
#define Q18(A) (((A) >= 0) ? ((int32_t)((A)*(1<<18)+0.5)) : ((int32_t)((A)*(1<<18)-0.5)))
#define Q19(A) (((A) >= 0) ? ((int32_t)((A)*(1<<19)+0.5)) : ((int32_t)((A)*(1<<19)-0.5)))
#define Q20(A) (((A) >= 0) ? ((int32_t)((A)*(1<<20)+0.5)) : ((int32_t)((A)*(1<<20)-0.5)))
#define Q21(A) (((A) >= 0) ? ((int32_t)((A)*(1<<21)+0.5)) : ((int32_t)((A)*(1<<21)-0.5)))
#define Q22(A) (((A) >= 0) ? ((int32_t)((A)*(1<<22)+0.5)) : ((int32_t)((A)*(1<<22)-0.5)))
#define Q23(A) (((A) >= 0) ? ((int32_t)((A)*(1<<23)+0.5)) : ((int32_t)((A)*(1<<23)-0.5)))
#define Q24(A) (((A) >= 0) ? ((int32_t)((A)*(1<<24)+0.5)) : ((int32_t)((A)*(1<<24)-0.5)))
#define Q25(A) (((A) >= 0) ? ((int32_t)((A)*(1<<25)+0.5)) : ((int32_t)((A)*(1<<25)-0.5)))
#define Q26(A) (((A) >= 0) ? ((int32_t)((A)*(1<<26)+0.5)) : ((int32_t)((A)*(1<<26)-0.5)))
#define Q27(A) (((A) >= 0) ? ((int32_t)((A)*(1<<27)+0.5)) : ((int32_t)((A)*(1<<27)-0.5)))
#define Q28(A) (((A) >= 0) ? ((int32_t)((A)*(1<<28)+0.5)) : ((int32_t)((A)*(1<<28)-0.5)))
#define Q29(A) (((A) >= 0) ? ((int32_t)((A)*(1<<29)+0.5)) : ((int32_t)((A)*(1<<29)-0.5)))
#define Q30(A) (((A) >= 0) ? ((int32_t)((A)*(1<<30)+0.5)) : ((int32_t)((A)*(1<<30)-0.5)))
#define Q31(A) (((1.00 - (A))<0.00000001)? ((int32_t)0x7FFFFFFF) : (((A) >= 0) ? ((int32_t)((A)*(1<<31)+0.5)) : ((int32_t)((A)*(1<<31)-0.5))))
#define MUL_14(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1<<13)) >> 14)
#define MUL_15(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1<<14)) >> 15)
#define MUL_16(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1<<15)) >> 16)
#define MUL_17(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1<<16)) >> 17)
#define MUL_18(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1<<17)) >> 18)
#define MUL_19(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1<<18)) >> 19)
#define MUL_20(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 19)) >> 20)
#define MUL_21(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 20)) >> 21)
#define MUL_22(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 21)) >> 22)
#define MUL_23(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 22)) >> 23)
#define MUL_24(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 23)) >> 24)
#define MUL_25(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 24)) >> 25)
#define MUL_26(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 25)) >> 26)
#define MUL_27(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 26)) >> 27)
#define MUL_28(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 27)) >> 28)
#define MUL_29(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 28)) >> 29)
#define MUL_30(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 29)) >> 30)
#define MUL_31(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)+(1 << 30)) >> 31)
#define MUL_32(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)) >> 32)
#define MUL_33(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)) >> 33)
#define MUL_34(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)) >> 34)
#define MUL_35(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)) >> 35)
#define MUL_36(A,B) (int32_t)(((int64_t)(A)*(int64_t)(B)) >> 36)

/*
#ifndef fixfast_mul
#ifdef fix_mul
#define FIXFAST_BSHIFT(b)	(b)
#define fixfast_mul(a,b)	fix_mul(a,b)
#else//fix_mul
#define FIXFAST_BSHIFT(b)	RSHIFT_ROUND(b,FIX_SHIFT_B+FIX_MUL_ADJ)
#define fixfast_mul(a,b)	(((a)>>(FIX_SHIFT_A-FIX_MUL_ADJ))*(b))
#endif//fix_mul
#endif//fixfast_mul

#ifndef fix_mul
#define fix_mul(a,b)		(((a)>>(FIX_SHIFT_A-FIX_MUL_ADJ))*((b)>>(FIX_SHIFT_B+FIX_MUL_ADJ)))
#endif//fix_mul

#ifndef fix_mul64
#define fix_mul64(a,b)		((fix_t)(((int64_t)(a)*(int64_t)(b))>>FIX_FRACBITS))
#endif//fix_mul64

#ifndef fix_1div
#define fix_1div(a)			((fix_t)((((int64_t)1)<<(FIX_FRACBITS*2))/(a)))
#endif

#define checkfix(a)	assert((a)>-0x60000000 && (a)<0x60000000);
#define fix_mul2(a,b) ((fix_t)(((int64_t)(a)*(int64_t)(b))))*/

#endif//_PORTAB_H