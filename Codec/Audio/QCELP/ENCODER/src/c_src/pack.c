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
/* pack.c - pack various parameters into the packet */
#include "celp.h"
#include "pack.h"
#define ANSI_733_ALL_ZEROS 1

static short NUMBITS[NUMMODES]={0, 20, 54, 124, 266};

static short BIT_DATA3[54][3] =
{ {LSPVs, 2, 2}, {LSPVs, 2, 1}, {LSPVs, 2, 0},
{LSPVs, 1, 6}, {LSPVs, 1, 5}, {LSPVs, 1, 4}, 
{LSPVs, 1, 3}, {LSPVs, 1, 2}, {LSPVs, 1, 1},
{LSPVs, 1, 0}, {LSPVs, 0, 5}, {LSPVs, 0, 4}, {LSPVs, 0, 3}, 
{LSPVs, 0, 2}, {LSPVs, 0, 1}, {LSPVs, 0, 0},
{LSPVs, 4, 5}, {LSPVs, 4, 4}, {LSPVs, 4, 3}, {LSPVs, 4, 2}, 
{LSPVs, 4, 1}, {LSPVs, 4, 0}, {LSPVs, 3, 5},
{LSPVs, 3, 4}, {LSPVs, 3, 3}, {LSPVs, 3, 2}, 
{LSPVs, 3, 1}, {LSPVs, 3, 0}, {LSPVs, 2, 6},
{LSPVs, 2, 5}, {LSPVs, 2, 4}, {LSPVs, 2, 3}, 

{CBGAIN, 3, 3}, {CBGAIN, 3, 2}, {CBGAIN, 3, 1}, {CBGAIN, 3, 0}, 

{CBGAIN, 2, 3}, {CBGAIN, 2, 2}, {CBGAIN, 2, 1}, {CBGAIN, 2, 0}, 

{CBGAIN, 1, 3}, {CBGAIN, 1, 2}, {CBGAIN, 1, 1}, {CBGAIN, 1, 0}, 

{CBGAIN, 0, 3}, {CBGAIN, 0, 2}, {CBGAIN, 0, 1}, {CBGAIN, 0, 0}, 
{RESERVE, 0, 0}, {RESERVE, 0, 0},
{CBGAIN, 4, 3}, {CBGAIN, 4, 2}, {CBGAIN, 4, 1}, {CBGAIN, 4, 0} 

};

static short BIT_DATA2[266][3] =
{ 

	{LSPVs, 2, 2}, {LSPVs, 2, 1}, {LSPVs, 2, 0},

	{LSPVs, 1, 6}, {LSPVs, 1, 5}, {LSPVs, 1, 4}, 
	{LSPVs, 1, 3}, {LSPVs, 1, 2}, {LSPVs, 1, 1}, {LSPVs, 1, 0},

	{LSPVs, 0, 5}, {LSPVs, 0, 4}, {LSPVs, 0, 3}, {LSPVs, 0, 2}, 
	{LSPVs, 0, 1}, {LSPVs, 0, 0}, 

	{LSPVs, 4, 5},  {LSPVs, 4, 4},{LSPVs, 4, 3}, {LSPVs, 4, 2}, 
	{LSPVs, 4, 1},  {LSPVs, 4, 0}, 

	{LSPVs, 3, 5}, {LSPVs, 3, 4}, {LSPVs, 3, 3}, {LSPVs, 3, 2}, 
	{LSPVs, 3, 1}, {LSPVs, 3, 0}, 

	{LSPVs, 2, 6}, {LSPVs, 2, 5}, {LSPVs, 2, 4}, {LSPVs, 2, 3}, 

	/* Bit 232 */
	{CBSIGN, 0, 0},
	{CBGAIN, 0, 3},{CBGAIN, 0, 2},{CBGAIN, 0, 1},{CBGAIN, 0, 0},

	{PFRAC, 0, 0}, {PLAG, 0, 6}, {PLAG, 0, 5}, {PLAG, 0, 4}, 
	{PLAG, 0, 3}, {PLAG, 0, 2}, {PLAG, 0, 1}, {PLAG, 0, 0}, 


	{PGAIN, 0, 2}, {PGAIN, 0, 1}, {PGAIN, 0, 0}, 

	{CBINDEX, 1, 3}, {CBINDEX, 1, 2}, {CBINDEX, 1, 1}, {CBINDEX, 1, 0}, 

	{CBSIGN, 1, 0}, 
	{CBGAIN, 1, 3},{CBGAIN, 1, 2},{CBGAIN, 1, 1},{CBGAIN, 1, 0},

	{CBINDEX, 0, 6}, {CBINDEX, 0, 5}, {CBINDEX, 0, 4}, {CBINDEX, 0, 3}, 
	{CBINDEX, 0, 2}, {CBINDEX, 0, 1}, {CBINDEX, 0, 0}, 

	/* Bit 200 */
	{CBGAIN, 3, 0},

	{CBINDEX, 2, 6}, {CBINDEX, 2, 5}, {CBINDEX, 2, 4}, {CBINDEX, 2, 3}, 
	{CBINDEX, 2, 2}, {CBINDEX, 2, 1}, {CBINDEX, 2, 0}, 

	{CBSIGN, 2, 0},
	{CBGAIN, 2, 3},{CBGAIN, 2, 2},{CBGAIN, 2, 1},{CBGAIN, 2, 0},

	{CBINDEX, 1, 6}, {CBINDEX, 1, 5}, {CBINDEX, 1, 4}, 

	{PLAG, 1, 2}, {PLAG, 1, 1}, {PLAG, 1, 0}, 

	{PGAIN, 1, 2}, {PGAIN, 1, 1}, {PGAIN, 1, 0}, 

	{CBINDEX, 3, 6}, {CBINDEX, 3, 5}, {CBINDEX, 3, 4}, {CBINDEX, 3, 3}, 
	{CBINDEX, 3, 2}, {CBINDEX, 3, 1}, {CBINDEX, 3, 0}, 

	{CBSIGN, 3, 0}, {CBGAIN, 3, 2}, {CBGAIN, 3, 1}, 

	{CBINDEX, 4, 5}, {CBINDEX, 4, 4}, {CBINDEX, 4, 3}, 
	{CBINDEX, 4, 2}, {CBINDEX, 4, 1}, {CBINDEX, 4, 0}, 

	{CBSIGN, 4, 0}, 
	{CBGAIN, 4, 3},{CBGAIN, 4, 2},{CBGAIN, 4, 1},{CBGAIN, 4, 0},

	{PFRAC, 1, 0}, 
	{PLAG, 1, 6}, {PLAG, 1, 5}, {PLAG, 1, 4}, {PLAG, 1, 3}, 

	{CBGAIN, 6, 2},{CBGAIN, 6, 1},{CBGAIN, 6, 0},

	{CBINDEX, 5, 6}, {CBINDEX, 5, 5}, {CBINDEX, 5, 4}, {CBINDEX, 5, 3}, 
	{CBINDEX, 5, 2}, {CBINDEX, 5, 1}, {CBINDEX, 5, 0}, 

	{CBSIGN, 5, 0}, 
	{CBGAIN, 5, 3},{CBGAIN, 5, 2},{CBGAIN, 5, 1},{CBGAIN, 5, 0},

	{CBINDEX, 4, 6}, 

	/* Bit 136 */
	{CBINDEX, 7, 2}, {CBINDEX, 7, 1}, {CBINDEX, 7, 0},

	{CBSIGN, 7, 0}, 
	{CBGAIN, 7, 2},{CBGAIN, 7, 1},{CBGAIN, 7, 0},

	{CBINDEX, 6, 6}, {CBINDEX, 6, 5}, {CBINDEX, 6, 4}, {CBINDEX, 6, 3}, 
	{CBINDEX, 6, 2}, {CBINDEX, 6, 1}, {CBINDEX, 6, 0}, 

	{CBSIGN, 6, 0}, 
	{CBGAIN, 6, 3},

	{CBGAIN, 8, 0},

	{PFRAC, 2, 0}, {PLAG, 2, 6}, {PLAG, 2, 5}, {PLAG, 2, 4}, 
	{PLAG, 2, 3},  {PLAG, 2, 2}, {PLAG, 2, 1}, {PLAG, 2, 0}, 

	{PGAIN, 2, 2}, {PGAIN, 2, 1}, {PGAIN, 2, 0}, 

	{CBINDEX, 7, 6}, {CBINDEX, 7, 5}, {CBINDEX, 7, 4}, {CBINDEX, 7, 3}, 

	{CBSIGN, 9, 0}, 
	{CBGAIN, 9, 3},{CBGAIN, 9, 2},{CBGAIN, 9, 1},{CBGAIN, 9, 0},

	{CBINDEX, 8, 6}, {CBINDEX, 8, 5}, {CBINDEX, 8, 4}, {CBINDEX, 8, 3}, 
	{CBINDEX, 8, 2}, {CBINDEX, 8, 1}, {CBINDEX, 8, 0}, 

	{CBSIGN, 8, 0}, 
	{CBGAIN, 8, 3},{CBGAIN, 8, 2},{CBGAIN, 8, 1},

	{CBINDEX, 10, 3}, {CBINDEX, 10, 2}, {CBINDEX, 10, 1}, {CBINDEX, 10, 0}, 

	{CBSIGN, 10, 0}, 
	{CBGAIN, 10, 3},{CBGAIN, 10, 2},{CBGAIN, 10, 1},{CBGAIN, 10, 0},

	{CBINDEX, 9, 6}, {CBINDEX, 9, 5}, {CBINDEX, 9, 4}, {CBINDEX, 9, 3}, 
	{CBINDEX, 9, 2}, {CBINDEX, 9, 1}, {CBINDEX, 9, 0}, 

	/* Bit 72 */
	{PGAIN, 3, 1}, {PGAIN, 3, 0}, 

	{CBINDEX, 11, 6}, {CBINDEX, 11, 5}, {CBINDEX, 11, 4}, {CBINDEX, 11, 3}, 
	{CBINDEX, 11, 2}, {CBINDEX, 11, 1}, {CBINDEX, 11, 0}, 

	{CBSIGN, 11, 0}, 
	{CBGAIN, 11, 2},{CBGAIN, 11, 1},{CBGAIN, 11, 0},

	{CBINDEX, 10, 6}, {CBINDEX, 10, 5}, {CBINDEX, 10, 4}, 

	{CBINDEX, 12, 1}, {CBINDEX, 12, 0},

	{CBSIGN, 12, 0}, 
	{CBGAIN, 12, 3},{CBGAIN, 12, 2},{CBGAIN, 12, 1},{CBGAIN, 12, 0},

	{PFRAC, 3, 0}, {PLAG, 3, 6}, {PLAG, 3, 5}, {PLAG, 3, 4}, 
	{PLAG, 3, 3}, {PLAG, 3, 2}, {PLAG, 3, 1}, {PLAG, 3, 0}, 

	{PGAIN, 3, 2}, 

	{CBINDEX, 13, 5}, {CBINDEX, 13, 4}, {CBINDEX, 13, 3}, {CBINDEX, 13, 2}, 
	{CBINDEX, 13, 1}, {CBINDEX, 13, 0}, 

	{CBSIGN, 13, 0}, 
	{CBGAIN, 13, 3},{CBGAIN, 13, 2},{CBGAIN, 13, 1},{CBGAIN, 13, 0},

	{CBINDEX, 12, 6}, {CBINDEX, 12, 5}, {CBINDEX, 12, 4}, {CBINDEX, 12, 3}, 
	{CBINDEX, 12, 2},  

	{CBGAIN, 15, 2},{CBGAIN, 15, 1},{CBGAIN, 15, 0},

	{CBINDEX, 14, 6}, {CBINDEX, 14, 5}, {CBINDEX, 14, 4}, {CBINDEX, 14, 3}, 
	{CBINDEX, 14, 2}, {CBINDEX, 14, 1}, {CBINDEX, 14, 0}, 


	{CBSIGN, 14, 0}, 
	{CBGAIN, 14, 3},{CBGAIN, 14, 2},{CBGAIN, 14, 1},{CBGAIN, 14, 0},

	{CBINDEX, 13, 6}, 

	{RESERVE, 0, 0},   {RESERVE, 0, 0}, 

	{CBINDEX, 15, 6}, {CBINDEX, 15, 5}, {CBINDEX, 15, 4}, {CBINDEX, 15, 3}, 
	{CBINDEX, 15, 2}, {CBINDEX, 15, 1}, {CBINDEX, 15, 0}, 

	{CBSIGN, 15, 0}, 

};

static short BIT_DATA1[124][3] =                     /* Rate 1/2 */
{ {LSPVs, 2, 2}, {LSPVs, 2, 1}, {LSPVs, 2, 0},

{LSPVs, 1, 6}, {LSPVs, 1, 5}, {LSPVs, 1, 4}, 
{LSPVs, 1, 3}, {LSPVs, 1, 2}, {LSPVs, 1, 1}, {LSPVs, 1, 0},

{LSPVs, 0, 5}, {LSPVs, 0, 4}, {LSPVs, 0, 3}, {LSPVs, 0, 2}, 
{LSPVs, 0, 1}, {LSPVs, 0, 0}, 

{LSPVs, 4, 5},  {LSPVs, 4, 4},{LSPVs, 4, 3}, {LSPVs, 4, 2}, 
{LSPVs, 4, 1},  {LSPVs, 4, 0}, 

{LSPVs, 3, 5}, {LSPVs, 3, 4}, {LSPVs, 3, 3}, {LSPVs, 3, 2}, 
{LSPVs, 3, 1}, {LSPVs, 3, 0}, 

{LSPVs, 2, 6}, {LSPVs, 2, 5}, {LSPVs, 2, 4}, {LSPVs, 2, 3}, 

{CBSIGN, 0, 0},{CBGAIN, 0, 3},{CBGAIN, 0, 2},{CBGAIN, 0, 1},{CBGAIN, 0, 0},

{PFRAC, 0, 0}, {PLAG, 0, 6}, {PLAG, 0, 5}, {PLAG, 0, 4}, 
{PLAG, 0, 3}, {PLAG, 0, 2}, {PLAG, 0, 1}, {PLAG, 0, 0}, 

{PGAIN, 0, 2}, {PGAIN, 0, 1}, {PGAIN, 0, 0}, 

{PLAG, 1, 5}, {PLAG, 1, 4}, {PLAG, 1, 3}, 
{PLAG, 1, 2}, {PLAG, 1, 1}, {PLAG, 1, 0}, 

{PGAIN, 1, 2}, {PGAIN, 1, 1}, {PGAIN, 1, 0},

{CBINDEX, 0, 6}, {CBINDEX, 0, 5}, {CBINDEX, 0, 4}, {CBINDEX, 0, 3}, 
{CBINDEX, 0, 2}, {CBINDEX, 0, 1}, {CBINDEX, 0, 0}, 

{PGAIN, 2, 1}, {PGAIN, 2, 0}, 

{CBINDEX, 1, 6}, {CBINDEX, 1, 5}, {CBINDEX, 1, 4}, {CBINDEX, 1, 3}, 
{CBINDEX, 1, 2}, {CBINDEX, 1, 1}, {CBINDEX, 1, 0}, 

{CBSIGN, 1, 0},{CBGAIN, 1, 3},{CBGAIN, 1, 2},{CBGAIN, 1, 1},{CBGAIN, 1, 0},

{PFRAC, 1, 0}, {PLAG, 1, 6}, 

{CBINDEX, 2, 1}, {CBINDEX, 2, 0}, 

{CBSIGN, 2, 0}, {CBGAIN, 2, 3},{CBGAIN, 2, 2},{CBGAIN, 2, 1},{CBGAIN, 2, 0},


{PFRAC, 2, 0}, {PLAG, 2, 6}, {PLAG, 2, 5}, {PLAG, 2, 4}, 
{PLAG, 2, 3}, {PLAG, 2, 2}, {PLAG, 2, 1}, {PLAG, 2, 0}, 

{PGAIN, 2, 2}, 

{PFRAC, 3, 0}, {PLAG, 3, 6}, {PLAG, 3, 5}, {PLAG, 3, 4}, 
{PLAG, 3, 3}, {PLAG, 3, 2}, {PLAG, 3, 1}, {PLAG, 3, 0}, 

{PGAIN, 3, 2}, {PGAIN, 3, 1}, {PGAIN, 3, 0}, 

{CBINDEX, 2, 6}, {CBINDEX, 2, 5}, {CBINDEX, 2, 4}, {CBINDEX, 2, 3}, 
{CBINDEX, 2, 2}, 

{CBINDEX, 3, 6}, {CBINDEX, 3, 5}, {CBINDEX, 3, 4}, {CBINDEX, 3, 3}, 
{CBINDEX, 3, 2}, {CBINDEX, 3, 1}, {CBINDEX, 3, 0}, 

{CBSIGN, 3, 0}, {CBGAIN, 3, 3},{CBGAIN, 3, 2},{CBGAIN, 3, 1},{CBGAIN, 3, 0},
};

static short BIT_DATA0[20][3] =
/* CBSEED bits actually correspond to the bits chosen from SD */ 
{ {CBSEED, 0, 15}, {LSPs, 0, 0}, {LSPs, 1, 0}, {LSPs, 2, 0}, 
{CBSEED, 0, 11}, {LSPs, 3, 0}, {LSPs, 4, 0}, {LSPs, 5, 0}, 
{CBSEED, 0, 7}, {LSPs, 6, 0}, {LSPs, 7, 0}, {LSPs, 8, 0}, 
{CBSEED, 0, 3}, {LSPs, 9, 0}, {CBGAIN, 0, 1}, {CBGAIN, 0, 0},
{RESERVE, 0, 0},{RESERVE, 0, 0},{RESERVE, 0, 0},{RESERVE, 0, 0}
};

int getbit(
struct PACKET *packet,
	short          type,
	short          number,
	short          loc)
{
	switch(type) {
case LSPs:
	return( truefalse(packet->lpc[number], loc) );
case PGAIN:
	return( truefalse(packet->b[number], loc) );
case PLAG:
	return( truefalse(packet->lag[number], loc) );
case PFRAC:
	return( truefalse(packet->frac[number], loc) );
case LSPVs:
	return( truefalse(packet->lpc[number], loc) );
case CBGAIN:
	return( truefalse(packet->G[number][0], loc) );
case CBSIGN:
	return( truefalse(packet->Gsign[number][0], loc) );
case CBINDEX:
	return( truefalse(packet->i[number][0], loc) );
case CBSEED:
	return( truefalse(packet->sd_enc, loc) );
case RESERVE:
	return(0);
	}
	return(0);
}/* end of getbit() */

void pack_frame(
				short         mode,
struct PACKET *packet)
{
	int    i, j, cnt;
	short  *data_ptr = NULL, *indata;
	int    bit[(WORDS_PER_PACKET-1)*16];

	switch(mode) {
case 4:			/* full rate */
	data_ptr= &BIT_DATA2[0][0];
	break;
case 3:			/* half rate */
	data_ptr= &BIT_DATA1[0][0];
	break;
case 2:			/* quarter rate */
	data_ptr= &BIT_DATA3[0][0];
	break;
case 1:
	data_ptr= &BIT_DATA0[0][0];
	break;
	}
	if(mode == EIGHTH || mode == QUARTERRATE_UNVOICED){
		for(i = 0; i < MAXSF; i++)
			for(j = 0; j < MAXNUMCB; j++)
				packet->i[i][j] = 0;
	}	
	indata = (short *)(bit + NUMBITS[mode]);
	for (i=(WORDS_PER_PACKET-1)*16 - NUMBITS[mode]; i; i--) {
		*indata++ = 0;
	}
	for (i=0; i<NUMBITS[mode]; i++) {
		bit[i]=getbit(packet, data_ptr[i*3], data_ptr[i*3+1],
			data_ptr[i*3+2]);
	}
	packet->data[0]=(char)mode;
	cnt=0;
	for (i=1; i<qcp_byte_num_per_frame[mode]; i++) {
		packet->data[i]=0;
		if(mode != BLANK){
			for (j=8; j; j--) {
				packet->data[i] = (packet->data[i]<<1)|(bit[cnt]);
				cnt += 1;
			}
		}		
	}
	if(mode == EIGHTH )
		packet->sd_enc = ((unsigned char)packet->data[1] << 8) | (unsigned char)packet->data[2];

}/* end of pack_frame() */


void clear_packet_params(struct PACKET *packet)
{
	int i, j;
	for (i=0; i<LPCORDER; i++) {
		packet->lsp[i]=0;
		packet->lpc[i]=0;
	}
	for (i=0; i<MAXSF; i++) {
		packet->b[i]=0;
		packet->lag[i]=0;
		packet->frac[i]=0;
		for (j=0; j<MAXNUMCB; j++) {
			packet->G[i][j]=0;
			packet->i[i][j]=0;
			packet->Gsign[i][j]=0;
		}
	}
}/* end of clear_packet_params() */

short truefalse(short word,short bitloc)
{
	if ((word&(1<<bitloc)) ==0) {
		return(0);
	}
	else {
		return(1);
	}
}

void pack_lpc(
			  short            mode,
struct LPCPARAMS *lpc_params,
struct PACKET    *packet)
{
	int i;
	for (i=0; i<LPCORDER; i++) {
		packet->lsp[i]=lpc_params->lsp[i];
	}
	if(mode == EIGHTH){
		for (i=0; i<LPCORDER; i++) {
			packet->lpc[i]=lpc_params->qcode_lsp[i];
		}
	}
	else{
		for (i=0; i<5; i++) {
			packet->lpc[i]=lpc_params->qcode_lsp[i];
			packet->lpc[i+5] = 0;
		}
	}
}

void pack_pitch(
struct PITCHPARAMS *pitch_params,
struct PACKET      *packet,
	short              sf)
{
	packet->b  [sf] = pitch_params->qcode_b;
	packet->lag[sf] = pitch_params->qcode_lag;
	packet->frac[sf]= pitch_params->qcode_frac;
}

void pack_cb(
			 short           mode,
struct CBPARAMS cb_params[MAXCBPERPIT][MAXNUMCB],
struct PACKET   *packet,
	short           psf, 
	short           cbsf)
{
	int j;
	if(mode != QUARTERRATE_UNVOICED && mode != EIGHTH){
		for (j=0; j<NUMCB[mode]; j++) {
			packet->G[psf*CBSF_DIV_PITCHSF[mode]+cbsf][j]=
				cb_params[cbsf][j].qcode_G;
			packet->Gsign[psf*CBSF_DIV_PITCHSF[mode]+cbsf][j]=
				cb_params[cbsf][j].qcode_Gsign;

			if(cb_params[cbsf][j].qcode_Gsign == POSITIVE)
				packet->i[psf*CBSF_DIV_PITCHSF[mode]+cbsf][j]=
				cb_params[cbsf][j].qcode_i;
			else
				packet->i[psf*CBSF_DIV_PITCHSF[mode]+cbsf][j]=
				(cb_params[cbsf][j].qcode_i+89)&(CBLENGTH - 1);
		}
	}
	else{
		for (j=0; j<NUMCB[mode]; j++) {
			packet->G[psf+cbsf][j] = cb_params[cbsf][j].qcode_G;
		}
	}
}

