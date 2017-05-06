//#include "mem_align.h"
#include "voResample.h"
#include "resample.h"

int LinerInterpolation(int val1, int val2, int scale)
{
	int result;

	result = val1 + MUL_10((val2 - val1), scale);

	return result;
}
#if 0
int CosInterpolation(int val1, int val2, int scale)
{
	int result, scale2;
#if 0
	angle = 0x200-scale;
	if(angle>=0)
		scale2 = 0x200 - ((SinTab[angle])>>21);
	else
		scale2 = 0x200 + ((SinTab[-angle])>>21);
#else
	scale2 = 0x200 - CosTab[scale];	//cos(a)*(1<<9)
#endif
	result = val1 + MUL_10((val2 - val1), scale2);
	return result;
}
#endif
int CubicInterpolation(int val0, int val1, int val2, int val3, int scale)
{
	int a0, a1, a2, scale2, result;

	scale2 = MUL_10(scale, scale);
//	a0 = val3 - val2 + val1 - val0;
//	a1 = val0 - val1 - a0;
//	a2 = val2 - val0;
//	result = val1;
	a0 = val1 - val2 + ((val3+val1-val2-val0)>>1);	//-0.5*val0+1.5*val1-1.5*val2+0.5*val3;
	a1 = val0 + ((val2-val1)<<1) - ((val1+val3)>>1);//val0-2.5*val1+2*val2-0.5*val3;
	a2 = ((val2 - val0)>>1);						//-0.5*val0+0.5*val2
	result = val1;

	a0 = MUL_10(a0, scale);
	result += MUL_10(a0, scale2);
	result += MUL_10(a1, scale2);
	result += MUL_10(a2, scale);

	return result;
}

int RateUp2x(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer)
{
	int nch, i, outdata;
	//FIRFilter *filter = NULL;

	for(nch=0; nch<InStride; nch++)
	{
		outdata = ((pRes->UplastSimple[nch][0] + inbuffer[nch]) >> 1);
		outbuffer[nch] = CLIPTOSHORT(outdata);
		for(i=0; i<length-1; i++)
		{
			outdata = inbuffer[InStride*i + nch];
			outbuffer[InStride*(2*i+1) + nch] = CLIPTOSHORT(outdata);
			outdata = ((inbuffer[InStride * i + nch] + inbuffer[InStride * (i+1) + nch])>>1);
			outbuffer[InStride*(2*i+2) + nch] = CLIPTOSHORT(outdata);
		}
		outdata = inbuffer[InStride * i + nch];
		outbuffer[InStride*(2*i+1) + nch] = CLIPTOSHORT(outdata);
		pRes->UplastSimple[nch][0] = outdata;
	}	
	return (length*2);
}

int RateUpLowMulti(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer)
{
	int i, endpos = -1, nch, outdata;
	int *inter_scale = pRes->UpInterScale;
	short *pOut = NULL;
	int UpMultiNum = pRes->UpMulti;

	for(nch=0; nch<InStride; nch++)
	{		
		pOut = outbuffer + nch;
		if(endpos < 0)
		{
			*pOut = (short)pRes->UplastSimple[nch][0];
			pOut += InStride;
			for(i=1; i<UpMultiNum; i++)
			{
				outdata = pRes->Interpolation(pRes->UplastSimple[nch][0], 
					inbuffer[nch], inter_scale[i]);
				*pOut = CLIPTOSHORT(outdata);
				pOut += InStride;
			}
			endpos++;
		}
		while(endpos<length - 1)
		{
			*pOut = inbuffer[InStride*endpos + nch];
			pOut += InStride;
			for(i=1; i<UpMultiNum; i++)
			{
				outdata = pRes->Interpolation(inbuffer[InStride*endpos + nch], 
					inbuffer[InStride*(endpos + 1) + nch], inter_scale[i]);
				*pOut = CLIPTOSHORT(outdata);
				pOut += InStride;
			}
			endpos++;
		}
		pRes->UplastSimple[nch][0] = inbuffer[InStride*endpos + nch];
		endpos = -1;
	}
	pRes->UplastScale = inter_scale[1];
	return length * UpMultiNum;
}

int RateUpLow(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer)
{
	int n =0, endpos = -1, pos = -1, inter_scale = pRes->UplastScale;
	int outdata=0, tmppos, nch, inv_scale = pRes->inv_Scale;

	for(nch=0; nch<InStride; nch++)
	{
		while(endpos < 0)
		{
			outdata = pRes->Interpolation(pRes->UplastSimple[nch][0], 
				inbuffer[nch], inter_scale);
			outbuffer[InStride*n + nch] = CLIPTOSHORT(outdata);
			n++;

			tmppos = MUL_10(n,inv_scale);

			endpos = pos + (tmppos >> 10);
			inter_scale = tmppos & 1023;
		}

		while(endpos < length - 1)
		{
			outdata = pRes->Interpolation(inbuffer[InStride*endpos + nch], 
				inbuffer[InStride*(endpos + 1) + nch], inter_scale);
			outbuffer[InStride*n + nch] = CLIPTOSHORT(outdata);
			n++;

			tmppos = MUL_10(n,inv_scale);

			endpos = pos + (tmppos >> 10);
			inter_scale = tmppos & 1023;
		}
		pRes->UplastSimple[nch][0] = inbuffer[InStride*endpos + nch];
		endpos = -1;
	}
	pRes->UplastScale = inter_scale;
	return n;
}

int RateUpNormHighMulti(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer)
{
	
	int i, endpos = -2;//, inter_scale = pRes->UplastScale;
	int outdata=0, nch;
	int *inter_scale = pRes->UpInterScale;
	short *pOut = NULL;
	int UpMultiNum = pRes->UpMulti;

	for(nch=0; nch<InStride; nch++)
	{
		pOut = outbuffer + nch;
		if(endpos == -2)
		{
			*pOut = (short)pRes->UplastSimple[nch][1];
			pOut += InStride;
			for(i=1; i<UpMultiNum; i++)
			{
				outdata = CubicInterpolation(pRes->UplastSimple[nch][0], pRes->UplastSimple[nch][1],
					pRes->UplastSimple[nch][2], inbuffer[nch], inter_scale[i]);
				*pOut = CLIPTOSHORT(outdata);
				pOut += InStride;
			}
			endpos++;
		}
		if(endpos == -1)
		{
			*pOut = (short)pRes->UplastSimple[nch][2];
			pOut += InStride;
			for(i=1; i<UpMultiNum; i++)
			{
				outdata = CubicInterpolation(pRes->UplastSimple[nch][1], pRes->UplastSimple[nch][2],
					inbuffer[nch], inbuffer[InStride+nch], inter_scale[i]);
				*pOut = CLIPTOSHORT(outdata);
				pOut += InStride;
			}
			endpos++;
		}
		if(endpos == 0)
		{
			*pOut = (short)inbuffer[nch];
			pOut += InStride;
			for(i=1; i<UpMultiNum; i++)
			{
				outdata = CubicInterpolation(pRes->UplastSimple[nch][2], inbuffer[nch],
					inbuffer[InStride+nch], inbuffer[InStride*2+nch], inter_scale[i]);
				*pOut = CLIPTOSHORT(outdata);
				pOut += InStride;
			}
			endpos++;
		}
		
		while(endpos < length - 2)
		{
			*pOut = (short)inbuffer[InStride*endpos + nch];
			pOut += InStride;
			for(i=1; i<UpMultiNum; i++)
			{
				outdata = CubicInterpolation(inbuffer[InStride*(endpos-1) + nch], 
					inbuffer[InStride*endpos + nch], 
					inbuffer[InStride*(endpos + 1) + nch], 
					inbuffer[InStride*(endpos + 2) + nch], inter_scale[i]);
				*pOut = CLIPTOSHORT(outdata);
				pOut += InStride;
			}
			endpos++;			
		}

		for(i=0; i<=length-endpos; i++)
		{
			pRes->UplastSimple[nch][i] = inbuffer[InStride*(endpos+i-1) + nch];
		}
		endpos = -2;
	}
	pRes->UplastScale = inter_scale[1];
	return length * UpMultiNum;
}
int RateUpNormHigh(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer)
{
	int i, n =0, endpos = -2, pos = -2, inter_scale = pRes->UplastScale;
	int outdata=0, tmppos, nch, inv_scale = pRes->inv_Scale;

	for(nch=0; nch<InStride; nch++)
	{
		while(endpos == -2)
		{
			outdata = CubicInterpolation(pRes->UplastSimple[nch][0], pRes->UplastSimple[nch][1],
				pRes->UplastSimple[nch][2], inbuffer[nch], inter_scale);
			outbuffer[InStride*n + nch] = CLIPTOSHORT(outdata);
			n++;

			tmppos = MUL_10(n,inv_scale);

			endpos = pos + (tmppos >> 10);
			inter_scale = tmppos & 1023;
		}

		while(endpos == -1)
		{
			outdata = CubicInterpolation(pRes->UplastSimple[nch][1], pRes->UplastSimple[nch][2], 
				inbuffer[nch], inbuffer[InStride +nch], inter_scale);
			outbuffer[InStride*n + nch] = CLIPTOSHORT(outdata);
			n++;

			tmppos = MUL_10(n,inv_scale);

			endpos = pos + (tmppos >> 10);
			inter_scale = tmppos & 1023;
		}

		while(endpos == 0)
		{
			outdata = CubicInterpolation(pRes->UplastSimple[nch][2], inbuffer[nch], 
				inbuffer[InStride +nch], inbuffer[InStride*2 +nch], inter_scale);
			outbuffer[InStride*n + nch] = CLIPTOSHORT(outdata);
			n++;

			tmppos = MUL_10(n,inv_scale);

			endpos = pos + (tmppos >> 10);
			inter_scale = tmppos & 1023;
		}

		while(endpos < length - 2)
		{
			outdata = CubicInterpolation(inbuffer[InStride*(endpos-1) + nch], inbuffer[InStride*endpos + nch], 
				inbuffer[InStride*(endpos + 1) + nch], inbuffer[InStride*(endpos + 2) + nch], inter_scale);
			outbuffer[InStride*n + nch] = CLIPTOSHORT(outdata);
			n++;

			tmppos = MUL_10(n,inv_scale);

			endpos = pos + (tmppos >> 10);
			inter_scale = tmppos & 1023;
		}

		for(i=0; i<=length-endpos; i++)
		{
			pRes->UplastSimple[nch][i] = inbuffer[InStride*(endpos+i-1) + nch];
		}
		endpos = -2;
	}
	pRes->UplastScale = inter_scale;
	return n;
}

int SmoothFilterProcess(ResInfo *pRes, int samples, int outStride, short* outbuffer)
{
	int nch, outdata, endpos = 0, maxout=0, n=0;
	FIRFilter *filter = pRes->filter;
	short *pTemp = NULL;

	for(nch=0; nch<outStride; nch++)
	{
//		total = samples;
		maxout = samples - pRes->Filtlen/2;

		if(pRes->tmplen < (samples + MAXFILLEN)*outStride)
		{
			if(pRes->tmpbuf)
			{
				voRESFree(pRes->tmpbuf);
				pRes->tmpbuf = NULL;
			}

			pRes->tmplen = (samples + MAXFILLEN)*outStride;
			pRes->tmpbuf = voRESMalloc(pRes->tmplen*sizeof(short));

			if(pRes->tmpbuf == NULL)
			{
				continue;
			}
		}

		pTemp = pRes->tmpbuf + nch;
		endpos = pRes->lastpos;

		while(endpos < maxout)
		{
			outdata = FilterProcess(filter, outbuffer, endpos, nch, outStride);
			*pTemp = CLIPTOSHORT(outdata);
			pTemp += outStride;
			endpos++; 
		}	
	}
	n = endpos-pRes->lastpos;
	pRes->lastpos = endpos - samples;
	filter->nRemain = (pRes->Filtlen/2 - pRes->lastpos)*outStride;
	voRESMemcpy(filter->RemainBuf, outbuffer + (endpos - pRes->Filtlen/2)*outStride,  
		filter->nRemain*sizeof(short));
	
	if(pRes->tmpbuf)
	{
		voRESMemcpy(outbuffer, pRes->tmpbuf, n*outStride*sizeof(short));
	}
	return n;
}

int RateCore(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer)
{
	int outdata;
	FIRFilter *filter = NULL;
//	VO_MEM_OPERATOR *pvoMemop;
	int total, maxout;

	int scale;
	int inv_scale;
	//int inter_scale;
	int nch, n = 0, outch;
	int outsimple = 0;
	int pos, endpos = 0;
	//int tmppos;

	filter = pRes->filter;
	scale = pRes->Scale;
	inv_scale = pRes->inv_Scale;
//	pvoMemop = pRes->pvoMemop;

	total = length;

	if(pRes->RateUp2Down >= 1)
	{
		//upsample 
		if(pRes->level == 0)
		{
			if(pRes->RateUp2Down == 2)
				outsimple = RateUp2x(pRes, inbuffer, length, InStride, outbuffer);
			else if(pRes->RateUp2Down == 3)
				outsimple = RateUpLowMulti(pRes, inbuffer, length, InStride, outbuffer);
			else
				outsimple = RateUpLow(pRes, inbuffer, length, InStride, outbuffer);
		}
		else if(pRes->level >= 1)
		{
			if(pRes->RateUp2Down >= 2)
				outsimple = RateUpNormHighMulti(pRes, inbuffer, length, InStride, outbuffer);
			else
				outsimple = RateUpNormHigh(pRes, inbuffer, length, InStride, outbuffer);
		}
		if(pRes->level == 2)
		{
			outsimple = SmoothFilterProcess(pRes, outsimple, InStride, outbuffer);
		}
	}
	else if(pRes->RateUp2Down == -1)
	{
		//downsample		
		maxout = total - pRes->Filtlen/2;

		outch = 0;

		for(nch = 0; nch < InStride; nch++)
		{
//			if(!(pRes->ChanMap[nch]&pRes->ChanSel))
//				continue;

			n = 0;
			pos = pRes->lastpos;
			endpos = pos;

			while(endpos < maxout)
			{
				outdata = FilterProcess(filter, inbuffer, endpos, nch, InStride);
				outbuffer[InStride*n + outch] = CLIPTOSHORT(outdata);
				n++;
				endpos = pos + (int)MUL_20(n, inv_scale); 
			}
			outch++;
		}
		
		pRes->lastpos = endpos - total;
		filter->nRemain = (pRes->Filtlen/2 - pRes->lastpos)*InStride;
		voRESMemcpy(filter->RemainBuf, pRes->input + (endpos - pRes->Filtlen/2)*InStride, 
			filter->nRemain*sizeof(short));
		outsimple = n;	
	}

	return outsimple;
}

