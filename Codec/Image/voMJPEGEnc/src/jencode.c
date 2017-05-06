#include "mem_align.h"
#include "jcommon.h"
#include "jencoder.h"

static void jpeg_add_quant_table(JPEG_QUANT_TBL *tbl_ptr, const UINT8 *basic_table, UINT32 scale_factor)
{
	UINT32 i;
	UINT32 temp;
	
	for (i = 0; i < JPEG_BLOCK_SIZE; i++) 
	{
		temp = ((UINT32) basic_table[i] * scale_factor + 50) / 100;
		
        if (temp <= 0)		temp = 1;
		if (temp > 255)		temp = 255;
		tbl_ptr->quantrawval[i] = (UINT8)temp;
	}
}

void jpeg_set_quality(JPEG_QUANT_TBL *tbl_ptrs[NUM_BLOCK_TBLS], UINT32 quality)
{
	if (quality <= 0) 
		quality = 1;

	if (quality > 100) 
		quality = 100;
	
	if (quality < 50)
		quality = 5000 / quality;
	else
		quality = 200 - (quality << 1);
	
	jpeg_add_quant_table(tbl_ptrs[0], std_luminance_quant_tbl, quality);
	jpeg_add_quant_table(tbl_ptrs[1], std_chrominance_quant_tbl, quality);
}

#ifdef IPP_SUPPORT
INT32 Init_QuantTab(JPEG_QUANT_TABLE* quant_tbl)
{
	UINT32 i;
	IppStatus status;


	for(i = 0; i < NUM_BLOCK_TBLS; i++)
	{
		status = ippiDCTQuantFwdTableInit_JPEG_8u16u(
			(Ipp8u*)(quant_tbl->Quant_tbl_ptrs[i]->quantrawval),
			(Ipp16u*)(quant_tbl->Quant_tbl_ptrs[i]->quantval));

		if(ippStsNoErr != status)
			return -1;
	}	

	return 0;
}

INT32 Init_HuffTab(JPEG_HUFF_ENTROPY* entropy)
{
	UINT32 tbl_size;
	IppStatus status;
	status = ippiEncodeHuffmanSpecGetBufSize_JPEG_8u(&tbl_size);
	if(ippStsNoErr != status)
	{
		return -1;
	}
	
	if(entropy->dc_huff_tbls[0].pHuffTbl)
	{
		mem_free(entropy->dc_huff_tbls[0].pHuffTbl);
	}
	entropy->dc_huff_tbls[0].pHuffTbl = (IppiDecodeHuffmanSpec *)mem_malloc(tbl_size, DCACHE_ALIGN_NUM);

	if(entropy->dc_huff_tbls[1].pHuffTbl)
	{
		mem_free(entropy->dc_huff_tbls[1].pHuffTbl);
	}
	entropy->dc_huff_tbls[1].pHuffTbl = (IppiDecodeHuffmanSpec *)mem_malloc(tbl_size, DCACHE_ALIGN_NUM);

	if(entropy->ac_huff_tbls[0].pHuffTbl)
	{
		mem_free(entropy->ac_huff_tbls[0].pHuffTbl);
	}
	entropy->ac_huff_tbls[0].pHuffTbl = (IppiDecodeHuffmanSpec *)mem_malloc(tbl_size, DCACHE_ALIGN_NUM);

	if(entropy->ac_huff_tbls[1].pHuffTbl)
	{
		mem_free(entropy->ac_huff_tbls[1].pHuffTbl);
	}
	entropy->ac_huff_tbls[1].pHuffTbl = (IppiDecodeHuffmanSpec *)mem_malloc(tbl_size, DCACHE_ALIGN_NUM);

	/* Load and init the huffman table, use the default huffman table */
	status = ippiEncodeHuffmanSpecInit_JPEG_8u(
		&bits_dc_luminance[1],
		val_dc_luminance,
		entropy->dc_huff_tbls[0].pHuffTbl);
	if(ippStsNoErr != status)
	{
		return -1;
	}
	
	status = ippiEncodeHuffmanSpecInit_JPEG_8u(
		&bits_ac_luminance[1],
		val_ac_luminance,
		entropy->ac_huff_tbls[0].pHuffTbl);
	if(ippStsNoErr != status)
	{
		return -1;
	}
	
	status = ippiEncodeHuffmanSpecInit_JPEG_8u(
		&bits_dc_chrominance[1],
		val_dc_chrominance,
		entropy->dc_huff_tbls[1].pHuffTbl);
	if(ippStsNoErr != status)
	{
		return -1;
	}
	
	status = ippiEncodeHuffmanSpecInit_JPEG_8u(
		&bits_ac_chrominance[1],
		val_ac_chrominance,
		entropy->ac_huff_tbls[1].pHuffTbl);
	if(ippStsNoErr != status)
	{
		return -1;
	}
	
	return 0;
}

#else //IPP_SUPPORT

INT32 Init_QuantTab(JPEG_QUANT_TABLE* quant_tbl)
{
	INT32	i, j, temp;
	UINT16	*quant, *invquant;
	UINT8	*rawdata;	

	static const INT16 aanscales[JPEG_BLOCK_SIZE] = {
		/* precomputed values scaled up by 14 bits */
		16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
		22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
		21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
		19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
		16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
		12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
		 8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
		 4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
	};
	
	for(i = 0; i < NUM_BLOCK_TBLS; i++)
	{
		rawdata = quant_tbl->Quant_tbl_ptrs[i]->quantrawval;
		quant = quant_tbl->Quant_tbl_ptrs[i]->quantval;
		invquant = quant_tbl->Quant_tbl_ptrs[i]->invquantval;

		for(j = 0; j < JPEG_BLOCK_SIZE; j++)
		{
			temp = (((UINT32)rawdata[j] * (UINT32) aanscales[j]) >> (CONST_BITS-3));
			if(!temp) temp = 1;
			quant[j] = (UINT16) temp;
			invquant[j] =(UINT16)( 0x10000 / temp);
		}
	}

	return 0;
}

/* isn't this function nicer than the one in the libjpeg */
static void build_huffman_codes(const UINT8 *bits_table, const UINT8 *val_table,
								UINT8 *huff_size, UINT16 *huff_code)
{
    INT32 i, j, k, nb, code, sym;
	
    code = 0;
    k = 0;
    for(i = 1; i <= 16; i++) 
	{
        nb = bits_table[i];
        for(j = 0; j < nb; j++) 
		{
            sym = val_table[k++];
            huff_size[sym] = (UINT8)i;
            huff_code[sym] = (UINT16)code;
            code++;
        }
        code <<= 1;
    }
}

INT32 Init_HuffTab(JPEG_HUFF_ENTROPY* entropy)
{
	/* Load and init the huffman table, use the default huffman table */
	build_huffman_codes(bits_dc_luminance,
		val_dc_luminance,
		entropy->dc_huff_tbls[0].hufsize,
		entropy->dc_huff_tbls[0].hufcode);	
	
	build_huffman_codes(
		bits_ac_luminance,
		val_ac_luminance,
		entropy->ac_huff_tbls[0].hufsize,
		entropy->ac_huff_tbls[0].hufcode);
	
	build_huffman_codes(
		bits_dc_chrominance,
		val_dc_chrominance,
		entropy->dc_huff_tbls[1].hufsize,
		entropy->dc_huff_tbls[1].hufcode);	
	
	build_huffman_codes(
		bits_ac_chrominance,
		val_ac_chrominance,
		entropy->ac_huff_tbls[1].hufsize,
		entropy->ac_huff_tbls[1].hufcode);	
	
	return 0;
}

#endif //IPP_SUPPORT