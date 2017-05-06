#include "global.h"
#include "struct.h"
#include "ps_dec.h"

#if SUPPORT_MUL_CHANNEL

#define DM_MUL 5248/16384  //3203/10000
#define RSQRT2 5818/8192	//7071/10000

int DownMixto2Chs(AACDecoder* decoder,int chans,short* outbuf)
{
	int i;
	short* acturalBuf = outbuf;
	int C,L_S,R_S,tmp,tmp1,cum;
	int samples = MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1);
	//Down mix
#ifdef MSORDER
	for(i = 0; i < samples; i++)
	{
		C   = outbuf[2]*RSQRT2;
		L_S = outbuf[4]*RSQRT2;
		cum = outbuf[0] + C + L_S;
		tmp = cum*DM_MUL;
		
		R_S = outbuf[5]*RSQRT2;
		cum = outbuf[1] + C + R_S;
		tmp1 = cum*DM_MUL;
	
		acturalBuf[0] = CLIPTOSHORT(tmp);
		acturalBuf[1] = CLIPTOSHORT(tmp1);
		acturalBuf+=2;
		outbuf+=chans;
	}
#else
	for(i = 0; i < samples; i++)
	{
		C   = outbuf[1]*RSQRT2;
		L_S = outbuf[3]*RSQRT2;
		cum = outbuf[0] + C + L_S;
		tmp = cum*DM_MUL;
		
		R_S = outbuf[4]*RSQRT2;
		cum = outbuf[2] + C + R_S;
		tmp1 = cum*DM_MUL;
	
		acturalBuf[0] = CLIPTOSHORT(tmp);
		acturalBuf[1] = CLIPTOSHORT(tmp1);
		acturalBuf+=2;
		outbuf+=chans;
	}
#endif
	return 1;	
}

int Selectto2Chs(AACDecoder* decoder,int chans,short* outbuf)
{
	int i;
	short* acturalBuf = outbuf;
	//int C,L_S,R_S,tmp,tmp1,cum;
	int samples = MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1);

	short* nextBuf;
	nextBuf = acturalBuf + chans;
	acturalBuf+=2;

#ifdef MSORDER
	for(i = 1; i < samples; i++)
	{
		acturalBuf[0] = nextBuf[0];
		acturalBuf[1] = nextBuf[1];
		acturalBuf+=2;
		nextBuf +=chans;
	}
#else
	for(i = 1; i < samples; i++)
	{
		acturalBuf[0] = nextBuf[0];
		acturalBuf[1] = nextBuf[2];
		acturalBuf+=2;
		nextBuf +=chans;
	}
#endif
	return 1;	
}

#define SIMPLE_S2M 1
int Stereo2Mono(AACDecoder* decoder,short *outbuf,int sampleSize)
{
	short* left  = outbuf;
	short* right = outbuf+1;
	int i;
	for(i=0;i<sampleSize;i++)
	{
		*outbuf++ = (*left+*right)/2;
		left +=2;
		right+=2;
	}

	return 0;
}

int Mono2Stereo(AACDecoder* decoder,short *outbuf,int sampleSize)
{
	short* left  = outbuf;
	short* right = outbuf+1;
	int i;
	for(i=0; i<sampleSize; i++)
	{
		*right = *left;
		left +=2;
		right+=2;
	}

	return 0;
}


int PostChannelProcess(AACDecoder* decoder,short *outbuf,int sampleSize)
{
	short* left  = outbuf;
	short* right = outbuf+1;
	int i;

	if(decoder->chSpec == VO_AUDIO_CHAN_DUALLEFT)
	{
		for(i=0; i<sampleSize; i++)
		{
			*right = *left;
			left +=2;
			right+=2;
		}	
	}
	else if(decoder->chSpec == VO_AUDIO_CHAN_DUALRIGHT)
	{
		for(i=0; i<sampleSize; i++)
		{
			*left = *right;
			left +=2;
			right+=2;
		}
	}

	return 0;
}
#endif//SUPPORT_MUL_CHANNEL
