
/***********************************************************************
*                                                                      *
*    VisualOn, Inc. Confidential and Proprietary, 2003-2010            *
*                                                                      *
************************************************************************/

/* Double precision operations */
/* $Id$ */

#ifndef __OPER_32B_H__
#define __OPER_32B_H__

void L_Extract (Word32 L_32, Word16 *hi, Word16 *lo);
Word32 Div_32 (Word32 L_num, Word16 denom_hi, Word16 denom_lo);

#define voMpy_32(hi1, lo1, hi2, lo2) (((hi1) * (hi2) + (((hi1) * (lo2)) >> 15) + (((lo1) * (hi2)) >> 15)) << 1)
#define voMpy_32_16(hi, lo, n) ((((Word32)(hi) * (n)) << 1) + (((Word32)(lo) * (n) >> 15) << 1))


#endif //__OPER_32B_H__

