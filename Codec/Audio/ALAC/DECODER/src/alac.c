/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		alac.c
* 
* Abstact:	
*
*		Apple Lossless Audio Codec for VisualOn Decoder process frame file.
*
* Author:
*
*		Witten Wen 12-April-2010
*
* Revision History:
*
******************************************************/

#include "voalacdec.h"
#include "alacstrmdec.h"

#define OUTBITS16

#define RESERVEBITS(val, bits) ((val << (32 - bits)) >> (32 - bits))
#define SIGN_ONLY(v) ((v<0)?(-1):((v>0)?(1):(0)))


//#if !defined(ARM)
/* Compute bit number of leading 0 for input */
#if 0
int CountHeadZeros(int input)
{	
    int output = 0;
    if (!input) return 32;
    __asm
    {
        mov eax, input;
        mov edx, 0x1f;
        bsr ecx, eax;
        sub edx, ecx;
        mov output, edx;
    }
    return output;
}
#else
int CountHeadZeros(int input)
{
    int output = 0;
    int curbyte = 0;

    curbyte = input >> 24;
    if (curbyte) goto found;
    output += 8;

    curbyte = input >> 16;
    if (curbyte & 0xff) goto found;
    output += 8;

    curbyte = input >> 8;
    if (curbyte & 0xff) goto found;
    output += 8;

    curbyte = input;
    if (curbyte & 0xff) goto found;
    output += 8;

    return output;

found:
    if (!(curbyte & 0xf0))
    {
        output += 4;
    }
    else
        curbyte >>= 4;

    if (curbyte & 0x8)
        return output;
    if (curbyte & 0x4)
        return output + 1;
    if (curbyte & 0x2)
        return output + 2;
    if (curbyte & 0x1)
        return output + 3;

    /* shouldn't get here: */
    return output + 4;
}
#endif
//#endif

#define MAXRICENUM	8	// maximum number of bits for a rice prefix.
VO_S32 EntropyDecodeValue(CALACObjectDecoder *palacdec, int readsize, int k, int kmodifier_mask)
{
	VO_S32 x = 0;	//decode value
	CALACBitStream *pstrmbits = palacdec->m_pBitStream;

	/* read x, number of 1s before 0 represent the rice value. */
	while(x <= MAXRICENUM && ReadBit(pstrmbits))x++;

	if(x>MAXRICENUM)
	{
		x = ReadBits32(pstrmbits, readsize);
	}
	else
	{
		if(k!=1)
		{
			int ExtraBit = ReadBits32(pstrmbits, k);

			x *= (((1<<k)-1) & kmodifier_mask);

			if(ExtraBit>1)
				x += ExtraBit - 1;
			else
				UnReadBits(pstrmbits, 1);
		}
	}
	return x;
}

VO_VOID EntropyRiceDec(CALACObjectDecoder *palacdec, VO_S32 *OutBuffer, int outsize, int readsize, int RiceHistoryMult)
{
	int outcount = 0;
	CALACSetInfo *palacinfo = palacdec->m_pALACInfo;
	int history = palacinfo->m_RiceInitHistory;
	int kmodifier = palacinfo->m_RiceKmodifier;
	int kmodifier_mask = (1<<palacinfo->m_RiceKmodifier) - 1;
	int SignMod = 0;

	for(outcount=0; outcount<outsize; outcount++)
	{
		VO_S32	k, Decodevalue, finalValue;

		k = 31 - kmodifier - CountHeadZeros((history >> 9) + 3);
		if(k<0)k += kmodifier;
		else k = kmodifier;

		// note: don't use rice_kmodifier_mask here (set mask to 0xFFFFFFFF)
		Decodevalue = EntropyDecodeValue(palacdec, readsize, k, 0xFFFFFFFF);
		Decodevalue += SignMod;
		finalValue = (Decodevalue+1)/2;		// inc by 1 and shift out sign bit
		if(Decodevalue&1)					// the sign is stored in the low bit
			finalValue*= -1;

		OutBuffer[outcount] = finalValue;

		SignMod = 0;

		/* update history */
		history += (Decodevalue*RiceHistoryMult) - ((history*RiceHistoryMult)>>9);

		if(Decodevalue>0xFFFF)history = 0xFFFF;

		/* special case, for compressed block of 0 */
		if((history<128) && (outcount+1 < outsize))
		{
			VO_U32 blocksize = 0;
			SignMod = 1;

			k = CountHeadZeros(history) + ((history + 16)>>6) - 24;

			/* blocksize always be 16 bits */
			blocksize = EntropyDecodeValue(palacdec, 16, k, kmodifier_mask);
			if(blocksize>0)
			{
				voALACMemset(&OutBuffer[outcount+1], 0, blocksize*sizeof(*OutBuffer));
				outcount += blocksize;
			}

			if(blocksize>0xFFFF)SignMod = 0;

			history = 0;
		}
	}
}

VO_VOID	PredictDecompressAdaptFIR(VO_S32 *EBuffer, 
								  VO_S32 *pOut, 
								  int OutSize, 
								  int SampleSizeRead, 
								  VO_S16 *PredictCoef,
								  int PredictCoefNum,
								  int PredictQunt)
{
	int i;
	VO_S32 *pOutT = pOut;
	/* First sample must be identical */
	*pOut = *EBuffer;

	if(!PredictCoefNum)
	{
		if(OutSize<=1)return;
		voALACMemcpy(pOut+1, EBuffer+1, (OutSize-1)*4);
		return;
	}

	/* 11111 - max value of predictor_coef_num */
	if(PredictCoefNum == 0x1f)
	{/* second-best case scenario for fir decompression,
       * error describes a small difference from the previous sample only
       */
		VO_S32 PreValue = 0;
		if(OutSize<=1)return;
		OutSize--;
		EBuffer++;
		while(OutSize--)
		{
			PreValue = *pOut++;
			*pOut = RESERVEBITS((PreValue + *EBuffer++), SampleSizeRead);
		}
		return;
	}

	/* read warm-up samples */
    if (PredictCoefNum > 0)
	{
		VO_S32 PreValue = 0;
		i = PredictCoefNum;
		EBuffer++;
		while(i--)
		{
			PreValue = *pOutT++;
			*pOutT = RESERVEBITS((PreValue + *EBuffer++), SampleSizeRead);
		}
	}
	if(PredictCoefNum > 0)
	{
		pOutT = pOut;
		for(i=PredictCoefNum+1; i<OutSize; i++)
		{
			int j, sum=0, outval, ErrorVal=*EBuffer++;
			for(j=0; j<PredictCoefNum; j++)
				sum += (pOutT[PredictCoefNum-j] - *pOutT) * PredictCoef[j];
			
			outval = sum + (1<<(PredictQunt-1));
			outval >>= PredictQunt;
			outval += *pOutT + ErrorVal;
			outval = RESERVEBITS(outval, SampleSizeRead);

			pOutT[PredictCoefNum+1] = outval;

			if(ErrorVal>0)
			{
				int predictnum = PredictCoefNum - 1;
				while(predictnum>=0 && ErrorVal>0)
				{
					int value = *pOutT - pOutT[PredictCoefNum-predictnum];
					int sign = SIGN_ONLY(value);

					PredictCoef[predictnum] -= sign;
					value *= sign;
					ErrorVal -= (value>>PredictQunt) * (PredictCoefNum-predictnum);
					predictnum--;
				}
			}
			else if(ErrorVal<0)
			{
				int predictnum = PredictCoefNum - 1;
				while(predictnum>=0 && ErrorVal<0)
				{
					int value = *pOutT - pOutT[PredictCoefNum-predictnum];
					int sign = -SIGN_ONLY(value);

					PredictCoef[predictnum] -= sign;
					value *= sign;
					ErrorVal -= (value>>PredictQunt) * (PredictCoefNum-predictnum);
					predictnum--;
				}
			}
			pOutT++;
		}
	}
}

VO_S32 monodec(CALACObjectDecoder *palacdec, VO_PBYTE outbuffer, VO_U32 *outlength, VO_U32 outsamples)
{
	int sizeflag = 0, uncopressedflag = 0, samplesizeread = 0;
	int i, UncompressByte = 0;
	CALACSetInfo *palacinfo = palacdec->m_pALACInfo;
	CALACBitStream *pstrbits = palacdec->m_pBitStream;

	/* 4bits, 2^result = something to do with output waiting
		12bits, unknown, skip it!*/
	SkipBits(pstrbits, 16);

	/* Out sample size flag */
	sizeflag = ReadBits16(pstrbits, 1);

	/* number of bytes in the (compressed) stream that are not compressed */
	UncompressByte = ReadBits16(pstrbits, 2);

	/* Compressed? */
	uncopressedflag = ReadBits16(pstrbits, 1);

	if(sizeflag)
	{
		/* The number of samples */
		outsamples = ReadBits32(pstrbits, 32);
		*outlength = outsamples * (palacinfo->m_SampleSize >> 3) * palacdec->m_Channels;
	}

	samplesizeread = palacinfo->m_SampleSize - (UncompressByte << 3);

	if(!uncopressedflag)
	{
		/* Compressed */
		VO_S16 PredictCoef[32];
		int PredictCoefNum, PredictType, PredictQunt;
		int RiceHistoryMult;

		/* Skip 16bits, unknown, it seems that is in ralation to channel */
		SkipBits(pstrbits, 16);

		PredictType = ReadBits16(pstrbits, 4);
		PredictQunt = ReadBits16(pstrbits, 4);

		RiceHistoryMult = (ReadBits16(pstrbits, 3) * palacinfo->m_RiceHistorymult) >> 2;

		PredictCoefNum = ReadBits16(pstrbits, 5);
		for(i = 0; i<PredictCoefNum; i++)
			PredictCoef[i] = (VO_U16)ReadBits16(pstrbits, 16);

		if(UncompressByte)
		{
			for(i=0; i<(int)outsamples; i++)
				palacinfo->m_pUncprsseBytsBufA[i] = ReadBits32(pstrbits, UncompressByte * 8);
		}

		/*  */
		EntropyRiceDec(palacdec, palacinfo->m_pErrorBuffA, outsamples, samplesizeread, RiceHistoryMult);

		if(PredictType == 0)
		{
			/* adaptive fir */
			PredictDecompressAdaptFIR(	palacinfo->m_pErrorBuffA, 
										palacinfo->m_pOutBuffA, 
										outsamples, 
										samplesizeread,
										PredictCoef, 
										PredictCoefNum,
										PredictQunt);
		}
	}
	else
	{
		/* Uncompressed */
		if(palacinfo->m_SampleSize <= 16)
			for(i=0; i<(int)outsamples; i++)
			{
				palacinfo->m_pOutBuffA[i] = ReadBits16(pstrbits, palacinfo->m_SampleSize);
//				VO_S32 aubits = ReadBits16(pstrbits, palacinfo->m_SampleSize);
//				aubits = RESERVEBITS(aubits, palacinfo->m_SampleSize);//¿ÉÒÔÊ¡µô!
//				palacinfo->m_pOutBuffA[i] = aubits;
			}
		else
		{
			for(i=0; i<(int)outsamples; i++)
				palacinfo->m_pOutBuffA[i] = ReadBits32(pstrbits, palacinfo->m_SampleSize);
		}
		UncompressByte = 0;
	}

	pstrbits->m_Length = 0;

	if(palacinfo->m_SampleSize == 16)
	{
		for(i=0; i<(int)outsamples; i++)
			((VO_S16 *)outbuffer)[i] = (VO_S16)palacinfo->m_pOutBuffA[i];
	}
	else if(palacinfo->m_SampleSize == 24)
	{
		VO_S32 sample;
		VO_U32 mask = ~(0xFFFFFFFF << (UncompressByte * 8));
		for(i=0; i<(int)outsamples; i++)
		{
			sample = palacinfo->m_pOutBuffA[i];
			if(UncompressByte)
			{
				sample <<= (UncompressByte * 8);
				sample |= palacinfo->m_pUncprsseBytsBufA[i] & mask;
			}
#ifdef OUTBITS16
			((VO_S16 *)outbuffer)[i] = (VO_S16)(sample >> 8);
#else
			outbuffer[i*3] = sample & 0xFF;
			outbuffer[i*3 + 1] = (sample>>8) & 0xFF;
			outbuffer[i*3 + 2] = (sample>>16) & 0xFF;
#endif
		}
	}
	else 
		return -1;

	return 0;
}

VO_VOID DeInterLace16(CALACSetInfo *palacinfo,
					  VO_S16 *outbuffer, 
					  VO_U32 outsamples, 
					  VO_U8 InterLaceShift, 
					  VO_U8 InterLaceLeftW)
{
	VO_S16	Left, Right;
	VO_S32	*BufferA = palacinfo->m_pOutBuffA;
	VO_S32	*BufferB = palacinfo->m_pOutBuffB;

	if(outsamples<=0)return;
	
	/* weighted interlacing */
	if(InterLaceLeftW)
	{
		VO_S32	difference, midright;
		while(outsamples--)
		{
			midright	= *BufferA++;
			difference	= *BufferB++;

			/* compute right and left */
			Right	= (VO_S16)(midright - ((difference * InterLaceLeftW) >> InterLaceShift));
			Left	= Right + (VO_S16)difference;

			*outbuffer++ = Left;
			*outbuffer++ = Right;
		}
	}
	else
	{
		while(outsamples--)
		{
			*outbuffer++ = (VO_S16 )*BufferA++;
			*outbuffer++ = (VO_S16 )*BufferB++;
		}
	}
}

VO_VOID DeInterLace24(CALACSetInfo *palacinfo,
					  int UncompressByte,
					  VO_U8 *outbuffer, 
					  VO_U32 outsamples, 
					  VO_U8 InterLaceShift, 
					  VO_U8 InterLaceLeftW)
{
	VO_S32	Left, Right;
	VO_S32	*BufferA = palacinfo->m_pOutBuffA;
	VO_S32	*BufferB = palacinfo->m_pOutBuffB;
	VO_S32	*UncprsseBytsBufA = palacinfo->m_pUncprsseBytsBufA;
	VO_S32	*UncprsseBytsBufB = palacinfo->m_pUncprsseBytsBufB;
	VO_U32	mask = ~(0xFFFFFFFF << (UncompressByte * 8));
#ifdef OUTBITS16
	VO_S16 *pout = (VO_S16 *)outbuffer;
#endif
	if(outsamples<=0)return;

	if(InterLaceLeftW)
	{
		VO_S32	difference, midright;
		while(outsamples--)
		{
			midright	= *BufferA++;
			difference	= *BufferB++;

			/* compute right and left */
			Right	= midright - ((difference * InterLaceLeftW) >> InterLaceShift);
			Left	= Right + difference;

			if(UncompressByte)
			{
				Left	<<= (UncompressByte * 8);
				Right	<<= (UncompressByte * 8);

				Left	|= (*UncprsseBytsBufA++) & mask;
				Right	|= (*UncprsseBytsBufB++) & mask;
			}

#ifdef OUTBITS16
			*pout++ = (VO_S16)(Left >> 8);
			*pout++ = (VO_S16)(Right >> 8);
#else
			*outbuffer++ = left & 0xFF;
			*outbuffer++ = (left>>8) & 0xFF;
			*outbuffer++ = (left>>16) & 0xFF;

			*outbuffer++ = right & 0xFF;
			*outbuffer++ = (right>>8) & 0xFF;
			*outbuffer++ = (right>>16) & 0xFF;
#endif
		}
	}
	else
	{
		while(outsamples--)
		{
			Left	= (VO_S16)*BufferA++;
			Right	= (VO_S16)*BufferB++;

			if(UncompressByte)
			{
				Left	<<= (UncompressByte * 8);
				Right	<<= (UncompressByte * 8);

				Left	|= (*UncprsseBytsBufA++) & mask;
				Right	|= (*UncprsseBytsBufB++) & mask;
			}

#ifdef OUTBITS16
			*pout++ = (VO_S16)(Left >> 8);
			*pout++ = (VO_S16)(Right >> 8);
#else
			*outbuffer++ = left & 0xFF;
			*outbuffer++ = (left>>8) & 0xFF;
			*outbuffer++ = (left>>16) & 0xFF;

			*outbuffer++ = right & 0xFF;
			*outbuffer++ = (right>>8) & 0xFF;
			*outbuffer++ = (right>>16) & 0xFF;
#endif
		}
	}
}

VO_S32 stereodec(CALACObjectDecoder *palacdec, VO_PBYTE outbuffer, VO_U32 *outlength, VO_U32 outsamples)
{
	int sizeflag = 0, uncopressedflag = 0, samplesizeread = 0;
	int i, UncompressByte = 0;
	CALACSetInfo *palacinfo = palacdec->m_pALACInfo;
	CALACBitStream *pstrbits = palacdec->m_pBitStream;
	VO_U8 InterLaceShift = 0, InterLaceLeftWeight = 0;

	/* 4bits, 2^result = something to do with output waiting
		12bits, unknown, skip it!*/
	SkipBits(pstrbits, 16);

	/* Out sample size flag */
	sizeflag = ReadBits16(pstrbits, 1);

	/* number of bytes in the (compressed) stream that are not compressed */
	UncompressByte = ReadBits16(pstrbits, 2);

	/* Compressed? */
	uncopressedflag = ReadBits16(pstrbits, 1);

	if(sizeflag)
	{
		/* The number of samples */
		outsamples = ReadBits32(pstrbits, 32);
		*outlength = outsamples * (palacinfo->m_SampleSize >> 3) * palacdec->m_Channels;
	}

	samplesizeread = palacinfo->m_SampleSize - (UncompressByte << 3) + 1;

	if(!uncopressedflag)
	{
		/* Compressed */
		VO_S16 PredictCoefA[32];
		int PredictCoefNumA, PredictTypeA, PredictQuntA;
		int RiceHistoryMultA;

		VO_S16 PredictCoefB[32];
		int PredictCoefNumB, PredictTypeB, PredictQuntB;
		int RiceHistoryMultB;

		InterLaceShift		= (VO_U8)ReadBits16(pstrbits, 8);
		InterLaceLeftWeight	= (VO_U8)ReadBits16(pstrbits, 8);

		/* channel 1 */
		PredictTypeA	= ReadBits16(pstrbits, 4);
		PredictQuntA	= ReadBits16(pstrbits, 4);

		RiceHistoryMultA = (ReadBits16(pstrbits, 3) * palacinfo->m_RiceHistorymult) >> 2;
		PredictCoefNumA	= ReadBits16(pstrbits, 5);

		for(i=0; i<PredictCoefNumA; i++)
			PredictCoefA[i] = (VO_U16)ReadBits16(pstrbits, 16);
		
		/* channel 2 */
		PredictTypeB = ReadBits16(pstrbits, 4);
		PredictQuntB = ReadBits16(pstrbits, 4);

		RiceHistoryMultB = (ReadBits16(pstrbits, 3) * palacinfo->m_RiceHistorymult) >> 2;
		PredictCoefNumB = ReadBits16(pstrbits, 5);

		for(i=0; i<PredictCoefNumB; i++)
			PredictCoefB[i] = (VO_U16)ReadBits16(pstrbits, 16);

		if(UncompressByte)
		{
			for(i=0; i<(int)outsamples; i++)
			{
				palacinfo->m_pUncprsseBytsBufA[i] = ReadBits32(pstrbits, UncompressByte * 8);
				palacinfo->m_pUncprsseBytsBufB[i] = ReadBits32(pstrbits, UncompressByte * 8);
			}
		}

		/* channel 1 */
		EntropyRiceDec(palacdec, palacinfo->m_pErrorBuffA, outsamples, samplesizeread, RiceHistoryMultA);
		if(PredictTypeA == 0)
		{
			/* adaptive fir */
			PredictDecompressAdaptFIR(	palacinfo->m_pErrorBuffA, 
										palacinfo->m_pOutBuffA, 
										outsamples, 
										samplesizeread,
										PredictCoefA, 
										PredictCoefNumA,
										PredictQuntA);
		}

		/* channel 2 */
		EntropyRiceDec(palacdec, palacinfo->m_pErrorBuffB, outsamples, samplesizeread, RiceHistoryMultB);
		if(PredictTypeB == 0)
		{
			/* adaptive fir */
			PredictDecompressAdaptFIR(	palacinfo->m_pErrorBuffB, 
										palacinfo->m_pOutBuffB, 
										outsamples, 
										samplesizeread,
										PredictCoefB, 
										PredictCoefNumB,
										PredictQuntB);
		}
	}
	else
	{
		/* Uncompressed */
		if(palacinfo->m_SampleSize <= 16)
		{
			for(i=0; i<(int)outsamples; i++)
			{
				palacinfo->m_pOutBuffA[i] = ReadBits16(pstrbits, palacinfo->m_SampleSize);
				palacinfo->m_pOutBuffB[i] = ReadBits16(pstrbits, palacinfo->m_SampleSize);
			}
		}
		else
		{
			for(i=0; i<(int)outsamples; i++)
			{
				palacinfo->m_pOutBuffA[i] = ReadBits32(pstrbits, palacinfo->m_SampleSize);
				palacinfo->m_pOutBuffB[i] = ReadBits32(pstrbits, palacinfo->m_SampleSize);
			}
		}
	}

	pstrbits->m_Length = 0;

	if(palacinfo->m_SampleSize == 16)
	{
		DeInterLace16(palacinfo, (VO_S16*)outbuffer, outsamples, InterLaceShift, InterLaceLeftWeight);
	}
	else if(palacinfo->m_SampleSize == 24)
	{
		DeInterLace24(palacinfo, UncompressByte, (VO_U8 *)outbuffer, 
						outsamples, InterLaceShift, InterLaceLeftWeight);
	}
	else
		return -1;

	return 0;
}

VO_S32 voALACDecodeFrame(CALACObjectDecoder *palacdec, VO_PBYTE outbuffer, VO_U32 *outlength)
{
	VO_U32	channels, outsamples = palacdec->m_pALACInfo->m_MaxSamplesPerFrame;
	CALACBitStream *pstrbits = palacdec->m_pBitStream;

	channels = ReadBits16(pstrbits, 3);

	*outlength = outsamples * (palacdec->m_pALACInfo->m_SampleSize >> 3) * palacdec->m_Channels;

	//change for task 10842
	if (palacdec->m_Channels != (channels + 1))
	{
		*outlength = 0;
		palacdec->m_pBitStream->m_Length = 0;
		return 0;
	}

	if(channels == 1)
	{
		if(stereodec(palacdec, outbuffer, outlength, outsamples)<0)
			return -1;
	}
	else if(channels == 0)
	{
		if(monodec(palacdec, outbuffer, outlength, outsamples)<0)
			return -1;
	}

	return 0;
}

VO_VOID	ALACSetInfo(CALACObjectDecoder *palacdec)
{
	CALACSetInfo *psetinfo = palacdec->m_pALACInfo;
	palacdec->m_pInBuffer = palacdec->m_pCodecData;

//	palacdec->m_pInBuffer += 4; /* size */
//	palacdec->m_pInBuffer += 4; /* frma */
//	palacdec->m_pInBuffer += 4; /* alac */
//	palacdec->m_pInBuffer += 4; /* size */
//	palacdec->m_pInBuffer += 4; /* alac */

	palacdec->m_pInBuffer += 4; /* 0 ? */

	psetinfo->m_MaxSamplesPerFrame	= ReadU32(palacdec);
	psetinfo->m_Info7a			= *palacdec->m_pInBuffer++;
	psetinfo->m_SampleSize		= *palacdec->m_pInBuffer++;
	psetinfo->m_RiceHistorymult	= *palacdec->m_pInBuffer++;
	psetinfo->m_RiceInitHistory	= *palacdec->m_pInBuffer++;
	psetinfo->m_RiceKmodifier	= *palacdec->m_pInBuffer++;
	psetinfo->m_Info7f			= *palacdec->m_pInBuffer++;
	psetinfo->m_Info80			= ReadU16(palacdec);
	psetinfo->m_Info82			= ReadU32(palacdec);
	psetinfo->m_Info86			= ReadU32(palacdec);
	psetinfo->m_Info8aRate		= ReadU32(palacdec);

	/* malloc buffer */
	psetinfo->m_pErrorBuffA			= voALACMalloc(psetinfo->m_MaxSamplesPerFrame * 4);
	psetinfo->m_pErrorBuffB			= voALACMalloc(psetinfo->m_MaxSamplesPerFrame * 4);

	psetinfo->m_pOutBuffA			= voALACMalloc(psetinfo->m_MaxSamplesPerFrame * 4);
	psetinfo->m_pOutBuffB			= voALACMalloc(psetinfo->m_MaxSamplesPerFrame * 4);

	psetinfo->m_pUncprsseBytsBufA	= voALACMalloc(psetinfo->m_MaxSamplesPerFrame * 4);
	psetinfo->m_pUncprsseBytsBufB	= voALACMalloc(psetinfo->m_MaxSamplesPerFrame * 4);
}
