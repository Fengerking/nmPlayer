/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |                                                                           |
 | $Id: basic_op.h,v 1.8 2005-02-20 12:07:26 ehr Exp $
 |___________________________________________________________________________|
*/

#ifndef __BASIC_OP_H
#define __BASIC_OP_H

#include "typedef.h"

#ifdef LINUX
#define __inline static __inline__
#endif

//extern Flag Overflow;
// extern Flag Carry;
#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000
#define ABS(a)	((a) >= 0) ? (a) : (-(a))

#if (SATRUATE_IS_MACRO && (!SATRUATE_IS_INLINE))
#define saturate(L_var1) (((L_var1) > 0X00007fffL) ? (MAX_16): (((L_var1) < (Word32) 0xffff8000L) ? (MIN_16): ((L_var1) & 0xffff)))
#elif (SATRUATE_IS_INLINE && (!SATRUATE_IS_MACRO))
__inline Word16 saturate(Word32 L_var1);
#else
Word16 saturate(Word32 L_var1);
#endif

/* Short shift left,    1   */
#if((!SHL_IS_MACRO) && SHL_IS_INLINE)
__inline Word16 shl (Word16 var1, Word16 var2);
#else
Word16 shl (Word16 var1, Word16 var2);
#endif

/* Short shift right,   1   */
#if((!SHR_IS_MACRO) && SHR_IS_INLINE)
__inline Word16 shr (Word16 var1, Word16 var2);
#else
Word16 shr (Word16 var1, Word16 var2);
#endif

#if (L_MULT_IS_MACRO && (!L_MULT_IS_INLINE))
#define L_mult(var1, var2) ((((Word32) var1 *(Word32) var2) != (Word32)0x40000000L) ? (((Word32) var1 *(Word32) var2) << 1) : (MAX_32))
#elif ((!L_MULT_IS_MACRO) && L_MULT_IS_INLINE)
__inline Word32 L_mult(Word16 var1, Word16 var2);
#else
Word32 L_mult(Word16 var1, Word16 var2);
#endif

/* Msu,  1  */
#if (L_MSU_IS_MACRO && (!L_MSU_IS_INLINE))
#define L_msu (L_var3, var1, var2)
#elif ((!L_MSU_IS_MACRO) && L_MSU_IS_INLINE)
__inline Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);
#else
Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2);
#endif
    
/* Long sub,        2 */
#if (L_SUB_IS_MACRO && (!L_SUB_IS_INLINE))
#define L_sub(L_var1, L_var2) ((((((L_var1) ^ (L_var2)) & MIN_32) != 0) && ((((L_var1) - (L_var2)) ^ (L_var1)) & MIN_32)) ? \
                               (((L_var1) < 0L) ? MIN_32 : MAX_32) : ((L_var1) - (L_var2)))
#elif ((!L_SUB_IS_MACRO) && L_SUB_IS_INLINE)
__inline Word32 L_sub(Word32 L_var1, Word32 L_var2);
#else
Word32 L_sub(Word32 L_var1, Word32 L_var2);
#endif

/* Long shift left, 2 */
#if (L_SHL_IS_MACRO && (!L_SHL_IS_INLINE))
#define L_shl (L_var3, var1, var2)
#elif ((!L_SHL_IS_MACRO) && L_SHL_IS_INLINE)
__inline Word32 L_shl (Word32 L_var1, Word16 var2);
#else
Word32 L_shl (Word32 L_var1, Word16 var2);
#endif

/* Long shift right, 2*/
#if (L_SHR_IS_MACRO && (!L_SHR_IS_INLINE))
#define L_msu (L_var3, var1, var2)
#elif ((!L_SHR_IS_MACRO) && L_SHR_IS_INLINE)
__inline Word32 L_shr (Word32 L_var1, Word16 var2);
#else
Word32 L_shr (Word32 L_var1, Word16 var2);
#endif

/* Short add,           1   */
#if (ADD_IS_MACRO && (!ADD_IS_INLINE))
#define add(var1, var2) ((((Word32)(var1) + (var2)) > 0X00007fffL) ?\
                         (MAX_16):\
                         ((((Word32)(var1) + (var2)) < (Word32)0xffff8000L) ? (MIN_16): ((Word16)(((Word32)(var1) + (var2)) & 0xffff))))
#elif ((!ADD_IS_MACRO) && ADD_IS_INLINE)
__inline Word16 add (Word16 var1, Word16 var2);
#else
Word16 add (Word16 var1, Word16 var2);
#endif
    
/* Short sub,           1   */
#if (SUB_IS_MACRO && (!SUB_IS_INLINE))
#define sub(var1, var2) ((Word16)((((Word32)(var1) - (Word32)(var2)) > 0X00007fffL) ?\
                         MAX_16 :\
                         ((((Word32)(var1) - (Word32)(var2)) < (Word32) 0xffff8000L) ? MIN_16 : ((var1) - (var2)))))
#elif ((!SUB_IS_MACRO) && SUB_IS_INLINE)
__inline Word16 sub(Word16 var1, Word16 var2);
#else
Word16 sub(Word16 var1, Word16 var2);
#endif

/* Short division,       18  */
#if (DIV_S_IS_MACRO && (!DIV_S_IS_INLINE))
#define div_s
#elif ((!DIV_S_IS_MACRO) && DIV_S_IS_INLINE)
__inline Word16 div_s (Word16 var1, Word16 var2);
#else
Word16 div_s (Word16 var1, Word16 var2);
#endif

/* Short mult,          1   */
#if (MULT_IS_MACRO && (!MULT_IS_INLINE))
#define mult
#elif ((!MULT_IS_MACRO) && MULT_IS_INLINE)
__inline Word16 mult (Word16 var1, Word16 var2);
#else
Word16 mult (Word16 var1, Word16 var2);
#endif

/* Short norm,           15  */
#if (NORM_S_IS_MACRO && (!NORM_S_IS_INLINE))
#define norm_s
#elif ((!NORM_S_IS_MACRO) && NORM_S_IS_INLINE)
__inline Word16 norm_s (Word16 var1);
#else
Word16 norm_s (Word16 var1);
#endif

/* Long norm,            30  */
#if (NORM_L_IS_MACRO && (!NORM_L_IS_INLINE))
#define norm_l
#elif ((!NORM_L_IS_MACRO) && NORM_L_IS_INLINE)
__inline Word16 norm_l (Word32 L_var1);
#else
Word16 norm_l (Word32 L_var1);
#endif

/* Round,               1   */
#if (ROUND_IS_MACRO && (!ROUND_IS_INLINE))
#define round16(L_var1) ((((L_var1) > 0) && (((L_var1) + 0x8000) < 0)) ? (MAX_16) : ((Word16)(((L_var1) + 0x8000) >> 16)))
#elif ((!ROUND_IS_MACRO) && ROUND_IS_INLINE)
__inline Word16 round16(Word32 L_var1);
#else
Word16 round16(Word32 L_var1);
#endif

/* Mac,  1  */
#if (L_MAC_IS_MACRO && (!L_MAC_IS_INLINE))
#define L_mac(L_var3, var1, var2) ((L_var3) + (((Word32)(var1) * (var2)) << 1))
#elif ((!L_MAC_IS_MACRO) && L_MAC_IS_INLINE)
__inline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);
#else
Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2);
#endif

#if (L_ADD_IS_MACRO && (!L_ADD_IS_INLINE))
#define L_add(L_var1, L_var2) \
  (((((L_var1) ^ (L_var2)) & MIN_32) != 0) ? ((L_var1) + (L_var2)) :\
    (((((L_var1) + (L_var2)) ^ (L_var1)) & MIN_32) ? (((L_var1) < 0) ? MIN_32 : MAX_32) : ((L_var1) + (L_var2))))
#elif ((!L_ADD_IS_MACRO) && L_ADD_IS_INLINE)
__inline Word32 L_add (Word32 L_var1, Word32 L_var2);
#else
Word32 L_add (Word32 L_var1, Word32 L_var2);
#endif

/* Short abs,           1   */
#if (ABS_S_IS_MACRO && (!ABS_S_IS_INLINE))
#define abs_s(x)       ((Word16)(((x) != MIN_16) ? (((x) >= 0) ? (x) : (-(x))) : MAX_16))
#elif ((!ABS_S_IS_MACRO) && ABS_S_IS_INLINE)
__inline Word16 abs_s (Word16 var1);
#else
Word16 abs_s (Word16 var1);
#endif

/* Extract high,        1   */
#if (EXTRACT_H_IS_MACRO && (!EXTRACT_H_IS_INLINE))
#define extract_h(x)   ((Word16)(((x) >> 16) & 0xffff))
#elif ((!EXTRACT_H_IS_MACRO) && EXTRACT_H_IS_INLINE)
__inline Word16 extract_h (Word32 L_var1);
#else
Word16 extract_h (Word32 L_var1);
#endif

/* Extract low,         1   */
#if (EXTRACT_L_MACRO && (!EXTRACT_L_IS_INLINE))
#define extract_l(x) ((Word16)((x) & 0xffff))
#elif ((!EXTRACT_L_MACRO) && EXTRACT_L_IS_INLINE)
__inline Word16 extract_l(Word32 L_var1);
#else
Word16 extract_l(Word32 L_var1);
#endif

/* 16 bit var1 -> MSB,     2 */
#if (L_DEPOSIT_H_MACRO && (!L_DEPOSIT_H_IS_INLINE))
#define L_deposit_h(x) (((Word32)(x)) << 16)
#elif ((!L_DEPOSIT_H_MACRO) && L_DEPOSIT_H_IS_INLINE)
__inline Word32 L_deposit_h (Word16 var1);
#else
Word32 L_deposit_h (Word16 var1);
#endif

/* 16 bit var1 -> LSB,     2 */
#if (L_DEPOSIT_L_MACRO && (!L_DEPOSIT_L_INLINE))
#define L_deposit_l(x) ((Word32)(x))
#elif ((!L_DEPOSIT_L_MACRO) && L_DEPOSIT_L_INLINE)
__inline Word32 L_deposit_l (Word16 var1);
#else
Word32 L_deposit_l (Word16 var1);
#endif

/* Long abs,              3  */
#if (L_ABS_IS_MACRO && (!L_ABS_IS_INLINE))
#define L_abs(x) (((x) != MIN_32) ? (((x) >= 0) ? (x) : (-(x))) : MAX_32)
#elif ((!L_ABS_IS_MACRO) && L_ABS_IS_INLINE)
__inline Word32 L_abs (Word32 L_var1);
#else
Word32 L_abs (Word32 L_var1);
#endif

/* Short negate,        1   */
#if (NEGATE_IS_MACRO && (!NEGATE_IS_INLINE))
#define negate(var1) ((Word16)(((var1) == MIN_16) ? MAX_16 : (-(var1))))
#elif ((!NEGATE_IS_MACRO) && NEGATE_IS_INLINE)
__inline Word16 negate (Word16 var1);
#else
Word16 negate (Word16 var1);
#endif

/* Long negate,     2 */
#if (L_NEGATE_IS_MACRO && (!L_NEGATE_IS_INLINE))
#define L_negate(L_var1) (((L_var1) == (MIN_32)) ? (MAX_32) : (-(L_var1)))
#elif ((!L_NEGATE_IS_MACRO) && L_NEGATE_IS_INLINE)
__inline Word32 L_negate (Word32 L_var1);
#else
Word32 L_negate (Word32 L_var1);
#endif

/* Mult with round, 2 */
#if (MULT_R_IS_MACRO && (!MULT_R_IS_INLINE))
#define
#elif ((!MULT_R_IS_MACRO) && MULT_R_IS_INLINE)
__inline Word16 mult_r(Word16 var1, Word16 var2);
#else
Word16 mult_r(Word16 var1, Word16 var2);
#endif

/* Shift right with round, 2           */
#if (SHR_R_IS_MACRO && (!SHR_R_IS_INLINE))
#define
#elif ((!SHR_R_IS_MACRO) && SHR_R_IS_INLINE)
__inline Word16 shr_r (Word16 var1, Word16 var2);
#else
Word16 shr_r (Word16 var1, Word16 var2);
#endif

/* Mac with rounding,2 */
#if (MAC_R_IS_MACRO && (!MAC_R_IS_INLINE))
#define
#elif ((!MAC_R_IS_MACRO) && MAC_R_IS_INLINE)
__inline Word16 mac_r (Word32 L_var3, Word16 var1, Word16 var2);
#else
Word16 mac_r (Word32 L_var3, Word16 var1, Word16 var2);
#endif

/* Msu with rounding,2 */
#if (MSU_R_IS_MACRO && (!MSU_R_IS_INLINE))
#define
#elif ((!MSU_R_IS_MACRO) && MSU_R_IS_INLINE)
__inline Word16 msu_r (Word32 L_var3, Word16 var1, Word16 var2);
#else
Word16 msu_r (Word32 L_var3, Word16 var1, Word16 var2);
#endif

/* Long shift right with round,  3             */
#if (L_SHR_R_IS_MACRO && (!L_SHR_R_IS_INLINE))
#define
#elif ((!L_SHR_R_IS_MACRO) && L_SHR_R_IS_INLINE)
__inline Word32 L_shr_r (Word32 L_var1, Word16 var2);
#else
Word32 L_shr_r (Word32 L_var1, Word16 var2);
#endif

#if ARMV4_INASM
__inline Word32 ASM_L_shr(Word32 L_var1, Word16 var2)
{
#ifdef  LINUX
	Word32 result =0; 
	asm volatile( 
		"MOV %[result], %[L_var1], ASR %[var2] \n" 
		:[result]"=r"(result)
		:[L_var1]"r"(L_var1), [var2]"r"(var2)
		); 
	return result;	
#else
    __asm
    {
        MOV    r0,r0,ASR r1
    }
#endif	
}
 
__inline Word32 ASM_L_shl(Word32 L_var1, Word16 var2)
{
#ifdef  LINUX
	Word32 result = 0; 
	asm volatile( 
		"MOV	r2, %[L_var1] \n"
		"MOV	r3, #0x7fffffff\n"
		"MOV	%[result], %[L_var1], ASL %[var2] \n" 
		"TEQ	r2, %[result], ASR %[var2]\n"
		"EORNE  %[result],r3,r2,ASR#31\n"
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1), [var2]"r"(var2)
		:"r2", "r3"
		); 
	return result;	
#else
    __asm
    {
        
        MOV        r2,r0
        MOV        r3, #0x7fffffff
        MOV        r0,r2,ASL r1
        
        TEQ        r2,r0,ASR r1        ;
        EORNE	   r0,r3,r2,ASR#31
    }
#endif
}

__inline Word32 ASM_shr(Word32 L_var1, Word16 var2)
{
#ifdef  LINUX
	Word32 result =0; 
	asm volatile( 
		"CMP	%[var2], #15\n"
		"MOVGE  %[var2], #15\n"
		"MOV	%[result], %[L_var1], ASR %[var2]\n"
		:[result]"=r"(result)
		:[L_var1]"r"(L_var1), [var2]"r"(var2) 
		); 
	return result;	
#else	
    __asm
    {
        cmp    r1,#15
        MOVGE  r1,#15
        MOV    r0,r0,ASR r1
    }
#endif	
} 
__inline Word32 ASM_shl(Word32 L_var1, Word16 var2)
{
#ifdef  LINUX
	Word32 result =0; 
	asm volatile( 
		"CMP	%[var2], #16\n"
		"MOVGE  %[var2], #16\n"
		"MOV    %[result], %[L_var1], ASL %[var2]\n"
		"MOV    r3, #1\n"
        "MOV    r2, %[result], ASR #15\n"
        "RSB    r3,r3,r3,LSL #15 \n"
        "TEQ    r2, %[result], ASR #31 \n"
        "EORNE  %[result], r3, %[result],ASR #31"
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1), [var2]"r"(var2)
		:"r2", "r3"
		); 
	return result;	
#else
    __asm
    {
        ;LDR    r3,=#0x7fff
        cmp        r1,#16
        MOVGE    r1,#16
        MOV        r0,r0,ASL r1
        MOV       r3,#1
        MOV     r2,r0,ASR#15    ;
        RSB        r3,r3,r3,LSL#15; LDR    r3,=#0x7fff
        TEQ        r2,r0,ASR#31    ;
        EORNE    r0,r3,r0,ASR#31
    }
#endif
} 
#endif

/*___________________________________________________________________________
 |                                                                           |
 |   definitions for inline basic arithmetic operators                       |
 |___________________________________________________________________________|
*/
#if (SATRUATE_IS_INLINE && (!SATRUATE_IS_MACRO))
__inline Word16 saturate(Word32 L_var1)
{
#if ARMV5TE_SAT
#ifdef LINUX
	Word16 result =0;
	asm volatile (
		"MOV	%[result], %[L_var1]\n"
		"MOV	r3, #1\n"
		"MOV	r2,%[L_var1],ASR#15\n"
		"RSB	r3, r3, r3, LSL #15\n"
		"TEQ	r2,%[L_var1],ASR#31\n"	
		"EORNE	%[result],r3,%[L_var1],ASR#31\n"
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1)
		:"r2", "r3"			
	);

	return result;
#else
	__asm
	{
		LDR	r3,=#0x7fff
		MOV r2,r0,ASR#15	;r2= high16bits of r0
		TEQ	r2,r0,ASR#31	;
		EORNE	r0,r3,r0,ASR#31			
	}
#endif	
#else
    Word16 var_out;
    
    //var_out = (L_var1 > (Word32)0X00007fffL) ? (MAX_16) : ((L_var1 < (Word32)0xffff8000L) ? (MIN_16) : ((Word16)L_var1));

    if (L_var1 > 0X00007fffL)
    {
        var_out = MAX_16;
    }
    else if (L_var1 < (Word32) 0xffff8000L)
    {
        var_out = MIN_16;
    }
    else
    {
        var_out = extract_l(L_var1);
    }

    return (var_out);
#endif
}
#endif

/* Short shift left,    1   */
#if ((!SHL_IS_MACRO) && SHL_IS_INLINE)
__inline Word16 shl (Word16 var1, Word16 var2)
{
#if ARMV5TE_SHL
	if(var2>=0)
	{
		return ASM_shl( var1, var2);
	}
	else
	{
		return ASM_shr( var1, -var2);
	}
#else
    Word16 var_out =0;
    Word32 result =0;

    if (var2 < 0)
    {
//        if (var2 < -16)
//            var2 = -16;
        var_out = shr (var1, (Word16)-var2);
    }
    else
    {
        result = (Word32) var1 *((Word32) 1 << var2);

        if ((var2 > 15 && var1 != 0) || (result != (Word32) ((Word16) result)))
        {
            var_out = (Word16)((var1 > 0) ? MAX_16 : MIN_16);
        }
        else
        {
            var_out = extract_l(result);
        }
    }
    return (var_out);
#endif
}
#endif

/* Short shift right,   1   */
#if ((!SHR_IS_MACRO) && SHR_IS_INLINE)
__inline Word16 shr (Word16 var1, Word16 var2)
{
#if ARMV5TE_SHR
	if(var2>=0)
	{
		return  ASM_shr( var1, var2);
	}
	else
	{
		return  ASM_shl( var1, -var2);
	}
#else
    Word16 var_out;

    if (var2 < 0)
    {
        var_out = shl (var1, (Word16)-var2);
    }
    else
    {
        if (var2 >= 15)
        {
            var_out = (Word16)((var1 < 0) ? -1 : 0);
        }
        else
        {
            if (var1 < 0)
            {
                var_out = (Word16)(~((~var1) >> var2));
            }
            else
            {
                var_out = (Word16)(var1 >> var2);
            }
        }
    }

    return (var_out);
#endif
}
#endif


#if ((!L_MULT_IS_MACRO) && L_MULT_IS_INLINE)
__inline Word32 L_mult(Word16 var1, Word16 var2)
{
#if ARMV5TE_L_MULT
#ifdef LINUX
	Word32 result =0; 
	asm volatile( 
		"SMULBB %[result], %[var1], %[var2] \n" 
		"QADD %[result], %[result], %[result] \n" 
		:[result]"+r"(result)
		:[var1]"r"(var1), [var2]"r"(var2)
		); 
	return result;
#else
    __asm
    {
        SMULBB    r1,r1,r0
        QADD    r0,r1,r1
    }
#endif
#else
    Word32 L_var_out;

    L_var_out = (Word32) var1 *(Word32) var2;

    if (L_var_out != (Word32) 0x40000000L)
    {
        L_var_out <<= 1;
    }
    else
    {
        L_var_out = MAX_32;
    }
    return (L_var_out);
#endif
}
#endif

#if ((!L_MSU_IS_MACRO) && L_MSU_IS_INLINE)
__inline Word32 L_msu (Word32 L_var3, Word16 var1, Word16 var2)
{
#if ARMV5TE_L_MSU
#ifdef LINUX
	Word32 result=0; 
	asm volatile( 
		"SMULBB %[result], %[var1], %[var2] \n" 
		"QADD %[result], %[result], %[result] \n"
		"QSUB %[result], %[L_var3], %[result]\n"
		:[result]"+r"(result)
		:[L_var3]"r"(L_var3), [var1]"r"(var1), [var2]"r"(var2)
		); 
	return result;
#else	
    __asm
    {
        SMULBB    r1,r1,r2
        QADD    r1,r1,r1
        QSUB    r0,r0,r1
    }  
#endif	
#else
    Word32 L_var_out;
    Word32 L_product;

    L_product = L_mult(var1, var2);
    L_var_out = L_sub(L_var3, L_product);
    return (L_var_out);
#endif
}
#endif

#if ((!L_SUB_IS_MACRO) && L_SUB_IS_INLINE)
__inline Word32 L_sub(Word32 L_var1, Word32 L_var2)
{
#if ARMV5TE_L_SUB
#ifdef LINUX
	Word32 result=0; 
	asm volatile( 
		"QSUB %[result], %[L_var1], %[L_var2]\n"
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1), [L_var2]"r"(L_var2)
		); 
	return result;
#else
    __asm
    {
        QSUB    r0,r0,r1
    }
#endif
#else
    Word32 L_var_out;

    L_var_out = L_var1 - L_var2;

    if (((L_var1 ^ L_var2) & MIN_32) != 0)
    {
        if ((L_var_out ^ L_var1) & MIN_32)
        {
            L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
        }
    }

    return (L_var_out);
#endif
}
#endif

#if ((!L_SHL_IS_MACRO) && L_SHL_IS_INLINE)
__inline Word32 L_shl(Word32 L_var1, Word16 var2)
{
#if ARMV5TE_L_SHL
    if(var2>=0)
    {
        return  ASM_L_shl( L_var1, var2);
    }
    else
    {
        return  ASM_L_shr( L_var1, -var2);
    }
#else
    Word32 L_var_out = 0L;

    if (var2 <= 0)
    {
        L_var1 = L_shr(L_var1, (Word16)-var2);
    }
    else
    {
        for (; var2 > 0; var2--)
        {
            if (L_var1 > (Word32) 0X3fffffffL)
            {
                return MAX_32;
            }
            else
            {
                if (L_var1 < (Word32) 0xc0000000L)
                {
                    return MIN_32;
                }
            }
            L_var1 <<= 1;
            L_var_out = L_var1;
        }
    }
    return (L_var1);
#endif
}
#endif

#if ((!L_SHR_IS_MACRO) && L_SHR_IS_INLINE)
__inline Word32 L_shr (Word32 L_var1, Word16 var2)
{
#if ARMV5TE_L_SHR
	if(var2>=0)
	{
		return ASM_L_shr( L_var1, var2);
	}
	else
	{
		return ASM_L_shl( L_var1, -var2);
	}
#else
    Word32 L_var_out;

    if (var2 < 0)
    {
        L_var_out = L_shl (L_var1, (Word16)-var2);
    }
    else
    {
        if (var2 >= 31)
        {
            L_var_out = (L_var1 < 0L) ? -1 : 0;
        }
        else
        {
            if (L_var1 < 0)
            {
                L_var_out = ~((~L_var1) >> var2);
            }
            else
            {
                L_var_out = L_var1 >> var2;
            }
        }
    }
    return (L_var_out);
#endif
}
#endif

/* Short add,           1   */
#if ((!ADD_IS_MACRO) && ADD_IS_INLINE)
__inline Word16 add (Word16 var1, Word16 var2)
{
#if ARMV5TE_ADD
#ifdef LINUX
	Word32 result=0; 
	asm volatile( 
		"ADD  %[result], %[var1], %[var2] \n" 
		"MOV  r3, #0x1\n"
		"MOV  r2, %[result], ASR #15\n"
		"RSB  r3, r3, r3, LSL, #15\n"
		"TEQ  r2, %[result], ASR #31\n"
		"EORNE %[result], r3, %[result], ASR #31"
		:[result]"+r"(result)
		:[var1]"r"(var1), [var2]"r"(var2)
		:"r2", "r3"
		); 
	return result;
#else
	__asm
	{
		;MOV	r1,=#0x7fff
		ADD	r0,r0,r1
		LDR	r3,=#0x7fff
		MOV r2,r0,ASR#15	;
		TEQ	r2,r0,ASR#31	;
		EORNE	r0,r3,r0,ASR#31
	}
#endif
#else
    Word16 var_out;
    Word32 L_sum;

    L_sum = (Word32) var1 + var2;
    var_out = saturate(L_sum);

    return (var_out);
#endif
}
#endif

/* Short sub,           1   */
#if ((!SUB_IS_MACRO) && SUB_IS_INLINE)
__inline Word16 sub(Word16 var1, Word16 var2)
{
#if ARMV5TE_SUB
#ifdef LINUX
	Word32 result = 0; 
	asm volatile( 
		"MOV   r3, #1\n"
		"SUB   %[result], %[var1], %[var2] \n"		
		"RSB   r3,r3,r3,LSL#15\n"
		"MOV   r2, %[var1], ASR #15 \n" 
		"TEQ   r2, %[var1], ASR #31 \n"
		"EORNE %[result], r3, %[result], ASR #31 \n"
		:[result]"+r"(result)
		:[var1]"r"(var1), [var2]"r"(var2)
		:"r2", "r3"
		); 
	return result;
#else //LINUX
	__asm
	{
		LDR	r3,=#0x7fff
		sub	r0,r0,r1
		MOV r2,r0,ASR#15	;r2= high16bits of r0
		TEQ	r2,r0,ASR#31	;
		EORNE	r0,r3,r0,ASR#31
	}
#endif //LINUX
#else
    Word16 var_out;
    Word32 L_diff;

    L_diff = (Word32) var1 - var2;
    var_out = saturate(L_diff);
   
    return (var_out);
#endif
}
#endif

/* Short division,       18  */
#if ((!DIV_S_IS_MACRO) && DIV_S_IS_INLINE)
__inline Word16 div_s (Word16 var1, Word16 var2)
{
    Word16 var_out = 0;
    Word16 iteration;
    Word32 L_num;
    Word32 L_denom;

    var_out = MAX_16;
    if (var1!= var2)//var1!= var2
    {
    	var_out = 0;
    	L_num = (Word32) var1;
    
    	L_denom = (Word32) var2;
    	
		return (L_num<<15)/var2;

    	for (iteration = 0; iteration < 15; iteration++)
    	{
    		var_out <<= 1;
    		L_num <<= 1;
    		
    		if (L_num >= L_denom)
    		{
    			L_num -= L_denom;
    			var_out++;
    		}
    	}
    }
    return (var_out);
}
#endif

/* Short mult,          1   */
#if ((!MULT_IS_MACRO) && MULT_IS_INLINE)
__inline Word16 mult (Word16 var1, Word16 var2)
{
#if ARMV5TE_MULT
#ifdef LINUX
	Word32 result = 0; 
	asm volatile( 
		"SMULBB r2, %[var1], %[var2] \n"
		"MOV	r3, #1\n"
		"MOV	%[result], r2, ASR #15\n"
		"RSB	r3, r3, r3, LSL #15\n"
		"MOV	r2, %[result], ASR #15\n"
		"TEQ	r2, %[result], ASR #31\n"
		"EORNE  %[result], r3, %[result], ASR #31 \n"
		:[result]"+r"(result)
		:[var1]"r"(var1), [var2]"r"(var2)
		:"r2", "r3"
		); 
	return result;
#else //LINUX
	__asm
	{		
		SMULBB	r1,r1,r0
		MOV 	r3,#1
		MOV       r0,r1,ASR#15
		RSB		r3,r3,r3,LSL#15;r3=0x7fff
		MOV 	r2,r0,ASR#15	;
		TEQ		r2,r0,ASR#31	;
		EORNE	r0,r3,r0,ASR#31
	}
#endif	
#else
    Word16 var_out;
    Word32 L_product;

    L_product = (Word32) var1 *(Word32) var2;
    L_product = (L_product & (Word32) 0xffff8000L) >> 15;
    if (L_product & (Word32) 0x00010000L)
        L_product = L_product | (Word32) 0xffff0000L;
    var_out = saturate(L_product);

    return (var_out);
#endif
}
#endif


/* Short norm,           15  */
#if ((!NORM_S_IS_MACRO) && NORM_S_IS_INLINE)
__inline Word16 norm_s (Word16 var1)
{
#if ARMV5TE_NORM_S
#ifdef LINUX
	Word16 result = 0; 
	asm volatile( 
		"MOV   r2,%[var1] \n"
		"CMP   r2, #0\n"
		"RSBLT %[var1], %[var1], #0 \n"
		"CLZNE %[result], %[var1]\n"
		"SUBNE %[result], %[result], #17\n"
		"MOVEQ %[result], #0\n"
		"CMP   r2, #-1\n"
		"MOVEQ %[result], #15\n" 
		:[result]"+r"(result)
		:[var1]"r"(var1)
		:"r2"
		); 
	return result;
#else //LINUX
    __asm
	{
		MOV r2,r0
		CMP r2,#0
		RSBLT r0,r0,#0;//r0 = -r0 if r0<0
		CLZNE r0,r0
		SUBNE r0,r0,#17
		MOVEQ r0,#0 ;//if r0 ==0
		CMP r2,#-1
		MOVEQ r0,#15 	
	}
#endif	
#else
    Word16 var_out;

    if (var1 == 0)
    {
        var_out = 0;
    }
    else
    {
        if (var1 == -1)
        {
            var_out = 15;
        }
        else
        {
            if (var1 < 0)
            {
                var1 = (Word16)~var1;
            }
            for (var_out = 0; var1 < 0x4000; var_out++)
            {
                var1 <<= 1;
            }
        }
    }
    return (var_out);
#endif
}
#endif

/* Long norm,            30  */
#if ((!NORM_L_IS_MACRO) && NORM_L_IS_INLINE)
__inline Word16 norm_l (Word32 L_var1)
{
#if ARMV5TE_NORM_L
#ifdef LINUX
	Word16 result = 0; 
	asm volatile( 
		"CMP    %[L_var1], #0\n"
		"CLZNE  %[result], %[L_var1]\n"
		"SUBNE  %[result], %[result], #1\n" 
		"MOVEQ  %[result], #0\n"
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1)
		); 
	return result;
#else //LINUX
    __asm
	{
		;//MOV r2,r0
		CMP r0,#0
		;//RSBLT r0,r0,#0;//r0 = -r0 if r0<0
		CLZNE r0,r0
		SUBNE r0,r0,#1
		MOVEQ r0,#0 ;//if r0 ==0
		;//CMP r2,#-1
		;//MOVEQ r0,#31	
	}
#endif	//LINUX
#else
    //Word16 var_out;

    //if (L_var1 == 0)
    //{
    //    var_out = 0;
    //}
    //else
    //{
    //    if (L_var1 == (Word32) 0xffffffffL)
    //    {
    //        var_out = 31;
    //    }
    //    else
    //    {
    //        if (L_var1 < 0)
    //        {
    //            L_var1 = ~L_var1;
    //        }
    //        for (var_out = 0; L_var1 < (Word32) 0x40000000L; var_out++)
    //        {
    //            L_var1 <<= 1;
    //        }
    //    }
    //}
    //return (var_out);
  Word16 a16;
  Word16 r = 0 ;       

   
  if ( L_var1 < 0 ) {
    L_var1 = ~L_var1; 
  }

  if (0 == (L_var1 & 0x7fff8000)) {
    a16 = extract_l(L_var1);
    r += 16;
       
    if (0 == (a16 & 0x7f80)) {
      r += 8;
         
      if (0 == (a16 & 0x0078)) {
        r += 4;
           
        if (0 == (a16 & 0x0006)) {
          r += 2;
             
          if (0 == (a16 & 0x0001)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x0004)) {
            r += 1;
          }
        }
      }
      else {
           
        if (0 == (a16 & 0x0060)) {
          r += 2;
             
          if (0 == (a16 & 0x0010)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x0040)) {
            r += 1;
          }
        }
      }
    } 
    else { 
         
      if (0 == (a16 & 0x7800)) {
        r += 4;
           
        if (0 == (a16 & 0x0600)) {
          r += 2;
             
          if (0 == (a16 & 0x0100)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x0400)) {
            r += 1;
          }
        }
      }
      else {
           
        if (0 == (a16 & 0x6000)) {
          r += 2;
             
          if (0 == (a16 & 0x1000)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x4000)) {
            r += 1;
          }
        }
      }
    }
  }
  else {
    a16 = extract_h(L_var1);
       
    if (0 == (a16 & 0x7f80)) {
      r += 8;
         
      if (0 == (a16 & 0x0078)) {
        r += 4 ;
           
        if (0 == (a16 & 0x0006)) {
          r += 2;
             
          if (0 == (a16 & 0x0001)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x0004)) {
            r += 1;
          }
        }
      }
      else {
           
        if (0 == (a16 & 0x0060)) {
          r += 2;
             
          if (0 == (a16 & 0x0010)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x0040)) {
            r += 1;
          }
        }
      }
    }
    else {
         
      if (0 == (a16 & 0x7800)) {
        r += 4;
           
        if (0 == (a16 & 0x0600)) {
          r += 2;
             
          if (0 == (a16 & 0x0100)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x0400)) {
            r += 1;
          }
        }
      }
      else {
           
        if (0 == (a16 & 0x6000)) {
          r += 2;
             
          if (0 == (a16 & 0x1000)) {
            r += 1;
          }
        }
        else {
             
          if (0 == (a16 & 0x4000)) {
            return 1;
          }
        }
      }
    }
  }
  
  return r ;
#endif
}
#endif

/* Round,               1   */
#if ((!ROUND_IS_MACRO) && ROUND_IS_INLINE)
__inline Word16 round16(Word32 L_var1)
{
#if ARMV5TE_ROUND
#ifdef LINUX
	Word16 result=0; 
	asm volatile( 
		"MOV   r1,#0x00008000\n"
		"QADD  %[result], %[L_var1], r1\n"
		"MOV   %[result], %[result], ASR #16 \n" 
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1)
		:"r1"
		); 
	return result;
#else //LINUX
	__asm
	{
		MOV     r1,#0x00008000
		QADD	r0,r1,r0
		MOV     r0,r0,ASR#16
	}
#endif//LINUX
#else   
    Word16 var_out;
    Word32 L_rounded;

    L_rounded = L_add (L_var1, (Word32) 0x00008000L);
    var_out = extract_h (L_rounded);
    return (var_out);
#endif
}
#endif

/* Mac,  1  */
#if ((!L_MAC_IS_MACRO) && L_MAC_IS_INLINE)
__inline Word32 L_mac (Word32 L_var3, Word16 var1, Word16 var2)
{
#if ARMV5TE_L_MAC
#ifdef LINUX
	Word32 result=0; 
	asm volatile( 
		"SMULBB %[result], %[var1], %[var2]\n"
		"QADD	%[result], %[result], %[result]\n"
		"QADD   %[result], %[result], %[L_var3]\n"
		:[result]"+r"(result)
		: [L_var3]"r"(L_var3), [var1]"r"(var1), [var2]"r"(var2)
		); 
	return result;
#else
    __asm
    {
        SMULBB    r1,r1,r2
        QADD    r1,r1,r1
        QADD    r0,r1,r0
    } 
#endif	
#else
    Word32 L_var_out;
    Word32 L_product;

    L_product = L_mult(var1, var2);
    L_var_out = L_add (L_var3, L_product);
    return (L_var_out);
#endif
}
#endif

#if ((!L_ADD_IS_MACRO) && L_ADD_IS_INLINE)
__inline Word32 L_add (Word32 L_var1, Word32 L_var2)
{
#if ARMV5TE_L_ADD
#ifdef LINUX
	Word32 result=0; 
	asm volatile( 
		"QADD %[result], %[L_var1], %[L_var2]\n"
		:[result]"+r"(result)
		:[L_var1]"r"(L_var1), [L_var2]"r"(L_var2)
		); 
	return result;
#else	
    __asm
    {
        QADD    r0,r1,r0
    }
#endif
#else
    Word32 L_var_out;

    L_var_out = L_var1 + L_var2;
    if (((L_var1 ^ L_var2) & MIN_32) == 0)
    {
        if ((L_var_out ^ L_var1) & MIN_32)
        {
            L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
        }
    }
    return (L_var_out);
#endif
}
#endif

#if ((!NEGATE_IS_MACRO) && (NEGATE_IS_INLINE))
__inline Word16 negate (Word16 var1)
{
    Word16 var_out;

    var_out = (Word16)((var1 == MIN_16) ? MAX_16 : -var1);

    return (var_out);
}
#endif

#if ((!L_NEGATE_IS_MACRO) && (L_NEGATE_IS_INLINE))
__inline Word32 L_negate (Word32 L_var1)
{
    Word32 L_var_out;

    L_var_out = (L_var1 == MIN_32) ? MAX_32 : -L_var1;

    return (L_var_out);
}
#endif

#if ((!MULT_R_IS_MACRO) && (MULT_R_IS_INLINE))
__inline Word16 mult_r (Word16 var1, Word16 var2)
{
    Word16 var_out;
    Word32 L_product_arr;

    L_product_arr = (Word32)var1 *(Word32)var2;       /* product */
    L_product_arr += (Word32)0x00004000L;      /* round */
    L_product_arr >>= 15;       /* shift */

    var_out = saturate(L_product_arr);

    return (var_out);
}
#endif

#if ((!SHR_R_IS_MACRO) && (SHR_R_IS_INLINE))
__inline Word16 shr_r (Word16 var1, Word16 var2)
{
    Word16 var_out;

    if (var2 > 15)
    {
        var_out = 0;
    }
    else
    {
        var_out = shr(var1, var2);

        if (var2 > 0)
        {
            if ((var1 & ((Word16) 1 << (var2 - 1))) != 0)
            {
                var_out++;
            }
        }
    }

    return (var_out);
}
#endif

#if ((!MAC_R_IS_MACRO) && (MAC_R_IS_INLINE))
__inline Word16 mac_r (Word32 L_var3, Word16 var1, Word16 var2)
{
    Word16 var_out;

    L_var3 = L_mac (L_var3, var1, var2);
//    L_var3 = L_add (L_var3, (Word32) 0x00008000L);
//    var_out = extract_h (L_var3);
    var_out = (Word16)((L_var3 + 0x8000L) >> 16);

    return (var_out);
}
#endif

#if ((!MSU_R_IS_MACRO) && (MSU_R_IS_INLINE))
__inline Word16 msu_r (Word32 L_var3, Word16 var1, Word16 var2)
{
    Word16 var_out;

    L_var3 = L_msu (L_var3, var1, var2);
//    L_var3 = L_add (L_var3, (Word32) 0x00008000L);
//    var_out = extract_h (L_var3);
    var_out = (Word16)((L_var3 + 0x8000L) >> 16);
    
    return (var_out);
}
#endif

#if ((!L_SHR_R_IS_MACRO) && (L_SHR_R_IS_INLINE))
__inline Word32 L_shr_r (Word32 L_var1, Word16 var2)
{
    Word32 L_var_out;

    if (var2 > 31)
    {
        L_var_out = 0;
    }
    else
    {
        L_var_out = L_shr(L_var1, var2);

        if (var2 > 0)
        {
            if ((L_var1 & ((Word32) 1 << (var2 - 1))) != 0)
            {
                L_var_out++;
            }
        }
    }

    return (L_var_out);
}
#endif

#if ((!EXTRACT_H_IS_MACRO) && EXTRACT_H_IS_INLINE)
__inline Word16 extract_h (Word32 L_var1)
{
    Word16 var_out;

    var_out = (Word16) (L_var1 >> 16);

    return (var_out);
}
#endif

#if ((!EXTRACT_L_MACRO) && EXTRACT_L_IS_INLINE)
__inline Word16 extract_l(Word32 L_var1)
{
	return (Word16) L_var1;
//	
//    Word16 var_out;
//
//    var_out = (Word16) L_var1;
//
//    return (var_out);
}
#endif

#endif
