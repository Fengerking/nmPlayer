/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: codemlt.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Last modified by:	Jon Recker (jrecker@real.com)
 *						01/24/01
 */
#include "coder.h"
#include "stdio.h"


/* Takes the encoded bitstream and decodes it into gain control info, the
 *   rms power envelope, and the MLT vectors
 *
 * Fixed-point changes:
 *  - got rid of DequantEnvelope(), which calculated the power envelope given the
 *     rms_index vector. Instead, this process is now rolled into RA_TNI_DecodeVectors().
 */
void RA_TNI_DecodeMLT(short availbits, long *mlt, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	//reuse the temp buffer - not used until PostMultiply
	short *rms_index = (short*) bitstrmPtr->buf; 
	short *catbuf = rms_index + MAXCREGN ; 

#ifndef OPT_CATEGORIZE
	short catlist[MAXCATZNS-1];
#endif	// OPT_CATEGORIZE
	
	USHORT ratebits;
	USHORT rate_code;
	/* USHORT index, ncatzns; */
	short bitsleft = availbits;

	ratebits = 5;
	if(bitstrmPtr->nchannels == 2 && bitstrmPtr->dualmono == 0 ) {
		// stereo stream only.
		/* index = bitstrmPtr->nsamples>>9; */
		ratebits += (bitstrmPtr->nsamples>>9);
		/* while(index >>= 1) ratebits++; */
	}
	/* ncatzns = (1<<ratebits); */ /* replaced by the original value */

	bitsleft -= RA_TNI_DecodeEnvelope((short *)rms_index, pkbit, pktotal, bitstrmPtr);
	bitsleft -= RA_TNI_Unpackbits(ratebits, &rate_code, pkbit, pktotal, bitstrmPtr);
	ASSERT(bitsleft > 0);

// Define OPT_CATEGORIZE to enable the optimized version of RA_TNI_Categorize ( merged with RA_TNI_ExpandCategory)
#ifndef OPT_CATEGORIZE

	RA_TNI_Categorize((short *)rms_index, bitsleft, (short *)catbuf, (short *)catlist, (1<<ratebits), bitstrmPtr);
	RA_TNI_ExpandCategory((short *)catbuf, (short *)catlist, rate_code);

#else // OPT_CATEGORIZE 
/**/
	RA_TNI_Categorize((short *)rms_index, bitsleft, (short *)catbuf, (1<<ratebits), bitstrmPtr, rate_code);

#endif //OPT_CATEGORIZE

#ifdef CONST_TABLE_DMA
	// wait for sqvh tables
    ACPY2_wait( bitstrmPtr->dmaChannel_0 );
#endif


	RA_TNI_DecodeVectors((short *)catbuf, (short *)rms_index, mlt, pkbit, pktotal, bitstrmPtr);

}





