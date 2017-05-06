/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: coder.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
#include <stdlib.h>
#include "coder.h"
#include "fixtabs.h"

/* NOTE : refer to "\Docs\PP directives.txt" for list of pre-processor directives defined/
 *			used in the project 
 */
		   

#ifndef XDAIS_IALG		// defined to XDAIS interface IALG instead
/* Set global variables and initialize tables used in decoding
 *
 * Fixed-point changes:
 *  - none
 */
int InitDecoder(short nSamples, short nChannels, short nRegions, short nFrameBits, unsigned short sampRate,
				short cplStart, short cplQbits,	short *codingDelay, RA_TNI_Obj *bitstrmPtr)
{

	/* store in globals, for now */
	
	// [fj] : 06/02/2003 11:40
	bitstrmPtr->dualmono = (nChannels > 1 ) && (cplQbits == 0 ) ;
	
	bitstrmPtr->nsamples   = nSamples;
	bitstrmPtr->nchannels  = nChannels;
	bitstrmPtr->nregions   = nRegions;
	bitstrmPtr->nframebits = (bitstrmPtr->dualmono) ? nFrameBits / nChannels : nFrameBits ;
	bitstrmPtr->samprate   = sampRate;

	if (bitstrmPtr->nchannels == 2) {
		bitstrmPtr->cplstart = cplStart;
		bitstrmPtr->cplqbits = cplQbits;
	} else {	/* mono */
		bitstrmPtr->cplstart = 0;
		bitstrmPtr->cplqbits = 0;
	}

	InitDecBuffers(bitstrmPtr);

	// [fj] 10/02/2003 12:08 : Removing the newmlt buffers
	*codingDelay = CODINGDELAY-1;

	return 0;
}


/*
 * Globals
 *
 * Fixed-point changes:
 *  - float buffers made int (note that these are initially filled with zeros)
 *
 * Stereo changes:
 *  - doubled up the buffers
 *  - pointers, buffers inited in InitDecBuffers instead of statically
 */
void InitDecBuffers(RA_TNI_Obj *bitstrmPtr)
{
	int ch, i;

	/* long **oldmlt = bitstrmPtr->oldmlt, *newmlt[MAXCHAN] = bitstrmPtr->newmlt; */

	GAINC (*dgainc)[3] = bitstrmPtr->dgainc;
	
	//[fj] 11/02/2003 13:56
	//Removing the newmlt buffers

	for (ch = 0; ch < CH; ch++) {

		for (i = 0; i < 3; i++)
			dgainc[ch][i].nats = 0;

		for (i = 0; i < MAXNSAMP ; i++) {
			bitstrmPtr->decmlt[ch][i] = 0;
		
		}
		for (i = 0; i < bitstrmPtr->nsamples ; i++) 
			bitstrmPtr->overlap[ch][i] = 0;

		bitstrmPtr->oldlost = 0;
	}
	bitstrmPtr->numCalls = 0;
}



/*
 * Decode, with loss interpolation.
 *
 * NOTES:
 * -doesn't touch codebuf when lost.
 * -avoids using frame with attacks for interpolation.
 * -always shift gainc, use either gainc[0] or gainc[2] when gainc[1] lost.
 *
 * Fixed-point changes:
 *  - integer buffers
 *  - loss interpolation uses shift and add instead of multiplication
 *  - overlap-add and rounding to short now done in RA_TNI_GainCompensate
 *
 * Stereo changes:
 *  - doubled up buffers
 *  - calls RA_TNI_JointDecodeMLT for stereo data
 */
int Decode(UCHAR *codebuf, int lostflag, short *outbuf, REAL_TI_Obj *bitstrmPtr)
{
	short i, ch, availbits, pkbit, pktotal;
	//[fj] 11/02/2003 13:56
	//Removing the newmlt buffers
	GAINC (*dgainc)[3] = bitstrmPtr->dgainc;
	long *decpcm = bitstrmPtr->decpcm;
	

	/*
	 * If valid, decode the incoming frame into lookahead mlt.
	 */
	bitstrmPtr->numCalls++;

	if (!lostflag) {
		
		if( bitstrmPtr->dualmono ) { //[fj] 06/02/2003 11:40
		/* dual-mono case */
		
  		for( ch = 0 ; ch < bitstrmPtr->nchannels; ch++) {
				/* init bitstream */
				pkbit = 0;
				pktotal = 0;

		  		bitstrmPtr->pkptr = (ULONG *)codebuf; // not required [fj]
				
				availbits = bitstrmPtr->nframebits;
				
				DecodeBytes((USHORT *)codebuf, bitstrmPtr->nframebits, bitstrmPtr->pkptr);
				
				/* decode gain control info */
				availbits -= DecodeGainInfo(&dgainc[ch][2], &pkbit, &pktotal, bitstrmPtr);
				
				/* decode the mlt */+
				//[fj] 11/02/2003 13:56 Removing the newmlt buffers
				DecodeMLT(availbits, bitstrmPtr->decmlt[ch],  &pkbit, &pktotal, bitstrmPtr);
				
					// move the codebuf pointer ahead only if it can be aligned on a dword 
				// boundary
				if( ! (bitstrmPtr->nframebits >>3 & 0x01 ) 
						&& !(bitstrmPtr->nframebits >> 4 & 0x01) )
					codebuf += bitstrmPtr->nframebits >> 4 ; /*next channel - in words */
				else{
					/* shift the entire buffer */
					
					// the next frame is both byte and word misaligned - left shift 24
					if( (bitstrmPtr->nframebits >>3 & 0x01 ) && (bitstrmPtr->nframebits >> 4 & 0x01) ){
						UCHAR *shft_temp_dst = codebuf ;
						UCHAR *shft_temp_src = codebuf + (bitstrmPtr->nframebits>>4) ; 
						int counter ; 
						for( counter =(bitstrmPtr->nframebits >> 4); 
						 		counter >= 0 ; counter-- ){
						 	*shft_temp_dst = (*shft_temp_src & 0x00FF ) <<8|
						 						(*(shft_temp_src+1) & 0xFF00 )>>8 ;
						 	shft_temp_dst ++ ;
						 	shft_temp_src ++ ;
						 }
					
					}
					// the next frame is dword misaligned - left shift 16
					else if ((bitstrmPtr->nframebits >> 4 & 0x01) ) {
						UCHAR *shft_temp_dst = codebuf ;
						UCHAR *shft_temp_src = codebuf + (bitstrmPtr->nframebits>>4) ; 
						int counter ;
						// no of bytes to move
						/*locate next channel - in words */
						for( counter =(bitstrmPtr->nframebits >> 4); 
						 counter >= 0 ; counter-- )
							*shft_temp_dst++ = *shft_temp_src++;
					}
					// next frame is byte misaligned - left shift 8
					else{
						UCHAR *shft_temp_dst = codebuf ;
						UCHAR *shft_temp_src = codebuf + (bitstrmPtr->nframebits>>4) ; 
						int counter ; 
						for( counter =(bitstrmPtr->nframebits >> 4); 
						 		counter >= 0 ; counter-- ){
						 	*shft_temp_dst = (*shft_temp_src & 0x00FF )<<8 |
						 						(*(shft_temp_src+1) & 0xFF00 )>>8 ;
						 	shft_temp_dst ++ ;
						 	shft_temp_src ++ ;
						 }
					
					} 
				}
			}

		}

		else{ //[fj] 06/02/2003 11:40
		/* mono or stereo case */
			/* init bitstream */
			pkbit = 0;
			pktotal = 0;
			/* pklimit = bitstrmPtr->nframebits; */
	
#ifdef NOPACK
			bitstrmPtr->pkptr = bitstrmPtr->pkbuf;
#else
			bitstrmPtr->pkptr = (ULONG *)codebuf;
#endif
			availbits = bitstrmPtr->nframebits;
	
#ifdef NOPACK
			DecodeBytes((UCHAR *)codebuf, bitstrmPtr->nframebits, bitstrmPtr->pkptr);
#else
			DecodeBytes((USHORT *)codebuf, bitstrmPtr->nframebits, bitstrmPtr->pkptr);
#endif
	
			/* decode gain control info */
			availbits -= DecodeGainInfo(&dgainc[LEFT][2], &pkbit, &pktotal, bitstrmPtr);
			dgainc[RGHT][2] = dgainc[LEFT][2];	/* replicate for stereo */
	
			/* decode the mlt */
			if (bitstrmPtr->nchannels == 2)
			//[fj] 11/02/2003 13:56 Removing the newmlt buffers
				JointDecodeMLT(availbits, bitstrmPtr->decmlt[LEFT],
						bitstrmPtr->decmlt[RGHT], &pkbit, &pktotal, bitstrmPtr);
			else
			//[fj] 11/02/2003 13:56 Removing the newmlt buffers
				DecodeMLT(availbits, bitstrmPtr->decmlt[LEFT], 
						&pkbit, &pktotal, bitstrmPtr);
		}		
		// [fj] 11/02/2003 12:08 : Removing the newmlt buffers
		/*let the lookahead params = current params */
		dgainc[LEFT][1] = dgainc[LEFT][2] ;
		dgainc[RGHT][1] = dgainc[RGHT][2] ;

	}

	/* If current mlt is lost, create one using interpolation */
	if (bitstrmPtr->oldlost) {
		for (ch = 0; ch < bitstrmPtr->nchannels; ch++) {
			// [fj] 11/02/2003 13:56: Removing the newmlt buffers
			/* current mlt is lost, recreate */
			for (i = 0; i < bitstrmPtr->nsamples; i++)
					bitstrmPtr->decmlt[ch][i] = 
						(bitstrmPtr->decmlt[ch][i] >> 1) + (bitstrmPtr->decmlt[ch][i] >> 2);	
						/* fast multiply by 0.75 */
		}
	}

	if(bitstrmPtr->numCalls < 2) {
		for (ch = 0; ch < bitstrmPtr->nchannels; ch++) {
			
			// [fj] 11/02/2003 13:56: Removing the newmlt buffers
	
			dgainc[ch][0] = dgainc[ch][1];
			dgainc[ch][1] = dgainc[ch][2];	/* keeps previous, if lost */
	
			bitstrmPtr->oldlost = lostflag;
		}
	}
	else {

		/*
		 * Synthesize the current mlt.
		 */
		for (ch = 0; ch < bitstrmPtr->nchannels; ch++) {
			
			/* inverse transform, without overlap */
			IMLTNoOverlap((long *)	bitstrmPtr->decmlt[ch], decpcm, bitstrmPtr->nsamples, bitstrmPtr->buf);
	
	
			/* run gain compensator, then overlap-add */
			/* JR - now interleaves stereo PCM */
			GainCompensate(decpcm, &dgainc[ch][0], &dgainc[ch][1], bitstrmPtr->nsamples,
			                       bitstrmPtr->nchannels, (long *)bitstrmPtr->overlap[ch], outbuf + ch);
	
			// [fj] 11/02/2003 13:56: Removing the newmlt buffers
	
			dgainc[ch][0] = dgainc[ch][1];
			dgainc[ch][1] = dgainc[ch][2];	/* keeps previous, if lost */
	
			bitstrmPtr->oldlost = lostflag;
		}
	}
	
	return 0;
}

#endif 
