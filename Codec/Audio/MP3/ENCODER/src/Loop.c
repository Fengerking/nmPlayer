#include "Frame.h"

int calc_runlen( short *ix, SideInfoSub *si )
{
	int  p, i, sum = 0;
	
	for(i=SAMPL2; i-=2; ) {
		if(*(unsigned int*)&ix[i-2]) /* !!!! short *ix; !!!!! */
			break;
	}
	
	si->count1 = 0;
	
	for( ; i>3; i-=4)
	{
		int v = ix[i-1];
		int w = ix[i-2];
		int x = ix[i-3];
		int y = ix[i-4];
		
		if((v | w | x | y) <= 1)
		{
			p = (y<<3) + (x<<2) + (w<<1) + (v);
			
			sum += tab01[p];
			
			si->count1++;
		}
		else 
			break;
	}
	
	si->nBigvals = i;
	
	if((sum >> 16) < (sum & 0xffff))
	{
		si->count1TableSelect = 0;
		return sum >> 16;
	}
	else
	{
		si->count1TableSelect = 1;
		return sum & 0xffff;
	}
}


int quantize_int(int *xr, short *ix, SideInfoSub *si)
{
	int   i, s, frac_pow[] = { 0x10000, 0xd745, 0xb505, 0x9838 };
	
	s = frac_pow[si->quantStep & 3] >> si->quantStep / 4;
	
	if(((si->max_val + 256) >> 8) * s >= (1622 << 8))
		return 0;
	
	for(i=SAMPL2; i--; )
		ix[i] = int2idx[(ABS(xr[i]) * s + 0x8000) >> 16];
	
	return 1;
}

void subdivide(SideInfoSub *si)
{
	int scfb, count0, count1;
	int *scalefac;
	
	if( !si->nBigvals )
	{ 
		si->region_0_1 = 0;
		si->region0Count   = 0;
		si->region1Count   = 0;
	}
	else
	{
		scalefac = si->factScale;
		for(scfb=0; scalefac[scfb] < si->nBigvals; )
			scfb++;
		
		count0 = subdv_table[scfb].region0_cnt;
		count1 = subdv_table[scfb].region1_cnt;
		
		si->region_0_1 = (count0 << 3) | count1;
		si->region0Count  = scalefac[count0 + 1];
		si->region1Count  = scalefac[count0 + 1 + count1 + 1];
	}
}

int bigv_bitcount(short *ix, SideInfoSub *gi)
{
	int b1=0, b2=0, b3=0;
	
	gi->tableSelect[0] = 0;
	gi->tableSelect[1] = 0;
	gi->tableSelect[2] = 0;
	
	if( gi->region0Count > 0 )            
		gi->tableSelect[0] = choose_table(ix, 0           , gi->region0Count, &b1);
	
	if( gi->region1Count > gi->region0Count ) 
		gi->tableSelect[1] = choose_table(ix, gi->region0Count, gi->region1Count, &b2);
	
	if( gi->nBigvals > gi->region1Count ) 
		gi->tableSelect[2] = choose_table(ix, gi->region1Count, gi->nBigvals, &b3);
	
	return b1+b2+b3;
}

int quantize_and_count_bits(int *xr, short *ix, SideInfoSub *si)
{
	int bits = 10000;
	
	if(quantize_int(xr, ix, si))
	{
		bits = calc_runlen(ix, si);      
		subdivide(si);                   
		bits += bigv_bitcount(ix,si);    
	}
	
	return bits;
}

int inner_loop(int *xr, short *xi, int max_bits, SideInfoSub *si)
{
	int bits;
	
	while((bits=quantize_and_count_bits(xr, xi, si)) < max_bits-64)
	{
		if(si->quantStep == 0)
			break;
		
		if(si->quantStep <= 2)
			si->quantStep  = 0;
		else
			si->quantStep -= 2;
	}
	
	while(bits > max_bits)
	{
		si->quantStep++;
		bits = quantize_and_count_bits(xr, xi, si);
	}
	
	return bits;
}

void iteration_loop(FrameInfo *encInfo, Bitstream *stream, SideInfoSub *si, int *xr, int gr_cnt, int ch)
{
	int remain, tar_bits, max_bits, ResvSize;
	short *ix = encInfo->ix;
	
	max_bits = encInfo->mean_bits;
	ResvSize = encInfo->ResvSize;
	
	tar_bits = max_bits + (ResvSize / gr_cnt & ~7);
	if(tar_bits > max_bits + max_bits/2)
		tar_bits = max_bits + max_bits/2;
	
	si->part23Length = inner_loop(xr, ix, tar_bits, si);
	si->globalGain    = si->quantStep + 142 - si->additStep;
	
	ResvSize += max_bits - si->part23Length;
	
	if(gr_cnt == 1)
	{
		si->part23Length += ResvSize;
		if(si->part23Length > 4092)
		{
			remain = (si->part23Length - 4092 + 31) >> 5;
			si->part23Length    -= remain << 5;
			si[-1].part23Length += remain << 5;
			
			while(remain--)
			{
				putbits(stream, 0xFFFF, 16);
				putbits(stream, 0xFFFF, 16);
			}
		}
	}
	
	encInfo->ResvSize = ResvSize;
}
