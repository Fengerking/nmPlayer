/**********************************************************************
Each of the companies; Lucent, Motorola, Nokia, and Qualcomm (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the Enhanced Variable Rate Codec (EVRC) as 
it exists on the date of submittal of this form, should the EVRC be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the EVRC or the field of use of practice of the 
EVRC Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
 
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Conexant Systems, Inc., Qualcomm Inc., Nokia Inc,. Nortel         */
/* Networks, Lucent Technologies, and Motorola Inc. grants a free,   */
/* irrevocable license to 3GPP2 and its Organizational Partners to   */
/* incorporate text or other copyrightable material contained in the */
/* contribution and any modifications thereof in the creation of     */
/* 3GPP2 publications; to copyright and sell in Organizational       */
/* Partner's name any Organizational Partner's standards publication */
/* even though it may include portions of the contribution; and at   */
/* the Organizational Partner's sole discretion to permit others to  */
/* reproduce in whole or in part such contributions or the resulting */
/* Organizational Partner's standards publication. The contributor   */
/* must also be willing to grant licenses under such contributor     */
/* copyrights to third parties on reasonable, non-discriminatory     */
/* terms and conditions, as appropriate.                             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Copyright 2001 Conexant Systems, Inc.  This document is provided  */
/* as a discussion instrument only and is not binding on Conexant    */
/* Systems, Inc. The contributor reserves the right to add to, amend */
/* or otherwise modify the contents thereof.                         */
/*                                                                   */
/* Conexant Systems, Inc. grants a free, irrevocable license to 3GPP2*/
/* and its Organization Partners to incorporate, for any legitimate  */
/* 3GPP2 purpose, any copyrightable material contained in this       */
/* contribution and any revisions thereof, in publications of 3GPP2. */
/* The contributor may hold one or more patents or applications for  */
/* letters patent that cover the information contained in this       */
/* contribution.  Noting contained in this copyright statement shall */
/* be construed as conferring by implication or estoppel, or         */
/* otherwise a license or any other right under any existing  or     */
/* later issuing patent, whether or not the use of information       */
/* herein necessarily employs an invention of any such patent or     */
/* later issuing patent.                                             */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Copyright QUALCOMM Incorporated. All rights reserved.             */
/* The information contained in this contribution is provided for    */
/* the sole purpose of promoting discussion within the 3GPP2 and its */ 
/* Organization Partners and is not binding on the contributor.      */
/* The contributor reserves the right to add to, amend, or withdraw  */
/* the statements contained herein.                                  */
/* QUALCOMM Incorporated grants a free, irrevocable license to 3GPP2 */
/* and its Organization Partners to incorporate text or other        */
/* copyrightable material contained in the  contribution and any     */
/* modifications thereof in the creation of 3GPP2 publications;      */
/* to copyright and sell in Organizational Partner's name any        */
/* Organizational Partner's standards publication even though it may */
/* include portions of the contribution; and at the Organization     */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such contributions or the resulting Organizational     */
/* Partner's standards publication.  The contributor may hold one or */
/* more patents or copyrights that cover information contained in    */
/* this contribution. A license will be made available to applicants */
/* under reasonable terms and conditions that are demonstrably free  */
/* of any unfair discrimination.                                     */
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel, or otherwise any license or right under    */
/* any patent, whether or not the use of information herein          */
/* necessarily employs an invention of any existing or later issued  */
/* patent, or copyright. The contributor reserves the right to use   */
/* all material submitted in this contribution for his own purposes, */
/* including republication and distribution to others.               */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* The proposals in this submission have been formulated by Nokia,   */
/* Inc. (Nokia), to assist the 3GPP2 Standards Committee. This       */
/* document is offered  to the committee as a basis for discussion   */
/* and is not binding on Nokia. The requirements are subject to      */
/* change in form and in numerical values after more study. Nokia    */
/* specifically reserves the right to add to, or amend, the          */
/* quantitative statements made herein. Nothing contained herein     */
/* shall be construed as conferring by implication, estoppel, or     */
/* otherwise any license or right under any patent, whether or not   */
/* the use of information herein necessarily employs an invention of */
/* any existing or later issued patent.                              */
/* Copyright 2001 Nokia, Inc. All Rights Reserved. Nokia hereby      */
/* gives permission for copying this submission for the legitimate   */
/* purposes of the 3GPP2 Standards Committee, provided Nokia, Inc.   */
/* is credited on all copies. Distribution or reproduction of this   */
/* document, by any means, electronic, mechanical, or otherwise, in  */
/* its entirety, or any portion thereof, for monetary gain or any    */
/*  non-3GPP2 purposes is expressly prohibited.                      */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Nortel Networks grants a free, irrevocable license to 3GPP2 and   */
/* its Organizational Partners to incorporate text or other          */
/* copyrightable material contained in the contribution and any      */
/* modifications thereof in the creation of 3GPP2 publications; to   */
/* copyright and sell in Organizational Partner's name any           */
/* Organizational Partner's standards publication even though it may */
/* include portions of the contribution; and at the Organizational   */
/* Partner's sole discretion to permit others to reproduce in whole  */
/* or in part such contributions or the resulting Organizational     */
/* Partner's standards publication. The contributor must also be     */
/* willing to grant licenses under such contributor copyrights to    */
/* third parties on reasonable, non-discriminatory terms and         */
/* conditions, as appropriate.                                       */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Notice:                                                           */
/* This contribution has been prepared by the contributor to assist  */
/* 3GPP2 Technical Specifications Group C. This document is offered  */
/* to 3GPP2 Technical Specifications Group C as a basis for          */
/* discussion and should not be construed as a binding proposal on   */
/* Lucent Technologies Inc. or any other company.  Specifically,     */
/* Lucent Technologies Inc. reserves the right to modify, amend, or  */
/* withdraw the statements contained herein.                         */
/*                                                                   */
/* Permission is granted to 3GPP2 Technical Specifications Group C   */
/* participants to copy any portion of this document for legitimate  */
/* purposes of 3GPP2 Technical Specifications Group C. Copying for   */
/* monetary gain or other non-3GPP2 Technical Specifications Group C */
/* purposes is prohibited.                                           */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/* Grant of license Motorola Inc. grants a free, irrevocable license */
/* to 3GPP2 and its organizational partners to incorporate Motorola- */
/* supplied text or other copyrightable material contained in the    */
/* contribution and any modifications thereof in the creation of     */ 
/* 3GPP2 publications, to copyright and sell in organizational       */
/* partners name any organizational partners standards publications  */
/* even though it may include portions of the contribution; and at   */
/* the organizational partners sole discretion to permit others      */
/* to reproduce in whole or in part such contributions or the        */
/* resulting organizational partners standards publication. Motorola */
/* is also willing to grant licenses under such Motorola copyrights  */
/* to third parties on reasonable, non-discriminatory terms and      */
/* conditions, as appropriate.                                       */
/*                                                                   */
/* Notice:                                                           */
/* This document has been prepared by Motorola Inc. to assist the    */
/* 3GPP2 standards committee. This document is offered to the        */
/* committee as a basis for discussion and should not be considered  */
/* as a binding proposal on Motorola Inc.  or any other company.     */
/* Specifically, Motorola Inc. reserves the right to modify, amend,  */
/* or withdraw the statement contained herein. Permission is granted */
/* to 3GPP2 and its organizational partners to copy any portion of   */
/* this document for the legitimate purposes of the 3GPP2.  Copying  */
/* this document for monetary gain or other non-3GPP2 purpose is     */
/* prohibited.  Motorola Inc. may hold one or more patents of        */
/* copyrights that cover information contained in this contribution, */
/* and agrees that a license under those rights will be made         */
/* available on reasonable and non-discriminatory terms and          */
/* conditions, subject to receiving a reciprocal license in return.  */
/* Nothing contained herein shall be construed as conferring by      */
/* implication, estoppel, or otherwise any license or right under    */
/* any patent, whether or not the use of information herein          */
/* necessarily employs an invention of any existing or later issued  */
/* patent, or copyright.                                             */
/*                                                                   */
/* Notice                                                            */
/* Permission is granted to 3GPP2 participants to copy any portion of*/
/* this contribution for the legitimate purpose of the 3GPP2.        */
/* Copying this contribution for monetary gain or other non-3GPP2    */
/* purpose is prohibited.                                            */
/*                                                                   */
/*-------------------------------------------------------------------*/


#include <stdio.h>
#include "basic_op.h"
#include "math_ext32.h"
#include "math_adv.h"


/*****************************************************************************
 *                                                                           *
 *   Function Name : L_divide                                                *
 *                                                                           *
 *   Purpose :                                                               *
 *             Fractionnal integer division of two 32 bit numbers.           *
 *             L_num / L_denom.                                              *
 *             L_num and L_denom must be positive and L_num < L_denom.       *
 *                                                                           *
 *   Inputs :                                                                *
 *                                                                           *
 *    L_num                                                                  *
 *             32 bit long signed integer whose value falls in the           *
 *             range : 0x0000 0000 < L_num < L_denom                         *
 *                                                                           *
 *    L_denom                                                                *
 *             32 bit positive normalized integer whose value falls in the   *
 *             range : 0x40000000 < L_denom < 0x7fffffff                     *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *    L_div                                                                  *
 *             32 bit long signed integer whose value falls in the           *
 *             range : 0x0000 0000 <= L_div <= 0x7fff ffff.                  *
 *                                                                           *
 *  Algorithm:                                                               *
 *                                                                           *
 *  - find = 1/L_denom.                                                      *
 *      First approximation: approx = 1 / extract_h(L_denom)                 *
 *      1/L_denom = approx * (2.0 - L_denom * approx )                       *
 *                                                                           *
 *  - result = L_num * (1/L_denom)                                           *
 *****************************************************************************
*/

Word32 L_divide(Word32 L_num, Word32 L_denom)
{
#if (FUNC_L_DIVIDE_OPT)
	Word16 approx;
	Word32 L_div;
	if (L_num < 0 || L_denom < 0 || L_num > L_denom)
	{
		return (0);
	}
	/* First approximation: 1 / L_denom = 1/extract_h(L_denom) */

	approx = divide_s((Word16) 0x3fff, (short)(L_denom >> 16));

	/* 1/L_denom = approx * (2.0 - L_denom * approx) */

	L_div = L_mpy_ls(L_denom, approx);

	L_div = (Word32)0x7fffffffL - L_div;

	L_div = L_mpy_ls(L_div, approx);

	/* L_num * (1/L_denom) */

	L_div = L_mpy_ll(L_num, L_div);
	L_div <<= 2;

	return (L_div);

#else

	Word16 approx;
	Word32 L_div;

	if (L_num < 0 || L_denom < 0 || L_num > L_denom)
	{
		EVRC_PRINTF("ERROR: Invalid input into L_divide!\n");
		return (0);
	}

	/* First approximation: 1 / L_denom = 1/extract_h(L_denom) */

	approx = divide_s((Word16) 0x3fff, extract_h(L_denom));

	/* 1/L_denom = approx * (2.0 - L_denom * approx) */

	L_div = L_mpy_ls(L_denom, approx);

	L_div = L_sub((Word32) 0x7fffffffL, L_div);

	L_div = L_mpy_ls(L_div, approx);

	/* L_num * (1/L_denom) */

	L_div = L_mpy_ll(L_num, L_div);
	L_div = L_shl(L_div, 2);

	return (L_div);
#endif
}

/***************************************************************************
 *
 *   FUNCTION NAME: sqroot
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform a single precision square
 *     root function on a Word32
 *
 *   INPUTS:
 *
 *     L_SqrtIn
 *                     input to square root function
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swSqrtOut
 *                     output to square root function
 *
 *   DESCRIPTION:
 *
 *      Input assumed to be normalized
 *
 *      The algorithm is based around a six term Taylor expansion :
 *
 *        y^0.5 = (1+x)^0.5
 *             ~= 1 + (x/2) - 0.5*((x/2)^2) + 0.5*((x/2)^3)
 *                - 0.625*((x/2)^4) + 0.875*((x/2)^5)
 *
 *      Max error less than 0.08 % for normalized input ( 0.5 <= x < 1 )
 *
 *************************************************************************/

Word16 sqroot(Word32 L_SqrtIn)
{
#if (FUNC_SQROOT_OPT)
/*_________________________________________________________________________
 |                                                                         |
 |                              Local Constants                            |
 |_________________________________________________________________________|
*/

#define    PLUS_HALF          0x40000000L	/* 0.5 */
#define    MINUS_ONE          0x80000000L	/* -1 */
#define    TERM5_MULTIPLER    0x5000	/* 0.625 */
#define    TERM6_MULTIPLER    0x7000	/* 0.875 */

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/

	Word32 L_Temp0, L_Temp1;

	Word16 swTemp, swTemp2, swTemp3, swTemp4, swSqrtOut;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* determine 2nd term x/2 = (y-1)/2 */
	/* -------------------------------- */

	L_Temp1 = L_SqrtIn >> 1;	/* L_Temp1 = y/2 */
	L_Temp1 -= (long)PLUS_HALF;	/* L_Temp1 = (y-1)/2 */
	swTemp = (short)(L_Temp1 >> 16);	/* swTemp = x/2 */

	/* add contribution of 2nd term */
	/* ---------------------------- */

	L_Temp1 -= (long)MINUS_ONE;	/* L_Temp1 = 1 + x/2 */

	/* determine 3rd term */
	/* ------------------ */

	L_Temp0 = -((long)swTemp * swTemp);	/* L_Temp0 = -(x/2)^2 */
	swTemp2 = (short)(L_Temp0 >> 15);	/* swTemp2 = -(x/2)^2 */

	/* add contribution of 3rd term */
	/* ---------------------------- */

	L_Temp0 += L_Temp1;	/* L_Temp0 = 1 + x/2 - 0.5*(x/2)^2 */

	/* determine 4rd term */
	/* ------------------ */

	L_Temp1 = -((long)swTemp * swTemp2);	/* L_Temp1 = (x/2)^3 */
	swTemp3 = (short)(L_Temp1 >> 15);	/* swTemp3 = (x/2)^3 */
	//L_Temp1 >>= 1;	/* L_Temp1 = 0.5*(x/2)^3 */

	/* add contribution of 4rd term */
	/* ---------------------------- */

	/* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */

	L_Temp1 += L_Temp0;

	/* determine partial 5th term */
	/* -------------------------- */

	L_Temp0 = (long)swTemp * swTemp3;	/* L_Temp0 = (x/2)^4 */
	swTemp4 = (short)((L_Temp0 + 0x4000) >> 15);	/* swTemp4 = (x/2)^4 */

	/* determine partial 6th term */
	/* -------------------------- */

	L_Temp0 = -((long)swTemp2 * swTemp3);	/* L_Temp0 = (x/2)^5 */
	swTemp2 = (short)((L_Temp0 + 0x4000) >> 15);	/* swTemp2 = (x/2)^5 */

	/* determine 5th term and add its contribution */
	/* ------------------------------------------- */

	/* L_Temp0 = -0.625*(x/2)^4 */

	L_Temp0 = -(((long)TERM5_MULTIPLER * swTemp4) << 1);

	/* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 - 0.625*(x/2)^4 */

	L_Temp1 += L_Temp0;

	/* determine 6th term and add its contribution */
	/* ------------------------------------------- */

	/* swSqrtOut = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */
	/* - 0.625*(x/2)^4 + 0.875*(x/2)^5     */


	L_Temp0 = L_Temp1 + ((long)TERM6_MULTIPLER * swTemp2 << 1);
    if (L_Temp0 < 0x7fff8000)
    {
        swSqrtOut = (short)((L_Temp0 + 0x8000) >> 16);
    }
    else
    {
        swSqrtOut = MAX_16;
    }
    
	/* return output */
	/* ------------- */

	return (swSqrtOut);

#else

/*_________________________________________________________________________
 |                                                                         |
 |                              Local Constants                            |
 |_________________________________________________________________________|
*/

#define    PLUS_HALF          0x40000000L	/* 0.5 */
#define    MINUS_ONE          0x80000000L	/* -1 */
#define    TERM5_MULTIPLER    0x5000	/* 0.625 */
#define    TERM6_MULTIPLER    0x7000	/* 0.875 */

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/

	Word32 L_Temp0, L_Temp1;

	Word16 swTemp, swTemp2, swTemp3, swTemp4, swSqrtOut;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* determine 2nd term x/2 = (y-1)/2 */
	/* -------------------------------- */

	L_Temp1 = L_shr(L_SqrtIn, 1);	/* L_Temp1 = y/2 */
	L_Temp1 = L_sub(L_Temp1, PLUS_HALF);	/* L_Temp1 = (y-1)/2 */
	swTemp = extract_h(L_Temp1);	/* swTemp = x/2 */

	/* add contribution of 2nd term */
	/* ---------------------------- */

	L_Temp1 = L_sub(L_Temp1, MINUS_ONE);	/* L_Temp1 = 1 + x/2 */

	/* determine 3rd term */
	/* ------------------ */

	L_Temp0 = L_msu(0L, swTemp, swTemp);	/* L_Temp0 = -(x/2)^2 */
	swTemp2 = extract_h(L_Temp0);	/* swTemp2 = -(x/2)^2 */
	L_Temp0 = L_shr(L_Temp0, 1);	/* L_Temp0 = -0.5*(x/2)^2 */

	/* add contribution of 3rd term */
	/* ---------------------------- */

	L_Temp0 = L_add(L_Temp1, L_Temp0);	/* L_Temp0 = 1 + x/2 - 0.5*(x/2)^2 */

	/* determine 4rd term */
	/* ------------------ */

	L_Temp1 = L_msu(0L, swTemp, swTemp2);	/* L_Temp1 = (x/2)^3 */
	swTemp3 = extract_h(L_Temp1);	/* swTemp3 = (x/2)^3 */
	L_Temp1 = L_shr(L_Temp1, 1);	/* L_Temp1 = 0.5*(x/2)^3 */

	/* add contribution of 4rd term */
	/* ---------------------------- */

	/* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */

	L_Temp1 = L_add(L_Temp0, L_Temp1);

	/* determine partial 5th term */
	/* -------------------------- */

	L_Temp0 = L_mult(swTemp, swTemp3);	/* L_Temp0 = (x/2)^4 */
	swTemp4 = round32(L_Temp0);	/* swTemp4 = (x/2)^4 */

	/* determine partial 6th term */
	/* -------------------------- */

	L_Temp0 = L_msu(0L, swTemp2, swTemp3);	/* L_Temp0 = (x/2)^5 */
	swTemp2 = round32(L_Temp0);	/* swTemp2 = (x/2)^5 */

	/* determine 5th term and add its contribution */
	/* ------------------------------------------- */

	/* L_Temp0 = -0.625*(x/2)^4 */

	L_Temp0 = L_msu(0L, TERM5_MULTIPLER, swTemp4);

	/* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 - 0.625*(x/2)^4 */

	L_Temp1 = L_add(L_Temp0, L_Temp1);

	/* determine 6th term and add its contribution */
	/* ------------------------------------------- */

	/* swSqrtOut = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */
	/* - 0.625*(x/2)^4 + 0.875*(x/2)^5     */

	swSqrtOut = mac_r(L_Temp1, TERM6_MULTIPLER, swTemp2);

	/* return output */
	/* ------------- */

	return (swSqrtOut);
#endif
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnLog2
 *
 *   PURPOSE:
 *     The purpose of this function is to take the log base 2 of input and
 *     divide by 32 and return; i.e. output = log2(input)/32
 *
 *   INPUTS:
 *
 *     L_Input
 *                     input
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     Word32
 *                     output
 *
 *   DESCRIPTION:
 *
 *     log2(x) = 4.0 * (-.3372223*x*x + .9981958*x -.6626105)
 *                           c0            c1          c2   (includes sign)
 *
 *************************************************************************/

Word32 fnLog2(Word32 L_Input)
{
#if (FUNC_FNLOG2_OPT)
	static Word16
	    swC0 = -0x2b2a, swC1 = 0x7fc5, swC2 = -0x54d0;

	Word32 siShiftCnt;
	Word16 swInSqrd, swIn;
	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* normalize input and store shifts required */
	/* ----------------------------------------- */

	siShiftCnt = norm_l(L_Input);
	LwIn = L_Input << siShiftCnt;
	siShiftCnt++;
	siShiftCnt = -siShiftCnt;

	/* calculate x*x*c0 */
	/* ---------------- */

	swIn = (short)(LwIn >> 16);
	swInSqrd = (short)(((long)swIn * swIn + 0x4000) >> 15);
	LwIn = ((long)swInSqrd * swC0) << 1;

	/* add x*c1 */
	/* --------- */

	LwIn += ((long)swIn * swC1) << 1;

	/* add c2 */
	/* ------ */

	LwIn += (long)swC2 << 16;

	/* apply *(4/32) */
	/* ------------- */

	LwIn >>= 3;
	LwIn = LwIn & 0x03ffffff;
	siShiftCnt <<= 10;
	LwIn += (long)siShiftCnt << 16;

	/* return log2 */
	/* ----------- */

	return (LwIn);

#else

	static Word16
	    swC0 = -0x2b2a, swC1 = 0x7fc5, swC2 = -0x54d0;

	Word16 siShiftCnt, swInSqrd, swIn;
	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* normalize input and store shifts required */
	/* ----------------------------------------- */

	siShiftCnt = norm_l(L_Input);
	LwIn = L_shl(L_Input, siShiftCnt);
	siShiftCnt = add(siShiftCnt, 1);
	siShiftCnt = negate(siShiftCnt);

	/* calculate x*x*c0 */
	/* ---------------- */

	swIn = extract_h(LwIn);
	swInSqrd = mult_r(swIn, swIn);
	LwIn = L_mult(swInSqrd, swC0);

	/* add x*c1 */
	/* --------- */

	LwIn = L_mac(LwIn, swIn, swC1);

	/* add c2 */
	/* ------ */

	LwIn = L_add(LwIn, L_deposit_h(swC2));

	/* apply *(4/32) */
	/* ------------- */

	LwIn = L_shr(LwIn, 3);
	LwIn = LwIn & 0x03ffffff;
	siShiftCnt = shl(siShiftCnt, 10);
	LwIn = L_add(LwIn, L_deposit_h(siShiftCnt));

	/* return log2 */
	/* ----------- */

	return (LwIn);
#endif
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnLog
 *
 *   PURPOSE:
 *     The purpose of this function is to take the natural log of input and
 *     divide by 32 and return; i.e. output = log(input)/32
 *
 *   INPUTS:
 *
 *     L_Input
 *                     input
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     Word32
 *                     output
 *
 *   DESCRIPTION:
 *
 *     log(x) = log(2) * log2(x)
 *            = 0.693147 * log2(x)
 *
 *     log2(x) = 4.0 * (-.3372223*x*x + .9981958*x -.6626105)
 *                           c0            c1          c2   (includes sign)
 *
 *************************************************************************/

Word32 fnLog(Word32 L_Input)
{

	static Word16
	    Scale = 22713;			/* 0.693147 = log(2) */
	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* 0.693147*log2(x) */
	/* ---------------- */

	LwIn = fnLog2(L_Input);
	LwIn = L_mpy_ls(LwIn, Scale);

	return (LwIn);
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnLog10
 *
 *   PURPOSE:
 *     The purpose of this function is to take the log base 10 of input and
 *     divide by 32 and return; i.e. output = log10(input)/32
 *
 *   INPUTS:
 *
 *     L_Input
 *                     input
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     Word32
 *                     output
 *
 *   DESCRIPTION:
 *
 *     log10(x) = log10(2) * log2(x)
 *              = 0.30103  * log2(x)
 *
 *     log2(x) = 4.0 * (-.3372223*x*x + .9981958*x -.6626105)
 *                           c0            c1          c2   (includes sign)
 *
 *************************************************************************/

Word32 fnLog10(Word32 L_Input)
{

	static Word16
	    Scale = 9864;			/* 0.30103 = log10(2) */
	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* 0.30103*log2(x) */
	/* ------------------- */

	LwIn = fnLog2(L_Input);
	LwIn = L_mpy_ls(LwIn, Scale);

	return (LwIn);
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnExp2
 *
 *   PURPOSE:
 *     The purpose of this function is to implement a base two exponential
 *     2**(32*x) by polynomial approximation
 *
 *
 *   INPUTS:
 *
 *     L_Input
 *                     unnormalized input exponent (input range constrained
 *                     to be < 0).
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     LwIn
 *                     exponential output
 *
 *   DESCRIPTION:
 *
 *     polynomial approximation is used for the generation of the exponential
 *
 *     2**(32*X) = 0.1713425*X*X + 0.6674432*X + 0.9979554
 *                     c2              c1            c0
 *
 *************************************************************************/

Word32 fnExp2(Word32 L_Input)
{
#if (FUNC_FNEXP2_OPT)
/*_________________________________________________________________________
 |                                                                         |
 |                           Local Static Variables                        |
 |_________________________________________________________________________|
*/
	static Word16 pswPCoefE[3] =
	{					/* c2,   c1,    c0 */
		0x15ef, 0x556f, 0x7fbd
	};

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/

	Word16 swTemp1, swTemp2, swTemp3, swTemp4;
	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* initialize */
	/* ---------- */

	swTemp3 = 0x0020;

	/* determine normlization shift count */
	/* ---------------------------------- */

	swTemp1 = (short)(L_Input >> 16);
	LwIn = ((long)swTemp1 * swTemp3) << 1;
	swTemp2 = (short)(LwIn >> 16);

	/* determine un-normalized shift count */
	/* ----------------------------------- */

	swTemp3 = -0x0001;
	swTemp4 = swTemp3 - swTemp2;

	/* normalize input */
	/* --------------- */

	LwIn = LwIn & 0xffff;
	LwIn += (long)swTemp3 << 16;

	LwIn >>= 1;
	swTemp1 = (short)LwIn;

	/* calculate x*x*c2 */
	/* ---------------- */

	swTemp2 = (short)((((long)swTemp1 * swTemp1) + 0x4000) >> 15);
	LwIn = ((long)swTemp2 * pswPCoefE[0]) << 1;

	/* calculate x*x*c2 + x*c1 */
	/* ----------------------- */

	LwIn += ((long)swTemp1 * pswPCoefE[1]) << 1;

	/* calculate x*x*c2 + x*c1 + c0 */
	/* --------------------------- */

	LwIn += (long)pswPCoefE[2] << 16;

	/* un-normalize exponent if its requires it */
	/* ---------------------------------------- */

	if (swTemp4 > 0)
	{
		LwIn >>= swTemp4;
	}

	/* return result */
	/* ------------- */

	return (LwIn);

#else

/*_________________________________________________________________________
 |                                                                         |
 |                           Local Static Variables                        |
 |_________________________________________________________________________|
*/
	static Word16 pswPCoefE[3] =
	{					/* c2,   c1,    c0 */
		0x15ef, 0x556f, 0x7fbd
	};

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/

	Word16 swTemp1, swTemp2, swTemp3, swTemp4;
	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	/* initialize */
	/* ---------- */

	swTemp3 = 0x0020;

	/* determine normlization shift count */
	/* ---------------------------------- */

	swTemp1 = extract_h(L_Input);
	LwIn = L_mult(swTemp1, swTemp3);
	swTemp2 = extract_h(LwIn);

	/* determine un-normalized shift count */
	/* ----------------------------------- */

	swTemp3 = -0x0001;
	swTemp4 = sub(swTemp3, swTemp2);

	/* normalize input */
	/* --------------- */

	LwIn = LwIn & 0xffff;
	LwIn = L_add(LwIn, L_deposit_h(swTemp3));

	LwIn = L_shr(LwIn, 1);
	swTemp1 = extract_l(LwIn);

	/* calculate x*x*c2 */
	/* ---------------- */

	swTemp2 = mult_r(swTemp1, swTemp1);
	LwIn = L_mult(swTemp2, pswPCoefE[0]);

	/* calculate x*x*c2 + x*c1 */
	/* ----------------------- */

	LwIn = L_mac(LwIn, swTemp1, pswPCoefE[1]);

	/* calculate x*x*c2 + x*c1 + c0 */
	/* --------------------------- */

	LwIn = L_add(LwIn, L_deposit_h(pswPCoefE[2]));

	/* un-normalize exponent if its requires it */
	/* ---------------------------------------- */

	if (swTemp4 > 0)
	{
		LwIn = L_shr(LwIn, swTemp4);
	}

	/* return result */
	/* ------------- */

	return (LwIn);
#endif
}

/***************************************************************************
 *
 *   FUNCTION NAME: fnExp10
 *
 *   PURPOSE:
 *     The purpose of this function is to implement a base ten exponential
 *     10**(32*x) by polynomial approximation
 *
 *
 *   INPUTS:
 *
 *     L_Input
 *                     unnormalized input exponent (input range constrained
 *                     to be < 0)
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     LwIn
 *                     exponential output
 *
 *   DESCRIPTION:
 *
 *     polynomial approximation is used for the generation of the exponential
 *
 *     10**(32*X) = 2**((32*X) / log10(2))
 *                = 2**((32*X) / 0.30103)
 *
 *     2**(32*X) = 0.1713425*X*X + 0.6674432*X + 0.9979554
 *                     c2              c1            c0
 *
 *************************************************************************/

Word32 fnExp10(Word32 L_Input)
{
#if (FUNC_FNEXP10_OPT)
/*_________________________________________________________________________
 |                                                                         |
 |                           Local Static Variables                        |
 |_________________________________________________________________________|
*/
	static Word16
	    InvScale = 27213;		/* (1/log10(2))/4 */

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/

	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	LwIn = L_mpy_ls(L_Input, InvScale);
	LwIn = L_shl2(LwIn, 2);
	LwIn = fnExp2(LwIn);

	/* return result */
	/* ------------- */

	return (LwIn);

#else

/*_________________________________________________________________________
 |                                                                         |
 |                           Local Static Variables                        |
 |_________________________________________________________________________|
*/
	static Word16
	    InvScale = 27213;		/* (1/log10(2))/4 */

/*_________________________________________________________________________
 |                                                                         |
 |                            Automatic Variables                          |
 |_________________________________________________________________________|
*/

	Word32 LwIn;

/*_________________________________________________________________________
 |                                                                         |
 |                              Executable Code                            |
 |_________________________________________________________________________|
*/

	LwIn = L_mpy_ls(L_Input, InvScale);
	LwIn <<= 2;
	LwIn = fnExp2(LwIn);

	/* return result */
	/* ------------- */

	return (LwIn);
#endif
}


