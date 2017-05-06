/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : sp_dec.c
*      Purpose          : Decoding and post filtering of one speech frame.
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "sp_dec.h"
const char sp_dec_id[] = "@(#)$Id $" sp_dec_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "set_zero.h"
#include "dec_amr.h"
#include "pstfilt.h"
#include "bits2prm.h"
#include "mode.h"
#include "post_pro.h"

#ifdef MMS_IO
#include "bitno.tab"
#endif

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*---------------------------------------------------------------*
*    Constants (defined in "cnst.h")                            *
*---------------------------------------------------------------*
* L_FRAME     :
* M           :
* PRM_SIZE    :
* AZ_SIZE     :
* SERIAL_SIZE :
*---------------------------------------------------------------*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/

/*************************************************************************
*
*  Function:   Speech_Decode_Frame_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int Speech_Decode_Frame_init (Speech_Decode_FrameState **state,VO_MEM_OPERATOR *pMemOP)
{
	Speech_Decode_FrameState* s;

	if (state == (Speech_Decode_FrameState **) NULL){
		return -1;
	}
	*state = NULL;

	/* allocate memory */
	if ((s= (Speech_Decode_FrameState *)voAMRNBDecmem_malloc(pMemOP, sizeof(Speech_Decode_FrameState), 32)) == NULL) {
			return -1;
	}
	s->decoder_amrState = NULL;
	s->post_state = NULL;
	s->postHP_state = NULL;

	if (Decoder_amr_init(&s->decoder_amrState, pMemOP) ||
		Post_Filter_init(&s->post_state, pMemOP) ||
		Post_Process_init(&s->postHP_state, pMemOP) ) {
			Speech_Decode_Frame_exit(&s, pMemOP);
			return -1;
	}

	Speech_Decode_Frame_reset(s);
	*state = s;

	return 0;
}

/*************************************************************************
*
*  Function:   Speech_Decode_Frame_reset
*  Purpose:    Resetses state memory
*
**************************************************************************
*/
int Speech_Decode_Frame_reset (Speech_Decode_FrameState *state)
{
	if (state == (Speech_Decode_FrameState *) NULL){
		return -1;
	}

	Decoder_amr_reset(state->decoder_amrState, (enum Mode)0);
	Post_Filter_reset(state->post_state);
	Post_Process_reset(state->postHP_state);

	state->prev_mode = (enum Mode)0;

	return 0;
}

/*
**************************************************************************
*
*  Function:   Speech_Decode_Frame_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void Speech_Decode_Frame_exit (Speech_Decode_FrameState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	Decoder_amr_exit(&(*state)->decoder_amrState, pMemOP);
	Post_Filter_exit(&(*state)->post_state, pMemOP);
	Post_Process_exit(&(*state)->postHP_state, pMemOP);
	/* deallocate memory */
	voAMRNBDecmem_free(pMemOP, *state);
	*state = NULL;

	return;
}

int Speech_Decode_Frame (
						 Speech_Decode_FrameState *st, /* io: post filter states                */
						 enum Mode mode,               /* i : AMR mode                          */
						 Word16 *serial,               /* i : serial bit stream                 */
						 enum RXFrameType frame_type,    /* i : Frame type                        */
						 unsigned short *synth                 /* o : synthesis speech (postfiltered output)                           */
						 )
{
	Word16 parm[MAX_PRM_SIZE + 1];  /* Synthesis parameters                */
	Word16 Az_dec[AZ_SIZE];         /* Decoded Az for post-filter in 4 subframes */

#ifndef NO13BIT
	Word16 i;
#endif
	/* Serial to parameters   */
	if ((frame_type == RX_SID_BAD) ||(frame_type == RX_SID_UPDATE))
	{
    	Bits2prm (MRDTX, serial, parm);			/* Override mode to MRDTX */
	} 
	else 
	{
		Bits2prm (mode, serial, parm);
	}

	Decoder_amr(st->decoder_amrState, mode, parm, frame_type, (Word16 *)synth, Az_dec);

	Post_Filter(st->post_state, mode, (Word16 *)synth, Az_dec);   /* Post-filter */
	/* post HP filter, and 15->16 bits */
#if 0 //lhp asm 
	Post_Process_asm(st->postHP_state, synth, L_FRAME);  
#else
	Post_Process(st->postHP_state, (Word16 *)synth, L_FRAME);  
#endif

#ifndef NO13BIT
	/* Truncate to 13 bits */
	for (i = 0; i < L_FRAME; i++) 
	{
		synth[i] = synth[i] & 0xfff8;
	}
#endif
	return 0;
}

#ifdef MMS_IO

/*
**************************************************************************
*
*  Function    : UnpackBits
*  Purpose     : Unpack and re-arrange bits from file storage format to the
*                format required by speech decoder.
*
**************************************************************************
*/
enum RXFrameType voAMRNBDecUnpackBits (
							 Word8  q,              /* i : Q-bit (i.e. BFI)        */
							 Word16 ft,             /* i : frame type (i.e. mode)  */
							 UWord8 packed_bits[],  /* i : sorted & packed bits    */
							 VoiAMRNBFrameType frameType,
							 enum Mode *mode,       /* o : mode information        */
							 Word16 bits[]          /* o : serial bits             */
)
{
	Word16  sid_type;
	UWord8 *pack_ptr, temp;
	nativeInt i,bit;
	pack_ptr = (UWord8*)packed_bits;
	/* real NO_DATA frame or unspecified frame type */
	if (ft >8)
	{
		*mode = (enum Mode)-1;
		return RX_NO_DATA;
	}
	i = 1;
	if(frameType==VOI_IF2)
	{
		temp = *pack_ptr;
		temp>>=4;
		pack_ptr++;
		for(;i<5;i++)
		{
			bits[voAMRNBDecsort_ptr[ft][i-1]] = (temp & 0x01);
			temp >>= 1;
		}
		temp = *pack_ptr;
		pack_ptr++;
		for (bit=1; i < voAMRNBDecunpacked_size[ft] + 1; i++)
		{
			bits[voAMRNBDecsort_ptr[ft][i-1]] = (temp & 0x01);//(temp & 0x80)>>7;

			if (bit<8)
			{
				temp >>= 1;
				bit++;
			}
			else
			{
				temp = *pack_ptr;
				pack_ptr++;
				bit = 1;
			}
		}
	}
	else
	{
		switch(frameType)
		{
		case VOI_IF2:
			temp = *pack_ptr;
			//temp>>=4;
			pack_ptr++;
			//temp <<= 1;i=2;
			for(;i<5;i++)
			{
				bits[voAMRNBDecsort_ptr[ft][i-1]] = (temp & 0x80)>>7;
				temp <<= 1;
			}
			temp = *pack_ptr;
			pack_ptr++;
			break;
		case VOI_IF1:
		case VOI_RAWDATA:
		case VOI_RFC3267:
		default:
			temp = *pack_ptr;
			pack_ptr++;
			break;

		}
		for (bit=1; i < voAMRNBDecunpacked_size[ft] + 1; i++)
		{
			/*if (temp & 0x80)	
			{
			if((temp & 0x80)!=1)
			bits[voAMRNBDecsort_ptr[ft][i-1]] = BIT_1;
			}
			else
			bits[voAMRNBDecsort_ptr[ft][i-1]] = BIT_0;*/
			bits[voAMRNBDecsort_ptr[ft][i-1]] = (temp & 0x80)>>7;
			if (bit<8)
			{
				temp <<= 1;
				bit++;
			}
			else
			{
				temp = *pack_ptr;
				pack_ptr++;
				bit = 1;
			}
		}
	}

	/* SID frame */
	if (ft == MRDTX)
	{
		if (temp & 0x80)	sid_type = 1;
		else				sid_type = 0;
		*mode = (enum Mode)((temp >> 4) & 0x07);

		if (q)
		{
			if (sid_type)	return  RX_SID_UPDATE;
			else			return	RX_SID_FIRST;
		}
		else
		{
			return	RX_SID_BAD;
		}
	}
	/* speech frame */
	else
	{
		*mode = (enum Mode)ft;
		if (q)	return RX_SPEECH_GOOD;
		else	return RX_SPEECH_BAD;
	}
}

#endif
