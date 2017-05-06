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
/*  EVRC Decoder -- main module                                         */
/*======================================================================*/
/*         ..Includes.                                                  */
/*----------------------------------------------------------------------*/
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#include "basic_op.h"
#include  "decode.h"
#include  "d_globs.h"
#include  "fer.h"
#include  "globs.h"
#include  "macro.h"
#include  "proto.h"
#include  "rom.h"

//#include "tty.h"


/*======================================================================*/
/*         ..Reset RCELP decode rate.                                   */
/*----------------------------------------------------------------------*/
void SetDecodeRate(ERVC_COMPONENT *evrc_com, short rate)
{

	/*....execute.... */
	evrc_com->bit_rate = rate;

	switch (rate)
	{
	case 1:
		evrc_com->FCBGainSize = 16;		/*...use half-rate... */
		evrc_com->gnvq = gnvq_4;
		break;
	case 3:
		evrc_com->nsub = nsub22;
		evrc_com->nsize = nsize22;
		evrc_com->lognsize = lognsize22;
		evrc_com->lsptab = lsptab22;
		evrc_com->knum = 3;
		evrc_com->FCBGainSize = 16;
		evrc_com->gnvq = gnvq_4;
		break;
	case 4:
		evrc_com->nsub = nsub28;
		evrc_com->nsize = nsize28;
		evrc_com->lognsize = lognsize28;
		evrc_com->lsptab = lsptab28;
		evrc_com->knum = 4;
		evrc_com->FCBGainSize = 32;
		evrc_com->gnvq = gnvq_8;
		break;
	}
}

/*======================================================================*/
/*         ..Reset RCELP decoder parameters.                            */
/*----------------------------------------------------------------------*/
void InitDecoder(EVRC_DEC_COMPONENT *evrc_dcom)
{
	/*....(local) variables.... */
	ERVC_COMPONENT	*pevrc_com;
	ERVC_DEC_OBJ	*pevrc_decobj;
	int j;

	pevrc_com    = evrc_dcom->evrc_com;
	pevrc_decobj = evrc_dcom->evrc_decobj;
	
	/*....execute.... */
	SetDecodeRate(pevrc_com, 4);
	pevrc_com->ran_g_iset = 0;
	pevrc_com->ran_g_gset = 0;
	pevrc_com->e_ran_g_iset = 0;
	pevrc_com->e_ran_g_gset = 0;

	pevrc_com->ext_Seed = 1234;
	pevrc_com->ext_PrevBest = 0;
	pevrc_decobj->last_valid_rate = 1;		/* reset last_valid_rate */
	pevrc_decobj->last_fer_flag = 0;			/* reset last_fer_flag */
	pevrc_decobj->decode_fcnt = 0;

	for (j = 0; j < ORDER; j++)
		pevrc_decobj->SynMemory[j] = 0;

	pevrc_decobj->OldlspD[0] = 1573;
	pevrc_decobj->OldlspD[1] = 3146;
	pevrc_decobj->OldlspD[2] = 4719;
	pevrc_decobj->OldlspD[3] = 6291;
	pevrc_decobj->OldlspD[4] = 7864;
	pevrc_decobj->OldlspD[5] = 9437;
	pevrc_decobj->OldlspD[6] = 11010;
	pevrc_decobj->OldlspD[7] = 12583;
	pevrc_decobj->OldlspD[8] = 14156;
	pevrc_decobj->OldlspD[9] = 15729;

	for (j = 0; j < ACBMemSize; j++)
		pevrc_decobj->PitchMemoryD[j] = pevrc_decobj->PitchMemoryD_back[j] = 0;
	pevrc_decobj->pdelayD = 40;
	pevrc_decobj->ave_acb_gain = pevrc_decobj->ave_fcb_gain = 0;
	pevrc_decobj->FadeScale = 32767;
	pevrc_decobj->fer_counter = 0;

	pevrc_decobj->erasureFlag = 0;
	pevrc_decobj->errorFlag = 0;

	for (j = 0; j < ORDER; j++)
		pevrc_decobj->apf_FIRmem[j] = 0;
	for (j = 0; j < ORDER; j++)
		pevrc_decobj->apf_IIRmem[j] = 0;
	for (j = 0; j < ACBMemSize; j++)
		pevrc_decobj->apf_Residual[j] = 0;
	pevrc_decobj->apf_last = 0;
}

/*======================================================================*/
/*         ..Decode bitstream data.                                     */
/*----------------------------------------------------------------------*/
void decode(
			EVRC_DEC_COMPONENT *evrc_dcom, 
			unsigned char* codeBuf,
			short *outFbuf)
{
	/*....(local) variables.... */
	ERVC_DEC_OBJ *pdeobj;
	short local_rate;
	short post_filter;
	
	pdeobj = evrc_dcom->evrc_decobj;
	post_filter = pdeobj->pf_flag;
	/*....execute.... */
	local_rate = *codeBuf;
	codeBuf++;
	
	FrameErrorHandler(pdeobj, &local_rate, codeBuf);  /* check frame for some errors */
	SetDecodeRate(evrc_dcom->evrc_com, local_rate);

	pdeobj->fer_flag = pdeobj->erasureFlag | pdeobj->errorFlag;

	if (!pdeobj->fer_flag){
		decode_no_fer(evrc_dcom, codeBuf, post_filter, outFbuf); /* no FER */
		pdeobj->fer_flag = pdeobj->erasureFlag | pdeobj->errorFlag;
		}

	if (pdeobj->fer_flag)
		decode_fer(evrc_dcom, post_filter, outFbuf); /* FER has occured */
}
