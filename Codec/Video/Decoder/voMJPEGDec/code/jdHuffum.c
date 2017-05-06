#include "jcommon.h"
#include "mem_align.h"
#include "jdecoder.h"
#include "jdHuffum.h"

static void Build_Vlc_Tab(Jpeg_DecOBJ* decoder,
						  VLC_TAB* tab, 
						 JPEG_ENC_HUM_SPEC* huf_spec,
						 INT32 count,
						 INT32 tabbits,
						 INT32 prefix,
						 INT32 prefixbits)
{
	register INT32	i,j,n;
	INT32	data, code, bits;
	UINT16*	tab_val;
	INT32	size = 1 << tabbits;
	INT32	pos = tab->pos;
	tab->pos += size;

	if (tab->pos > tab->size)
	{
		tab->size = (tab->pos + 31) & ~31;
		tab->table = (UINT16*) realloc(tab->table, tab->size*sizeof(UINT16));
	}

	tab_val = tab->table + pos;

	if(decoder->memoryOperator.Set)
	{
#ifdef VOJPEGFLAGE
		decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, tab_val, 0, size*sizeof(UINT16));
#else
		decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG, tab_val, 0, size*sizeof(UINT16));
#endif // VOJPEGFLAGE
		
	} 
	else
	{
		memset(tab_val, 0, size*sizeof(UINT16));
	}
	

	for (i=0; i<count; i++)
	{
        bits = huf_spec->huffsize[i] - prefixbits;
		code = huf_spec->huffcode[i];

        if (bits > 0 && (code >> bits) == prefix) 
		{
            if (bits <= tabbits) 
			{
				data = huf_spec->huffval[i] & 0xFFF;
                j = (code << (tabbits - bits)) & (size - 1);
                n = j + (1 << (tabbits - bits));
                for ( ; j<n; j++) 
					tab_val[j] = (UINT16)((bits << 12) | data);
            } 
			else 
			{
				// subtable
                bits -= tabbits;
                j = (code >> bits) & (size - 1);
				if (!tab_val[j])
					n = bits;
				else
				{
					n = tab_val[j] & 31;
					if (bits > n)
						n = bits;
				}
				
                tab_val[j] = (UINT16)(0xC000 | n);
            }
        }
	}
	
	for (i=0; i<size; ++i)
	{
		if ((tab_val[i] >> 12) >= 12)
		{
			bits = tab_val[i] & 31;
			if (bits > tabbits)
				bits = tabbits;

			j = tab->pos - pos;

			if (j & 7)
			{
				n = 8 - (j & 7);
				tab->pos += n;
				j += n;
			}

			tab_val[i] = (UINT16)(0xC000 | (bits << 10) | (j >> 3));

			Build_Vlc_Tab(decoder, tab, huf_spec, count, bits, (prefix << tabbits)|i, prefixbits+tabbits);

			tab_val = tab->table + pos;
		}
	}
}


INT32 InitHuffTab(Jpeg_DecOBJ* decoder,
				  JPEG_HUFF_VLC		*huff_vlc,
				  JPEG_HUFF_ENTROPY *entropy,
				  JPEG_HUFF_TABLE	*huff_tbl)
{
	INT32 i;

	if(huff_tbl)
	{
		for(i = 0; i < NUM_HUFF_TBLS; i++)
		{
			if(huff_tbl->dc_huff_tbl_ptrs[i])
			{
				if(huff_vlc->dc_vlc_tab[i].table)
				{
					free(huff_vlc->dc_vlc_tab[i].table);
					huff_vlc->dc_vlc_tab[i].table = NULL;
					huff_vlc->dc_vlc_tab[i].pos  = 0;
					huff_vlc->dc_vlc_tab[i].size = 0;
				}			
	
				InitVLCTab(decoder, &huff_vlc->dc_vlc_tab[i], &entropy->dc_huff_tbls[i],
					huff_tbl->dc_huff_tbl_ptrs[i]->bits, huff_tbl->dc_huff_tbl_ptrs[i]->huffval);
			}
			
			if(huff_tbl->ac_huff_tbl_ptrs[i])
			{
				if(huff_vlc->ac_vlc_tab[i].table)
				{
					free(huff_vlc->ac_vlc_tab[i].table);
					huff_vlc->ac_vlc_tab[i].table = NULL;
					huff_vlc->ac_vlc_tab[i].pos  = 0;
					huff_vlc->ac_vlc_tab[i].size = 0;
				}
				
				InitVLCTab(decoder, &huff_vlc->ac_vlc_tab[i], &entropy->ac_huff_tbls[i],
					huff_tbl->ac_huff_tbl_ptrs[i]->bits, huff_tbl->ac_huff_tbl_ptrs[i]->huffval);
			}
		}
	}
	else
	{
		InitVLCTab(decoder, &huff_vlc->dc_vlc_tab[0], &entropy->dc_huff_tbls[0],
			bits_dc_luminance,	 val_dc_luminance);
		InitVLCTab(decoder, &huff_vlc->dc_vlc_tab[1], &entropy->dc_huff_tbls[1],
			bits_dc_chrominance, val_dc_chrominance);
		InitVLCTab(decoder, &huff_vlc->ac_vlc_tab[0], &entropy->ac_huff_tbls[0],
			bits_ac_luminance,	 val_ac_luminance);
		InitVLCTab(decoder, &huff_vlc->ac_vlc_tab[1], &entropy->ac_huff_tbls[1],
			bits_ac_chrominance, val_ac_chrominance);
	}

	return 0;
}

INT32	InitpHuffTab(Jpeg_DecOBJ *decoder)
{
	JPEG_HUFF_VLC		*huff_vlc;
	JPEG_HUFF_ENTROPY	*entropy;
	JPEG_HUFF_TABLE		*huff_tbl;
	JPEG_PARA			*jpara;
	INT32				is_DC_band, bad;
	UINT32				ci, tbl;
	JPEG_COMP_ITEM		*compptr;

	huff_vlc	= &decoder->Huff_vlc;
	entropy		= &decoder->dentropy;
	jpara		= &decoder->jpara;
	huff_tbl	= &jpara->huffum_tbl;
	
	if(jpara->Ss == 0)
		is_DC_band = 1;
	else
		is_DC_band = 0;
	
	bad = 0;
	if (is_DC_band) {
		if (jpara->Se != 0)
			bad = 1;
	} else {
		if (jpara->Ss > jpara->Se || jpara->Se >= JPEG_BLOCK_SIZE)
			bad = 1;
		if (jpara->Ns != 1)
			bad = 1;
	}
	if (jpara->Ah != 0) {
		if (jpara->Al != jpara->Ah-1)
			bad = 1;
	}
	
	if (bad)	return -1;

	if (jpara->Ah == 0) {
		if (is_DC_band)
			decoder->MCU_funct = 0;
		else
			decoder->MCU_funct = 1;
	} else {
		if (is_DC_band)
			decoder->MCU_funct = 2;
		else
			decoder->MCU_funct = 3;
	}
	
	for (ci = 0; ci < jpara->Ns; ci++) 
	{
		compptr = jpara->cur_com_ptr[ci];
		if (is_DC_band) 
		{
			if (jpara->Ah == 0) 
			{	
				tbl = compptr->dc_tbl_no;
				if(huff_tbl->dc_huff_tbl_ptrs[tbl])
				{
					if(huff_vlc->dc_vlc_tab[tbl].table)
					{
						free(huff_vlc->dc_vlc_tab[tbl].table);
						huff_vlc->dc_vlc_tab[tbl].table = NULL;
						huff_vlc->dc_vlc_tab[tbl].pos  = 0;
						huff_vlc->dc_vlc_tab[tbl].size = 0;
					}			
					
					InitVLCTab(decoder, &huff_vlc->dc_vlc_tab[tbl], &entropy->dc_huff_tbls[tbl],
						huff_tbl->dc_huff_tbl_ptrs[tbl]->bits, huff_tbl->dc_huff_tbl_ptrs[tbl]->huffval);
				}				
			}
		} else {
			tbl = compptr->ac_tbl_no;
			if(huff_tbl->ac_huff_tbl_ptrs[tbl])
			{
				if(huff_vlc->ac_vlc_tab[tbl].table)
				{
					free(huff_vlc->ac_vlc_tab[tbl].table);
					huff_vlc->ac_vlc_tab[tbl].table = NULL;
					huff_vlc->ac_vlc_tab[tbl].pos  = 0;
					huff_vlc->ac_vlc_tab[tbl].size = 0;
				}			
				
				InitVLCTab(decoder, &huff_vlc->ac_vlc_tab[tbl], &entropy->ac_huff_tbls[tbl],
					huff_tbl->ac_huff_tbl_ptrs[tbl]->bits, huff_tbl->ac_huff_tbl_ptrs[tbl]->huffval);
			}
		}		
	}

	jpara->EOBRUN = 0;
	jpara->restarts_to_go = jpara->restart_intval;
	
	return 0;
}


INT32 InitVLCTab(Jpeg_DecOBJ* decoder,
				 VLC_TAB *vtab, 
				 JPEG_ENC_HUM_SPEC *huf_spec,
				 const UINT8 *bits,
				 const UINT8 *val)
{
	register UINT32  i,j,k;
	UINT32	n,code;

    code = 0;
    k = 0;
    for(i=1; i<=16; i++) 
	{
        n = bits[i];
        for(j=0; j<n; j++) 
		{
			huf_spec->huffcode[k] = code++;
			huf_spec->huffsize[k] = (UINT8)i;
            huf_spec->huffval[k] = val[k];
			k++;
        }
        code <<= 1;
    }

	Build_Vlc_Tab(decoder, vtab, huf_spec, k, VLC_BITS, 0, 0);

	return 0;
}

INT32	decode_MCU(Jpeg_DecOBJ *decoder)
{
	UINT32	i, n;
	UINT32	index,len;
	UINT32  readlen;
	INT32	tval, tmp_v;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_CACHE		*bcache;
	JPEG_STREAM		*bitstream;
	JPEG_COMP_ITEM	*compptr;
	JPEG_PARA		*jpara;
	JPEG_HUFF_VLC	*huff_vlc;
	JPEG_QUANT_TBL	*qtbl;
	VLC_TAB			*htbl;
	INT16			*block;	
	INT8*			quanraw8;
	UINT8			ch;

	UINT32			cache;
	UINT32			used_bits;
	UINT32			unread_marker;
	UINT8			*next_output_byte;
	INT32			free_in_buffer;

	huff_vlc = &decoder->Huff_vlc;
	jpara = &decoder->jpara;
	bitstream = &decoder->instream;
	bcache = &decoder->b_cache;		
	mcublock = &jpara->MCUBlock;
	
	cache = bcache->cache;
	used_bits = bcache->used_bits;
	unread_marker = bcache->unread_marker;
	next_output_byte = bitstream->next_output_byte;
	free_in_buffer = bitstream->free_in_buffer;
	
	for(i = 0; i < mcublock->blockNum; i++)
	{
		block = mcublock->WorkBlock[i];
		index = mcublock->dcImage[i];
		compptr = jpara->cur_com_ptr[index];
		qtbl = jpara->quant_tbl.Quant_tbl_ptrs[compptr->quant_tbl_no];
		quanraw8 = (INT8*)SAMPLE_ALIGN8(qtbl->quantrawval);		
		htbl = &huff_vlc->dc_vlc_tab[compptr->dc_tbl_no];
		
		GETVLCVALUE(bitstream, index, tval, htbl);
		if(tval)
		{
			BITGETCAVALUE(bitstream, tval, tmp_v);
		}
		index = mcublock->dcImage[i];
		tval = tval * quanraw8[0] + mcublock->DC_pred[index];
		mcublock->DC_pred[index] = (INT16)tval;
		block[0] = (INT16)tval;

		htbl = &huff_vlc->ac_vlc_tab[compptr->ac_tbl_no];
		len = 1;		
		for ( ; ; )
		{
			GETVLCVALUE(bitstream, index, tval, htbl);
			if (!tval)
				break;
			if (tval == 0xF0) 
			{
				len += 16;
				if (len>=64)
					break;
			}
			else
			{
				n = tval >> 4;
				tval &= 15;
				len += n;
				n = len & 63;
				BITGETCAVALUE(bitstream, tval, tmp_v);
				tval *= quanraw8[n];
				n = zig_zag_tab_index[n];
				block[n] = (INT16)tval;
				if (++len>=64)
					break;
			}
		}
	}
	
	--jpara->restarts_to_go;
	if (jpara->restart_intval && !jpara->restarts_to_go) 
	{
		if(unread_marker >= M_RST0 && unread_marker <= M_RST7)
			unread_marker = 0;
		index = 8 - used_bits & 7;
		UPDATACACHE(bitstream, index);
		mcublock->DC_pred[0] = mcublock->DC_pred[1]
			= mcublock->DC_pred[2] = 1024;
		jpara->restarts_to_go = jpara->restart_intval;
	}

	bcache->cache = cache;
	bcache->used_bits = used_bits;
	bcache->unread_marker = unread_marker;
	bitstream->next_output_byte = next_output_byte;
	bitstream->free_in_buffer = free_in_buffer;

	return 0;
}


INT32	decode_MCU_DC_first(Jpeg_DecOBJ *decoder)
{
	UINT32	i;
	UINT32	index, Al;
	UINT32  readlen;
	INT32	tval, tmp_v;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_CACHE		*bcache;
	JPEG_STREAM		*bitstream;
	JPEG_COMP_ITEM	*compptr;
	JPEG_PARA		*jpara;
	JPEG_HUFF_VLC	*huff_vlc;
	VLC_TAB			*htbl;
	INT16			*block;	
	UINT8			ch;

	UINT32			cache;
	UINT32			used_bits;
	UINT32			unread_marker;
	UINT8			*next_output_byte;
	INT32			free_in_buffer;

	huff_vlc = &decoder->Huff_vlc;
	jpara = &decoder->jpara;
	bitstream = &decoder->instream;
	bcache = &decoder->b_cache;		
	mcublock = &jpara->MCUBlock;
	Al = jpara->Al;
	
	cache = bcache->cache;
	used_bits = bcache->used_bits;
	unread_marker = bcache->unread_marker;
	next_output_byte = bitstream->next_output_byte;
	free_in_buffer = bitstream->free_in_buffer;
	
	for(i = 0; i < mcublock->blockNum; i++)
	{
		block = mcublock->WorkBlock[i];
		index = mcublock->dcImage[i];
		compptr = jpara->cur_com_ptr[index];
		htbl = &huff_vlc->dc_vlc_tab[compptr->dc_tbl_no];
		
		GETVLCVALUE(bitstream, index, tval, htbl);
		if(tval)
		{
			BITGETCAVALUE(bitstream, tval, tmp_v);
		}
		index = mcublock->dcImage[i];
		tval = tval + mcublock->DC_pred[index];
		mcublock->DC_pred[index] = (INT16)tval;
		block[0] = (INT16)(tval << Al);		
	}
	
	--jpara->restarts_to_go;
	if (jpara->restart_intval && !jpara->restarts_to_go) 
	{
		if(unread_marker >= M_RST0 && unread_marker <= M_RST7)
			unread_marker = 0;
		index = 8 - used_bits & 7;
		UPDATACACHE(bitstream, index);
		mcublock->DC_pred[0] = mcublock->DC_pred[1]
			= mcublock->DC_pred[2] = 0;
		jpara->restarts_to_go = jpara->restart_intval;
	}

	bcache->cache = cache;
	bcache->used_bits = used_bits;
	bcache->unread_marker = unread_marker;
	bitstream->next_output_byte = next_output_byte;
	bitstream->free_in_buffer = free_in_buffer;

	return 0;
}

INT32	decode_MCU_AC_first(Jpeg_DecOBJ *decoder)
{
	UINT32	len;
	UINT32	index, Al, n;
	UINT32  readlen, EOBRUN;
	INT32	tval, tmp_v;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_CACHE		*bcache;
	JPEG_STREAM		*bitstream;
	JPEG_PARA		*jpara;
	JPEG_HUFF_VLC	*huff_vlc;
	JPEG_COMP_ITEM	*compptr;
	VLC_TAB			*htbl;
	INT16			*block;	
	UINT8			ch;

	UINT32			cache;
	UINT32			used_bits;
	UINT32			unread_marker;
	UINT8			*next_output_byte;
	INT32			free_in_buffer;

	huff_vlc = &decoder->Huff_vlc;
	jpara = &decoder->jpara;
	bitstream = &decoder->instream;
	bcache = &decoder->b_cache;		
	mcublock = &jpara->MCUBlock;
	EOBRUN = jpara->EOBRUN;
	Al = jpara->Al;

	cache = bcache->cache;
	used_bits = bcache->used_bits;
	unread_marker = bcache->unread_marker;
	next_output_byte = bitstream->next_output_byte;
	free_in_buffer = bitstream->free_in_buffer;

	if (EOBRUN > 0)		/* if it's a band of zeroes... */
      EOBRUN--;
	else
	{		
		block = mcublock->WorkBlock[0];
		compptr = jpara->cur_com_ptr[0];
		index = compptr->ac_tbl_no;
		htbl = &huff_vlc->ac_vlc_tab[index];
		
		for (len = jpara->Ss; len <= jpara->Se; len++)
		{
			GETVLCVALUE(bitstream, index, tval, htbl);
			n = tval >> 4;
			tval &= 15;
			if (tval) 
			{
				len += n;
				n = len & 63;
				BITGETCAVALUE(bitstream, tval, tmp_v);
				block[n] = (INT16)(tval << Al);
			}
			else
			{
				if(n == 15) 
				{
					len += 15;
				} 
				else 
				{		
					EOBRUN = 1 << n;
					if (n) {		
						GETCAVALUE(n, tval);
						UPDATACACHE(bitstream, n);
						EOBRUN += tval;
					}
					EOBRUN--;	
					break;
				}
			}
		}
	}

	jpara->EOBRUN = EOBRUN;
	--jpara->restarts_to_go;

	if (jpara->restart_intval && !jpara->restarts_to_go) 
	{
		if(unread_marker >= M_RST0 && unread_marker <= M_RST7)
			unread_marker = 0;
		index = 8 - used_bits & 7;
		UPDATACACHE(bitstream, index);
		jpara->restarts_to_go = jpara->restart_intval;
	}

	bcache->cache = cache;
	bcache->used_bits = used_bits;
	bcache->unread_marker = unread_marker;
	bitstream->next_output_byte = next_output_byte;
	bitstream->free_in_buffer = free_in_buffer;

	return 0;
}

INT32	decode_MCU_DC_refine(Jpeg_DecOBJ *decoder)
{
	UINT32	i;
	UINT32	index;
	UINT32  readlen;
	INT32	tval, tmp_v;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_CACHE		*bcache;
	JPEG_STREAM		*bitstream;
	JPEG_PARA		*jpara;
	INT16			*block;	
	UINT8			ch;

	UINT32			cache;
	UINT32			used_bits;
	UINT32			unread_marker;
	UINT8			*next_output_byte;
	INT32			free_in_buffer;

	jpara = &decoder->jpara;
	bitstream = &decoder->instream;
	bcache = &decoder->b_cache;		
	mcublock = &jpara->MCUBlock;
	tmp_v = 1 << jpara->Al;

	cache = bcache->cache;
	used_bits = bcache->used_bits;
	unread_marker = bcache->unread_marker;
	next_output_byte = bitstream->next_output_byte;
	free_in_buffer = bitstream->free_in_buffer;

	for(i = 0; i < mcublock->blockNum; i++)
	{
		block = mcublock->WorkBlock[i];

		GETCAVALUE(1, tval);
		UPDATACACHE(bitstream, 1);
		
		if(tval)
			block[0] = (INT16)tmp_v;		
	}
	


	--jpara->restarts_to_go;
	if (jpara->restart_intval && !jpara->restarts_to_go) 
	{
		if(unread_marker >= M_RST0 && unread_marker <= M_RST7)
			unread_marker = 0;
		index = 8 - used_bits & 7;
		UPDATACACHE(bitstream, index);
		mcublock->DC_pred[0] = mcublock->DC_pred[1]
			= mcublock->DC_pred[2] = 0;
		jpara->restarts_to_go = jpara->restart_intval;
	}

	bcache->cache = cache;
	bcache->used_bits = used_bits;
	bcache->unread_marker = unread_marker;
	bitstream->next_output_byte = next_output_byte;
	bitstream->free_in_buffer = free_in_buffer;

	return 0;
}

INT32	decode_MCU_AC_refine(Jpeg_DecOBJ *decoder)
{
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_CACHE		*bcache;
	JPEG_STREAM		*bitstream;
	JPEG_PARA		*jpara;
	JPEG_HUFF_VLC	*huff_vlc;
	JPEG_COMP_ITEM	*compptr;
	VLC_TAB			*htbl;
	INT16			*block, *thiscoef;	
	UINT32			len;
	UINT32			index;
	UINT32			readlen, EOBRUN;
	INT32			tval, tmp_v, n;
	INT32			p1, m1;
	UINT8			ch;

	UINT32			cache;
	UINT32			used_bits;
	UINT32			unread_marker;
	UINT8			*next_output_byte;
	INT32			free_in_buffer;

	huff_vlc = &decoder->Huff_vlc;
	jpara = &decoder->jpara;
	bitstream = &decoder->instream;
	bcache = &decoder->b_cache;		
	mcublock = &jpara->MCUBlock;
	EOBRUN = jpara->EOBRUN;

	cache = bcache->cache;
	used_bits = bcache->used_bits;
	unread_marker = bcache->unread_marker;
	next_output_byte = bitstream->next_output_byte;
	free_in_buffer = bitstream->free_in_buffer;

	p1 = 1 << jpara->Al;
	m1 = (-1) << jpara->Al;	
	block = mcublock->WorkBlock[0];
	compptr = jpara->cur_com_ptr[0];
	index = compptr->ac_tbl_no;
	htbl = &huff_vlc->ac_vlc_tab[index];

	len = jpara->Ss;

	if (EOBRUN == 0)		
	{		
		for ( ; len <= jpara->Se; len++)
		{
			GETVLCVALUE(bitstream, index, tval, htbl);
			n = tval >> 4;
			tval &= 15;
			if (tval) 
			{
				if(tval != 1) 
					return -1;
				GETCAVALUE(1, tval);
				UPDATACACHE(bitstream, 1);
				if (tval)
					tval = p1;
				else
					tval = m1;
			}
			else
			{
				if(n != 15) 
				{		
					EOBRUN = 1 << n;
					if (n) {		
						GETCAVALUE(n, tval);
						UPDATACACHE(bitstream, n);
						EOBRUN += tval;
					}
					break;
				}
			}

			do {
				thiscoef = block + len;
				if (*thiscoef != 0) 
				{
					GETCAVALUE(1, tmp_v);
					UPDATACACHE(bitstream, 1);
					if (tmp_v) 
					{
						if ((*thiscoef & p1) == 0) 
						{ 
							if (*thiscoef >= 0)
								*thiscoef += (INT16)p1;
							else
								*thiscoef += (INT16)m1;
						}
					}
				} 
				else 
				{
					if (--n < 0)
						break;	
				}
				len++;
			} while (len <= jpara->Se);
			
			if (tval) 
			{
				thiscoef = block + len;
				*thiscoef = (INT16)tval;
			}
		}		
	}

    if (EOBRUN > 0) 
	{
		for ( ; len <= jpara->Se; len++) 
		{
			thiscoef = block + len;
			if (*thiscoef != 0) 
			{
				GETCAVALUE(1, tmp_v);
				UPDATACACHE(bitstream, 1);
				if (tmp_v) 
				{
					if ((*thiscoef & p1) == 0)
					{ 
						if (*thiscoef >= 0)
							*thiscoef += (INT16)p1;
						else
							*thiscoef += (INT16)m1;
					}
				}
			}
		}
		EOBRUN--;
    }

	jpara->EOBRUN = EOBRUN;
	--jpara->restarts_to_go;

	if (jpara->restart_intval && !jpara->restarts_to_go) 
	{
		if(unread_marker >= M_RST0 && unread_marker <= M_RST7)
			unread_marker = 0;
		index = 8 - used_bits & 7;
		UPDATACACHE(bitstream, index);
		jpara->restarts_to_go = jpara->restart_intval;
	}

	bcache->cache = cache;
	bcache->used_bits = used_bits;
	bcache->unread_marker = unread_marker;
	bitstream->next_output_byte = next_output_byte;
	bitstream->free_in_buffer = free_in_buffer;

	return 0;
}


void FreeHuffTab(JPEG_HUFF_VLC	*huff_vlc)
{
	INT32 i;
	for(i = 0; i < NUM_HUFF_TBLS; i++)
	{
		if(huff_vlc->dc_vlc_tab[i].table)
		{
			free(huff_vlc->dc_vlc_tab[i].table);
			huff_vlc->dc_vlc_tab[i].table = NULL;
		}
		huff_vlc->dc_vlc_tab[i].pos  = 0;
		huff_vlc->dc_vlc_tab[i].size = 0;

		if(huff_vlc->ac_vlc_tab[i].table)
		{
			free(huff_vlc->ac_vlc_tab[i].table);
			huff_vlc->ac_vlc_tab[i].table = NULL;
		}
		huff_vlc->ac_vlc_tab[i].pos  = 0;
		huff_vlc->ac_vlc_tab[i].size = 0;
	}
}

