/**********************************************************************
Each of the companies; Qualcomm, and Lucent (hereinafter 
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
all Normative portions of the QCELP-13 Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the QCELP-13 be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the QCELP-13 or the field of use of practice of the 
QCELP-13 Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/* filter.c - all initialization, filtering, and shutdown       */
/*    filtering routines.                                       */

#include <stdio.h>
#include <stdlib.h>
//#include <malloc.h>
#include "basic_op.h"
#include "mem_align.h"
#include "celp.h"
short gArray0[200];

#ifdef ASM_OPT
void do_ploe_filter_response1_asm( 
							   short                *output,
							   short                numsamples,
							   struct POLE_FILTER   *filter,
	                           short                update_flag
							   );
void do_ploe_filter_response_asm( 
						   short                *output,
						   short                numsamples,
						   struct POLE_FILTER   *filter,
						   short                update_flag);
#endif

int initialize_pole_filter(
			struct  POLE_FILTER     *filter,
			short                   order,
			VO_MEM_OPERATOR     *pMemOP)
{
	short    i;
	filter->order = order;

	filter->memory = mem_malloc(pMemOP, order*2*sizeof(short), 32);
	if(NULL == filter->memory)
	{
		return -1;
	}

	filter->pole_coeff = mem_malloc(pMemOP, order*sizeof(short), 32);
	if(NULL == filter->pole_coeff)
	{
		return -1;
	}

	for( i=0 ; i < order*2; i++ )
	{
		filter->memory[i] = 0;
	}
	return 0;
}

int initialize_pole_1_tap_filter(
			struct POLE_FILTER_1_TAP  *filter,
	        short                     max_order,
			VO_MEM_OPERATOR     *pMemOP)
{
	short    i;
	filter->max_order = max_order;
	filter->memory = mem_malloc(pMemOP, max_order * sizeof(short), 32);

	if(NULL == filter->memory)
	{
		return -1;
	}
	for( i=0 ; i < max_order ; i++ )
	{
		filter->memory[i] = 0;
	}
	return 0;
}

int initialize_zero_filter(
			struct ZERO_FILTER  *filter,
			short               order,
			VO_MEM_OPERATOR     *pMemOP)
{
	int     i;
	filter->order=order;
	filter->memory = (short *)mem_malloc(pMemOP, (order * sizeof(short)), 32);
	if(NULL == filter->memory)
	{
		return -1;
	}

	filter->zero_coeff = (short *)mem_malloc(pMemOP, (order * sizeof(short)), 32);
	if(NULL == filter->zero_coeff)
	{
		return -1;
	}

	for( i=0 ; i < order ; i++ )
	{
		filter->memory[i] = 0;
	}
	return 0;	
}

void free_pole_filter(struct  POLE_FILTER  *filter, VO_MEM_OPERATOR  *pMemOP)
{
	mem_free(pMemOP, (char*)filter->memory);
	mem_free(pMemOP, (char*)filter->pole_coeff);
}

void free_zero_filter(struct ZERO_FILTER  *filter, VO_MEM_OPERATOR  *pMemOP)
{
	mem_free(pMemOP, (char*)filter->memory);
	mem_free(pMemOP, (char*)filter->zero_coeff);
}
void do_pole_filter_high( 
						 short                *input,
						 short                *output,
						 short                numsamples,
						 struct POLE_FILTER   *filter,
	                     short                update_flag)
{
	int total, total_fra, total_s;
	short i;
	short para1, para2;
	short tmp1, tmp2;
	short tmp1_fra, tmp2_fra;

	tmp2 = filter->memory[0];
	tmp1 = filter->memory[1];
	tmp2_fra = filter->memory[2];
	tmp1_fra = filter->memory[3];
	para1 = filter->pole_coeff[0];
	para2 = filter->pole_coeff[1];

	/* filter the buffer                      */
	for (i=0; i<numsamples; i++)
	{
		total = tmp1 * para2;
		total_fra = tmp1_fra * para2;
		total += tmp2 * para1;
		total_fra +=  tmp2_fra * para1;

		total_fra = L_add(total_fra ,8192) >> 14;

		total = L_add(total, total_fra);

		total_s = (L_add(total , 8192) >> 14);

		total_fra = L_sub(total,total_s << 14);
		output[i] =  (short)L_add(input[i] ,total_s); 

		tmp1 = tmp2;
		tmp1_fra = tmp2_fra;
		tmp2 = output[i];
		tmp2_fra = total_fra;
	}

	/* if flag is set, update the memories    */
	if (update_flag==UPDATE) 
	{
		filter->memory[0] = tmp2;
		filter->memory[1] = tmp1;
		filter->memory[2] = tmp2_fra;
		filter->memory[3] = tmp1_fra;
	}	
}

void do_pole_filter( 
					short                *input,
					short                *output,
					short                numsamples,
                    struct POLE_FILTER   *filter,
	                short                update_flag)
{
	short *tmpbuf, *pcoeff;
	int total, total_s;
	int i;
	tmpbuf = filter->memory;
	pcoeff = filter->pole_coeff;
	/* initialize first "order" locations to  */
	/* previous memories                      */
	for (i=0; i<numsamples; i++)
	{
		total = pcoeff[9] * tmpbuf[9];
		tmpbuf[9] = tmpbuf[8];
		total = L_add(total, pcoeff[8] * tmpbuf[8]);
		tmpbuf[8] = tmpbuf[7];
		total = L_add(total, pcoeff[7] * tmpbuf[7]);
		tmpbuf[7] = tmpbuf[6];
		total = L_add(total, pcoeff[6] * tmpbuf[6]);
		tmpbuf[6] = tmpbuf[5];
		total = L_add(total, pcoeff[5] * tmpbuf[5]);
		tmpbuf[5] = tmpbuf[4];
		total = L_add(total, pcoeff[4] * tmpbuf[4]);
		tmpbuf[4] = tmpbuf[3];
		total = L_add(total, pcoeff[3] * tmpbuf[3]);
		tmpbuf[3] = tmpbuf[2];
		total = L_add(total, pcoeff[2] * tmpbuf[2]);
		tmpbuf[2] = tmpbuf[1];
		total = L_add(total, pcoeff[1] * tmpbuf[1]);
		tmpbuf[1] = tmpbuf[0];
		total = L_add(total, pcoeff[0] * tmpbuf[0]);
		total_s = ((total + 2048) >> 12);
		output[i] = saturate(input[i] + total_s);
		tmpbuf[0] = output[i];
	}
}

void do_ploe_filter_response( 
							 short                *output,
							 short                numsamples,
							 struct POLE_FILTER   *filter,
	                         short                update_flag)
{
	short *tmpbuf, *pcoeff;
	int total, total_s;
	int i;

	tmpbuf = filter->memory;
	pcoeff = filter->pole_coeff;
	/* initialize first "order" locations to  */
	/* previous memories                      */
	for (i=0; i<numsamples; i++)
	{
		total = pcoeff[9] * tmpbuf[9];
		tmpbuf[9] = tmpbuf[8];
		total = L_add(total, pcoeff[8] * tmpbuf[8]);
		tmpbuf[8] = tmpbuf[7];
		total = L_add(total, pcoeff[7] * tmpbuf[7]);
		tmpbuf[7] = tmpbuf[6];
		total = L_add(total, pcoeff[6] * tmpbuf[6]);
		tmpbuf[6] = tmpbuf[5];
		total = L_add(total, pcoeff[5] * tmpbuf[5]);
		tmpbuf[5] = tmpbuf[4];
		total = L_add(total, pcoeff[4] * tmpbuf[4]);
		tmpbuf[4] = tmpbuf[3];
		total = L_add(total, pcoeff[3] * tmpbuf[3]);
		tmpbuf[3] = tmpbuf[2];
		total = L_add(total, pcoeff[2] * tmpbuf[2]);
		tmpbuf[2] = tmpbuf[1];
		total = L_add(total, pcoeff[1] * tmpbuf[1]);
		tmpbuf[1] = tmpbuf[0];
		total = L_add(total, pcoeff[0] * tmpbuf[0]);
		total_s = ((total + 2048) >> 12);       
		output[i] = saturate(total_s);

		tmpbuf[0] = output[i];
	}
}

void do_ploe_filter_response_1( 
							   short                *output,
							   short                numsamples,
							   struct POLE_FILTER   *filter,
							   short                update_flag)
{
	short *tmpbuf, *pcoeff;
	int total, total_s;
	int i;

	tmpbuf = filter->memory;
	pcoeff = filter->pole_coeff;
	/* initialize first "order" locations to  */
	/* previous memories                      */
	total = pcoeff[9] * tmpbuf[9];
	tmpbuf[9] = tmpbuf[8];
	total = L_add(total, pcoeff[8] * tmpbuf[8]);
	tmpbuf[8] = tmpbuf[7];
	total = L_add(total, pcoeff[7] * tmpbuf[7]);
	tmpbuf[7] = tmpbuf[6];
	total = L_add(total, pcoeff[6] * tmpbuf[6]);
	tmpbuf[6] = tmpbuf[5];
	total = L_add(total, pcoeff[5] * tmpbuf[5]);
	tmpbuf[5] = tmpbuf[4];
	total = L_add(total, pcoeff[4] * tmpbuf[4]);
	tmpbuf[4] = tmpbuf[3];
	total = L_add(total, pcoeff[3] * tmpbuf[3]);
	tmpbuf[3] = tmpbuf[2];
	total = L_add(total, pcoeff[2] * tmpbuf[2]);
	tmpbuf[2] = tmpbuf[1];
	total = L_add(total, pcoeff[1] * tmpbuf[1]);
	tmpbuf[1] = tmpbuf[0];
	total = L_add(total, pcoeff[0] * tmpbuf[0]);
	total_s = ((total + 2048) >> 12);       
	output[0] = saturate(16384 + total_s);

	tmpbuf[0] = output[0];

	for (i=1; i<numsamples; i++)
	{
		total = pcoeff[9] * tmpbuf[9];
		tmpbuf[9] = tmpbuf[8];
		total = L_add(total, pcoeff[8] * tmpbuf[8]);
		tmpbuf[8] = tmpbuf[7];
		total = L_add(total, pcoeff[7] * tmpbuf[7]);
		tmpbuf[7] = tmpbuf[6];
		total = L_add(total, pcoeff[6] * tmpbuf[6]);
		tmpbuf[6] = tmpbuf[5];
		total = L_add(total, pcoeff[5] * tmpbuf[5]);
		tmpbuf[5] = tmpbuf[4];
		total = L_add(total, pcoeff[4] * tmpbuf[4]);
		tmpbuf[4] = tmpbuf[3];
		total = L_add(total, pcoeff[3] * tmpbuf[3]);
		tmpbuf[3] = tmpbuf[2];
		total = L_add(total, pcoeff[2] * tmpbuf[2]);
		tmpbuf[2] = tmpbuf[1];
		total = L_add(total, pcoeff[1] * tmpbuf[1]);
		tmpbuf[1] = tmpbuf[0];
		total = L_add(total, pcoeff[0] * tmpbuf[0]);
		total_s = ((total + 2048) >> 12);       
		output[i] = saturate(total_s);

		tmpbuf[0] = output[i];
	}
}

void debug_do_pole_filter( 
						  short                *input,
						  short                *output,
						  short                numsamples,
						  struct POLE_FILTER   *filter,
	                      short                update_flag)
{
	//int    total;
	short  tmpbuf[160+10];
	short   i,j;

	/* initialize first "order" locations to  */
	/* previous memories                      */
	for (i=0; i<filter->order; i++) {
		tmpbuf[i]=filter->memory[filter->order-i-1];
	}
	/* initialize next locations to the input */
	for (i=0; i<numsamples; i++) {
		tmpbuf[i+filter->order]=input[i];
	}

	/* filter the buffer                      */
	for (i=0; i<numsamples; i++)
	{
		for (j=i; j<i+(filter->order); j++)
		{
			tmpbuf[i+(filter->order)] +=
				((int)tmpbuf[j]*(filter->pole_coeff[(filter->order)+i-j-1]) + 2048) >> 12;
		}
	}

	/* copy the filtered samples to the output*/
	for (i=0; i<numsamples; i++) {
		output[i]=tmpbuf[i+filter->order];
	}
	/* if flag is set, update the memories    */
	if (update_flag==UPDATE) {
		for (i=0; i<filter->order; i++) {
			filter->memory[i]=tmpbuf[filter->order+numsamples-i-1];
		}
	}		
}

void do_pole_filter_1_tap_interp(
								 short                     *input,
								 short                     *output,
								 short                     numsamples,
								 struct POLE_FILTER_1_TAP  *filter,
								 short                     update_flag)
{
	short  *tmpbuf = gArray0;
	short  *indata, *outdata;	
	int    i, coeff;	
	//tmpbuf=(short *)(calloc((unsigned)numsamples+filter->max_order, sizeof(short)));
	/* initialize first "order" locations to  */
	/* previous memories                      */
	indata = filter->memory + filter->max_order - 1;
	outdata = tmpbuf;
	for (i=filter->max_order; i; i-=10) {
		*outdata++ = *indata--; *outdata++ = *indata--;
		*outdata++ = *indata--; *outdata++ = *indata--;
		*outdata++ = *indata--; *outdata++ = *indata--;
		*outdata++ = *indata--; *outdata++ = *indata--;
		*outdata++ = *indata--; *outdata++ = *indata--;
	}
	/* initialize next locations to the input */
	outdata = tmpbuf + filter->max_order;
	indata = input;
	for (i=numsamples; i; i-= 10) {
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
	}

	/* apply pitch filter to the buffer                      */	
	if (filter->frac==0) 
	{
		coeff = filter->coeff;
		outdata = tmpbuf + filter->max_order;
		indata = tmpbuf + filter->max_order-filter->delay;
		for (i=numsamples; i; i -= 10) 
		{
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
             outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
			*outdata = *outdata + ((coeff*(*indata++) + 2048) >> 12);
			outdata++;
		}
	}

	/* copy the filtered samples to the output*/
	indata = tmpbuf + filter->max_order;
	outdata = output;
	for (i=numsamples; i; i-= 10) {
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
	}
	/* if flag is set, update the memories    */
	if (update_flag==UPDATE) {
		indata = tmpbuf + filter->max_order + numsamples - 1;
		outdata = filter->memory;
		for (i=filter->max_order; i; i-=10) {
			*outdata++ = *indata--; *outdata++ = *indata--;
			*outdata++ = *indata--; *outdata++ = *indata--;
			*outdata++ = *indata--; *outdata++ = *indata--;
			*outdata++ = *indata--; *outdata++ = *indata--;
			*outdata++ = *indata--; *outdata++ = *indata--;
		}
	}
	//	free((char*)tmpbuf);
}/* end of void do_pole_filter_1_tap_interp() */

#if 1 //divide into two function
void do_zero_filter_front(
						  short               *input,
						  short               *output,
						  short               numsamples,
						  struct ZERO_FILTER  *filter,
						  short               update_flag)
{
	int total;
	short *tmpbuf;
	int i, temp1, temp2;
	temp1 = filter->zero_coeff[0];
	temp2 = filter->zero_coeff[1];
	tmpbuf = filter->memory;

	for (i = 0; i < numsamples; i++)
	{
		total = L_shl2((int)input[i], 12);
		total = L_add(total, temp2 * (int)tmpbuf[1]);
		tmpbuf[1] = tmpbuf[0];
		total = L_add(total, temp1 * (int)tmpbuf[0]);
		tmpbuf[0] = input[i];
		total = L_add(total, 2048) >> 12;
		output[i] = saturate(total);
	}
}

void do_zero_filter(
					short               *input,
					short               *output,
					short               numsamples,
					struct ZERO_FILTER  *filter,
					short               update_flag)
{
	int    total;
	short  *coeffs, *tmpbuf;
	int    i;
	tmpbuf = filter->memory;
	coeffs = filter->zero_coeff;

	for (i = 0; i < numsamples; i++)
	{
		total = L_shl2((int)input[i], 12);
		total = L_add(total, (int)coeffs[9] * (int)tmpbuf[9]);
		tmpbuf[9] = tmpbuf[8];
		total = L_add(total, (int)coeffs[8] * (int)tmpbuf[8]);
		tmpbuf[8] = tmpbuf[7];
		total = L_add(total, (int)coeffs[7] * (int)tmpbuf[7]);
		tmpbuf[7] = tmpbuf[6];
		total = L_add(total, (int)coeffs[6] * (int)tmpbuf[6]);
		tmpbuf[6] = tmpbuf[5];
		total = L_add(total, (int)coeffs[5] * (int)tmpbuf[5]);
		tmpbuf[5] = tmpbuf[4];
		total = L_add(total, (int)coeffs[4] * (int)tmpbuf[4]);
		tmpbuf[4] = tmpbuf[3];
		total = L_add(total, (int)coeffs[3] * (int)tmpbuf[3]);
		tmpbuf[3] = tmpbuf[2];
		total = L_add(total, (int)coeffs[2] * (int)tmpbuf[2]);
		tmpbuf[2] = tmpbuf[1];
		total = L_add(total, (int)coeffs[1] * (int)tmpbuf[1]);
		tmpbuf[1] = tmpbuf[0];		
		total = L_add(total, (int)coeffs[0] * (int)tmpbuf[0]);
		tmpbuf[0] = input[i];
		total = L_add(total, 2048) >> 12;
		output[i] = saturate(total);
	}
}
#else
void do_zero_filter(
					short               *input,
					short               *output,
					short               numsamples,
					struct ZERO_FILTER  *filter,
					short               update_flag)
{
	int total;
	short *coeffs, *tmpbuf;
	int i, j;
	tmpbuf = filter->memory;
	coeffs = filter->zero_coeff;

	for (i = 0; i < numsamples; i++)
	{
		total = L_shl((int)input[i], 12);
		for (j = filter->order-1; j; j--)
		{
			total = L_add(total, (int)coeffs[j] * (int)tmpbuf[j]);
			tmpbuf[j] = tmpbuf[j - 1];
		}
		total = L_add(total, (int)coeffs[0] * (int)tmpbuf[0]);
		tmpbuf[0] = input[i];
		total = L_add(total, 2048) >> 12;
		output[i] = saturate(total);
	}
}
#endif

void do_fir_linear_filter( 
						  short               *input,
						  short               *output,
						  short               numsamples,
						  struct ZERO_FILTER  *filter,
						  short               update_flag)
{
	int   total;
	short *coeffs, *tmpbuf;
	int i, j;

	tmpbuf = filter->memory;
	coeffs = filter->zero_coeff;

    for (i = 0; i < numsamples; i++)
    {
        total = 0;
        for (j = filter->order - 1 ; j; j--)
        {
            total += (int)tmpbuf[j]*(int)coeffs[j+1];
            tmpbuf[j] = tmpbuf[j - 1];
        }
        total += (int)tmpbuf[j]*(int)coeffs[j+1];
        total += (int)input[i]*(int)coeffs[0];
        tmpbuf[0] = input[i];
        total += 8192;
        output[i] = (short)(total >> 14);
	}

}

void get_impulse_response_pole(
							   short               *response,
							   short               length,
							   struct POLE_FILTER  *filter)
{
	int    i;
	short  *tmpmemory = gArray0;
	short  *indata, *outdata;

	indata = filter->memory;
	outdata = tmpmemory;
	for (i=filter->order*2; i; i-= 5) {
		*outdata++ = *indata; *indata++ = 0;
		*outdata++ = *indata; *indata++ = 0;
		*outdata++ = *indata; *indata++ = 0;
		*outdata++ = *indata; *indata++ = 0;
		*outdata++ = *indata; *indata++ = 0;
	}
#ifdef ASM_OPT
	do_ploe_filter_response1_asm(response, length, filter, NO_UPDATE);
#else
	do_ploe_filter_response_1(response, length, filter, NO_UPDATE);
#endif
	indata = tmpmemory;
	outdata = filter->memory;
	for (i=filter->order*2; i; i-= 10) {
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
		*outdata++ = *indata++; *outdata++ = *indata++;
	}
}

void get_zero_input_response_pole(
								  short               *response,
								  short               length,
								  struct POLE_FILTER  *filter)
{
	int    i;
	short  *tmpmemory = gArray0;
	short  *indata, *outdata;

	indata = filter->memory;
	outdata = tmpmemory;
	for (i=filter->order*2; i; i-=2) {
		*outdata++ = *indata++; *outdata++ = *indata++;
	}
#ifdef ASM_OPT  // asm opt
	do_ploe_filter_response_asm(response, length, filter, NO_UPDATE);
#else
	do_ploe_filter_response(response, length, filter, NO_UPDATE);
#endif

	indata = tmpmemory;
	outdata = filter->memory;
	for (i=filter->order*2; i; i-=2 ) {
		*outdata++ = *indata++; *outdata++ = *indata++;
	}
}

void get_zero_input_response_pole_1_tap_interp(
				short                     *response,
				short                     length,
				struct POLE_FILTER_1_TAP  *filter)
{
	int    i;
	short  *input;
	short  *tmpmemory;
	short  *indata, *outdata;

	input=(short *)(calloc((unsigned)length, sizeof(short)));
	tmpmemory=(short *)(calloc((unsigned)filter->max_order, sizeof(short)));

	indata = filter->memory;
	outdata = tmpmemory;
	//memcpy(outdata, indata, filter->max_order*sizeof(short));
	for (i=filter->max_order; i; i--) {
		*outdata++ = *indata++;
	}

#ifdef ASM_OPT 
	filter_1_tap_interp_asm(input, response, length, filter, NO_UPDATE);
#else
	do_pole_filter_1_tap_interp(input, response, length, filter, NO_UPDATE);
#endif

	indata = tmpmemory;
	outdata = filter->memory;
	//memcpy(outdata, indata, filter->max_order*sizeof(short));
	for (i=filter->max_order; i; i--) {
	   *outdata++ = *indata++;
	}

	free((char*)input);
	free((char*)tmpmemory);
}
