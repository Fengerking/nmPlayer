/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: category.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM   
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.   
 * All Rights Reserved.   
 *   
 * The contents of this file, and the files included with this file, 
 * are subject to the current version of the RealNetworks Community 
 * Source License (the "RCSL"), including Attachment G and any 
 * applicable attachments, all available at 
 * http://www.helixcommunity.org/content/rcsl.  You may also obtain 
 * the license terms directly from RealNetworks.  You may not use this 
 * file except in compliance with the RCSL and its Attachments. There 
 * are no redistribution rights for the source code of this 
 * file. Please see the applicable RCSL for the rights, obligations 
 * and limitations governing use of the contents of the file. 
 *   
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the 
 * portions it created. 
 *   
 * This file, and the files included with this file, is distributed 
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
 * ENJOYMENT OR NON-INFRINGEMENT. 
 *   
 * Technology Compatibility Kit Test Suite(s) Location:   
 * https://rarvcode-tck.helixcommunity.org   
 *   
 * Contributor(s):   
 *   
 * ***** END LICENSE BLOCK ***** */   
 
/*
 * Gecko2 stereo audio codec.
 * Developed by Ken Cooke (kenc@real.com)
 * August 2000
 *
 * This is Ken's new smoking-fast version
 *  - uses incrementally updated heaps
 *  - 3x speedup vs. original
 *
 */
// Define OPT_CATEGORIZE to enable the optimized version of RA_TNI_Categorize ( merged with RA_TNI_ExpandCategory)

// Define CATEGORIZE_ASM to enable the assembly version of RA_TNI_Categorize ( merged with RA_TNI_ExpandCategory)

#include "coder.h"


/* heap indexing, where heap[1] is root */
#define PARENT(i) ((i) >> 1)
#define LCHILD(i) ((i) << 1)
#define RCHILD(i) (LCHILD(i)+1)




#ifndef OPT_CATEGORIZE
/*
 * Compute the categorizations.
 * This is a symmetrical operation, so the decoder goes through
 *   the same process as the encoder when computing the categorizations.
 *   The decoder builds them all and chooses the one given by rate_code,
 *		which is extracted from the bitstream by RA_TNI_DecodeEnvelope()
 * rms_index[i] is the RMS power of region i
 * NCATZNS = (1 << RATEBITS) = max number of categorizations we can try
 *
 * cat runs from 0 to 7. Lower number means finer quantization.
 * RA_TNI_expbits_tab[i] tells us how many bits we should expect to spend
 *   coding a region with cat = i
 *
 * When finished:
 *   catbuf contains the best (most bits) categorization (length = nRegions)
 *   catlist is a list of region numbers used to build all the categorizations
 *      (on pass i we do catbuf[catlist[i]]++, making the quantizer in region
 *       catlist[i] one notch coarser)
 *
 * Fixed-point changes:
 *  - none
 */
void RA_TNI_Categorize(short *rms_index, short availbits, short *catbuf, short *catlist, short ncatzns, RA_TNI_Obj *bitstrmPtr)
{
	short r, n;
	short offset, delta, cat;
	short expbits, maxbits, minbits;
	short maxcat[MAXCREGN], mincat[MAXCREGN];
	short changes[2*MAXCATZNS];					/* grows from middle outward */
	short maxptr, minptr, nchanges;
	/* heaps */
	long maxheap[MAXCREGN+1] = { 0x7fffffff };	/* upheap sentinel */
	long minheap[MAXCREGN+1] = { 0x80000000 };	/* upheap sentinel */
	short nminheap = 0, nmaxheap = 0, k;

	short cregions, catPrefix;
	long val;

	/* Hack to compensate for different statistics at higher bits/sample */
	if (availbits > bitstrmPtr->nsamples)
		availbits = bitstrmPtr->nsamples + (((availbits - bitstrmPtr->nsamples) * 5)>>3);
	/*
	 * Initial categorization.
	 *
	 * This attempts to assigns categories to regions using
	 * a simple approximation of perceptual masking.
	 * Offset is chosen via binary search for desired bits.
	 */

	cregions = bitstrmPtr->nregions + bitstrmPtr->cplstart;

	offset = -32;	/* start with minimum offset */
	for (delta = 32; delta > 0; delta >>= 1) {

		expbits = 0;
		catPrefix = offset + delta;
		for (r = 0; r < cregions; r++) {

			/* Test categorization at (offset+delta) */
			cat = ((catPrefix - rms_index[r])>>1);
			if (cat < 0) cat = 0;	/* clip */
			if (cat > 7) cat = 7;	/* clip */
			expbits += RA_TNI_expbits_tab[cat];	/* tally expected bits */
		}
		/* Find largest offset such that (expbits >= availbits-32) */
		if (expbits >= availbits-32)	/* if still over budget, */
			offset += delta;			/* choose increased offset */
	}

	/* Use the selected categorization */
	expbits = 0;
	for (r = 0; r < cregions; r++) {
		cat = ((offset - rms_index[r])>>1);
		if (cat < 0) cat = 0;	/* clip */
		if (cat > 7) cat = 7;	/* clip */
		expbits += RA_TNI_expbits_tab[cat];
		mincat[r] = cat;	/* init */
		maxcat[r] = cat;	/* init */

		val = offset - rms_index[r] - (cat<<1);
		val = (val << 16) | r;	/* max favors high r, min favors low r */

		/* build maxheap */
		if (cat < 7) {
			/* insert at heap[N+1] */
			k = ++nmaxheap;
		//	maxheap[k] = val;
			/* upheap */
			while (val > maxheap[PARENT(k)]) {
				maxheap[k] = maxheap[PARENT(k)];
				k = PARENT(k);
			}
			maxheap[k] = val;
		}

		/* build minheap */
		if (cat > 0) {
			/* insert at heap[N+1] */
			k = ++nminheap;
			minheap[k] = val;
			/* upheap */
			while (val < minheap[PARENT(k)]) {
				minheap[k] = minheap[PARENT(k)];
				k = PARENT(k);
			}
			minheap[k] = val;
		}
	}

	/* init */
	/* minbits = expbits; */
	maxbits = minbits = expbits;
	minptr = ncatzns;	/* grows up, post-increment */
	maxptr = ncatzns;	/* grows down, pre-decrement */

	/*
	 * Derive additional categorizations.
	 *
	 * Each new categorization differs from the last in a single region,
	 * where the categories differ by one, and are ordered by decreasing
	 * expected bits.
	 */
	for (n = 1; n < ncatzns; n++) {
		/* Choose whether new cat should have larger/smaller bits */

		if (maxbits + minbits <= 2*availbits) {
			/* if average is low, add one with more bits */
			if (!nminheap) {
				/* printf("all quants at min\n"); */
				break;
			}
			r = minheap[1] & 0xffff;

			/* bump the chosen region */
			changes[--maxptr] = r;				/* add to change list */
			maxbits -= RA_TNI_expbits_tab[maxcat[r]];	/* sub old bits */
			maxcat[r] -= 1;						/* dec category */
			maxbits += RA_TNI_expbits_tab[maxcat[r]];	/* add new bits */

			/* update heap[1] */
			k = 1;
			if (maxcat[r] == 0)
				minheap[k] = minheap[nminheap--];	/* remove */
			else
				minheap[k] += ((long)2 << 16);			/* update */

			/* downheap */
			val = minheap[k];
			while (k <= PARENT(nminheap)) {
				short child = LCHILD(k);
				short right = RCHILD(k);
				if ((right <= nminheap) && (minheap[right] < minheap[child]))
					child = right;
				if (val < minheap[child])
					break;
				minheap[k] = minheap[child];
				k = child;
			}
			minheap[k] = val;

		} else {
			/* average is high, add one with less bits */
			if (!nmaxheap) {
				/* printf("all quants at max\n"); */
				break;
			}
			r = maxheap[1] & 0xffff;

			/* bump the chosen region */
			changes[minptr++] = r;				/* add to change list */
			minbits -= RA_TNI_expbits_tab[mincat[r]];	/* sub old bits */
			mincat[r] += 1;						/* inc category */
			minbits += RA_TNI_expbits_tab[mincat[r]];	/* add new bits */

			/* update heap[1] */
			k = 1;
			if (mincat[r] == 7)
				maxheap[k] = maxheap[nmaxheap--];	/* remove */
			else
				maxheap[k] -= ((long)2 << 16);			/* update */

			/* downheap */
			val = maxheap[k];
			while (k <= PARENT(nmaxheap)) {
				short child = LCHILD(k);
				short right = RCHILD(k);
				if ((right <= nmaxheap) && (maxheap[right] > maxheap[child]))
					child = right;
				if (val > maxheap[child])
					break;
				maxheap[k] = maxheap[child];
				k = child;
			}
			maxheap[k] = val;
		}
	}

	/* return largest categorization */
	for (r = 0; r < cregions; r++)
		catbuf[r] = maxcat[r];

	/* return change list */
	nchanges = minptr - maxptr;
	for (n = 0; n < nchanges; n++)
		catlist[n] = changes[maxptr++];

	/* when change list is short, fill with -1 */
	for (n = nchanges; n < ncatzns-1; n++)
		catlist[n] = -1;
}

/*
 * Since the categorizations are computed differentially, we build the
 *   first one then go through the change list (catlist[]) bumping one
 *   region at a time.
 * rate_code is the number of the desired categorization.
 * Overwrites catbuf.
 *
 * Fixed-point changes:
 *  - none
 */
void RA_TNI_ExpandCategory(short *catbuf, short *catlist, short rate_code)
{
	short c;

	for (c = 0; c < rate_code; c++) {
		ASSERT(catlist[c] != -1);
		catbuf[catlist[c]] += 1;	/* bump region to change */
	}
}

#else //OPT_CATEGORIZE

////////////////////////////////////////////////////////////////////////////////////////////////
// Optimized version of RA_TNI_Categorize. 
// RA_TNI_ExpandCategory function has been merged into this function
//
//
#ifndef CATEGORIZE_ASM

void RA_TNI_Categorize(short *rms_index, short availbits, short *catbuf,  short ncatzns, RA_TNI_Obj *bitstrmPtr, short rate_code)
{
	short r, n,  c;
	short offset, delta, cat;
	short expbits, maxbits, minbits;
	// Firdaus : Optimization changes -- use catbuf in place of maxcat
	short /*maxcat[MAXCREGN],*/ mincat[MAXCREGN];
	short changes[2*MAXCATZNS];					/* grows from middle outward */
	short maxptr, minptr, nchanges;
	/* heaps */
	long maxheap[MAXCREGN+1] = { 0x7fffffff };	/* upheap sentinel */
	long minheap[MAXCREGN+1] = { 0x80000000 };	/* upheap sentinel */
	short nminheap = 0, nmaxheap = 0, k;

	short cregions, catPrefix;
	long val;

	/* Hack to compensate for different statistics at higher bits/sample */
	if (availbits > bitstrmPtr->nsamples)
		availbits = bitstrmPtr->nsamples + (((availbits - bitstrmPtr->nsamples) * 5)>>3);
	/*
	 * Initial categorization.
	 *
	 * This attempts to assigns categories to regions using
	 * a simple approximation of perceptual masking.
	 * Offset is chosen via binary search for desired bits.
	 */

	cregions = bitstrmPtr->nregions + bitstrmPtr->cplstart;

	offset = -32;	/* start with minimum offset */
	for (delta = 32; delta > 0; delta >>= 1) {

		expbits = 0;
		catPrefix = offset + delta;
		for (r = 0; r < cregions; r++) {

			/* Test categorization at (offset+delta) */
			cat = ((catPrefix - rms_index[r])>>1);
			if (cat < 0) cat = 0;	/* clip */
			if (cat > 7) cat = 7;	/* clip */
			expbits += RA_TNI_expbits_tab[cat];	/* tally expected bits */
		}
		/* Find largest offset such that (expbits >= availbits-32) */
		if (expbits >= availbits-32)	/* if still over budget, */
			offset += delta;			/* choose increased offset */
	}

	/* Use the selected categorization */
	expbits = 0;
	for (r = 0; r < cregions; r++) {
		cat = ((offset - rms_index[r])>>1);
		if (cat < 0) cat = 0;	/* clip */
		if (cat > 7) cat = 7;	/* clip */
		expbits += RA_TNI_expbits_tab[cat];
		mincat[r] = cat;	/* init */
		catbuf[r] = cat;	/* init maxcat*/

		val = offset - rms_index[r] - (cat<<1);
		val = (val << 16) | r;	/* max favors high r, min favors low r */

		/* build maxheap */
		if (cat < 7) {
			/* insert at heap[N+1] */
			k = ++nmaxheap;
		//	maxheap[k] = val;
			/* upheap */
			while (val > maxheap[PARENT(k)]) {
				maxheap[k] = maxheap[PARENT(k)];
				k = PARENT(k);
			}
			maxheap[k] = val;
		}

		/* build minheap */
		if (cat > 0) {
			/* insert at heap[N+1] */
			k = ++nminheap;
		//	minheap[k] = val;
			/* upheap */
			while (val < minheap[PARENT(k)]) {
				minheap[k] = minheap[PARENT(k)];
				k = PARENT(k);
			}
			minheap[k] = val;
		}
	}

	/* init */
	/* minbits = expbits; */
	maxbits = minbits = expbits;
	minptr = ncatzns;	/* grows up, post-increment */
	maxptr = ncatzns;	/* grows down, pre-decrement */

	/*
	 * Derive additional categorizations.
	 *
	 * Each new categorization differs from the last in a single region,
	 * where the categories differ by one, and are ordered by decreasing
	 * expected bits.
	 */
	for (n = 1; n < ncatzns; n++) {
		/* Choose whether new cat should have larger/smaller bits */

		if (maxbits + minbits <= 2*availbits) {
			/* if average is low, add one with more bits */
			if (!nminheap) {
				/* printf("all quants at min\n"); */
				break;
			}
			r = minheap[1] & 0xffff;

			/* bump the chosen region */
			changes[--maxptr] = r;				/* add to change list */
			maxbits -= RA_TNI_expbits_tab[catbuf[r]];	/* sub old bits */
			catbuf[r] -= 1;						/* dec category */
			maxbits += RA_TNI_expbits_tab[catbuf[r]];	/* add new bits */

			/* update heap[1] */
			k = 1;
			if (catbuf[r] == 0)
				minheap[k] = minheap[nminheap--];	/* remove */
			else
				minheap[k] += ((long)2 << 16);			/* update */

			/* downheap */
			val = minheap[k];
			while (k <= PARENT(nminheap)) {
				short child = LCHILD(k);
				short right = RCHILD(k);
				if ((right <= nminheap) && (minheap[right] < minheap[child]))
					child = right;
				if (val < minheap[child])
					break;
				minheap[k] = minheap[child];
				k = child;
			}
			minheap[k] = val;

		} else {
			/* average is high, add one with less bits */
			if (!nmaxheap) {
				/* printf("all quants at max\n"); */
				break;
			}
			r = maxheap[1] & 0xffff;

			/* bump the chosen region */
			changes[minptr++] = r;				/* add to change list */
			minbits -= RA_TNI_expbits_tab[mincat[r]];	/* sub old bits */
			mincat[r] += 1;						/* inc category */
			minbits += RA_TNI_expbits_tab[mincat[r]];	/* add new bits */

			/* update heap[1] */
			k = 1;
			if (mincat[r] == 7)
				maxheap[k] = maxheap[nmaxheap--];	/* remove */
			else
				maxheap[k] -= ((long)2 << 16);			/* update */

			/* downheap */
			val = maxheap[k];
			while (k <= PARENT(nmaxheap)) {
				short child = LCHILD(k);
				short right = RCHILD(k);
				if ((right <= nmaxheap) && (maxheap[right] > maxheap[child]))
					child = right;
				if (val > maxheap[child])
					break;
				maxheap[k] = maxheap[child];
				k = child;
			}
			maxheap[k] = val;
		}
	}

	/*
	 * Since the categorizations are computed differentially, we build the
	 *   first one then go through the change list (changes[]) adjusting one
	 *   region at a time.
	 * rate_code is the number of the desired categorization.
	 * Overwrites maxcat.
	 *
	 */
	
	for (c = 0; c < rate_code; c++) 
		catbuf[changes[maxptr++]] += 1;	/* bump region to change */


}
#endif 	//CATEGORIZE_ASM

#endif //OPT_CATEGORIZE

