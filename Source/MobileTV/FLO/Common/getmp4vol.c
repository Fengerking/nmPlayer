#include "getmp4vol.h"

#define VOL_STC_MSK						0x0f
#define VOL_STC							0x120
#define ASPECT_CUSTOM					15

void initbits(bitstream* bs,const unsigned char *ptr, int len)
{
	bs->bits = 0;
	bs->bitpos = 32;
	bs->bitptr = ptr;
	bs->bitend = ptr+len+6; //adding more just to be safe
}

void loadbits( bitstream * bs )
{
	int bitpos = bs->bitpos;
	int bits = bs->bits;

	if (bitpos >= 8) {							
		const unsigned char * bitptr = bs->bitptr;	
		do {									
			bits = (bits << 8) | *bitptr++;		
			bitpos -= 8;						
		} while (bitpos >= 8);					
		bs->bits = bits;						
		bs->bitptr = bitptr;					
	}											

	bs->bitpos = bitpos;
}

static int gethdr_vol(bitstream *pbs, int * width, int *height)
{
	int aspect,result;
	int visual_object_layer_verid;
	int is_object_layer_identifier;
	int time_increment_resolution, time_increment_bits;


	flushbits(pbs,32+1+8); // start_code + random_accessible_vol + video_object_type_indication
	loadbits(pbs);
	 
	is_object_layer_identifier = getbits(pbs,1);

	if (is_object_layer_identifier) {
		visual_object_layer_verid = getbits(pbs,4);
		flushbits(pbs,3); // video_object_layer_priority
	} 
	else {
		visual_object_layer_verid = 1;
	}

	aspect = getbits(pbs,4); // aspect ratio
	if (aspect == ASPECT_CUSTOM)
	{
		loadbits(pbs);
		getbits(pbs,8); //aspect_width
		getbits(pbs,8); //aspect_height
	}

	if (getbits(pbs,1)) // vol control parameters
	{
		flushbits(pbs,2); // chroma_format
		if (!getbits(pbs,1)) // b-frames
			;
		//TBD
		//	CodecIDCTSetCount(&dec->Codec,3);
		if (getbits(pbs, 1)) // vbv parameters
			flushbits(pbs,15+1+15+1+15+1+3+11+1+15+1);
	}

	flushbits(pbs,2+1); // shape + marker
	loadbits(pbs);

	time_increment_resolution = getbits(pbs,16);

	time_increment_bits = _log2(time_increment_resolution-1);

	flushbits(pbs,1); // marker
	if (getbits(pbs, 1)) //fixed_vop_rate
		flushbits(pbs,time_increment_bits);

	flushbits(pbs,1); // marker
	loadbits(pbs);
	*width = getbits(pbs,13); //width
	flushbits(pbs,1);
	loadbits(pbs);
	*height = getbits(pbs,13); //height
	flushbits(pbs,1); // marker
	return 0;
}

int GetMpeg4SequenceHr(const unsigned char *ptr, int len, int * width, int *height)
{
	int code, result;
	bitstream bs, *pbs;

	pbs = &bs;

	initbits(pbs,ptr,len);

	for (;;)
	{
		pbs->bitpos = (pbs->bitpos + 7) & ~7;

		loadbits(pbs);
		if (pbs->bitptr >= pbs->bitend)
			break;
	
		code = showbits(pbs, 32);

		if ((code & ~VOL_STC_MSK) == VOL_STC){
			result = gethdr_vol(pbs, width, height);
			 if(!result)
				return 0;
		}
		else
			flushbits(pbs,8);
	}
	return -1;
}
