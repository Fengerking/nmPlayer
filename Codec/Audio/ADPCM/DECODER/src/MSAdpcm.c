#include "Frame.h"

static int AdaptationTable [] = {	
	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230
};

static int AdaptCoeff1 [7] =
{	
	256, 512, 0, 192, 240, 460, 392
};

static int AdaptCoeff2 [7] =
{	
	0, -256, 0, 64, 0, -208, -232
};


MSADPCMInfo *voADPCMDecInitMSInfo(VO_MEM_OPERATOR *pMemOP, int blockalign, int framesperblock)
{
	MSADPCMInfo *MSInfo; 
	MSInfo = (MSADPCMInfo *)voADPCMDecmem_malloc(pMemOP, sizeof(MSADPCMInfo), 32);
	if(MSInfo == NULL)
		return NULL;

	return MSInfo;
}

int voADPCMDecMSADPCMDec(ADPCMDecInfo *decoder, short *samples)
{
	int		chan, k, blockindx, sampleindx;
	short	bytecode, bpred[2], chan_idelta[2];
	unsigned char* buf;
	int channels;
    int predict;
    int current;
    int idelta;

	buf = decoder->stream->this_frame;
	channels = decoder->channel;

	if (channels == 1)
	{	
		bpred [0] = buf[0];

		chan_idelta [0] = buf[1] | (buf[2] << 8);
		chan_idelta [1] = 0;

		samples[1] = buf[3] | (buf[4] << 8);
		samples[0] = buf[5] | (buf[6] << 8);
		blockindx = 7;
	}
	else
	{	
		bpred[0] = buf[0] ;
		bpred[1] = buf[1] ;

		chan_idelta[0] = buf[2] | (buf[3] << 8) ;
		chan_idelta[1] = buf[4] | (buf[5] << 8) ;

		samples[2] = buf[6] | (buf[7] << 8) ;
		samples[3] = buf[8] | (buf[9] << 8) ;

		samples[0] = buf[10] | (buf[11] << 8) ;
		samples[1] = buf[12] | (buf[13] << 8) ;

		blockindx = 14 ;
	}

	sampleindx = 2 * channels ;
	while (blockindx < decoder->blockalign)
	{	
		bytecode = buf[blockindx++] ;
  		samples[sampleindx++] = (bytecode >> 4) & 0x0F ;
		samples[sampleindx++] = bytecode & 0x0F ;
	}

	for (k = 2 * channels ; k < (decoder->framesperblock * channels) ; k ++)
	{	
		chan = (channels > 1) ? (k & 1) : 0 ;

		bytecode = samples[k] & 0xF ;

		idelta = chan_idelta[chan] ;
		chan_idelta [chan] = (AdaptationTable [bytecode] * idelta) >> 8;	
		if (chan_idelta [chan] < 16)
			chan_idelta [chan] = 16 ;
		if (bytecode & 0x8)
			bytecode -= 0x10 ;

    	predict = ((samples[k - channels] * AdaptCoeff1[bpred[chan]])
					+ (samples[k - 2 * channels] * AdaptCoeff2[bpred[chan]])) >> 8; 
		current = (bytecode * idelta) + predict ;

		if (current > 32767)
			current = 32767;
		else if (current < -32768)
			current = -32768;

		samples[k] = current;
	}

	return 0;
}