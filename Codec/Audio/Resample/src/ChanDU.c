#include "mem_align.h"
#include "voResample.h"
#include "resample.h"

#define DM_MUL 5248  //16384  //3203/10000
#define RSQRT2 5818  //8192	//7071/10000

static int COPYNCH(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i, ch;

	for(i = 0; i < length; i++)
	{
		for(ch = 0; ch < InStride; ch++)
		{
			outbuffer[i*outStride + ch] = inbuffer[i*InStride + ch]; 
		}
	}

	return length;	
}

static int In2Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[0]; 

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In3Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[2]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In4Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[2]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In5Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[2]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In6Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[2]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In7Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[2]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In8Out1(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[2]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In1Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[0]; 
		outbuffer[1] = inbuffer[0]; 

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In3Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[0]];  //just select, need more compute
		outbuffer[1] = inbuffer[ChanPos[1]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In4Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[0]];  //just select, need more compute
		outbuffer[1] = inbuffer[ChanPos[1]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In5Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[0]];  //just select, need more compute
		outbuffer[1] = inbuffer[ChanPos[1]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In6Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;
	short* acturalBuf = outbuffer;
	int C,L_S,R_S,tmp,tmp1,cum;

	for(i = 0; i < length; i++)
	{
		C   = (inbuffer[ChanPos[2]]*RSQRT2) >> 13;  //center
		L_S = (inbuffer[ChanPos[3]]*RSQRT2) >> 13;  //left surround
		cum = inbuffer[ChanPos[0]] + C + L_S;		//left
		tmp = (cum*DM_MUL) >> 14;
		
		R_S = (inbuffer[ChanPos[4]]*RSQRT2) >> 13;	//right surround
		cum = inbuffer[ChanPos[1]] + C + R_S;		//right
		tmp1 = (cum*DM_MUL) >> 14;
	
		outbuffer[0] = CLIPTOSHORT(tmp);
		outbuffer[1] = CLIPTOSHORT(tmp1);
		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In7Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[0]];  //just select, need more compute
		outbuffer[1] = inbuffer[ChanPos[1]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

static int In8Out2(short* inbuffer, int InStride, int length, int *ChanPos, short* outbuffer, int outStride)
{
	int i;

	for(i = 0; i < length; i++)
	{
		outbuffer[0] = inbuffer[ChanPos[0]];  //just select, need more compute
		outbuffer[1] = inbuffer[ChanPos[1]];  //just select, need more compute

		outbuffer += outStride;
		inbuffer  += InStride;
	}

	return length;	
}

typedef int (*ChProcess)(short* inbuffer, int length, int InStride, int *ChanPos, short* outbuffer, int outStride);

static ChProcess DUProcess[16] = 
{
	COPYNCH, In2Out1, In3Out1, In4Out1, In5Out1, In6Out1, In7Out1, 
	In8Out1, In1Out2, In3Out2, In4Out2, In5Out2, In6Out2, In7Out2,
	In8Out2, NULL
};

static int SelectCh(short* inbuffer, int InStride, int length, int *ChapMap, short* outbuffer, int outStride, int ChanSel)
{
	int n, nch, outch;
	short *inbuf;

	outch = 0;
	for(nch = 0; nch < InStride; nch++)
	{
		inbuf = inbuffer + nch;
		if(!(ChapMap[nch]&ChanSel))
			continue;

		for(n = 0; n < length; n++)
		{
			outbuffer[n*outStride + outch] = inbuffer[n*InStride + nch];
		}

		outch++;
	}

	return length;
}

int ChanCore(ResInfo *pRes, short* inbuffer, int length, short* outbuffer)
{
	int InStride, outStride;
	int *Chpos, *ChMap;
	
	InStride = pRes->ChanIn;
	outStride = pRes->ChanOut;
	Chpos = pRes->ChanPos;
	ChMap = pRes->ChanMap;

	if(pRes->ChanUpDown == -2)
	{
		return SelectCh(inbuffer, InStride, length, ChMap, outbuffer,  outStride, pRes->ChanSel);
	}
	else 
	{
		if(pRes->ChanIndex >= 0)
			return DUProcess[pRes->ChanIndex](inbuffer, InStride, length, Chpos, outbuffer,  outStride);
	}

	return 0;
}


