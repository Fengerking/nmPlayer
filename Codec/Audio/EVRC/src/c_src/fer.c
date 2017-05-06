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
 
/*======================================================================*/
/*     Enhanced Variable Rate Codec - Bit-Exact C Specification         */
/*     Copyright (C) 1997-1998 Telecommunications Industry Association. */
/*     All rights reserved.                                             */
/*----------------------------------------------------------------------*/
/* Note:  Reproduction and use of this software for the design and      */
/*     development of North American Wideband CDMA Digital              */
/*     Cellular Telephony Standards is authorized by the TIA.           */
/*     The TIA does not authorize the use of this software for any      */
/*     other purpose.                                                   */
/*                                                                      */
/*     The availability of this software does not provide any license   */
/*     by implication, estoppel, or otherwise under any patent rights   */
/*     of TIA member companies or others covering any use of the        */
/*     contents herein.                                                 */
/*                                                                      */
/*     Any copies of this software or derivative works must include     */
/*     this and all other proprietary notices.                          */
/*======================================================================*/
/*  Module:     fer.c                                                   */
/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include "d_globs.h"

/*======================================================================*/
/*         ..Defines.                                                   */
/*----------------------------------------------------------------------*/

/*======================================================================*/
/*         ..Set frame error flags.                                     */
/*----------------------------------------------------------------------*/
void FrameErrorHandler (
						ERVC_DEC_OBJ *decobj,
                        short *rate,
                        unsigned char *packet
						)
{
    /*...(local) variables...*/
#if ANSI_EVRC_ALL_ZEROS
    unsigned long  ULtmp;
    int            j;
    unsigned char *buf_temp = NULL;
    int counter_temp = 0;
#endif /* ANSI_EVRC_ALL_ZEROS */

    /*....execute.... */
#if ANSI_EVRC_ALL_ZEROS
    ULtmp = 0;
    switch (*rate)
    {
        case 4:
            counter_temp = 22;
            break;
        case 3:
            counter_temp = 10;
            break;
        case 1:
            counter_temp = 2;
            break;
        default:
            break;
    }
    buf_temp = packet;
    for (j = 0; j < counter_temp; j++)
    {
        ULtmp += (unsigned long)(*buf_temp);
        buf_temp++;
    }
    
    /* Force erasure if non-8th rate packet contents consists of all zeros. */
    if ((ULtmp == 0) && (*rate != 1))
    {
        *rate = 0xe;
    }
#endif /* ANSI_EVRC_ALL_ZEROS */

	switch (*rate)
	{
	case 1:
        /* Trap for all ones packet */
//        if ((unsigned short)packet[0]==(unsigned short)0xffff)
        if ((packet[0]==(unsigned char)0xff) && (packet[1]==(unsigned char)0xff))
        {
#if ANSI_EVRC_ALL_ONES
            if(++decobj->ones_dec_cnt>2)
            {
                decobj->ones_dec_cnt=3;
                decobj->erasureFlag = 0;
                decobj->errorFlag = 0;
            }
            else
            {
                decobj->erasureFlag = 1;
            }
#else
			decobj->erasureFlag = 1;
#endif
		}
		else
        {
            decobj->erasureFlag = 0; 
			decobj->errorFlag = 0;
#if ANSI_EVRC_ALL_ONES
            decobj->ones_dec_cnt = 0;
#endif
        }
        break;
    case 3:
	case 4:
#if ANSI_EVRC_ALL_ONES
        decobj->ones_dec_cnt = 0;
#endif
		decobj->erasureFlag = 0;
        decobj->errorFlag = 0;
		break;
	case 0xe:
#if ANSI_EVRC_ALL_ONES
        decobj->ones_dec_cnt = 0;
#endif
		decobj->erasureFlag = 1;
		break;
	default:
#if ANSI_EVRC_ALL_ONES
        decobj->ones_dec_cnt = 0;
#endif
		decobj->errorFlag = 1;
		break;
	}

	if ((*rate == 1 && decobj->last_valid_rate == 4) && (!decobj->last_fer_flag))
	{
		decobj->errorFlag = 1;
		decobj->erasureFlag = 1;
		*rate = 4;
	}

	if (decobj->erasureFlag || decobj->errorFlag)
	{
		switch (decobj->last_valid_rate)
		{
		case 1:
#if 0
			fprintf(stdout,
					"FrameErrorHandler():  Erasure in frame #%d - rx rate of (%d) using rate of (%d).\n",
					(int) decobj->decode_fcnt + 1,
					*rate,
					decobj->last_valid_rate
				);
#endif
			*rate = 1;
			break;
		case 3:
		case 4:
		default:
#if 0
			fprintf(stdout,
					"FrameErrorHandler():  Erasure in frame #%d - rx rate of (%d) using rate of (%d).\n",
					(int) decobj->decode_fcnt + 1,
					*rate,
					decobj->last_valid_rate
				);
#endif
			*rate = 4;
			break;
		}
	}
	else
	{
		decobj->last_valid_rate = *rate;
	}
}
