/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
 
//#include "stdafx.h"
#include "voMpeg4Decoder.h"
#include "voMpeg4Parser.h"
#include "voMpegReadbits.h"

#ifdef VODIVX3

#define get012(pDec) (GetBits(pDec,1) ? GetBits(pDec,1)+1 : 0)

VO_U32 DIVX3GetHdr( VO_MPEG4_DEC *pDec , const VO_S32 nHeaderFlag)
{
	VO_S32 i;
	UPDATE_CACHE(pDec);

    pDec->nVOPType = GetBits(pDec,2);

	if(nHeaderFlag == DEC_FRAMETYPE)
		return VO_ERR_NONE;

    pDec->nQuant = GetBits(pDec, 5);

    switch (pDec->nVOPType) 
	{
	case VO_VIDEO_FRAME_I:
		pDec->slice_height = pDec->nMBHeight;
        i = GetBits(pDec,5)-0x16;
        if (i>1)
			pDec->slice_height /= i;

		i = 3+get012(pDec);
		pDec->rl_vlc[1] = pDec->rl_table[i];
		pDec->rlmax_vlc[1] = pDec->rlmax+i*192;

		i = get012(pDec);
		pDec->rl_vlc[0] = pDec->rl_table[i];
		pDec->rlmax_vlc[0] = pDec->rlmax+i*192;

		i = GetBits(pDec,1);
		pDec->dc_chr_vlc = pDec->dc_chr_table[i];
		pDec->dc_lum_vlc = pDec->dc_lum_table[i];
	    pDec->rounding = 1;

		break;

	case VO_VIDEO_FRAME_P:
		pDec->use_coded_bit = GetBits(pDec, 1);

		i = get012(pDec);

		pDec->rl_vlc[0] = pDec->rl_table[i];
		pDec->rlmax_vlc[0] = pDec->rlmax+i*192;

		pDec->rl_vlc[1] = pDec->rl_table[3+i];
		pDec->rlmax_vlc[1] = pDec->rlmax+(3+i)*192;

		i = GetBits(pDec,1);
		pDec->dc_chr_vlc = pDec->dc_chr_table[i];
		pDec->dc_lum_vlc = pDec->dc_lum_table[i];

		pDec->mv_vlc = pDec->mv_table[GetBits(pDec,1)];

		if (pDec->nFlipRounding)
			pDec->rounding = 1 - pDec->rounding;
		else
			pDec->rounding = 0;
		break;

	default:
		return VO_ERR_DEC_DIVX3_HEADER;
    }

	return VO_ERR_NONE;
}

#endif
