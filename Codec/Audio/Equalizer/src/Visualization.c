#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "voEqualizer.h"
#define MAX_SAMPLES	  1024

#define NEW_FREQBAND

#ifdef NEW_FREQBAND

#define NUM			2
int FreqSample[1024 >> NUM];
int FreqBand[256];

#if 1

const short COS256[512] = {   //Q14
   16384,    16383,    16379,    16373,    16364,    16353,    16340,    16324, 
   16305,    16284,    16261,    16235,    16207,    16176,    16143,    16107, 
   16069,    16029,    15986,    15941,    15893,    15843,    15791,    15736, 
   15679,    15619,    15557,    15493,    15426,    15357,    15286,    15213, 
   15137,    15059,    14978,    14896,    14811,    14724,    14635,    14543, 
   14449,    14354,    14256,    14155,    14053,    13949,    13842,    13733, 
   13623,    13510,    13395,    13279,    13160,    13039,    12916,    12792, 
   12665,    12537,    12406,    12274,    12140,    12004,    11866,    11727, 
   11585,    11442,    11297,    11151,    11003,    10853,    10702,    10549, 
   10394,    10238,    10080,     9921,     9760,     9598,     9434,     9269, 
    9102,     8935,     8765,     8595,     8423,     8250,     8076,     7900, 
    7723,     7545,     7366,     7186,     7005,     6823,     6639,     6455, 
    6270,     6084,     5897,     5708,     5520,     5330,     5139,     4948, 
    4756,     4563,     4370,     4176,     3981,     3786,     3590,     3393, 
    3196,     2999,     2801,     2603,     2404,     2205,     2006,     1806, 
    1606,     1406,     1205,     1005,      804,      603,      402,      201, 
       0,     -201,     -402,     -603,     -804,    -1005,    -1205,    -1406, 
   -1606,    -1806,    -2006,    -2205,    -2404,    -2603,    -2801,    -2999, 
   -3196,    -3393,    -3590,    -3786,    -3981,    -4176,    -4370,    -4563, 
   -4756,    -4948,    -5139,    -5330,    -5520,    -5708,    -5897,    -6084, 
   -6270,    -6455,    -6639,    -6823,    -7005,    -7186,    -7366,    -7545, 
   -7723,    -7900,    -8076,    -8250,    -8423,    -8595,    -8765,    -8935, 
   -9102,    -9269,    -9434,    -9598,    -9760,    -9921,   -10080,   -10238, 
  -10394,   -10549,   -10702,   -10853,   -11003,   -11151,   -11297,   -11442, 
  -11585,   -11727,   -11866,   -12004,   -12140,   -12274,   -12406,   -12537, 
  -12665,   -12792,   -12916,   -13039,   -13160,   -13279,   -13395,   -13510, 
  -13623,   -13733,   -13842,   -13949,   -14053,   -14155,   -14256,   -14354, 
  -14449,   -14543,   -14635,   -14724,   -14811,   -14896,   -14978,   -15059, 
  -15137,   -15213,   -15286,   -15357,   -15426,   -15493,   -15557,   -15619, 
  -15679,   -15736,   -15791,   -15843,   -15893,   -15941,   -15986,   -16029, 
  -16069,   -16107,   -16143,   -16176,   -16207,   -16235,   -16261,   -16284, 
  -16305,   -16324,   -16340,   -16353,   -16364,   -16373,   -16379,   -16383, 
  -16384,   -16383,   -16379,   -16373,   -16364,   -16353,   -16340,   -16324, 
  -16305,   -16284,   -16261,   -16235,   -16207,   -16176,   -16143,   -16107, 
  -16069,   -16029,   -15986,   -15941,   -15893,   -15843,   -15791,   -15736, 
  -15679,   -15619,   -15557,   -15493,   -15426,   -15357,   -15286,   -15213, 
  -15137,   -15059,   -14978,   -14896,   -14811,   -14724,   -14635,   -14543, 
  -14449,   -14354,   -14256,   -14155,   -14053,   -13949,   -13842,   -13733, 
  -13623,   -13510,   -13395,   -13279,   -13160,   -13039,   -12916,   -12792, 
  -12665,   -12537,   -12406,   -12274,   -12140,   -12004,   -11866,   -11727, 
  -11585,   -11442,   -11297,   -11151,   -11003,   -10853,   -10702,   -10549, 
  -10394,   -10238,   -10080,    -9921,    -9760,    -9598,    -9434,    -9269, 
   -9102,    -8935,    -8765,    -8595,    -8423,    -8250,    -8076,    -7900, 
   -7723,    -7545,    -7366,    -7186,    -7005,    -6823,    -6639,    -6455, 
   -6270,    -6084,    -5897,    -5708,    -5520,    -5330,    -5139,    -4948, 
   -4756,    -4563,    -4370,    -4176,    -3981,    -3786,    -3590,    -3393, 
   -3196,    -2999,    -2801,    -2603,    -2404,    -2205,    -2006,    -1806, 
   -1606,    -1406,    -1205,    -1005,     -804,     -603,     -402,     -201, 
       0,      201,      402,      603,      804,     1005,     1205,     1406, 
    1606,     1806,     2006,     2205,     2404,     2603,     2801,     2999, 
    3196,     3393,     3590,     3786,     3981,     4176,     4370,     4563, 
    4756,     4948,     5139,     5330,     5520,     5708,     5897,     6084, 
    6270,     6455,     6639,     6823,     7005,     7186,     7366,     7545, 
    7723,     7900,     8076,     8250,     8423,     8595,     8765,     8935, 
    9102,     9269,     9434,     9598,     9760,     9921,    10080,    10238, 
   10394,    10549,    10702,    10853,    11003,    11151,    11297,    11442, 
   11585,    11727,    11866,    12004,    12140,    12274,    12406,    12537, 
   12665,    12792,    12916,    13039,    13160,    13279,    13395,    13510, 
   13623,    13733,    13842,    13949,    14053,    14155,    14256,    14354, 
   14449,    14543,    14635,    14724,    14811,    14896,    14978,    15059, 
   15137,    15213,    15286,    15357,    15426,    15493,    15557,    15619, 
   15679,    15736,    15791,    15843,    15893,    15941,    15986,    16029, 
   16069,    16107,    16143,    16176,    16207,    16235,    16261,    16284, 
   16305,    16324,    16340,    16353,    16364,    16373,    16379,    16383
};

VO_S32	voGetWavFreqBand(short* wavFrameData,
					 int	wavFrameSizeInBytes,
					 int	channels,
					 int	sampleRate,
					 int	*bands,
					 int	banlen)
{
	int	i, k, b, up, num;
	int samples, scale, len;
	int result, *nspectrum;

	if(wavFrameData==NULL||wavFrameSizeInBytes<0
		||channels<=0)
		return -1;
	
	if(banlen < 1 || banlen > 256)
		return -1;

	samples = wavFrameSizeInBytes / (channels * sizeof(short));

	up = 0;
	if(sampleRate < 16000){
		num = 0;
		if(samples>MAX_SAMPLES/4)
			samples = MAX_SAMPLES/4;
	}
	else if(sampleRate == 16000)
	{
		num = 1;
		if(samples>MAX_SAMPLES/2)
			samples = MAX_SAMPLES/2;
		if(samples <= MAX_SAMPLES/4)
			up += 1;
	}
	else 
	{
	    num = 2;
		if(samples>MAX_SAMPLES)
			samples = MAX_SAMPLES;
		if(samples <= MAX_SAMPLES/4)
			up += 2;
		if(samples <= MAX_SAMPLES/2 && samples > MAX_SAMPLES/4)
			up += 1;
	}

	samples >>= num;
	len = 1 << num;

	for(b=0; b<samples; b++)
	{
		short* leftData = wavFrameData;
		result = 0;

		for(i=0; i<len; i++)
		{
			result += leftData[(b*len + i) * channels];			
		}

		FreqSample[b] = result >> num;	
	}

	
	num = samples/(banlen*2);
	for(b=num; b<samples/2+num; b++)
	{
		short* leftData = wavFrameData;
		result = 0;

		for(i=0; i<samples; i++)
		{
			k = (b * (2*i + 1) / 2);
			result += (FreqSample[i] * COS256[k & 511] + (1<< 13)) >> 14;			
		}
		
		FreqBand[b] = result < 0 ? -result : result;	
	}

	nspectrum = FreqBand + num;
	scale = 9 - up; 
	for(b=0; b<banlen; b++)
	{
		result = 0;
		for(i = 0; i < num; i++)
		{
			result += *nspectrum;
			nspectrum++;
		}

		result /= num;
		
		bands[b] = (result + (1 << (scale-1))) >> scale;

		if(bands[b] > 100)
		{
			bands[b] = 100;
		}		
	}

	return 0;
}

#else
const short COS64[128] = {   //Q14
   16384,    16364,    16305,    16207,    16069,    15893,    15679,    15426, 
   15137,    14811,    14449,    14053,    13623,    13160,    12665,    12140, 
   11585,    11003,    10394,     9760,     9102,     8423,     7723,     7005, 
    6270,     5520,     4756,     3981,     3196,     2404,     1606,      804, 
       0,     -804,    -1606,    -2404,    -3196,    -3981,    -4756,    -5520, 
   -6270,    -7005,    -7723,    -8423,    -9102,    -9760,   -10394,   -11003, 
  -11585,   -12140,   -12665,   -13160,   -13623,   -14053,   -14449,   -14811, 
  -15137,   -15426,   -15679,   -15893,   -16069,   -16207,   -16305,   -16364, 
  -16384,   -16364,   -16305,   -16207,   -16069,   -15893,   -15679,   -15426, 
  -15137,   -14811,   -14449,   -14053,   -13623,   -13160,   -12665,   -12140, 
  -11585,   -11003,   -10394,    -9760,    -9102,    -8423,    -7723,    -7005, 
   -6270,    -5520,    -4756,    -3981,    -3196,    -2404,    -1606,     -804, 
       0,      804,     1606,     2404,     3196,     3981,     4756,     5520, 
    6270,     7005,     7723,     8423,     9102,     9760,    10394,    11003, 
   11585,    12140,    12665,    13160,    13623,    14053,    14449,    14811, 
   15137,    15426,    15679,    15893,    16069,    16207,    16305,    16364
};

const short FreqBand32000[10]  = {1, 2, 3, 4,  5,  6,  7,  9, 12, 16};
const short FreqBand16000[10]  = {1, 2, 4, 6,  8, 10, 14, 18, 22, 26};
const short FreqBand12000[10]  = {1, 2, 4, 6, 10, 14, 18, 24, 30, 36};

int	voGetWavFreqBand(short* wavFrameData,
					 int	wavFrameSizeInBytes,
					 int	channels,
					 int	sampleRate,
					 int	bands[10])
{
	int	i, k, b, up, num;
	int samples, scale, len;
	const short* FreqBand;

	if(wavFrameData==NULL||wavFrameSizeInBytes<0
		||channels<=0||channels>6
		||sampleRate<8000||sampleRate>48000)
		return -1;
	
	samples = wavFrameSizeInBytes / (channels * sizeof(short));
	
	up = 0;
	if(sampleRate < 16000){
		num = 0;
		if(samples>MAX_SAMPLES/4)
			samples = MAX_SAMPLES/4;
		FreqBand = FreqBand12000; 
	}
	else if(sampleRate == 16000)
	{
		num = 1;
		if(samples>MAX_SAMPLES/2)
			samples = MAX_SAMPLES/2;
		if(samples <= MAX_SAMPLES/4)
			up += 1;
		FreqBand = FreqBand16000; 
	}
	else 
	{
	    num = 2;
		if(samples>MAX_SAMPLES)
			samples = MAX_SAMPLES;
		if(samples <= MAX_SAMPLES/4)
			up += 2;
		if(samples <= MAX_SAMPLES/2 && samples > MAX_SAMPLES/4)
			up += 1;
		FreqBand = FreqBand32000;
	}

	samples >>= num;
	len = 1 << num;

	for(b=0; b<samples; b++)
	{
		int result = 0;
		short* leftData = wavFrameData;

		for(i=0; i<len; i++)
		{
			result += leftData[(b*len + i) * channels];			
		}

		FreqSample[b] = result >> num;	
	}

	
	for(b=0; b<10; b++)
	{
		int result = 0;
		short* leftData = wavFrameData;

		scale = FreqBand[b];

		for(i=0; i<samples; i++)
		{
			k = (scale * (2*i + 1));
			result += (FreqSample[i] * COS64[k & 127] + (1<< 13)) >> 14;			
		}
		
		bands[b] = result < 0 ? -result : result;	
	}

	for(b=0; b<10; b++)
	{
		scale = 10 - up; 
		bands[b] = (bands[b] + (1 << (scale-1))) >> scale;

		if(bands[b] > 100)
		{
			bands[b] = 100;
		}
	}

	return 0;
}
#endif

#else

static const short cos_rel_0[]={
	Q14(1.000000),Q14(0.866025),Q14(0.500000),Q14(0.000000),Q14(-0.500000),Q14(-0.866025),Q14(-1.000000),Q14(-0.866025),Q14(-0.500000),
	Q14(-0.000000),Q14(0.500000),Q14(0.866025),
};//cos_rel_0[12]
static const short cos_rel_1[]={
	Q14(1.000000),Q14(0.500000),Q14(-0.500000),Q14(-1.000000),Q14(-0.500000),Q14(0.500000),Q14(1.000000),Q14(0.500000),Q14(-0.500000),
	Q14(-1.000000),Q14(-0.500000),Q14(0.500000),
};//cos_rel_1[12]
static const short cos_rel_2[]={
	Q14(1.000000),Q14(0.000000),Q14(-1.000000),Q14(-0.000000),Q14(1.000000),Q14(0.000000),Q14(-1.000000),Q14(-0.000000),Q14(1.000000),
	Q14(0.000000),Q14(-1.000000),Q14(-0.000000),
};//cos_rel_2[12]
static const short cos_rel_3[]={
	Q14(1.000000),Q14(-0.500000),Q14(-0.500000),Q14(1.000000),Q14(-0.500000),Q14(-0.500000),Q14(1.000000),Q14(-0.500000),Q14(-0.500000),
	Q14(1.000000),Q14(-0.500000),Q14(-0.500000),
};//cos_rel_3[12]
static const short cos_rel_4[]={
	Q14(1.000000),Q14(-0.866025),Q14(0.500000),Q14(0.000000),Q14(-0.500000),Q14(0.866025),Q14(-1.000000),Q14(0.866025),Q14(-0.500000),
	Q14(0.000000),Q14(0.500000),Q14(-0.866025),
};//cos_rel_4[12]
static const short cos_rel_5[]={
	Q14(1.000000),Q14(-1.000000),Q14(1.000000),Q14(-1.000000),Q14(1.000000),Q14(-1.000000),Q14(1.000000),Q14(-1.000000),Q14(1.000000),
	Q14(-1.000000),Q14(1.000000),Q14(-1.000000),
};//cos_rel_5[12]
static const short cos_rel_6[]={
	Q14(1.000000),Q14(-0.866025),Q14(0.500000),Q14(0.000000),Q14(-0.500000),Q14(0.866025),Q14(-1.000000),Q14(0.866025),Q14(-0.500000),
	Q14(-0.000000),Q14(0.500000),Q14(-0.866025),
};//cos_rel_6[12]
static const short cos_rel_7[]={
	Q14(1.000000),Q14(-0.500000),Q14(-0.500000),Q14(1.000000),Q14(-0.500000),Q14(-0.500000),Q14(1.000000),Q14(-0.500000),Q14(-0.500000),
	Q14(1.000000),Q14(-0.500000),Q14(-0.500000),
};//cos_rel_7[12]
static const short cos_rel_8[]={
	Q14(1.000000),Q14(-0.000000),Q14(-1.000000),Q14(0.000000),Q14(1.000000),Q14(-0.000000),Q14(-1.000000),Q14(-0.000000),Q14(1.000000),
	Q14(-0.000000),Q14(-1.000000),Q14(0.000000),
};//cos_rel_8[12]
static const short cos_rel_9[]={
	Q14(1.000000),Q14(0.500000),Q14(-0.500000),Q14(-1.000000),Q14(-0.500000),Q14(0.500000),Q14(1.000000),Q14(0.500000),Q14(-0.500000),
	Q14(-1.000000),Q14(-0.500000),Q14(0.500000),
};//cos_rel_9[12]
static const short sin_img_0[]={
	Q14(0.000000),Q14(0.500000),Q14(0.866025),Q14(1.000000),Q14(0.866025),Q14(0.500000),Q14(0.000000),Q14(-0.500000),Q14(-0.866025),
	Q14(-1.000000),Q14(-0.866025),Q14(-0.500000),
};//sin_img_0[12]
static const short sin_img_1[]={
	Q14(0.000000),Q14(0.866025),Q14(0.866025),Q14(0.000000),Q14(-0.866025),Q14(-0.866025),Q14(-0.000000),Q14(0.866025),Q14(0.866025),
	Q14(0.000000),Q14(-0.866025),Q14(-0.866025),
};//sin_img_1[12]
static const short sin_img_2[]={
	Q14(0.000000),Q14(1.000000),Q14(0.000000),Q14(-1.000000),Q14(-0.000000),Q14(1.000000),Q14(0.000000),Q14(-1.000000),Q14(-0.000000),
	Q14(1.000000),Q14(0.000000),Q14(-1.000000),
};//sin_img_2[12]
static const short sin_img_3[]={
	Q14(0.000000),Q14(0.866025),Q14(-0.866025),Q14(-0.000000),Q14(0.866025),Q14(-0.866025),Q14(-0.000000),Q14(0.866025),Q14(-0.866025),
	Q14(-0.000000),Q14(0.866025),Q14(-0.866025),
};//sin_img_3[12]
static const short sin_img_4[]={
	Q14(0.000000),Q14(0.500000),Q14(-0.866025),Q14(1.000000),Q14(-0.866025),Q14(0.500000),Q14(0.000000),Q14(-0.500000),Q14(0.866025),
	Q14(-1.000000),Q14(0.866025),Q14(-0.500000),
};//sin_img_4[12]
static const short sin_img_5[]={
	Q14(0.000000),Q14(0.000000),Q14(-0.000000),Q14(0.000000),Q14(-0.000000),Q14(0.000000),Q14(-0.000000),Q14(0.000000),Q14(-0.000000),
	Q14(0.000000),Q14(-0.000000),Q14(0.000000),
};//sin_img_5[12]
static const short sin_img_6[]={
	Q14(0.000000),Q14(-0.500000),Q14(0.866025),Q14(-1.000000),Q14(0.866025),Q14(-0.500000),Q14(-0.000000),Q14(0.500000),Q14(-0.866025),
	Q14(1.000000),Q14(-0.866025),Q14(0.500000),
};//sin_img_6[12]
static const short sin_img_7[]={
	Q14(0.000000),Q14(-0.866025),Q14(0.866025),Q14(-0.000000),Q14(-0.866025),Q14(0.866025),Q14(-0.000000),Q14(-0.866025),Q14(0.866025),
	Q14(-0.000000),Q14(-0.866025),Q14(0.866025),
};//sin_img_7[12]
static const short sin_img_8[]={
	Q14(0.000000),Q14(-1.000000),Q14(0.000000),Q14(1.000000),Q14(-0.000000),Q14(-1.000000),Q14(0.000000),Q14(1.000000),Q14(-0.000000),
	Q14(-1.000000),Q14(0.000000),Q14(1.000000),
};//sin_img_8[12]
static const short sin_img_9[]={
	Q14(0.000000),Q14(-0.866025),Q14(-0.866025),Q14(0.000000),Q14(0.866025),Q14(0.866025),Q14(-0.000000),Q14(-0.866025),Q14(-0.866025),
	Q14(-0.000000),Q14(0.866025),Q14(0.866025),
};//sin_img_9[12]
static const short* sin_img[10]={
	sin_img_0,
	sin_img_1,
	sin_img_2,
	sin_img_3,
	sin_img_4,
	sin_img_5,
	sin_img_6,
	sin_img_7,
	sin_img_8,
	sin_img_9,
};//sin_img[10]
static const short* cos_rel[10]={
	cos_rel_0,
	cos_rel_1,
	cos_rel_2,
	cos_rel_3,
	cos_rel_4,
	cos_rel_5,
	cos_rel_6,
	cos_rel_7,
	cos_rel_8,
	cos_rel_9,
};//cos_rel[10]

//static const int freqSun[10]	={1,	2,	3,	4,	10,	6,	70,	80,	90,	10};
static const int freqScale[10]	={4, 4,	1,	1,	1,	1,	1,	1,	2, 4};
#define MAX_BANDS 10
#define MAX_FRAC (MAX_BANDS+2)
//#define ABS(a) (a<0?a=-a:a=a)
/**
* Get the 10 frequency bands.
* \param wavFrameData [in].
* \param wavFrameSizeInBytes [in] the size of wav data in bytes. 
* \param channels	[in].channel number,the max value is 2.
* \param sampleRate [in].
* \param bands[10]	[out].
* \retval 0 Succeeded.
* \retval NON_ZERO Fail.
*/
int	voGetWavFreqBand(short* wavFrameData,
						 int	wavFrameSizeInBytes,
						 int	channels,
						 int	sampleRate,
						 int	bands[10])
{
	
	int		ch,i,samples,b,j,up,scale;
	short	*data[2],*rel_table,*img_table;
	if(wavFrameData==NULL||wavFrameSizeInBytes<0
		||channels<=0||channels>6
		||sampleRate<8000||sampleRate>48000)
		return -1;
	memset(bands,1,sizeof(bands));
	for (ch=0;ch<channels;ch++)
	{
		data[ch] = wavFrameData+ch;
	}
	up		= MAX_FRAC;
	samples = channels==1?wavFrameSizeInBytes/2:wavFrameSizeInBytes/4;
	if(samples>MAX_SAMPLES)
		samples = MAX_SAMPLES;
	scale	= samples/16;

	for(b=0;b<10;b++)
	{
		int rel = 0;
		int img = 0;
		unsigned int result;
		short* leftData = data[0];
		rel_table = cos_rel[b];
		img_table = sin_img[b];
		for(i=0,j=0;i<samples;i++,leftData+=channels)
		{
			rel+=MUL_14(leftData[0],rel_table[j]);
			img+=MUL_14(leftData[0],img_table[j]);
			if(++j>=up)
				j = 0;
		}
		//ABS(rel);ABS(img);
		//rel/=scale;
		//img/=scale;
		if(rel<0) rel=-rel;
		if(img<0) img=-img;
		result= (rel+img)/2;//(int)(sqrt(rel*rel+img*img));//(rel*rel+img*img)/(samples*samples);//(int)(sqrt(rel*rel+img*img));//(rel*rel+img*img)>>3;//(int)(sqrt(rel*rel+img*img));//(rel+img)/2;//
		result/=scale*freqScale[b];
		if(result<0)
			result = 0;
		if(result>80)
		{
			do 
			{
				result-=15;
			} while(result>80);
		}
		else
		{
			//trick
			if(result<40&&result!=0)
				result+=15;
		}
		
		bands[b] = result;
	}
	
	return ERR_NONE;
}

#endif

