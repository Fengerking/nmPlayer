/*************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : bgnscd.c
*      Purpose          : Background noise source charateristic detector (SCD)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "bgnscd.h"
const char bgnscd_id[] = "@(#)$Id $" bgnscd_h;
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"
#include "set_zero.h"
#include "gmed_n.h"
#include "sqrt_l.h"
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*-----------------------------------------------------------------*
*   Decoder constant parameters (defined in "cnst.h")             *
*-----------------------------------------------------------------*
*   L_FRAME       : Frame size.                                   *
*   L_SUBFR       : Sub-frame size.                               *
*-----------------------------------------------------------------*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Bgn_scd_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
Word16 Bgn_scd_init (Bgn_scdState **state, VO_MEM_OPERATOR *pMemOP)
{
	Bgn_scdState* s;
	if (state == (Bgn_scdState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (Bgn_scdState *) voAMRNBDecmem_malloc(pMemOP, sizeof(Bgn_scdState), 32)) == NULL){
		return -1;
	}
	Bgn_scd_reset(s);
	*state = s;
	return 0;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
Word16 Bgn_scd_reset (Bgn_scdState *state)
{
	if (state == (Bgn_scdState *) NULL){
		return -1;
	}
	/* Static vectors to zero */
	voAMRNBDecSetZero (state->frameEnergyHist, L_ENERGYHIST);
	/* Initialize hangover handling */
	state->bgHangover = 0;
	return 0;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void Bgn_scd_exit (Bgn_scdState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBDecmem_free(pMemOP, *state);
	*state = NULL;
	return;
}

/*
**************************************************************************
*
*  Function    : Bgn_scd
*  Purpose     : Charaterice synthesis speech and detect background noise
*  Returns     : background noise decision; 0 = no bgn, 1 = bgn
*
**************************************************************************
*/
Word16 Bgn_scd (Bgn_scdState *st,      /* i : State variables for bgn SCD */
				Word16 ltpGainHist[],  /* i : LTP gain history            */
				Word16 speech[],       /* o : synthesis speech frame      */
				Word16 *voicedHangover /* o : # of frames after last voiced frame  */
									   )
{
	int  i;
	Word16 prevVoiced, inbgNoise;
	Word16 temp;
	Word16 ltpLimit, frameEnergyMin;
	Word16 currEnergy = 0, noiseFloor, maxEnergy, maxEnergyLastPart;
	Word32 s;
	/* Update the inBackgroundNoise flag (valid for use in next frame if BFI) */
	/* it now works as a energy detector floating on top                      */ 
	/* not as good as a VAD.                                                  */
#ifdef ARMv7_opt
    s = voAMRNBDecsum_armv7_xx(s, speech, speech, L_FRAME);
#else
	s = (Word32) 0; 
	for (i = 0; i < L_FRAME; i++)
	{
#ifdef C_OPT
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
		i++;
		s += (speech[i] * speech[i])<<1;
#else
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i++]);
		s = L_mac (s, speech[i], speech[i]);
#endif
	}
#endif
	s = L_shl2(s, 2);  
	currEnergy = extract_h (s);
	frameEnergyMin = 32767; 
	for (i = 0; i < L_ENERGYHIST; i++)
	{
		if(st->frameEnergyHist[i] < frameEnergyMin)
			frameEnergyMin = st->frameEnergyHist[i];
	}
	noiseFloor = shl2 (frameEnergyMin, 4); /* Frame Energy Margin of 16 */
	maxEnergy = st->frameEnergyHist[0]; 
	for (i = 1; i < 56; i++)
	{
		if(maxEnergy < st->frameEnergyHist[i])
		{
			maxEnergy = st->frameEnergyHist[i]; 
		}
	}

	maxEnergyLastPart = st->frameEnergyHist[40];
	for (i = 41; i < L_ENERGYHIST; i++)
	{
		if(maxEnergyLastPart < st->frameEnergyHist[i])
		{
			maxEnergyLastPart = st->frameEnergyHist[i];    
		}
	}
	inbgNoise = 0;        /* false */ 
	/* Do not consider silence as noise */
	/* Do not consider continuous high volume as noise */
	/* Or if the current noise level is very low */
	/* Mark as noise if under current noise limit */
	/* OR if the maximum energy is below the upper limit */

	if ((maxEnergy > LOWERNOISELIMIT) &&(currEnergy < FRAMEENERGYLIMIT) &&
		(currEnergy > LOWERNOISELIMIT) &&((currEnergy < noiseFloor) || (maxEnergyLastPart < UPPERNOISELIMIT)))
	{
		if((st->bgHangover + 1) > 30)
		{
			st->bgHangover = 30;
		} else
		{
			st->bgHangover = (st->bgHangover + 1);
		}
	}
	else
	{
		st->bgHangover = 0;   
	}
	/* make final decision about frame state , act somewhat cautiosly */
	if(st->bgHangover > 1)
		inbgNoise = 1;  

	for (i = 0; i < 59; i++)
	{
		st->frameEnergyHist[i] = st->frameEnergyHist[i+1];
	}
	st->frameEnergyHist[59] = currEnergy; 

	/* prepare for voicing decision; tighten the threshold after some 
	time in noise */
	ltpLimit = 13926;             /* 0.85  Q14 */ 

	if(st->bgHangover > 8)
	{
		ltpLimit = 15565;          /* 0.95  Q14 */    
	}

	if(st->bgHangover > 15)
	{
		ltpLimit = 16383;          /* 1.00  Q14 */    
	}

	/* weak sort of voicing indication. */
	prevVoiced = 0;        /* false */                

	if(gmed_n(&ltpGainHist[4], 5) > ltpLimit)
	{
		prevVoiced = 1;     /* true  */               
	} 
	if(st->bgHangover > 20) {
		if(gmed_n(ltpGainHist, 9) > ltpLimit)
		{
			prevVoiced = 1;  /* true  */  
		}
		else
		{
			prevVoiced = 0;  /* false  */ 
		}
	}
	if (prevVoiced)
	{
		*voicedHangover = 0; 
	}
	else
	{
		temp = (*voicedHangover+ 1);
		if(temp > 10)
		{
			*voicedHangover = 10; 
		}
		else
		{
			*voicedHangover = temp; 
		}
	}

	return inbgNoise;
}
