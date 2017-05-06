#include "Frame.h"

void Huffmancodebits(Bitstream *stream, short *ix, int *xr, SideInfoSub *gi )
{
	int    region1   = gi->region0Count;
	int    region2   = gi->region1Count;
	int    bigvals   = gi->nBigvals;
	int    count1    = bigvals + (gi->count1 << 2);
	int    stuffBits = 0;
	int    bits      = 0;
	
	if(region1 > 0)
		bits += HuffmanCode(stream, ix, xr,   0    , region1, gi->tableSelect[0]);
	
	if(region2 > region1)
		bits += HuffmanCode(stream, ix, xr, region1, region2, gi->tableSelect[1]);
	
	if(bigvals > region2)
		bits += HuffmanCode(stream, ix, xr, region2, bigvals, gi->tableSelect[2]);
	
	if(count1 > bigvals)
		bits += HuffmanCod1(stream, ix, xr, bigvals,  count1, gi->count1TableSelect);
	
	if((stuffBits = gi->part23Length - bits) > 0)
	{
		int stuffWords = stuffBits >> 5;
		int remainBits = stuffBits & 31;
		
		if( remainBits )
			putbits(stream,  (1 << remainBits) - 1, remainBits );
		
		while( stuffWords-- )
		{
			putbits(stream,  0xFFFF, 16); 
			putbits(stream,  0xFFFF, 16);
		}
	}
}

int HuffmanCod1(Bitstream *stream, short *ix, int *xr, unsigned int begin, unsigned int end, int tbl)
{
	unsigned int  i, d, p;
	int     sumbit=0, s=0, l=0, v, w, x, y;
#define sgnv (xr[i+0] < 0 ? 1 : 0)
#define sgnw (xr[i+1] < 0 ? 1 : 0)
#define sgnx (xr[i+2] < 0 ? 1 : 0)
#define sgny (xr[i+3] < 0 ? 1 : 0)
	
	for(i=begin; i<end; i+=4)
	{
		v = ix[i+0];
		w = ix[i+1];
		x = ix[i+2];
		y = ix[i+3];
		p = (v << 3) + (w << 2) + (x << 1) + y;
		
		switch(p)
		{
		case  0: l=0; s = 0; break;
		case  1: l=1; s = sgnv; break;
		case  2: l=1; s =               sgnw; break;
		case  3: l=2; s = (sgnv << 1) + sgnw; break;
		case  4: l=1; s =                             sgnx; break;
		case  5: l=2; s = (sgnv << 1)               + sgnx; break;
		case  6: l=2; s =               (sgnw << 1) + sgnx; break;
		case  7: l=3; s = (sgnv << 2) + (sgnw << 1) + sgnx; break;
		case  8: l=1; s =                                           sgny; break;
		case  9: l=2; s = (sgnv << 1)                             + sgny; break;
		case 10: l=2; s =               (sgnw << 1)               + sgny; break;
		case 11: l=3; s = (sgnv << 2) + (sgnw << 1)               + sgny; break;
		case 12: l=2; s =                             (sgnx << 1) + sgny; break;
		case 13: l=3; s = (sgnv << 2)               + (sgnx << 1) + sgny; break;
		case 14: l=3; s =               (sgnw << 2) + (sgnx << 1) + sgny; break;
		case 15: l=4; s = (sgnv << 3) + (sgnw << 2) + (sgnx << 1) + sgny; break;
		}
		
		d = (ht_count[tbl][0][p] << l) + s;
		l =  ht_count[tbl][1][p];
		putbits(stream, d, l );
		sumbit += l;
	}
	
	return sumbit;
}

/* Implements the pseudocode of page 98 of the IS */
int HuffmanCode(Bitstream *stream, short *ix, int *xr, unsigned int begin, unsigned int end, int table)
{
	unsigned int       code;
	unsigned int       i, xl=0, yl=0, idx;
	int          x, y, bit, sumbit=0;
#define sign_x (xr[i+0] < 0 ? 1 : 0)
#define sign_y (xr[i+1] < 0 ? 1 : 0)
	
	if(table == 0)
		return 0;
	
	if( table > 15 )
	{ /* ESC-table is used */
		unsigned int linbits  = ht_big[table-16].linbits;
		const unsigned short *hffcode = (table < 24) ? t16HB : t24HB;
		const unsigned char  *hlen    = (table < 24) ? t16l  : t24l;
		
		for(i=begin; i<end; i+=2)
		{
			x = ix[ i ];
			y = ix[i+1];
			
			if(x > 14) { xl = x - 15;  x = 15; }
			if(y > 14) { yl = y - 15;  y = 15; }
			
			idx  = x * 16 + y;
			code = hffcode[idx];
			bit  = hlen   [idx];
			
			if(x)
			{
				if(x > 14)
				{
					code = (code << linbits) | xl;
					bit += linbits;
				}
				
				code = (code << 1) | sign_x;
				bit += 1;
			}
			
			if(y)
			{
				if(y > 14)
				{
					code = (code << linbits) | yl;
					bit += linbits;
				}
				
				code = (code << 1) | sign_y;
				bit += 1;
			}
			
			putbits(stream,  code, bit );
			sumbit += bit;
		}
	}
	else
	{ /* No ESC-words */
		const struct huffcodetab *h = &ht[table];
		
		for(i=begin; i<end; i+=2)
		{
			x = ix[i];
			y = ix[i+1];
			
			idx  = x * h->len + y;
			code = h->table[idx];
			bit  = h->hlen [idx];
			
			if(x)
			{
				code = (code << 1) | sign_x;
				bit += 1;
			}
			
			if(y)
			{
				code = (code << 1) | sign_y;
				bit += 1;
			}
			
			putbits(stream,  code, bit );
			sumbit += bit;
		}
	}
		
	return sumbit;
}

int choose_table( short *ix, unsigned int begin, unsigned int end, int *bits )
{
	unsigned int i;
	int    max, table0, table1;
	
	for(i=begin,max=0; i<end; i++)
	{
		if(ix[i] > max)
			max = ix[i];
	}
	
	if(max < 16)
	{
		/* tables without linbits */
		/* indx: 0  1  2  3  4  5  6  7  8  9 10 11 12  13 14  15 */
		/*  len: 0, 2, 3, 3, 0, 4, 4, 6, 6, 6, 8, 8, 8, 16, 0, 16 */
		switch(max)
		{
		case 0:  return  0;
		case 1:  return       count_bit1(ix, begin, end, bits);
		case 2:  return  2 + find_best_2(ix, begin, end, tab23, 3, bits);
		case 3:  return  5 + find_best_2(ix, begin, end, tab56, 4, bits);
		case 4:
		case 5:  return  7 + find_best_3(ix, begin, end, tab789, 6, bits);
		case 6:
		case 7:  return 10 + find_best_3(ix, begin, end, tabABC, 8, bits);
		default: return 13 + find_best_2(ix, begin, end, tab1315, 16, bits) * 2;
		}
	}
	else
	{
		/* tables with linbits */
		max -= 15;
		
		for(table0=0; table0<8; table0++) {
			if(ht_big[table0].linmax >= max)
				break;
		}
		
		for(table1=8; table1<16; table1++) {
			if(ht_big[table1].linmax >= max)
				break;
		}
		
		return 16 + count_bigv(ix, begin, end, table0, table1, bits);
	}
}

int find_best_2(short *ix, unsigned int start, unsigned int end, const unsigned int *table,
                unsigned int len, int *bits)
{
	unsigned int i, sum = 0;
	
	for(i=start; i<end; i+=2)
		sum += table[ix[i] * len + ix[i+1]];
	
	if((sum & 0xffff) <= (sum >> 16))
	{
		*bits = (sum & 0xffff);
		return 1;
	}
	else
	{
		*bits = sum >> 16;
		return 0;
	}
}

int find_best_3(short *ix, unsigned int start, unsigned int end, const unsigned int *table,
                unsigned int len, int *bits)
{
	unsigned int i, j, sum  = 0;
	int          sum1 = 0;
	int          sum2 = 0;
	int          sum3 = 0;
	
	/* avoid overflow in packed additions: 78*13 < 1024 */
	for(i=start; i<end; )
	{
		j = i + 2*78 > end ? end : i + 2*78;
		
		for(sum=0; i<j; i+=2)
			sum += table[ix[i] * len + ix[i+1]];
		
		sum1 += (sum >> 20);
		sum2 += (sum >> 10) & 0x3ff;
		sum3 += (sum >>  0) & 0x3ff;
	}
	
	i = 0;
	if(sum1 > sum2) { sum1 = sum2;  i = 1; }
	if(sum1 > sum3) { sum1 = sum3;  i = 2; }
	
	*bits = sum1;
	
	return i;
}

/*************************************************************************/
/* Function: Count the number of bits necessary to code the subregion.   */
/*************************************************************************/
int count_bit1(short *ix, unsigned int start, unsigned int end, int *bits )
{
	unsigned int i, sum = 0;
	
	for(i=start; i<end; i+=2)
		sum += t1l[4 + ix[i] * 2 + ix[i+1]];
	
	*bits = sum;
	
	return 1; /* this is table1 */
}

int count_bigv(short *ix, unsigned int start, unsigned int end, int table0,
               int table1, int *bits )
{
	unsigned int  i, sum0, sum1, sum=0, bigv=0, x, y;
	
	/* ESC-table is used */
	for(i=start; i<end; i+=2)
	{
		x = ix[i];
		y = ix[i+1];
		
		if(x > 14) { x = 15; bigv++; }
		if(y > 14) { y = 15; bigv++; }
		
		sum += tab1624[x * 16 + y];
	}
	
	sum0 = (sum  >>  16)  + bigv * ht_big[table0].linbits;
	sum1 = (sum & 0xffff) + bigv * ht_big[table1].linbits;
	
	if(sum0 <= sum1)
	{
		*bits = sum0;
		return table0;
	}
	else
	{
		*bits = sum1;
		return table1;
	}
}
