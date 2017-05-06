//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    codebook.c

Abstract:

    decode header 3 block, shared code book.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#include <stdlib.h>

#include "macros.h"
#include "codebook.h"
#include "vovorbisdec.h"

/**** pack/unpack helpers ******************************************/
int ILOG(unsigned int v)
{
    int ret = 0;
    while(v)
	{
        ret++;
        v >>= 1;
    } 
    return(ret);
}

int _ILOG(unsigned int v)
{
    int ret = 0;
	if(v)--v;
    while(v)
	{
        ret++;
        v >>= 1;
    } 
    return(ret);
}



/* there might be a straightforward one-line way to do the below
   that's portable and totally safe against roundoff, but I haven't
   thought of it.  Therefore, we opt on the side of caution */
long voOGGDecBookMaptype1Quantvals(const StaticCodebook *b)
{
    /* get us a starting hint, we'll polish it below */
    int bits = ILOG(b->m_Entries);
    int vals = b->m_Entries >> ((bits-1) * (b->m_Dim-1) / b->m_Dim);
	int i;
    while(1)
	{
        long acc = 1;
        long acc1 = 1;
        for (i=0; i<b->m_Dim; i++)
		{
            acc *= vals;
            acc1 *= vals + 1;
        }
        if (acc<=b->m_Entries && acc1>b->m_Entries)
		{
            return(vals);
        }else{
            if (acc>b->m_Entries)
			{
	            vals--;
            }else{
	            vals++;
            }
        }
    }
}

void voOGGDecClearStaticbook(CVOVorbisObjectDecoder *pvorbisdec, StaticCodebook *b)
{
    if (b->m_pQuantList)
		voOGGFree(b->m_pQuantList);

    if (b->m_pLenthList)
		voOGGFree(b->m_pLenthList);
    
	voOGGMemset(b,0,sizeof(*b));
}

void voOGGDecFreeStaticbook(CVOVorbisObjectDecoder *pvorbisdec, StaticCodebook *b)
{
	voOGGDecClearStaticbook(pvorbisdec, b);
	voOGGFree(b);
}


void voOGGDecClearVorbisBook(CVOVorbisObjectDecoder *pvorbisdec, CodeBook *b)
{
	/* static book is not cleared; we're likely called on the lookup and
	the static codebook belongs to the info struct */
	if (b->m_pValueList)
		voOGGFree(b->m_pValueList);

	if (b->m_pCodeList)
		voOGGFree(b->m_pCodeList);

	if (b->m_pDecIndex)
		voOGGFree(b->m_pDecIndex);

	if (b->m_pDecCodeLengths)
		voOGGFree(b->m_pDecCodeLengths);

	if (b->m_pDecFirstTable)
		voOGGFree(b->m_pDecFirstTable);

	voOGGMemset(b, 0, sizeof(*b));
}


OGG_U32 BitReverseCode(OGG_U32 code, long len){
	OGG_U32 temp = 0;
	int j;
	for(j=0; j<len; j++)
	{
		temp <<= 1;
		temp |= (code>>j)&1;
	}
	return temp;
}

/* given a list of word lengths, generate a list of codewords.  Works
   for length ordered or unordered, always assigns the lowest valued
   codewords first.  Extended to handle unused entries (when length is 0) */
OGG_U32 *MakeWords(CVOVorbisObjectDecoder *pvorbisdec, long *l, long n, long sparsecount)
{
	long i, j, length, count = 0;
	OGG_U32 marker[33] = {1,0};
	OGG_U32 *r = (OGG_U32 *)voOGGMalloc((sparsecount?sparsecount:n)*sizeof(*r));


	for(i=0; i<n; i++)
	{
		length = l[i];
		if (length>0&&length<32)
		{
			OGG_U32 entry = marker[length];

			/*step 1. get one code, check validity firstly */
			if((entry>=(OGG_U32)(1<<length)))
			{
				/* error condition; the lengths must specify an overpopulated tree */
				voOGGFree(r);
				return(NULL);
			}

			/*step 2.  */			
			for(j=length; j>0; j--)
			{
				if(marker[j]&1)
				{
					/* have to jump branches */					
					marker[j] = marker[j-1]<<1;
					break;
				}
				marker[j]++;
			}
			r[count++] = BitReverseCode(entry,length);
			

			/*step 3. make sure longer codes could not have same prefix as previous one ; */
			for(j=length+1; j<33; j++){
				if((marker[j]>>1) == entry)
				{
					entry = marker[j];
					marker[j] = marker[j-1]<<1;
				}else{
					break;
				}
			}

		}//end of if (length>0)
		else if(length>32||length<0){
				/* error condition; the lengths must < 32 */
				voOGGFree(r);
				return(NULL);
		}
		else{
				if(sparsecount==0)
					count++;
		}
	}//end of for(i=0; i<n; i++)

	return(r);
}

static int sort32a(const void *a, const void *b)
{
  return (**(OGG_U32 **)a>**(OGG_U32 **)b)-
    (**(OGG_U32 **)a<**(OGG_U32 **)b);
}

/* 32 bit float (not IEEE; nonnormalized mantissa +
   biased exponent) : neeeeeee eeemmmmm mmmmmmmm mmmmmmmm 
   Why not IEEE?  It's just not that important here. */

#define VQ_FEXP 10
#define VQ_FMAN 21
#define VQ_FEXP_BIAS 768 /* bias toward values smaller than 1. */

static OGG_S32 Float32Parse(long val,int *point)	//_float32_unpack
{
  long   mant = val&((1<<VQ_FMAN)-1);
  int    sign = 0;
  long   exp = -9999; 

  if(mant)
  {
	  sign = val>>31;
	  exp = (val>>VQ_FMAN)&0x3ff;

	  while((mant<0x40000000))
	  {
		  mant *= 2;
		  exp -= 1;
	  }

	  exp -= (VQ_FMAN-1)+VQ_FEXP_BIAS;

	  if(sign){
		  mant = -mant;
	  }
  }

  *point = exp;
  return mant;
}

/* we repack the book in a fixed point format that shares the same
   binary point.  Upon first use, we can shift point if needed */

/* we need to deal with two map types: 
   in map type 1, the values are   generated algorithmically 
   (each column of the vector counts throug  the values in the quant vector);
   in map type 2, all the values came  in in an explicit list. 
   Both value lists must be unpacked */

OGG_S32 *BookUnquantize(CVOVorbisObjectDecoder *pvorbisdec, const StaticCodebook *b, 
							int n, int *sparsemap, int *maxpoint)	
{
	if(b->m_MapType==1 || b->m_MapType==2)
	{
		long j, k, count=0;
		int quantvals;
		int minpoint, delpoint;
		OGG_S32 mindel = Float32Parse(b->m_Qmin, &minpoint);
		OGG_S32 delta = Float32Parse(b->m_Qdelta, &delpoint);
		OGG_S32 *r = (OGG_S32 *)voOGGCalloc(n*b->m_Dim, sizeof(*r));
		int *rp = (int *)voOGGCalloc(n*b->m_Dim, sizeof(*rp));
		if(r==NULL){
			return(NULL);
		}
		if(rp==NULL){
			voOGGFree(r);
			return(NULL);
		}

		*maxpoint = minpoint;

		/* maptype 1 and 2 both use a quantized value vector, however use
		different sizes */
		switch(b->m_MapType)
		{
		case 1:			
			quantvals = voOGGDecBookMaptype1Quantvals(b);
			for(j=0; j<b->m_Entries; j++)
			{
				if((sparsemap && b->m_pLenthList[j]) || !sparsemap)
				{
					OGG_S32 last = 0;
					int lastpoint = 0;
					int indexdiv = 1;
					for(k=0; k<b->m_Dim; k++)
					{
						int index = (j/indexdiv)%quantvals;
						int point = 0;
						int val = VFLOAT_MULTI(delta, delpoint,abs(b->m_pQuantList[index]), &point);

						indexdiv *= quantvals;

						val = VFLOAT_ADD(mindel, minpoint, val, point, &point);
						
						if (b->m_Qsequencep)
						{
							val = VFLOAT_ADD(last, lastpoint, val, point, &point);  //risky?
							last = val;	  
							lastpoint = point;
						}

						if(sparsemap)
						{
							r[sparsemap[count]*b->m_Dim+k] = val;
							rp[sparsemap[count]*b->m_Dim+k] = point;
						}else{
							r[count*b->m_Dim+k] = val;
							rp[count*b->m_Dim+k] = point;
						}
						if (*maxpoint<point)
							*maxpoint = point;
					
					}
					count++;
				}
			}
			break;
		case 2:
			for(j=0; j<b->m_Entries; j++)
			{
				if((sparsemap && b->m_pLenthList[j]) || !sparsemap)
				{
					OGG_S32 last = 0;
					int         lastpoint = 0;

					for(k=0; k<b->m_Dim; k++)
					{
						int point = 0;
						int val = VFLOAT_MULTI(delta, delpoint,
							abs(b->m_pQuantList[j*b->m_Dim+k]), &point);

						val = VFLOAT_ADD(mindel,minpoint,val,point,&point);
						val = VFLOAT_ADD(last,lastpoint,val,point,&point);

						if (b->m_Qsequencep)
						{
							last = val;	  
							lastpoint = point;
						}

						if (sparsemap)
						{
							r[sparsemap[count]*b->m_Dim+k] = val;
							rp[sparsemap[count]*b->m_Dim+k] = point;
						}else{
							r[count*b->m_Dim+k] = val;
							rp[count*b->m_Dim+k] = point;
						}
						if(*maxpoint<point)*maxpoint = point;
					}
					count++;
				}
			}
			break;
		}
		for (j=0; j<n*b->m_Dim; j++)
			if (rp[j]<*maxpoint)
				r[j] >>= *maxpoint - rp[j];

		voOGGFree(rp);
		return(r);
	}
	return(NULL);
}

/* decode codebook arrangement is more heavily optimized than encode */
int VorbisBookDecodeInit(CVOVorbisObjectDecoder *pvorbisdec, CodeBook *c, const StaticCodebook *s)//vorbis_book_init_decode
{
	int i, j, n=s->m_Entries, tabn;

	/* count actually used entries */
	for(i=0; i<s->m_Entries; i++){
		if (s->m_pLenthList[i]<=0){
			n--;
		}
	}

	voOGGMemset(c, 0, sizeof(*c));

	if (n>0)
	{
		/* two different remappings go on here. 	 */
		OGG_U32 *codes = MakeWords(pvorbisdec, s->m_pLenthList, s->m_Entries, n);
		int *sortindex = NULL;
		OGG_U32 **codep = NULL;	

		c->m_Entries = s->m_Entries;
		c->m_UsedEntries = n;
		c->m_Dim = s->m_Dim;

		if(codes==NULL)
		{
			voOGGDecClearVorbisBook(pvorbisdec, c);			
			return(-1);
		}

		c->m_pCodeList=(OGG_U32 *)voOGGMalloc(n*sizeof(*c->m_pCodeList));
		codep = (OGG_U32 **)voOGGCalloc(n, sizeof(*codep));
		sortindex = (int *)voOGGCalloc(n, sizeof(*sortindex));
		if(codep == NULL||sortindex == NULL||c->m_pCodeList == NULL){
			voOGGDecClearVorbisBook(pvorbisdec, c);		
			voOGGFree(codes);
			return(-1);
		}

		for(i=0; i<n; i++)
		{
			codep[i] = &codes[i];
			codes[i] = bitreverse(codes[i]); 			
		}
		
		/* perform sort and set related variables*/
		qsort(codep, n, sizeof(*codep), sort32a);
	
		for(i=0; i<n; i++)
		{
			int position = codep[i] - codes;
			sortindex[position] = i;
		}

		for(i=0; i<n; i++)
			c->m_pCodeList[sortindex[i]] = codes[i];

		c->m_pValueList = BookUnquantize(pvorbisdec, s, n, sortindex, &c->m_BinaryPoint);//fix point maptype1 and maptype2
		c->m_pDecIndex = (int *)voOGGMalloc(n*sizeof(*c->m_pDecIndex));


		c->m_pDecCodeLengths = (char *)voOGGMalloc(n*sizeof(*c->m_pDecCodeLengths));
		for (n=0,i=0; i<s->m_Entries; i++){
			if (s->m_pLenthList[i]>0){
				c->m_pDecCodeLengths[sortindex[n]] = (char)s->m_pLenthList[i];
				c->m_pDecIndex[sortindex[n++]] = i;
			}
		}

		/* generate lookup talbes for Huffman*/
		c->m_DecFirstTableN = ILOG(c->m_UsedEntries)-4; /* this is magic */
		if (c->m_DecFirstTableN<5)
			c->m_DecFirstTableN = 5;
		if (c->m_DecFirstTableN>8)
			c->m_DecFirstTableN = 8;

		tabn = 1<<c->m_DecFirstTableN;
		c->m_pDecFirstTable = (OGG_U32 *)voOGGCalloc(tabn, sizeof(*c->m_pDecFirstTable));
		c->m_DecMAXLength = c->m_DecFirstTableN;  //m_DecFirstTableN is more likely to be the max value, not 0

		for(i=0; i<n; i++)
		{

			if (c->m_pDecCodeLengths[i]<=c->m_DecFirstTableN)
			{
				OGG_U32 orig = bitreverse(c->m_pCodeList[i]);
				for (j=0; j<(1<<(c->m_DecFirstTableN-c->m_pDecCodeLengths[i])); j++){
					c->m_pDecFirstTable[orig|(j<<c->m_pDecCodeLengths[i])] = i+1;
				}
			}else if (c->m_DecMAXLength<c->m_pDecCodeLengths[i]){
				c->m_DecMAXLength = c->m_pDecCodeLengths[i];
			}
		}

		/* now fill in 'unused' entries in the firsttable with hi/lo search
		hints for the non-direct-hits */
		{
			OGG_U32 mask = 0xfffffffeUL<<(31-c->m_DecFirstTableN);
			long lo=0, hi=0;
			unsigned long loval;
			unsigned long hival;

			for(i=0; i<tabn; i++)
			{
				OGG_U32 word = i<<(32-c->m_DecFirstTableN);
				if (c->m_pDecFirstTable[bitreverse(word)]==0)
				{
					lo = hi;  //start to cal lo from last hi, which should be faster, maybe risky?
					while((lo+1)<n && c->m_pCodeList[lo+1]<=word){
						lo++;
					}
					while( hi<n && word>=(c->m_pCodeList[hi]&mask)){
						hi++;
					}

					/* only actually have 15 bits per hint to play with here.
					In order to overflow gracefully (nothing breaks, efficiency
					just drops), encode as the difference from the extremes. */
					
					loval = lo;
					if (loval>0x7fff){
						loval = 0x7fff;
					}

					hival =  hi;
					if (hival>0x7fff){
						hival = 0x7fff;
					}

					c->m_pDecFirstTable[bitreverse(word)] = 0x80000000UL | (hival<<16) | loval;
					
				}
			}
		}//end of fill in 'unused' entries
		if(codep != NULL)
			voOGGFree(codep);
		if(sortindex != NULL)
			voOGGFree(sortindex);
		if(codes != NULL)
			voOGGFree(codes);
	}//end of if (n>0)
	
	return(0);
}



