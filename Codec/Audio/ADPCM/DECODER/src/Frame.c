#include "voADPCM.h"
#include "Frame.h"

static int SampTable[4] = {
	5512, 11025, 22050, 44100
};

int voADPCMDecReadFMTChunk(ADPCMDecInfo *decoder, unsigned char* buf, int length)
{
	MSADPCMInfo *MSInfo;
	IMAADPCMInfo *IMAInfo;
	ITU_G726Info *G726Info;
	VO_MEM_OPERATOR *pMemOP;
	int i;
	int SampIndx;

	decoder->frametype = GET2BYTE(buf); 
	buf += 2;	
	decoder->channel = GET2BYTE(buf); 
	buf += 2;
	decoder->samplerate = GET4BYTE(buf); 
	buf +=4;
	decoder->avgbytespersec = GET4BYTE(buf); 
	buf += 4;
	decoder->blockalign = GET2BYTE(buf);
	buf += 2;
	decoder->BitsPerSample = GET2BYTE(buf);
	buf += 2;

	pMemOP = decoder->pvoMemop;

	switch(decoder->frametype) {
	case WAVE_FORMAT_MS_ADPCM:
		if(length >= 18)
		{
			decoder->extrabytes = GET2BYTE(buf);
			buf += 2;
		}
		if(length >= 20)
		{
			int lenblock;
			decoder->framesperblock = GET2BYTE(buf);
			buf += 2;
		
			if(decoder->channel)
			{
				lenblock = 2 + 2 * (decoder->blockalign - 7 * decoder->channel) / decoder->channel;

				if(decoder->framesperblock != lenblock)
					decoder->framesperblock = lenblock;
			}
		}
		else
		{
			if(decoder->channel)
			decoder->framesperblock = 2 + 2 * (decoder->blockalign - 7 * decoder->channel) / decoder->channel;
		}

		MSInfo = voADPCMDecInitMSInfo(pMemOP, decoder->blockalign, decoder->framesperblock);
		if(MSInfo == NULL)
			return VO_ERR_OUTOF_MEMORY;
		
		if(length >= 22)
		{
			MSInfo->numcoeffs = GET2BYTE(buf);
			buf += 2;
		}

		for(i = 0; i < MSInfo->numcoeffs; i++)
		{
			if(length >= 22 + 4 + 4*i) {
				MSInfo->coeffs->coeff1 = GET2BYTE(buf);
				buf += 2;
				MSInfo->coeffs->coeff2 = GET2BYTE(buf);
				buf += 2;
			}
		}	

		if(decoder->BitsPerSample == 0)
			decoder->BitsPerSample = 4;
		else if(decoder->BitsPerSample != 4)
			return VO_ERR_NOT_IMPLEMENT;

		decoder->adpcm = (void *)MSInfo;
		break;
	case WAVE_FORMAT_IMA_ADPCM:
		if(length >= 18)
		{
			decoder->extrabytes = GET2BYTE(buf);
			buf += 2;
		}
	
		if(length >= 20)
		{
			int lenblock;
			decoder->framesperblock = GET2BYTE(buf);
			buf += 2;

			if(decoder->channel)
			{
				lenblock = (decoder->blockalign - 4 * decoder->channel) * 8 / (4 * decoder->channel) + 1;

				if(decoder->framesperblock != lenblock)
					decoder->framesperblock = lenblock;
			}
		}
		else
		{
			if(decoder->channel)
			{
				decoder->framesperblock = (decoder->blockalign - 4 * decoder->channel) * 8 / (4 * decoder->channel) + 1;
			}
		}

		if(decoder->BitsPerSample != 4)
			return VO_ERR_NOT_IMPLEMENT;
		
		IMAInfo = voADPCMDecInitIMAInfo(pMemOP, decoder->blockalign, decoder->framesperblock);
		if(IMAInfo == NULL)
			return VO_ERR_OUTOF_MEMORY;
		decoder->adpcm = (void *)IMAInfo;
		break;
	case WAVE_FORMAT_ALAW:
		if(length >= 18)
		{
			decoder->extrabytes = GET2BYTE(buf);
			buf += 2;
		}
		
		if(decoder->channel != decoder->blockalign || decoder->BitsPerSample != 8)
			return VO_ERR_WRONG_STATUS;
		break;
	case WAVE_FORMAT_MULAW:
		if(length >= 18)
		{
			decoder->extrabytes = GET2BYTE(buf);
			buf += 2;
		}

		if(decoder->channel != decoder->blockalign || decoder->BitsPerSample != 8)
			return VO_ERR_WRONG_STATUS;
		break;
	case WAVE_FORMAT_ITU_G726:
		if(length >= 18)
		{
			decoder->extrabytes = GET2BYTE(buf);
			buf += 2;
		}

		if(decoder->BitsPerSample != 2 && decoder->BitsPerSample != 3 &&
			decoder->BitsPerSample != 4 && decoder->BitsPerSample != 5)
			return VO_ERR_WRONG_STATUS;

		if(decoder->samplerate != 8000)
			return VO_ERR_WRONG_STATUS;

		if(decoder->channel != 1)
			return VO_ERR_WRONG_STATUS;

		if(decoder->avgbytespersec/1000 != decoder->BitsPerSample)
			decoder->avgbytespersec = decoder->BitsPerSample * 1000;

		G726Info = voADPCMDecInitITUG726Info(pMemOP, decoder->avgbytespersec);
		if(G726Info == NULL)
			return VO_ERR_OUTOF_MEMORY;
		decoder->adpcm = (void *)G726Info;

		G726Info->code_size = decoder->BitsPerSample;

		break;
	case WAVW_FORMAT_SWF_ADPCM:
		if (length > 18)
		{
			buf += 2;
			/* 1 for SWF ADPCM */
			decoder->frametype = (buf[0] >> 4) & 0x0F;
			if (decoder->frametype == 1)
			{
				decoder->frametype = 0;
			}
			else if (decoder->frametype == 7)
			{
				decoder->frametype = 6;
			}
			else if (decoder->frametype == 8)
			{
				decoder->frametype = 7;
			}

			SampIndx = (buf[0] >> 2) & 0x3;
			decoder->samplerate = SampTable[SampIndx]; 
			decoder->channel = (buf[0] & 0x1) + 1;     
			decoder->framesperblock = 512 * (decoder->samplerate/11025);
			decoder->blockalign = decoder->framesperblock/2;
		}

		break;
	default:
		return VO_ERR_NOT_IMPLEMENT;			
	}

	return 0;
}


int voADPCMDecADPCM(ADPCMDecInfo *decoder, short *buf)
{
	MSADPCMInfo *MSInfo;
	IMAADPCMInfo *IMAInfo;
	ITU_G726Info *ITUG726Info;
	FrameStream		*stream;
	int ret;

	stream = decoder->stream;
	ret = VO_ERR_FAILED;
	
	switch(decoder->frametype) {
	case WAVE_FORMAT_MS_ADPCM:
		MSInfo = (MSADPCMInfo *)decoder->adpcm;
		if(decoder->BitsPerSample != 4)
		{
			stream->buffer += stream->length;		
			stream->length = 0;
			return VO_ERR_NOT_IMPLEMENT;
		}
		ret = voADPCMDecMSADPCMDec(decoder, buf);
		stream->this_frame += decoder->blockalign;
		stream->length -= decoder->blockalign;
		break;
	case WAVE_FORMAT_IMA_ADPCM:
		IMAInfo = (IMAADPCMInfo *)decoder->adpcm;
		if(decoder->BitsPerSample != 4)
		{
			stream->buffer += stream->length;		
			stream->length = 0;
			return VO_ERR_NOT_IMPLEMENT;
		}
		ret = voADPCMDecIMAADPCMDec(decoder, buf);
		stream->this_frame += decoder->blockalign;
		stream->length -= decoder->blockalign;
		break;
	case WAVE_FORMAT_ALAW:
		ret = voADPCMDecaLawADPCMDec(decoder, buf);
		stream->this_frame += stream->length;
		stream->length = 0;
		break;
	case WAVE_FORMAT_MULAW:
		ret = voADPCMDecuLawADPCMDec(decoder, buf);
		stream->this_frame += stream->length;
		stream->length = 0;
		break;
	case WAVE_FORMAT_ITU_G726:
		ITUG726Info = (ITU_G726Info *)decoder->adpcm;
		ret = voADPCMDecITUG726ADPCMDec(decoder, buf);
		stream->this_frame += stream->length;
		stream->length = 0;
		break;
	case WAVW_FORMAT_SWF_ADPCM:
		ret = voADPCMDecSWFADPCMDec(decoder, buf);
		stream->this_frame += stream->length;
		stream->length = 0;
		break;
	default:
		return VO_ERR_NOT_IMPLEMENT;	
	}
	
	return ret;
}

