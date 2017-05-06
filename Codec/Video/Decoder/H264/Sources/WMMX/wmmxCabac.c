
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		x86Cabac.c
*
* \brief
*		defines short MMX inline function for local usage;
*
************************************************************************
*/

#include "global.h"
#include "biaridecod.h"
#include "vlc.h"

avdNativeUInt biari_decode_symbol(Bitstream *dep, BiContextTypePtr bi_ct)
{
	avdNativeUInt bit = bi_ct->MPS;
	avdNativeUInt rLPS = avd_rLPS_table_64x4[bi_ct->state][(dep->Drange>>6) & 0x03];
	dep->Drange -= rLPS;
	if (dep->Dvalue < dep->Drange) /* MPS */ 
		bi_ct->state = avd_AC_next_state_MPS_64[bi_ct->state]; // next state
	else{						  /* LPS */
		dep->Dvalue -= dep->Drange;
		dep->Drange = rLPS;
		bit ^= 1;
		if (!bi_ct->state)			 // switch meaning of MPS if necessary	
			bi_ct->MPS ^= 1;              
		bi_ct->state = avd_AC_next_state_LPS_64[bi_ct->state]; // next state 
	}
	if (dep->Drange < QUARTER){
		avdUInt8 shift = avdNumLeadingZerosPlus1[dep->Drange];
		dep->Drange <<= shift;
		dep->Dvalue = (dep->Dvalue<<shift) | (avdUInt16)GetBits(dep, shift);
	}
	return(bit);
}

