/* Double precision operations */
/* $Id$ */

#if (!L_EXTRACT_IS_MACRO)
void voAMRWBPDecL_Extract (Word32 L_32, Word16 *hi, Word16 *lo);
#else
#define voAMRWBPDecL_Extract(L_32, hi, lo) do{\
                                    *(hi) = (Word16)((L_32) >> 16);\
                                    *(lo) = (Word16)(((L_32) & 0xffff) >> 1);\
                                  }while(0)
#endif

Word32 voAMRWBPDecL_Comp (Word16 hi, Word16 lo);

#if (!MPY_32_IS_MACRO)
Word32 Mpy_32 (Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);
#else
#define Mpy_32(hi1, lo1, hi2, lo2) (((hi1) * (hi2) + (((hi1) * (lo2)) >> 15) + (((lo1) * (hi2)) >> 15)) << 1)
#endif

Word32 voAMRWBPDecDiv_32 (Word32 L_num, Word16 denom_hi, Word16 denom_lo);

// @shanrong modified
#if (!FUNC_MPY_32_16_MACRO)
Word32 Mpy_32_16 (Word16 hi, Word16 lo, Word16 n);
#else
#define Mpy_32_16(hi, lo, n) ((((Word32)(hi) * (n)) << 1) + (((Word32)(lo) * (n) >> 15) << 1))
#endif
// end
