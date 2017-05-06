/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : sp_enc.c
*      Purpose          : Pre filtering and encoding of one speech frame.
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "set_zero.h"
#include "pre_proc.h"
#include "prm2bits.h"
#include "mode.h"
#include "cod_amr.h"
#ifdef MMS_IO
#include "frame.h"
#include "bitno.tab"
#endif

#include "sp_enc.h"
const char sp_enc_id[] = "@(#)$Id $" sp_enc_h;

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
*  Function:   voAMRNBEnc_Speech_Encode_Frame_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int voAMRNBEnc_Speech_Encode_Frame_init (Speech_Encode_FrameState **state,
							  Flag dtx, VO_MEM_OPERATOR *pMemOP)
{
	Speech_Encode_FrameState* s;
	if (state == (Speech_Encode_FrameState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (Speech_Encode_FrameState *) voAMRNBEnc_mem_malloc(pMemOP, sizeof(Speech_Encode_FrameState), 32)) == NULL){
		return -1;
	}
	s->pre_state = NULL;
	s->cod_amr_state = NULL;
	s->dtx = dtx;

	if (voAMRNBEnc_Pre_Process_init(&s->pre_state, pMemOP) ||voAMRNBEnc_cod_amr_init(&s->cod_amr_state, s->dtx, pMemOP)) {
		voAMRNBEnc_Speech_Encode_Frame_exit(&s, pMemOP);
		return -1;
	}

	voAMRNBEnc_Speech_Encode_Frame_reset(s);
	*state = s;
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_Speech_Encode_Frame_reset
*  Purpose:    Resetses state memory
*
**************************************************************************
*/
int voAMRNBEnc_Speech_Encode_Frame_reset (Speech_Encode_FrameState *state)
{
	if (state == (Speech_Encode_FrameState *) NULL){
		return -1;
	}

	voAMRNBEnc_Pre_Process_reset(state->pre_state);
	voAMRNBEnc_cod_amr_reset(state->cod_amr_state);
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_Speech_Encode_Frame_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_Speech_Encode_Frame_exit(Speech_Encode_FrameState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;

	voAMRNBEnc_Pre_Process_exit(&(*state)->pre_state, pMemOP);
	voAMRNBEnc_cod_amr_exit(&(*state)->cod_amr_state, pMemOP);

	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;
	return;
}

int voAMRNBEnc_Speech_Encode_Frame_First (
							   Speech_Encode_FrameState *st,  /* i/o : post filter states       */
							   Word16 *new_speech)            /* i   : speech input             */
{
#if !defined(NO13BIT)
	Word16 i;
	/* Delete the 3 LSBs (13-bit input) */
	for (i = 0; i < L_NEXT; i++) 
	{
		new_speech[i] = new_speech[i] & 0xfff8; 
	}
#endif
	/* filter + downscaling */
	voAMRNBEnc_Pre_Process (st->pre_state, new_speech, L_NEXT);
	voAMRNBEnc_cod_amr_first(st->cod_amr_state, new_speech);
	return 0;
}

int voAMRNBEnc_Speech_Encode_Frame (
						 Speech_Encode_FrameState *st, /* i/o : post filter states          */
						 enum Mode mode,               /* i   : speech coder mode           */
						 Word16 *new_speech,           /* i   : speech input                */
						 Word16 *serial,               /* o   : serial bit stream           */
						 enum Mode *usedMode           /* o   : used speech coder mode */
						 )
{
	Word16 prm[MAX_PRM_SIZE];   /* Analysis parameters.                  */
    Word16 syn[L_FRAME];        /* Buffer for synthesis speech           */
	/* initialize the serial output frame to zero */                                  
#if !defined(NO13BIT)
	int i; /* Delete the 3 LSBs (13-bit input) */
	for (i = 0; i < L_FRAME; i++)   
	{
		new_speech[i] = new_speech[i] & 0xfff8;    
	}
#endif
	memset(serial,0,MAX_SERIAL_SIZE<<1); 
	/* filter + downscaling */
	voAMRNBEnc_Pre_Process (st->pre_state, new_speech, L_FRAME);           
	/* Call the speech encoder */
	voAMRNBEnc_cod_amr(st->cod_amr_state, mode, new_speech, prm, usedMode, syn);
     
	/* Parameters to serial bits */
	voAMRNBEnc_Prm2bits(*usedMode, prm, &serial[0]);    
	return 0;
}

#ifdef MMS_IO
const UWord8 IF2_packed_size[16] = {13, 14, 16, 18, 19, 21, 26, 31, 6,  6,  6,  6,  0,  0,  0,  1};
/*************************************************************************
*
*  FUNCTION:    voAMRNBEnc_PackBits
*
*  PURPOSE:     Sorts speech bits according decreasing subjective importance
*               and packs into octets according to AMR file storage format
*               as specified in RFC 3267 (Sections 5.1 and 5.3).
*
*  DESCRIPTION: Depending on the mode, different numbers of bits are
*               processed. Details can be found in specification mentioned
*               above and in file "bitno.tab".
*
*************************************************************************/
Word16 voAMRNBEnc_PackBits(
				enum Mode used_mode,       /* i : actual AMR mode             */
				enum Mode mode,            /* i : requested AMR (speech) mode */
				enum TXFrameType fr_type,  /* i : frame type                  */
				VoiAMRNBFrameType frameType,/*raw/IF1/IF2 */
				Word16 bits[],             /* i : serial bits                 */
				UWord8 packed_bits[]       /* o : sorted&packed bits          */
)
{
	Word32   i,end;
	Word32   temp = 0,bit;
	UWord8   *pack_ptr;

	pack_ptr = (UWord8*)packed_bits;

	/* file storage format can handle only speech frames, AMR SID frames and NO_DATA frames */
	/* -> force NO_DATA frame */
	if (used_mode < 0 || used_mode > 15 || (used_mode > 8 && used_mode < 15))
	{
		used_mode = 15;
	}
#if ONLY_ENCODE_122
	used_mode = 7;
#endif
	/* mark empty frames between SID updates as NO_DATA frames */
	if (used_mode == MRDTX && fr_type == TX_NO_DATA)
	{
		used_mode = 15;
	}
	temp = 0;
	end = unpacked_size[used_mode] + 1;
	i = 1;
	/* insert table of contents (ToC) byte at the beginning of the frame */
	if(frameType == VOI_IF2)
	{
		*pack_ptr = used_mode;
		for (; i < 5 ; i++)
		{
#if !ONLY_ENCODE_122
			if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
#else
			if (bits[sort_122[i-1]] == BIT_1)
#endif
			{
				temp |=(1<<(i-1));
			}
		}
		*pack_ptr |=(temp<<4);
		pack_ptr++;

		temp = 0;
		for (bit=1; i < end ; i++)
		{
#if !ONLY_ENCODE_122
			if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
#else
			if (bits[sort_122[i-1]] == BIT_1)
#endif
			{
				temp |=(1<<(bit-1));
			}

			if (bit<8)
			{
				//temp <<= 1;
				bit++;
			}
			else
			{
				*pack_ptr = temp;
				pack_ptr++;
				temp = 0;
				bit=1;
			}
		}
	}
	else
	{
		switch(frameType)
		{
		case VOI_IF1:
			temp = mode;
			temp <<= 4;//frametype
			temp|=0x08;//quality=1
			temp|=mode;//mode indication
			*pack_ptr = temp;
			pack_ptr++;
			*pack_ptr = 1<<5;//mode request==1
			pack_ptr++;
			*pack_ptr=0;//crc
			pack_ptr++;
			break;
		case VOI_IF2:
			*pack_ptr = used_mode;
			for (; i < 5 ; i++)
			{
#if !ONLY_ENCODE_122
				if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
#else
				if (bits[sort_122[i-1]] == BIT_1)
#endif
				{
					temp++;
				}
				if(i!=4)
					temp <<= 1;
			}
			*pack_ptr |=(temp<<4);
			pack_ptr++;

			break;
		case VOI_RAWDATA:
			break;
		case VOI_RFC3267:
		default:
			*pack_ptr = toc_byte[used_mode];
			pack_ptr++;
			break;

		}
		temp = 0;
		/* note that NO_DATA frames (used_mode==15) do not need further processing */
		if (used_mode == 15)
		{
			return 1;
		}

		/*
		if(frameType==VOI_IF2)
		{
		for (; i < 5 ; i++)
		{
		#if !ONLY_ENCODE_122
		if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
		#else
		if (bits[sort_122[i-1]] == BIT_1)
		#endif
		{
		temp++;
		}
		temp <<= 1;
		}
		*pack_ptr |=(temp<<4);
		pack_ptr++;
		temp = 0;
		}*/
		/* sort and pack speech bits */
		for (bit=1; i < end ; i++)
		{
#if !ONLY_ENCODE_122
			if (bits[sort_ptr[used_mode][i-1]] == BIT_1)
#else
			if (bits[sort_122[i-1]] == BIT_1)
#endif
			{
				temp++;
			}
			if (bit<8)
			{
				temp <<= 1;
				bit++;
			}
			else
			{
				*pack_ptr = temp;
				pack_ptr++;
				temp = 0;
				bit=1;
			}
		}
	}
	/* insert SID type indication and speech mode in case of SID frame */
	if (used_mode == MRDTX)
	{
		if (fr_type == TX_SID_UPDATE)
		{
			temp++;
		}
		temp <<= 3;
		temp += mode & 0x0007;
		temp <<= 1;
	}
	/* insert unused bits (zeros) at the tail of the last byte */
	temp <<= (unused_size[used_mode] - 1);
	*pack_ptr = temp;

	switch(frameType)
	{
	case VOI_IF1:
		return packed_size[used_mode]+2;
		break;
	case VOI_IF2:
#if !ONLY_ENCODE_122
		return IF2_packed_size[used_mode];
#else
		return IF2_packed_size[7];
#endif
	case VOI_RAWDATA:
		return packed_size[used_mode]-1;
	case VOI_RFC3267:
	default:
		return packed_size[used_mode];

	}
}
#endif
