#include "jcommon.h"
#include "bitstream.h"
#include "mem_align.h"
#include "jdecoder.h"

static INT32 read_appN(JPEG_STREAM* bitstream)
{
	UINT32  readlen;
	INT32	len;
	
	GET2BYTE(bitstream, len);
	
	if(bitstream->free_in_buffer < len - 2)
	{
		FILLSBUFFER2(bitstream);
	}
	
	bitstream->next_output_byte += len - 2;
	bitstream->free_in_buffer  -= len - 2;
	
	return len;
} /* read_app() */

static INT32 read_dri(JPEG_STREAM* bitstream, UINT32* restartval)
{
	INT32 len;
	UINT32  readlen;
	
	GET2BYTE(bitstream, len);
	
	if (len != 4)
		return -1;
	
	GET2BYTE(bitstream, *restartval);
	
	return len;
}


static INT32 read_dqt(Jpeg_DecOBJ* decoder,
					  JPEG_STREAM* bitstream,
					  JPEG_QUANT_TABLE* quant_tbl)
{
	UINT32	i;
	UINT32	len;
	UINT32	byte;
	UINT32	index;
	UINT32	prec;
	UINT32  readlen;
	UINT8*	quantraw8;	
	INT32	marklen;
	JPEG_QUANT_TBL* qtbl;
	
	GET2BYTE(bitstream, len);
	marklen = len;

	len -= 2;
	while (len >= 65) 
	{
		GETBYTE(bitstream, index);
		prec = (index) >> 4;
		index &= 0x0F;

		if(index >= NUM_BLOCK_TBLS)
			break;
		
		qtbl = quant_tbl->Quant_tbl_ptrs[index];
		if(qtbl == NULL)
		{
			if(decoder->memoryOperator.Alloc)
			{
				VO_MEM_INFO MemInfo;

				MemInfo.Flag = 0;
				MemInfo.Size = sizeof(JPEG_QUANT_TBL);
#ifdef VOJPEGFLAGE
				decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
#else
				decoder->memoryOperator.Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
#endif // VOJPEGFLAGE
				
				qtbl = (JPEG_QUANT_TBL *)MemInfo.VBuffer;
			} 
			else
			{
				qtbl = (JPEG_QUANT_TBL *)mem_malloc(sizeof(JPEG_QUANT_TBL), DCACHE_ALIGN_NUM);
			}
			
			if(qtbl == NULL)
				return -1;
		}
		
		quantraw8 = (INT8*)SAMPLE_ALIGN8(qtbl->quantrawval);

		for (i = 0; i < 64; i++)
		{
			if(prec)
			{
				GET2BYTE(bitstream, byte);
			}
			else
			{
				GETBYTE(bitstream, byte);
			}
			
			quantraw8[i] = (UINT8)byte;
		}
		
		quant_tbl->Quant_tbl_ptrs[index] = qtbl;

		len -= 65;
	}

	for( ; len > 0; len--)
	{
		GETBYTE(bitstream, byte);
	}
	
	return marklen;
} /* read_dqt() */


static INT32 read_dht(Jpeg_DecOBJ* decoder,
					  JPEG_STREAM* bitstream,
					  JPEG_HUFF_TABLE* huff_tbl)
{
	UINT32	i;
	UINT32	len;
	UINT32	byte;
	UINT32	index;
	UINT32  readlen;
	INT32	marklen;
	JPEG_HUFF_TBL* htbl = NULL;
	
	GET2BYTE(bitstream, len);
	marklen = len;
	
	len -= 2;	
	while(len >= 17)
	{
		GETBYTE(bitstream, index);
		
		if (index & 0x10)		/* AC table definition */
		{
			index -= 0x10;

			if(index >= NUM_HUFF_TBLS)
				break;
			
			htbl = huff_tbl->ac_huff_tbl_ptrs[index];
			
			if(htbl == NULL)
			{
				if(decoder->memoryOperator.Alloc)
				{
					VO_MEM_INFO MemInfo;

					MemInfo.Flag = 0;
					MemInfo.Size = sizeof(JPEG_HUFF_TBL);
#ifdef VOJPEGFLAGE
					decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
#else
					decoder->memoryOperator.Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
#endif // VOJPEGFLAGE
					
					htbl = (JPEG_HUFF_TBL *)MemInfo.VBuffer;
				} 
				else
				{
					htbl = (JPEG_HUFF_TBL *)mem_malloc(sizeof(JPEG_HUFF_TBL), ALIGN_NUM_8);
				}
				
				if(htbl == NULL)
					return -1;
			}
			
			huff_tbl->ac_huff_tbl_ptrs[index] = htbl;
		}
		else					/* DC table definition */
		{	
			if(index >= NUM_HUFF_TBLS)
				break;

			htbl = huff_tbl->dc_huff_tbl_ptrs[index];

			if(htbl == NULL)
			{
				if(decoder->memoryOperator.Alloc)
				{
					VO_MEM_INFO MemInfo;

					MemInfo.Flag = 0;
					MemInfo.Size = sizeof(JPEG_HUFF_TBL);
#ifdef VOJPEGFLAGE
					decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
#else
					decoder->memoryOperator.Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
#endif // VOJPEGFLAGE
					
					htbl = (JPEG_HUFF_TBL *)MemInfo.VBuffer;
				} 
				else
				{
					htbl = (JPEG_HUFF_TBL *)mem_malloc(sizeof(JPEG_HUFF_TBL), ALIGN_NUM_8);
					
				}
				
				if(htbl == NULL)
					return -1;
			}
			
			huff_tbl->dc_huff_tbl_ptrs[index] = htbl;
		}
		
		index = 0;
		for(i = 1; i <= 16; i++)
		{
			GETBYTE(bitstream, byte);
			htbl->bits[i] = (UINT8)byte;
			index += byte;
		}

		len -= 17;
        if (len < index || index > 256)
			break;
		
		for(i = 0; i < index; i++)
		{
			GETBYTE(bitstream, byte); 
			htbl->huffval[i] = (UINT8)byte;
		}
		
		len -= index;
	}

	for( ; len > 0; len--)
	{
		GETBYTE(bitstream, byte);
	}
	
	return marklen;
} /* read_dht() */


static INT32 read_sof(JPEG_STREAM*	bitstream,
					  JPEG_PARA *	jpara)
{
	UINT32	i;
	UINT32	len;
	UINT32  readlen;
	INT32	marklen;
	UINT32	byte[3];

	byte[0] = byte[1] = byte[2] = 0;
		
	GET2BYTE(bitstream, len);	marklen = len;
	GETBYTE(bitstream, jpara->data_precision);	
	GET2BYTE(bitstream, jpara->heigth);
	GET2BYTE(bitstream, jpara->width);
	GETBYTE(bitstream, jpara->num_component);

	if(jpara->num_component != 1 && jpara->num_component != 3 )
			return -2;
	
	for(i = 0; i < jpara->num_component; i++)
	{
		GETBYTE(bitstream, jpara->compon[i].com_index);		
		GETBYTE(bitstream, byte[i]);

		jpara->compon[i].index = i;
		jpara->compon[i].h_sample = (byte[i] >> 4) & 0x0F;
		jpara->compon[i].v_sample = byte[i] & 0x0F;
		
		GETBYTE(bitstream, jpara->compon[i].quant_tbl_no);		
	}

	if(jpara->num_component == 1)
	{
		jpara->compon[0].h_sample = 1;
		jpara->compon[0].v_sample = 1;
		byte[0] = 0x11;

	}
	
	jpara->image_type = (byte[2] << 16)	| (byte[1] << 8) | byte[0];

	if(jpara->compon[0].com_index == 82 && jpara->compon[1].com_index == 71 
		&& jpara->compon[2].com_index == 66)
		jpara->image_type = 0x11111111;

	return marklen;
} /* read_sof0() */


static INT32 read_sos(JPEG_STREAM* bitstream, JPEG_PARA * jpara)
{
	UINT32	i, ci;
	UINT32	len;
	UINT32  readlen;
	UINT32	index;
	UINT32	byte;
	INT32	marklen;
	JPEG_COMP_ITEM* comptr;
	
	GET2BYTE(bitstream, len);
	marklen = len;
	GETBYTE(bitstream, jpara->Ns);
	
	for (i = 0; i < jpara->Ns; i++) 
	{
		GETBYTE(bitstream, index);
		GETBYTE(bitstream, byte);
		
		for (ci = 0, comptr = jpara->compon; ci < jpara->num_component;	
			 ci++, comptr++) {
			if (index == comptr->com_index)
				break;
		}

		jpara->cur_com_ptr[i] = comptr;
		comptr->dc_tbl_no = (byte >> 4) & 15;
		comptr->ac_tbl_no = (byte     ) & 15;
	}
	
	/* Collect the additional scan parameters Ss, Se, Ah/Al. */
	GETBYTE(bitstream, jpara->Ss);
	GETBYTE(bitstream, jpara->Se);
	GETBYTE(bitstream, byte);
	jpara->Ah = (byte >> 4) & 0x0F;
	jpara->Al = (byte     ) & 0x0F;

	return marklen;
} /* read_sos() */

static INT32 vo_memcmp(const UINT8* Des, const UINT8* Src, INT32 n)
{
	INT32 a;
	INT32 i;

	for(i = 0; i < n; i++)
	{
		a = Des[i] - Src[i];

		if(a < 0)
		{
			return -1;
		}

		if (a > 0)
		{
			return 1;
		}

	}

	return 0;

}

INT32	find_exifMark(JPEG_STREAM* bitstream)
{
	static UINT8 ExifHeader[] = "Exif\0\0";
	INT32 Marker;

	if(synic_header(bitstream) < 0)
		return -1;

	do {
		if(find_nextMark(bitstream, &Marker) < 0)
			return -1;
	} while(Marker != M_APP1 && Marker != M_SOS);
	
	//if(Marker != M_APP1 && memcmp(bitstream->next_output_byte + 2, ExifHeader, 6))
	//{
	//	return -1;
	//}

	if(Marker != M_APP1 && vo_memcmp(bitstream->next_output_byte + 2, ExifHeader, 6))
	{
		return -1;
	}

	return 0;	
}

INT32 read_marker(Jpeg_DecOBJ *decoder,
				  JPEG_STREAM* bitstream,
				  JPEG_PARA* jpara,
				  UINT32 Marker)
{
	INT32 readlen;
	INT32 totallen;

	totallen = 0;

	for( ; ; )
	{
		if(Marker == 0)
		{
			readlen = find_nextMark(bitstream, &Marker);
			if(readlen < 0)
				return -1;
			totallen += readlen;
		}

		switch(Marker)
		{
		case M_SOI:
			break;
		case M_SOF0:		/* Baseline */
		case M_SOF1:		/* Extended sequential, Huffman */
			jpara->progressMode = 0;
			readlen = read_sof(bitstream, jpara);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			break;
			
		case M_SOF2:		/* Progressive, Huffman, now not support */
			jpara->progressMode = 1;
			readlen = read_sof(bitstream, jpara);
			if (readlen < 0)
				return -1;
			totallen += readlen;
			break;
			
		/* Currently unsupported SOFn types */
		case M_SOF3:		/* Lossless, Huffman */
		case M_SOF5:		/* Differential sequential, Huffman */
		case M_SOF6:		/* Differential progressive, Huffman */
		case M_SOF7:		/* Differential lossless, Huffman */
		case M_SOF9:		/* Extended sequential, arithmetic */
		case M_SOF10:		/* Progressive, arithmetic */
		case M_JPG:			/* Reserved for JPEG extensions */
		case M_SOF11:		/* Lossless, arithmetic */
		case M_SOF13:		/* Differential sequential, arithmetic */
		case M_SOF14:		/* Differential progressive, arithmetic */
		case M_SOF15:		/* Differential lossless, arithmetic */
			return -2;
			
		case M_SOS:
			readlen = read_sos(bitstream, jpara);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			return totallen;
			
		case M_EOI:
			return 0;
			
		case M_DAC: /* arithmetic codec, now not support  */
			return -2;
			
		case M_DHT:
			readlen = read_dht(decoder, bitstream, &jpara->huffum_tbl);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			break;
			
		case M_DQT:
			readlen = read_dqt(decoder, bitstream, &jpara->quant_tbl);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			break;
			
		case M_DRI:
			readlen = read_dri(bitstream, &jpara->restart_intval);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			jpara->restarts_to_go = jpara->restart_intval;
			break;

		case M_APP0:	
		case M_APP1:
		case M_APP2:
		case M_APP3:
		case M_APP4:
		case M_APP5:
		case M_APP6:
		case M_APP7:
		case M_APP8:
		case M_APP9:
		case M_APP10:
		case M_APP11:
		case M_APP12:
		case M_APP13:
		case M_APP14:
		case M_APP15:
			readlen = read_appN(bitstream);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			break;

		case M_COM:  // now just skip it
			readlen = read_appN(bitstream);
			if(readlen < 0)
				return -1;
			totallen += readlen;
			break;

		default:
			return -2;
		}

		Marker = 0;		
    }

	return totallen;
}


INT32 read_jpeg_header(Jpeg_DecOBJ *decoder,
					   JPEG_STREAM*	bitstream,
					   JPEG_PARA* jpara)
{
	INT32 Marker;
	INT32 readlen;
	INT32 totallen;

	totallen = 0;
	readlen = synic_header(bitstream);
	if(readlen < 0)
		return -1;
	totallen += readlen;

	Marker = 0;
	readlen = read_marker(decoder, bitstream, jpara, Marker);
	if(readlen < 0) return readlen;
	totallen += readlen;

	return totallen;
}

INT32 synic_header(JPEG_STREAM* bitstream)
{
	INT32	byte, byte_mk;
	UINT32  readlen = 0;
		
	GETBYTE(bitstream, byte);
	readlen++;
	byte_mk = byte;
	do
	{
		byte = byte_mk;
		while (byte != 0xFF)
		{
			GETBYTE(bitstream, byte); 
			if(readlen++ > JPEG_BUFFER_LENGTH)
				return -1;
		}
		
		GETBYTE(bitstream, byte_mk);
		readlen++;
	}while(byte != 0xFF || byte_mk != (INT32) M_SOI);
		
    if(byte != 0xFF || byte_mk != (INT32) M_SOI)
		return -1;	

	return readlen;
}

INT32 find_nextMark(JPEG_STREAM* bitstream, INT32* Marker)
{
	UINT32 byte;
	UINT32  readlen = 0;
	
	for ( ; ; ) 
	{
		GETBYTE(bitstream, byte);
		readlen++;
		
		while(byte != 0xFF) 
		{
			GETBYTE(bitstream, byte);
			readlen++;
		}
		
		do 	{
			GETBYTE(bitstream, byte);
			readlen++;
		} while (byte == 0xFF);

		if (byte)
			break;			
	}

	*Marker = byte;	
	
	return readlen;
}