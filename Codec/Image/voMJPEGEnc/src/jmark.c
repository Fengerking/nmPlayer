#include "jcommon.h"
#include "jencoder.h"
#include "jexif.h"

#if 0
static INT32 write_app0(JPEG_STREAM* bitstream)
{
	UINT16 len;

	len = 2 + 5 + 2 + 1 + 2 + 2 + 1 + 1;
	
	putmarker(bitstream, M_APP0);	
	put2byte(bitstream, len, 0);
	
	/* Write format indicator "JFIF" \0 */
	putbyte (bitstream, 'J');
	putbyte (bitstream, 'F');
	putbyte (bitstream, 'I');
	putbyte (bitstream, 'F');
	
	
	putbyte (bitstream, 0);	
	put2byte(bitstream, 0x0102, 0);		/* Write version number */
	putbyte (bitstream, 0);				/* Write the density units */
	
	/* Write the aspect ratio */
	put2byte(bitstream, 0, 0);		
	put2byte(bitstream, 0, 0);
	
	/* Write the Thumbnail X and Y size */
	putbyte (bitstream, 0);	
	putbyte (bitstream, 0);
	
	return 0;
} /* write_app0() */
#endif


static INT32 write_dqt(JPEG_STREAM* bitstream, 
					   const UINT8* quant_tbl, 
					   UINT32 quant_tbl_index)
{
	UINT16 i;
	UINT16 len;

	len = 67;	
	putmarker(bitstream, M_DQT);
	put2byte(bitstream, 67, 0);	
	putbyte (bitstream, quant_tbl_index);
	
	for(i = 0; i < 64; i++)
	{
		putbyte(bitstream, quant_tbl[zig_zag_tab_index[i]]);
	}
	
	return 0;
} /* write_dqt() */


static INT32 write_dht(JPEG_STREAM*		bitstream, 
					   const UINT8*		huff_bits,
					   const UINT8*		huff_vals,
					   UINT32			huff_class)
{
	UINT16	i;
	UINT16	mt;
	UINT16	len;
	
	putmarker(bitstream, M_DHT);
	
	mt = 0;
	for(i = 1; i <= 16; i++)
	{
		mt += huff_bits[i];
	}
	
	len = 2 + 1 + 16 + mt;
	
	put2byte(bitstream, len, 0);	
	putbyte (bitstream, huff_class);
	
	for(i = 1; i <= 16; i++)
	{
		putbyte(bitstream, huff_bits[i]);
	}
    
	for(i = 0; i < mt; i++)
	{
		putbyte(bitstream, huff_vals[i]);
	}
	
	return 0;
} /* write_dht() */


static INT32 write_sof0(JPEG_STREAM*	bitstream,
						PICTURE_ATT*	picture)
{
	UINT16    i;
	UINT16    len;
	UINT16    tab_index[3];
	
	putmarker(bitstream, M_SOF0);
		
	/* Write the length len = 8 + num_component * 3 = 17 */
	len = 17;
	put2byte(bitstream, len, 0);	
	putbyte (bitstream, 8);	
	put2byte(bitstream, picture->heigth, 0);	
	put2byte(bitstream, picture->width, 0);
	
	putbyte (bitstream, 3);
	
	tab_index[0] = 0;
	tab_index[1] = tab_index[2] = 1;

	for(i = 0; i < 3; i++)
	{
		putbyte(bitstream, i+1);
		
		/* Write the V sampling and H sampling */
		putbyte(bitstream, (picture->h_sample[i]<<4) + (picture->v_sample[i] & 0xff));
		
		/* Write the quant table index */
		putbyte(bitstream, tab_index[i]);
	}
	
	return 0;
} /* write_sof0() */



static INT32 write_sos(JPEG_STREAM*		bitstream)
{
	UINT16 len;
	UINT16 num_component = 3;  
	
	/* Input the SOS marker */
	putmarker(bitstream, M_SOS);
	
	/* Input the length information */
	len = 6 + 2 * num_component;
	
	put2byte(bitstream, len, 0);
	putbyte(bitstream, num_component);
	putbyte(bitstream, 1);
	putbyte(bitstream, 0);
	putbyte(bitstream, 2);
	
	putbyte(bitstream, 0x11);
	putbyte(bitstream, 3);	
	putbyte(bitstream, 0x11);
	putbyte(bitstream, 0);
	putbyte(bitstream, 63);	
	putbyte(bitstream, 0);
	
	return 0;
} /* write_sos() */

INT32 write_jpeg_header(JPEG_ENCOBJ* jpeg_obj)
{
	JPEG_STREAM*			bitstream;
	PICTURE_ATT*			picture;
	JPEG_QUANT_TABLE*		quant_tbl;
	EXIFLibrary*			jExifLibary;

	bitstream = jpeg_obj->J_stream;
	picture = jpeg_obj->picture;
	quant_tbl = jpeg_obj->Quant_tbl;
	jExifLibary = jpeg_obj->jExifLibary;

	/* Write the SOI marker */
	putmarker(bitstream, M_SOI);

#ifndef MJPEG
	if(jpeg_obj->exif_support)
	{
		write_jpeg_exif(jpeg_obj);
		putmarker(bitstream, M_SOI);
	}
#endif

	/* Write the quant table for luminance */
	write_dqt(bitstream, quant_tbl->Quant_tbl_ptrs[0]->quantrawval,0);	
	/* Write the quant table for chrominance */
	write_dqt(bitstream, quant_tbl->Quant_tbl_ptrs[1]->quantrawval, 1);	
	
	/* Write the huffman  table */	
	/* Write the DC lumninance huffman table */
	write_dht(bitstream, bits_dc_luminance, val_dc_luminance, 0);	
	/* Write AC luminance huffman table */
	write_dht(bitstream, bits_ac_luminance, val_ac_luminance, 0x10);	
	/* Write DC Chrominance huffman table */
	write_dht(bitstream, bits_dc_chrominance, val_dc_chrominance, 0x01);	
	/* Write AC Chrominance huffman table */
	write_dht(bitstream, bits_ac_chrominance, val_ac_chrominance, 0x11);

	/* Write SOF infomation */
	write_sof0(bitstream, picture);
	
	/* Put the SOS and the table information into the bitstream */
	write_sos(bitstream);
	
	return 0;
} /* write_jpeg_header() */

INT32 write_jpeg_trailer(JPEG_STREAM*	bitstream)
{
	putmarker(bitstream, M_EOI);
	return 0;
}

