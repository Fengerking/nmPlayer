/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _ENCMB_H
#define _ENCMB_H

#include "../voMpegEnc.h"
#include "../lowlevel/voMpegEncWriteBits.h"
#include "../voMpegProtect.h"

VO_VOID MacroBlockTransFormIntra(const Mpeg4Frame * const cur,
				  VO_S32 stride,
				  VO_S32 quant,
				  VO_S16 * const data,
				  VO_S16 * const qcoeff,
				  VO_U32 codec_id);

VO_U8 MacroBlockTransFormInter(const Mpeg4Frame * const img_cur,
				  VO_S32 stride,
				  VO_S32 quant,
				  VO_S16 * const data,
				  VO_S16 * const qcoeff,
				  VO_U32 al_flag,
				  VO_U32 codec_id);


static __inline VO_VOID MBSkip(BitStream * bs)
{
	PutBit(bs, 1);
}

VO_VOID MacroBlockFDCT(VO_S16 *data);
VO_VOID MacroBlockTransDctIntra(const Mpeg4Frame * const img_cur,
				                        VO_S32 stride,
				                        VO_S16 * const data);

#endif //_ENCMB_H
