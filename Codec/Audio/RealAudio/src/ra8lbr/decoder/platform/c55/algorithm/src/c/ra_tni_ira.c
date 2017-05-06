/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: ra_tni_ira.c,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 *  ======== RA_TNI_iRA.c ========
 *
 *  Description : Implementation of IRA interface for Real Audio decoder algorithm
 *					for RA_TNI decoder module
 *					
 *
 *	Author : Firdaus Janoos
 *  
 */
 
#include <std.h>
#include <iRA.h>
#include "coder.h"
#include "sqvh_tables.h"

/* Definition of default parameters for IRA_Params structure */
/* Worst case values 										 */

#pragma DATA_SECTION( IRA_PARAMS, "params_section")
#pragma DATA_ALIGN( IRA_PARAMS, 2 )
const IRA_Params IRA_PARAMS = {
	sizeof(IRA_Params),		/* Size of this structure */
	1024,					/* Samples per frame : 1024, 512 or 256 */
	2240,					/* No.of bits per frame */
	0,						/* Sampling Rate */
	37,						/* No.of regions per frame */
	2,						/* No. of channels */
	0,						/* loss rate */
	8, 						/* start of coupling info */
	5						/* coupling Q bits */
} ;						
              
//[fj] : Setting burst and pack enabled on OMAP1510 gave significant performance
//		 improvement (~20000 cycles per decode )

// size of the sqvh tables 
// divide by 2 for 32-bit transfers
#define SQVH_TABLE_SIZE  ( 3920 / 2 ) 	

                                 

/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_findSync
   
   Description   :  no idea !! 
   
   Input		 :  Handle to decoder instance, pointer to stream buffer
   Output		 :  --
   Returns		 :  Int
   
   NOTE 		 : No functional implementation
--------------------------------------------------------------------------*/ 	
Int RA_TNI_findSync(IRA_Handle handle, UCHAR *in) 
{
	 
	return 0;
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_decode
   
   Description   :  To perform the decode operation
   
   Input		 :  Handle to decoder instance (IRA_Handle),
   					Pointer to input buffer (UCHAR*),
   					flag indicating loss status of previous frame (Int)
   					 
   Output		 :  Pcm samples buffer
   Returns		 :  
--------------------------------------------------------------------------*/ 	
Int RA_TNI_decode(IRA_Handle Handle, UCHAR *inbuf, Int *outbuf, Int *lostflag)
{
   short i, ch, availbits, pkbit, pktotal;
   
   RA_TNI_Obj *objPtr = (Void *)Handle;			// pointer to decoder instance object
   
   GAINC (*dgainc)[3] = objPtr->dgainc;
   long *decpcm = objPtr->decpcm;
	
   UCHAR *codebuf = inbuf	; 		// pointer to code buffer
   
#ifdef SWAP_ENABLE   
   unsigned int num_words = (((objPtr->nframebits + 7) /8) + !objPtr->dualmono) / (!objPtr->dualmono + 1) ;
   unsigned int num_bytes_evenOrodd = (((objPtr->nframebits + 7) /8) & 0x01);
   /* sreenu */
    
    RA_TNI_byte_swap (codebuf, num_words);  
    if (objPtr->odd_even_flag && num_bytes_evenOrodd && !objPtr->dualmono)
   	{
		 RA_TNI_byte_shift ( codebuf, num_words); 
	}

	objPtr->odd_even_flag =  !objPtr->odd_even_flag; 
#endif //SWAP_ENABLE
   
   
   
   /*sreenu */

	/*
	 * If valid, decode the incoming frame into lookahead mlt.
	 */
 
#ifdef WORKING_BUF_DMA
	objPtr->buf = (long*)outbuf ;			// reuse outbuf for temp buffer
   	
	// page out overlap[1] table, and make space for decmlt[1] only if 2 channels are present
	// 	Do it after the first 2 frames have been processed
	if( objPtr->nchannels == 2 && objPtr->numCalls >= 2)   
		ACPY2_start( objPtr->dmaChannel_3, IDMA2_ADRPTR (objPtr->overlap[1]), 
						IDMA2_ADRPTR(objPtr->overlap_extmem[1]) , 
						objPtr->nsamples
					 );  				
		
#endif
 

	if (!(*lostflag)) { // XXHPF: lostflag -> *lostflag

#ifdef CONST_TABLE_DMA		
	    // page in the sqvh tables - only if needed
		ACPY2_start( objPtr->dmaChannel_0 , 
						IDMA2_ADRPTR(RA_TNI_sqvh_table_0) , 
						IDMA2_ADRPTR(objPtr->sqvh_tab[0]) , 
						SQVH_TABLE_SIZE );
					
#endif 
 	
	 	if( objPtr->dualmono ) { 
			/* dual-mono case */
			for( ch = 0 ; ch < objPtr->nchannels; ch++) {

				/* init bitstream */
				pkbit = 0;
				pktotal = 0;
				/* pklimit = objPtr->nframebits; */
				objPtr->pkptr = (ULONG *)codebuf;

				availbits = objPtr->nframebits;
				
				RA_TNI_DecodeBytes((USHORT *)codebuf, objPtr->nframebits, objPtr->pkptr);
				
				/* decode gain control info */
				availbits -= RA_TNI_DecodeGainInfo(&dgainc[ch][2], &pkbit, &pktotal, objPtr);

				/* decode the mlt */
				RA_TNI_DecodeMLT(availbits, objPtr->decmlt[ch],  &pkbit, &pktotal, objPtr);

#ifdef CONST_TABLE_DMA
				if( ch == 1 ){
	  				// page in the cos4 and sin4 tables
					ACPY2_start( objPtr->dmaChannel_0, 
								IDMA2_ADRPTR(objPtr->fixcos4_sin4tab_extmem),
								IDMA2_ADRPTR( objPtr->fixcos4tab),
								objPtr->fixcos4sin4size		 );
				}
#endif 

#ifdef WORKING_BUF_DMA
				// write out the decmlt buffers, 
				if( ch == 0 ){
				    ACPY2_start( objPtr->dmaChannel_2, 
		             			  IDMA2_ADRPTR(objPtr->decmlt[0]),
		             			  IDMA2_ADRPTR(objPtr->decmlt_extmem[0]),
		             			  1024	 ) ;			
		            			
		        }

#endif	
				
				// move the codebuf pointer ahead only if it can be aligned on a dword 
				// boundary
				if( ! (objPtr->nframebits >>3 & 0x01 ) 
						&& !(objPtr->nframebits >> 4 & 0x01) && (ch == 0 ))
					codebuf += objPtr->nframebits >> 4 ; /*next channel - in words */
				else{
					/* shift the entire buffer */
					
					// the next frame is both byte and word misaligned - left shift 24
					if( (objPtr->nframebits >>3 & 0x01 ) && (ch == 0 ) ){
						UCHAR *shft_temp_dst = codebuf ;
						UCHAR *shft_temp_src = codebuf + (objPtr->nframebits>>4) ; 
						int counter ; 
						for( counter =(objPtr->nframebits >> 4); 
						 		counter >= 0 ; counter-- ){
						 	*shft_temp_dst = (*shft_temp_src & 0x00FF ) <<8|
						 						(*(shft_temp_src+1) & 0xFF00 )>>8 ;
						 	shft_temp_dst ++ ;
						 	shft_temp_src ++ ;
						 }
					
					}
					// the next frame is dword misaligned - left shift 16
					else if ((objPtr->nframebits >> 4 & 0x01) && (ch == 0 )) {
						UCHAR *shft_temp_dst = codebuf ;
						UCHAR *shft_temp_src = codebuf + (objPtr->nframebits>>4) ; 
						int counter ;
						// no of bytes to move
						/*locate next channel - in words */
						for( counter =(objPtr->nframebits >> 4); 
						 counter >= 0 ; counter-- )
							*shft_temp_dst++ = *shft_temp_src++;
					}

				}
			
			} // end of for_ch loop
	
		}	/* end of dual-mono case */
		
		else{ 
			/* mono or stereo case */
		
				/* init bitstream */
				pkbit = 0;
				pktotal = 0;
				/* pklimit = objPtr->nframebits; */
		
				objPtr->pkptr = (ULONG *)codebuf;
				availbits = objPtr->nframebits;
		
				RA_TNI_DecodeBytes((USHORT *)codebuf, objPtr->nframebits, objPtr->pkptr);
		
				/* decode gain control info */
				availbits -= RA_TNI_DecodeGainInfo(&dgainc[LEFT][2], &pkbit, &pktotal, objPtr);
				dgainc[RGHT][2] = dgainc[LEFT][2];	/* replicate for stereo */
		
				/* decode the mlt */
				if (objPtr->nchannels == 2) { /*stereo case*/
					RA_TNI_JointDecodeMLT(availbits, objPtr->decmlt[LEFT], 
									objPtr->decmlt[RGHT], &pkbit, &pktotal, objPtr);

					
					// write out the decmlt buffers on channel 1
#ifdef WORKING_BUF_DMA
				   ACPY2_start( objPtr->dmaChannel_2, 
		             			  IDMA2_ADRPTR(objPtr->decmlt[0]),
		             			  IDMA2_ADRPTR(objPtr->decmlt_extmem[0]),
		             			  1024	 ) ;					
		   		  	
#endif					            			
		       	}
		
				else{	/*mono case*/
					RA_TNI_DecodeMLT(availbits, objPtr->decmlt[LEFT], &pkbit, &pktotal, objPtr);

#ifdef CONST_TABLE_DMA
				  	// page in the cos4 and sin4 tables
					ACPY2_start( objPtr->dmaChannel_0, 
								 IDMA2_ADRPTR(objPtr->fixcos4_sin4tab_extmem),
					 			 IDMA2_ADRPTR( objPtr->fixcos4tab),
								 objPtr->fixcos4sin4size); 
														
#endif 					

#ifdef WORKING_BUF_DMA
		      		// write out decmlt[0] 
				     ACPY2_start( objPtr->dmaChannel_2, 
		            			  IDMA2_ADRPTR(objPtr->decmlt[0]),
		             			  IDMA2_ADRPTR(objPtr->decmlt_extmem[0]),
		             			  1024	 ) ;			            			
#endif		
					
				}
		}/* end of mono or stereo case */
		
		/*let the lookahead params = current params */
		dgainc[LEFT][1] = dgainc[LEFT][2] ;
		dgainc[RGHT][1] = dgainc[RGHT][2] ;
		
	}	/* end of no loss case */


	else {
		// lost flag is set. 
		/* current mlt is lost, recreate */ 
		
#ifdef CONST_TABLE_DMA
	  // page in the cos4 and sin4 tables
		ACPY2_start( objPtr->dmaChannel_0, 
					 IDMA2_ADRPTR(objPtr->fixcos4_sin4tab_extmem),
		 			 IDMA2_ADRPTR( objPtr->fixcos4tab),
					 objPtr->fixcos4sin4size); 					
	 									
#endif

		for (ch = 0; ch < objPtr->nchannels; ch++) {
 
#ifdef WORKING_BUF_DMA
 			for (i = 0; i < objPtr->nsamples; i++)
						objPtr->decmlt[ch][i] = (objPtr->decmlt_extmem[ch][i] >> 1)
									 + (objPtr->decmlt_extmem[ch][i] >> 2);	
						/* fast multiply by 0.75 */
			
			// write out the decmlt buffers
			if( ch == 0 ){
	            ACPY2_start( objPtr->dmaChannel_2, 
		            		 IDMA2_ADRPTR(objPtr->decmlt[0]),
		             		 IDMA2_ADRPTR(objPtr->decmlt_extmem[0]),
		             		 1024	 ) ;			
	            			
	        }
	       	else{
	           ACPY2_start( objPtr->dmaChannel_3, 
		             			  IDMA2_ADRPTR(objPtr->decmlt[1]),
		             			  IDMA2_ADRPTR(objPtr->decmlt_extmem[1]),
		             			  1024	 ) ;	 			
	       	}			
						
#else //WORKING_BUF_DMA
 			for (i = 0; i < objPtr->nsamples; i++)
						objPtr->decmlt[ch][i] = (objPtr->decmlt[ch][i] >> 1)
									 + (objPtr->decmlt[ch][i] >> 2);	
						/* fast multiply by 0.75 */
						
#endif //WORKING_BUF_DMA

		} 
 
 
	 } // end of lostflag condition


	if(objPtr->numCalls < 1) {
		objPtr->numCalls++;
		
		for (ch = 0; ch < objPtr->nchannels; ch++) {

			dgainc[ch][0] = dgainc[ch][1];
			dgainc[ch][1] = dgainc[ch][2];	/* keeps previous, if lost */
	
		}
#ifdef WORKING_BUF_DMA
		if( objPtr->nchannels== 2 )
        	ACPY2_start( objPtr->dmaChannel_3, 
		       			  IDMA2_ADRPTR(objPtr->decmlt[1]),
		       			  IDMA2_ADRPTR(objPtr->decmlt_extmem[1]),
		       			  1024	 ) ;
#endif		       		

	}
	else { /*numCalls > 1 */
		objPtr->numCalls = 2;	// set at 2 to prevent overflow

		/*
		 * Synthesize the current mlt.
		 */
		for (ch = 0; ch < objPtr->nchannels; ch++) {

			/* inverse transform, without overlap */
#ifdef CONST_TABLE_DMA
	   		objPtr->currChannel = ch ;  
	   		objPtr->rdAgainFlg = ( ch == 0 && objPtr->nchannels == 2) ? 1 : 0 ;
	   		// should we expect one more pass ?
	   		  
			RA_TNI_IMLTNoOverlap((long *)objPtr->decmlt[ch], decpcm, objPtr->nsamples, objPtr->buf, objPtr);
#else
			RA_TNI_IMLTNoOverlap((long *)objPtr->decmlt[ch], decpcm, objPtr->nsamples, objPtr->buf);	
#endif			
	        
#ifdef WORKING_BUF_DMA
			// relocate the temp buffer to the 2nd working buffer
			objPtr->buf = objPtr->decmlt[0] ;
		  
           	// Readjusting the DMA flow, to write out the right-chnl mlt after 1st channel 
           	// iMLT has been performed
           	if( objPtr->rdAgainFlg )
           		ACPY2_start( objPtr->dmaChannel_3, 
		             			  IDMA2_ADRPTR(objPtr->decmlt[1]),
		             			  IDMA2_ADRPTR(objPtr->decmlt_extmem[1]),
		             			  1024	 ) ;	
           	
           	
       		// wait for transfers on channel 1 
       	    ACPY2_wait( objPtr->dmaChannel_1) ; 
          
#endif 	        
			/* run gain compensator, then overlap-add */
#define NO_INTERLEAVE /* For use with audio devices that don't support interleaved stereo */
#ifdef NO_INTERLEAVE
			RA_TNI_GainCompensate(decpcm, &dgainc[ch][0], &dgainc[ch][1], objPtr->nsamples,
			                       1, (long *)objPtr->overlap[ch], outbuf + ch*(objPtr->nsamples));
#else
			/* JR - now interleaves stereo PCM */
			RA_TNI_GainCompensate(decpcm, &dgainc[ch][0], &dgainc[ch][1], objPtr->nsamples,
			                       objPtr->nchannels, (long *)objPtr->overlap[ch], outbuf + ch);
#endif // NO_INTERLEAVE

#ifdef WORKING_BUF_DMA
			// page out overlap[0] table, and make space for overlap[1] table for stereo channel
			if( ch==0 )    
				ACPY2_start( objPtr->dmaChannel_2, 
							 IDMA2_ADRPTR(objPtr->overlap[0]), 
							 IDMA2_ADRPTR(objPtr->overlap_extmem[0]), 
							 objPtr->nsamples);
#endif 	        
	
			dgainc[ch][0] = dgainc[ch][1];
			dgainc[ch][1] = dgainc[ch][2];	/* keeps previous, if lost */
	
		}  

	}
#ifdef CONST_TABLE_DMA
	ACPY2_wait( objPtr->dmaChannel_0) ;
#endif
#ifdef WORKING_BUF_DMA
	ACPY2_wait( objPtr->dmaChannel_1);
	ACPY2_wait(objPtr->dmaChannel_2);
	ACPY2_wait( objPtr->dmaChannel_3);
#endif

	return 0;
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_getStatus
   
   Description   :  To retreive decoder params at run time
   
   Input		 :  Handle to decoder instance, pointer to status structure
   Output		 :  status
   Returns		 :  Void
   
   NOTE 		 : No functional implementation
--------------------------------------------------------------------------*/ 	
Void RA_TNI_getStatus(IRA_Handle Handle, IRA_Status *status) 
{
  return ;
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_setStatus
   
   Description   :  To set decoder params at run time
   
   Input		 :  Handle to decoder instance, pointer to status structure
   Output		 :  --
   Returns		 :  Void
   
   NOTE 		 : No functional implementation
--------------------------------------------------------------------------*/ 	
Void RA_TNI_setStatus(IRA_Handle handle, const IRA_Status *status) 
{
	return;
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_reset
   
   Description	 :  Reset decoder state
   					
   Input		 :  Handle to decoder instance
   Output		 :  --
   Returns		 :  Void
--------------------------------------------------------------------------*/ 	
Void RA_TNI_reset (IRA_Handle Handle)
{
	
}



