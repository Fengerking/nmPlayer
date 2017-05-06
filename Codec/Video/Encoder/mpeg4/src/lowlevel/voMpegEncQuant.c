/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "../voMpegEnc.h"
#include "voMpegEncQuant.h"

#define VOSCALEBITS	16	
const VO_U32 quant_multi_table[32] =  
//((1L << VOSCALEBITS) / (X) + 1) ;X=0,2,4,6,...,62
{
	0,			32769,		16385,		10923,
	8193,		6554,			5462,			4682,
	4097,     3641,			3277,			2979,
	2731,		2521,			2341,			2185,
	2049,		1928,			1821,			1725,
	1639,		1561,			1490,			1425,
	1366,		1311,			1261,			1214,
	1171,		1130,			1093,			1058
};

VO_VOID ExpandByteToShort_C ( VO_S16* const dst, const VO_U8 * const src, const VO_U32 stride)
{
	VO_U32 j = 8;
	const VO_U8 *ptr_src = src;
	VO_S16 * ptr_dst = dst;

	do{
		*(ptr_dst++) = (VO_S16) *ptr_src;
		*(ptr_dst++) = (VO_S16) *(ptr_src + 1);
		*(ptr_dst++) = (VO_S16) *(ptr_src + 2);
		*(ptr_dst++) = (VO_S16) *(ptr_src + 3);
		*(ptr_dst++) = (VO_S16) *(ptr_src + 4);
		*(ptr_dst++) = (VO_S16) *(ptr_src + 5);
		*(ptr_dst++) = (VO_S16) *(ptr_src + 6);
		*(ptr_dst++) = (VO_S16) *(ptr_src + 7);
		ptr_src += stride;
	}while(--j!=0);
}

//	H263 quantize intra-block
VO_U32 QuantIntraH263_C (VO_S16 * dct_coeff, const VO_S16 * data, const VO_U32 quant, const VO_U32 dc_scalar)
{	
    VO_S32 i=1;
	const VO_U16 twofold_quant = (VO_U16)(quant << 1);
    const VO_U32 multi_param = quant_multi_table[quant];
  
    dct_coeff[0] = (data[0]>0 ?(VO_S16)( (data[0]+(dc_scalar>>1))/dc_scalar) :(VO_S16) ((data[0]-(dc_scalar>>1))/dc_scalar));

    VOCLAMP(dct_coeff[0], 1, 254);

    while(i<64){
		VO_S16 ac_level = data[i];
        VO_U16  abs_ac_level = VOGETABS(ac_level);

        if( abs_ac_level < twofold_quant ) {
            dct_coeff[i] = 0;
        }
        else if (ac_level < 0) {
            ac_level = (VO_S16)((-ac_level * multi_param) >> VOSCALEBITS);
            dct_coeff[i] = -ac_level;
            VOCLAMP_DOWN(dct_coeff[i],-128);	
        }
        else {
            dct_coeff[i] = (VO_S16)((ac_level * multi_param) >> VOSCALEBITS);
            VOCLAMP_UP(dct_coeff[i],127);
        }
        i++;
	}
	return(0);
}

VO_U32 QuantIntraMpeg4_C(VO_S16 * dct_coeff, const VO_S16 * data, const VO_U32 quant, const VO_U32 dc_scalar)
{
	const VO_U32 multi_param = quant_multi_table[quant];
	const VO_U16 twofold_quant = (VO_U16)(quant << 1);
    //const VO_S32 negative_twofold_quant = -twofold_quant;
	VO_S32 i =1;

	dct_coeff[0] = data[0]>0 ? (VO_S16)((data[0]+(dc_scalar>>1))/dc_scalar) : (VO_S16)((data[0]-(dc_scalar>>1))/dc_scalar);

	while(i<64){
		VO_S16 ac_level = data[i];
        VO_U16  abs_ac_level = VOGETABS(ac_level);
        if( abs_ac_level < twofold_quant ) {
            dct_coeff[i] = 0;
        }
        else if(ac_level < 0) {
            ac_level = (VO_S16)((-ac_level * multi_param) >> VOSCALEBITS);
            dct_coeff[i] = -ac_level;
        }
        else {
            dct_coeff[i] = (VO_S16)((ac_level * multi_param) >> VOSCALEBITS);
        }
        i++;
	}
	return(0);
}

VO_U32 QuantInterH263Mpeg4_C(VO_S16 * dct_coeff, const VO_S16 * data, const VO_U32 quant)
{
	const VO_U32 multi_param = quant_multi_table[quant];
	const VO_S32 twofold_quant = quant << 1;
	const VO_S32 half_quant = quant >> 1;
	VO_U32 dct_coeff_sum = 0;
	VO_U32 i =0;

	while(i<64) {
		VO_S32 ac_level = data[i];
		VO_S32 tmp = VOGETABS(ac_level) - half_quant;
		if(tmp < twofold_quant){
			dct_coeff[i] = 0;
		}else{
			tmp = (tmp * multi_param) >> VOSCALEBITS;
			dct_coeff_sum += tmp;

			if(ac_level < 0)
				tmp = -tmp;
			dct_coeff[i] = (VO_S16)tmp;
		}
        i++;
	}
	return(dct_coeff_sum);
}

//	dequantize intra-block of H263 and Mpeg4
//  clamp to [-2048,2047]
VO_U32 DequantIntraH263Mpeg4_C(VO_S16 * data,
					 const VO_S16 * dct_coeff,
					 const VO_U32 quant,
					 const VO_U32 dc_scalar,
					 const VO_U32 codec_id)
{
	const VO_S32 twofold_quant = quant << 1;
	VO_S32 quant_add, i =1;

	if(codec_id == VO_INDEX_ENC_H263)
		quant_add = quant;
	else
		quant_add = (quant & 1 ? quant : quant - 1);

	data[0] = (VO_S16)(dct_coeff[0] * dc_scalar);
    VOCLAMP(data[0],-2048,2047);

    while(i<64) {
        VO_S32 ac_level = dct_coeff[i];
        if (ac_level == 0) {
            data[i] = 0;
        } else if (ac_level < 0) {
            data[i] = (VO_S16)(ac_level*twofold_quant- quant_add);
            VOCLAMP_DOWN(data[i],-2048);
		} else {
            data[i] = (VO_S16)(ac_level *twofold_quant + quant_add);
            VOCLAMP_UP(data[i],2047);
		}
        i++;
    }

	return(0);
}
