#include "voResample.h"
#include "resample.h"

int InitFilter(FIRFilter *filter, int FilterLen, int scale)
{
	int len = FilterLen;
	int i;
	int	*Coeff;
	int Temp;
	int sinv;
	int len2=len/2;

	filter->length = FilterLen;
	Coeff = filter->Coef;

	Coeff[len2] = scale;
	for(i=1;i<=len2;i++)
	{
		Temp = ((scale * i) >> 10) & 0x7FF;

		sinv = SinTab[Temp&0x3FF];
		if(Temp > 0x400)
		{
			sinv = -sinv;
		}

		Coeff[len2+i]= Coeff[len2-i]= sinv/(PI10*i);
	}

	// Hamming windowing
	Coeff[len2] = MUL_20(Coeff[len2], HammTab[64]);
	Temp = 128/(len + 1);
	
	for(i=1; i<=len2; i++)
	{
		Coeff[len2+i] = Coeff[len2-i] = MUL_20(Coeff[len2+i], HammTab[64+Temp*i]);
	}

	return 0;
}

int FilterProcess(FIRFilter *filter, short *input, int pos, int nch, int InStride)
{
	int len, len2, rlen, n;
	int start, pos1,pos2;
	short *rbuf;
	int *Coef;
	int out;

	out = 0;

	len2 = filter->length/2;
	rlen = filter->nRemain;
	rbuf = filter->RemainBuf;
	Coef = filter->Coef;

	if(pos < 0)
	{
		start = rlen + pos*InStride + nch;
		out = MUL_30(rbuf[start], Coef[len2]);
		
		pos1 = start + InStride;
		pos2 = start - InStride;

		len = 1;
		for( ; len < -pos; len ++)
		{
			out += MUL_30((rbuf[pos1] + rbuf[pos2]), Coef[len2+len]);
			pos1 += InStride;
			pos2 -= InStride;
		}

		for( ; len <= len2; len++)
		{
			out += MUL_30((input[pos1-rlen] + rbuf[pos2]), Coef[len2+len]);
			pos1 += InStride;
			pos2 -= InStride;
		}
	}
	else if(pos < len2 && pos >= 0)
	{
		start = pos*InStride + nch;
		out = MUL_30(input[start], Coef[len2]);
		
		pos1 = start + InStride;
		pos2 = start - InStride;

		len = 1;
		for( ; len <= pos; len ++)
		{
			out += MUL_30((input[pos1] + input[pos2]), Coef[len2+len]);
			pos1 += InStride;
			pos2 -= InStride;
		}

		for( ; len <= len2; len++)
		{
			out += MUL_30((input[pos1] + rbuf[rlen + pos2]), Coef[len2+len]);
			pos1 += InStride;
			pos2 -= InStride;
		}
	}
	else
	{
		start = pos*InStride + nch;
		out = MUL_30(input[start], Coef[len2]);
		
		pos1 = start + InStride;
		pos2 = start - InStride;

		for(len = 1; len <= len2; len ++)
		{
			out += MUL_30((input[pos1] + input[pos2]), Coef[len2+len]);
			pos1 += InStride;
			pos2 -= InStride;
		}
	}

	n = out;

	return n;
}