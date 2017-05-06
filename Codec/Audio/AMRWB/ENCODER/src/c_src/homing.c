/***********************************************************************
*                                                                      *
*          VisualOn, Inc. Confidential and Proprietary, 2003-2010      *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: homing.c                                                 *
*                                                                      *
*	   Description:Performs the homing routines                        *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "cnst.h"
#include "basic_op.h"
#include "bits.h"
#include "homing.tab"

Word16 encoder_homing_frame_test(Word16 input_frame[])
{
	Word32 i;
	Word16 j = 0;

	/* check 320 input samples for matching EHF_MASK: defined in e_homing.h */
	for (i = 0; i < L_FRAME16k; i++)
	{
		j = (Word16) (input_frame[i] ^ EHF_MASK);

		if (j)
			break;
	}

	return (Word16) (!j);
}

