/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "../voMpegEnc.h"
#include "voMpegEncMB.h"
#include "../lowlevel/voMpegEncFastDct.h"
#include "../common/src/voMpegIdct.h"
#include "../lowlevel/voMpegEncQuant.h"

//Skip blocks having a coefficient sum below this value. This value will be
// corrected according to the MB quantizer to avoid artifacts for quant==1

#define PVOP_TOOSMALL_LIMIT 1

// Performs Forward DCT on all blocks
VO_VOID MacroBlockFDCT(VO_S16 *data)
{
	FastDct(data);
	FastDct(data + 64);
	FastDct(data + 128);
	FastDct(data + 192);
	FastDct(data + 256);
	FastDct(data + 320);
}

// Quantize all blocks -- Intra mode
static __inline VO_VOID MacroBlockQuantIntra(const VO_S32 quant, 
                                                            VO_S16 *qcoeff,
			                                                VO_S16 *data, 
                                                            const VO_S32 codec_id)
{
	VO_S32 scaler_lum, scaler_chr;

	if(codec_id == VO_INDEX_ENC_H263){
		scaler_lum = scaler_chr = 8;
		// Quantize the block 
		QuantIntraH263(data,       qcoeff,       quant, scaler_lum);
		QuantIntraH263(data +  64, qcoeff +  64, quant, scaler_lum);
		QuantIntraH263(data + 128, qcoeff + 128, quant, scaler_lum);
		QuantIntraH263(data + 192, qcoeff + 192, quant, scaler_lum);
		QuantIntraH263(data + 256, qcoeff + 256, quant, scaler_chr);
		QuantIntraH263(data + 320, qcoeff + 320, quant, scaler_chr);
	}else{
		scaler_lum = GeDcScaler(quant, 1);
		scaler_chr = GeDcScaler(quant, 0);
		// Quantize the block
		QuantIntraMpeg4(data,       qcoeff,       quant, scaler_lum);
		QuantIntraMpeg4(data +  64, qcoeff +  64, quant, scaler_lum);
		QuantIntraMpeg4(data + 128, qcoeff + 128, quant, scaler_lum);
		QuantIntraMpeg4(data + 192, qcoeff + 192, quant, scaler_lum);
		QuantIntraMpeg4(data + 256, qcoeff + 256, quant, scaler_chr);
		QuantIntraMpeg4(data + 320, qcoeff + 320, quant, scaler_chr);
	}
}


// DeQuantize all blocks -- Intra mode
static __inline VO_VOID MacroBlockDeQuantIntra(VO_S32 quant, 
                                                                VO_S16 *qcoeff, 
			                                                    VO_S16 *data, 
                                                                VO_S32 codec_id)
{
	VO_S32 scaler_lum, scaler_chr;

	if(codec_id == VO_INDEX_ENC_H263){
		scaler_lum = scaler_chr = 8;
	}else{
		scaler_lum = GeDcScaler(quant, 1);
		scaler_chr = GeDcScaler(quant, 0);
	}

	DequantIntraH263Mpeg4(qcoeff,       data,       quant, scaler_lum, codec_id);
	DequantIntraH263Mpeg4(qcoeff + 64,  data + 64,  quant, scaler_lum, codec_id);
	DequantIntraH263Mpeg4(qcoeff + 128, data + 128, quant, scaler_lum, codec_id);
	DequantIntraH263Mpeg4(qcoeff + 192, data + 192, quant, scaler_lum, codec_id);
	DequantIntraH263Mpeg4(qcoeff + 256, data + 256, quant, scaler_chr, codec_id);
	DequantIntraH263Mpeg4(qcoeff + 320, data + 320, quant, scaler_chr, codec_id);
}

static __inline VO_U8 MacroBlockQuantInter(VO_S32 algorithm_flag, 
                                                            VO_S32 quant, 
			                                                VO_S16 *data, 
                                                            VO_S16 *qcoeff,
                                                            VO_S32 codec_id)
{
	const VO_S32 limit = PVOP_TOOSMALL_LIMIT + ((quant == 1)? 1 : 0);
	VO_S32 i,sum,code_block;
	VO_U8 cbp = 0;
    VO_S16 *coeffb;

    for (i = 0; i < 6; i++) {
	    coeffb = qcoeff + (i << 6);
	    sum = QuantInterH263Mpeg4(coeffb, data + (i << 6), quant);

        if(codec_id == VO_INDEX_ENC_H263)
        {
            VOCLAMP(coeffb[0],-127,127);
        }

        if ((sum >= limit) || (coeffb[1] != 0) || (coeffb[8] != 0) || (coeffb[0] != 0)) 
            code_block = 1;
        else
            code_block = 0;

	    //Set the corresponding cbp bit 
	    cbp |= code_block << (5 - i);
    }
	return(cbp);
}

static __inline VO_VOID  MacroBlockTrans8to16( VO_U32 stride,
			                                                const Mpeg4Frame * const img_cur,
                                                            VO_S16 *data)
{
	const VO_U32 stride2 = stride >>1;

	ExpandByteToShort(data,       img_cur->y,                     stride);
	ExpandByteToShort(data + 64 , img_cur->y + 8,                 stride);
	ExpandByteToShort(data + 128, img_cur->y + (stride << 3),     stride);
	ExpandByteToShort(data + 192, img_cur->y + (stride << 3) + 8, stride);
	ExpandByteToShort(data + 256, img_cur->uv[0],                     stride2);
	ExpandByteToShort(data + 320, img_cur->uv[1],                     stride2);
}


//*****************************************************************************
//* Module functions
// ****************************************************************************
VO_VOID MacroBlockTransFormIntra (const Mpeg4Frame * const img_cur,
                                                VO_S32 stride,
                                                VO_S32 quant,
                                                VO_S16 * const data,
                                                VO_S16 * const qcoeff,
                                                VO_U32 codec_id)
{
#ifndef P_DOMAIN_RC
	MacroBlockTrans8to16(stride, img_cur, data);
	MacroBlockFDCT(data);
#endif
	MacroBlockQuantIntra(quant, data, qcoeff, codec_id);

    //reconstruct
	MacroBlockDeQuantIntra(quant, data, qcoeff, codec_id);
	IDCT_Block8x8(data, img_cur->y, stride, NULL, 0);
	IDCT_Block8x8(data + 64, img_cur->y + 8, stride, NULL, 0);
	IDCT_Block8x8(data + 128, img_cur->y + (stride << 3), stride, NULL, 0);
	IDCT_Block8x8(data + 192, img_cur->y + (stride << 3) + 8, stride, NULL, 0);
	IDCT_Block8x8(data + 256, img_cur->uv[0], stride>>1, NULL, 0);
	IDCT_Block8x8(data + 320, img_cur->uv[1], stride>>1, NULL, 0);
}

#ifdef P_DOMAIN_RC
// only do DCT 
VO_VOID MacroBlockTransDctIntra(const Mpeg4Frame * const img_cur,
				                        VO_S32 stride,
				                        VO_S16 * const data)
{
	MacroBlockTrans8to16(stride, img_cur, data);
	MacroBlockFDCT(data);	
}
#endif

VO_U8 MacroBlockTransFormInter(const Mpeg4Frame * const img_cur,
                                            VO_S32 stride,
                                            VO_S32 quant,
                                            VO_S16 * const data,
                                            VO_S16 * const qcoeff,
                                            VO_U32 al_flag,
                                            VO_U32 codec_id)
{
    VO_U8 cbp;
#ifndef P_DOMAIN_RC
    MacroBlockFDCT(data);
#endif
    if(!(cbp = MacroBlockQuantInter(al_flag, quant, data, qcoeff, codec_id)))
        return(cbp);

    return(cbp);
}