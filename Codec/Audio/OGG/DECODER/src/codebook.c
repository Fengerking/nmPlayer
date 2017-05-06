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

    decode header 3 block, code book.

Author:

    Witten Wen 07-September-2009

Revision History:

*************************************************************************/

#include "vovorbisdec.h"

/* unpacks a codebook from the packet buffer into the codebook struct,
   readies the codebook auxiliary structures for decode *************/
int voOGGDecStaticbookUnpack(CVOVorbisObjectDecoder *pvorbisdec, VOOGGInputBitStream *obs, StaticCodebook *s)
{
    long i, j;
    voOGGMemset(s, 0, sizeof(StaticCodebook));

    /* make sure alignment is correct */
    if (voOGGDecPackReadBits(obs, 24) != 0x564342)
		goto _eofout;

    /* first the basic parameters */
    s->m_Dim = voOGGDecPackReadBits(obs, 16);
    s->m_Entries = voOGGDecPackReadBits(obs, 24);
    if (s->m_Entries == -1)
		goto _eofout;

    /* codeword ordering....  length ordered or unordered? */
    switch((int)voOGGDecPackReadBits(obs, 1))
	{
    case 0:
        /* unordered */
        s->m_pLenthList = (long *)voOGGMalloc(sizeof(*s->m_pLenthList) * s->m_Entries);

        /* allocated but unused entries? */
	    if (voOGGDecPackReadBits(obs, 1))
		{
		    /* yes, unused entries */

		    for (i=0; i<s->m_Entries; i++)
			{
			    if (voOGGDecPackReadBits(obs, 1))
				{
				    long num = voOGGDecPackReadBits(obs, 5);
				    if(num == -1)
						goto _eofout;
				    s->m_pLenthList[i] = num + 1;
			    }else
				    s->m_pLenthList[i] = 0;
		    }
	    }else{
		    /* all entries used; no tagging */
		    for (i=0; i<s->m_Entries; i++)
			{
			    long num = voOGGDecPackReadBits(obs, 5);
			    if(num == -1)
					goto _eofout;
			    s->m_pLenthList[i] = num + 1;
		    }
        }
    
        break;
    case 1:
		/* ordered */
		{
		    long length = voOGGDecPackReadBits(obs, 5) + 1;
		    s->m_pLenthList = (long *)voOGGMalloc(sizeof(*s->m_pLenthList) * s->m_Entries);

		    for (i=0; i<s->m_Entries; )
			{
			    long num = voOGGDecPackReadBits(obs, ILOG(s->m_Entries - i));
			    if (num == -1)
					goto _eofout;
			    for(j=0; j<num && i<s->m_Entries; j++, i++)
				    s->m_pLenthList[i] = length;
			    length++;
		    }
	    }
        break;
    default:
        /* EOF */
        return(-1);
    }
  
    /* Do we have a mapping to unpack? */
    switch((s->m_MapType = voOGGDecPackReadBits(obs, 4)))
    {
    case 0:
        /* no mapping */
        break;
    case 1: case 2:
        /* implicitly populated value mapping */
        /* explicitly populated value mapping */

        s->m_Qmin = voOGGDecPackReadBits(obs, 32);
        s->m_Qdelta = voOGGDecPackReadBits(obs, 32);
        s->m_Qquant = voOGGDecPackReadBits(obs, 4) + 1;
        s->m_Qsequencep = voOGGDecPackReadBits(obs, 1);

	    {
		    int quantvals = 0;
		    switch(s->m_MapType)
			{
		    case 1:
			    quantvals = voOGGDecBookMaptype1Quantvals(s);
			    break;
		    case 2:
			    quantvals = s->m_Entries * s->m_Dim;
			    break;
			default:
				goto _errout;
            }
      
            /* read quantized values directly from bitstream */
            s->m_pQuantList = (long *)voOGGMalloc(sizeof(*s->m_pQuantList)*quantvals);
            for (i=0; i<quantvals; i++)
	            s->m_pQuantList[i] = voOGGDecPackReadBits(obs, s->m_Qquant);
      
			if (quantvals && s->m_pQuantList[quantvals-1]==-1){
			    goto _eofout;
			}
        }
        break;
    default:
        goto _errout;
    }

    /* all successfully set */
    return(0);
  
 _errout:
 _eofout:
    voOGGDecClearStaticbook(pvorbisdec, s);
    return(-1); 
}

/* the 'eliminate the decode tree' optimization actually requires the
   codewords to be MSb first, not LSb.  */

OGG_U32 bitreverse(OGG_U32 x)
{
	x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));
}

/* decode one entry from codebook
	step1 ,use m_pDecFirstTable
	step2, if hit m_pDecFirstTable,then return result, otherwise do binary search in  m_pCodeList
   */
STIN long DecodePackedEntryNumber(CodeBook *book, VOOGGInputBitStream *obs)
{	
	long lo, hi;
	long lok	= voOGGDecPackLookBits(obs, book->m_DecFirstTableN);
	int  read ;

	if (lok >= 0) 
	{
		long entry = book->m_pDecFirstTable[lok];
		if (entry < 0)
		{ 
			lo = entry & 0xffff;
			hi = (entry >> 16 ) & 0x7fff; 
		}else{
			//ease case: found right code, return value
			voOGGDecPackAdvBits(obs, book->m_pDecCodeLengths[entry-1]);
			return(entry-1);
		}
	}else{
		lo = 0;
		hi = book->m_UsedEntries;
	}
	
	read	= book->m_DecMAXLength;
	lok = voOGGDecPackLookBits(obs, read);  //look m_DecMAXLength bits

	while (lok<0 && read>1)
		lok = voOGGDecPackLookBits(obs, --read);

	if (lok<0)
	{
		voOGGDecPackAdvBits(obs, 1);		/* failure case, force eop */
		return -1;
	}

	/* binary search for the codeword in the ordered list , the candidata should match lok*/
	{
		OGG_U32 testword = bitreverse((OGG_U32)lok);

		while (hi-lo>1)
		{
			long p = (hi-lo)>>1;
			long test = book->m_pCodeList[lo+p]>testword;
			lo += p & (test-1);
			hi -= p & (-test);
		}

		if(book->m_pDecCodeLengths[lo] <= read)
		{
			voOGGDecPackAdvBits(obs, book->m_pDecCodeLengths[lo]);
			return(lo);
		}
	}

	voOGGDecPackAdvBits(obs, read+1);
	return(-1);
}

long voOGGDecBookDecodevSet(CodeBook *book, OGG_S32 *a,
						  VOOGGInputBitStream *obs, int n, int point)
{
	if (book->m_UsedEntries>0)
	{
		int i, j, entry;
		OGG_S32 *t;
		int shift = point - book->m_BinaryPoint;

		if (shift >= 0)
		{
			for(i=0; i<n;)
			{
				entry = DecodePackedEntryNumber(book, obs);
				if (entry==-1)
					return(-1);
				t = book->m_pValueList + entry * book->m_Dim;
				for (j=0; j<book->m_Dim;)
				{
					a[i++]=t[j++]>>shift;
				}
			}
		}else{

			for (i=0; i<n;)
			{
				entry = DecodePackedEntryNumber(book, obs);
				if (entry==-1)
					return(-1);
				t = book->m_pValueList + entry * book->m_Dim;
				for (j=0; j<book->m_Dim;)
				{
					a[i++] = t[j++]<<-shift;
				}
			}
		}
	}else{

		int i, j;
		for(i=0; i<n;)
		{
			for (j=0; j<book->m_Dim;)
			{
				a[i++] = 0;
			}
		}
	}
	return(0);
}

/* Decode side is specced and easier, because we don't need to find
   matches using different criteria; we simply read and map the table.  There are
   two things we may need to do :
   
   1. We may need to support interleave.  We don't really, but it's
   convenient to do it here rather than rebuild the vector later.

   2. Cascades may be additive or multiplicitive; this is not inherent in
   the codebook, but set in the code using the codebook.  Like
   interleaving, it's easiest to do it here.  
   addmul==0 -> declarative ;
   addmul==1 -> additive ;
   addmul==2 -> multiplicitive */

/* returns the [original, not compacted] entry number or -1 on eof *********/
long voOGGDecBookDecode(CodeBook *book, VOOGGInputBitStream *obs)	//vorbis_book_decode
{
	if (book->m_UsedEntries>0)
	{
		long packed_entry = DecodePackedEntryNumber(book, obs);
		if (packed_entry>=0){
			return(book->m_pDecIndex[packed_entry]);
		}
		else{
			return(-1);
		}
	}

	/* if there's no populated codebook entries */
	return(-1);
}

/* returns 0 on OK or -1 on eof *************************************/
long VorbisBookDecodevsAdd(CVOVorbisObjectDecoder *pvorbisdec, 
							  CodeBook *book,
							  OGG_S32 *a,
							  int n,
							  int point)
{
	if (book->m_UsedEntries>0)
	{
		int step = n / book->m_Dim;
		long entry;
		OGG_S32 **t = (OGG_S32 **)voOGGMalloc(sizeof(*t) * step);
		int i, j, o;
		int shift = point-book->m_BinaryPoint;
		VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;
		for (i = 0; i < step; i++) 
		{
			entry = DecodePackedEntryNumber(book, obs);
			if (entry==-1)
				return(-1);
			t[i] = book->m_pValueList + entry*book->m_Dim;
		}
		if (shift>=0)
		{			
			for(i=0,o=0; i<book->m_Dim; i++,o+=step)
				for (j=0; j<step; j++)
					a[o+j] += t[j][i]>>shift;
		}else{		
			for(i=0,o=0; i<book->m_Dim; i++,o+=step)
				for (j=0; j<step; j++)
					a[o+j] += t[j][i]<<-shift;
		}
		voOGGFree(t);
	}
	return(0);
}

long VorbisBookDecodevAdd(CVOVorbisObjectDecoder *pvorbisdec,
							 CodeBook *book,
							 OGG_S32 *a,
							 int n,
							 int point)
{
	if (book->m_UsedEntries>0)
	{
		int i, j, entry;
		OGG_S32 *t;
		int shift = point-book->m_BinaryPoint;
		VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;

		if (shift>=0)
		{
			for(i=0; i<n;)
			{
				entry = DecodePackedEntryNumber(book, obs);
				if(entry==-1)return(-1);
				t = book->m_pValueList + entry*book->m_Dim;
				for (j=0; j<book->m_Dim;)
					a[i++] += t[j++]>>shift;
			}
		}else{
			for(i=0; i<n;)
			{
				entry = DecodePackedEntryNumber(book, obs);
				if(entry==-1)return(-1);
				t = book->m_pValueList + entry*book->m_Dim;
				for (j=0; j<book->m_Dim;)
					a[i++] += t[j++]<<-shift;
			}
		}
	}
	return(0);
}

long VorbisBookDecodevvAdd(CVOVorbisObjectDecoder *pvorbisdec,
							  CodeBook *book,
							  OGG_S32 **a,
							  long offset,
							  int ch,
							  int n,
							  int point)
{
	if (book->m_UsedEntries>0)
	{
		long i, j, entry;
		int chptr = 0;
		int shift = point-book->m_BinaryPoint;
		VOOGGInputBitStream *obs = pvorbisdec->m_oggbs;

		if (shift>=0)
		{

			for(i=offset; i<offset+n;)
			{
				entry = DecodePackedEntryNumber(book, obs);
				if (entry==-1)return(-1);

				{
					const OGG_S32 *t = book->m_pValueList + entry*book->m_Dim;
					for (j=0; j<book->m_Dim; j++)
					{
						a[chptr++][i] += t[j]>>shift;
						if (chptr==ch)
						{
							chptr = 0;
							i++;
						}
					}
				}
			}
		}else{

			for (i=offset; i<offset+n;)
			{
				entry = DecodePackedEntryNumber(book, obs);
				if (entry==-1)return(-1);

				{
					const OGG_S32 *t = book->m_pValueList+entry*book->m_Dim;
					for (j=0; j<book->m_Dim; j++)
					{
						a[chptr++][i] += t[j]<<-shift;
						if (chptr==ch)
						{
							chptr = 0;
							i++;
						}
					}
				}
			}
		}
	}
	return(0);
}
