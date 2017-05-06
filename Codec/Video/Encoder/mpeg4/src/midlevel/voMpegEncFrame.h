/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/


#ifndef _ENCFRAME_
#define _ENCFRAME_

#include "../lowlevel/voMpegEncWriteBits.h"
#include "../voMpegEnc.h"
#include "../voMpegProtect.h"

RETURN_CODE EncodeIVOP(ENCHND * const enc_hnd, BitStream * const bs, const VO_S32 vt_quant);
RETURN_CODE EncodePVOP(ENCHND * const enc_hnd, BitStream * const bs);
#endif/* _ENCFRAME_ */


