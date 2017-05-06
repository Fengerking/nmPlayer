/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: ra_tni_ialg.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 *  ======== RA_TNI_ialg.c ========
 *
 *  Description : Implementation of IALG interface for Real Audio decoder algorithm
 *					required by XDAIS.
 *
 *	Author : Firdaus Janoos
 *
 */



#pragma CODE_SECTION(RA_TNI_algAlloc, ".text:algAlloc");
#pragma CODE_SECTION(RA_TNI_algFree, ".text:algFree");
#pragma CODE_SECTION(RA_TNI_algInit, ".text:algInit");
#pragma CODE_SECTION(RA_TNI_algNumAlloc, ".text:algNumAlloc");
#pragma CODE_SECTION(RA_TNI_algMoved, ".text:algMoved");

#include <std.h>
#include <ialg.h>
#include <iRA.h>
#include "RA_tni.h"
#include "coder.h"
#include "fixtabs.h"
#include "sqvh_tables.h"
#include "huffmantable.h"


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_algInit

   Description   :  This function will initialize the instance object.
   					It will read data from the params structure and initialize
   					the decoder. If params is NULL then IRA_PARAMS default values
   					will be applied.

   Input		 :  Handle (IALG_Handle)
   					mem		( IALG_MemRec[] )
   					parent
   Output		 :  --
   Returns		 :  Int (status for success or failure )

--------------------------------------------------------------------------*/
Int RA_TNI_algInit(IALG_Handle Handle,const IALG_MemRec *memTab, IALG_Handle parent, const IALG_Params *params)
{
#ifdef XDAIS_IALG			// to use XDAIS interface IALG

    RA_TNI_Obj *objPtr = (Void *)Handle;
    IRA_Params *parms;
	int ch, i;
	GAINC (*dgainc)[3] ;
	long lTemp;


	// do u have the right parameter structure ?
	if( params != NULL && params->size == sizeof(IRA_Params) )
		parms = (IRA_Params*)params;
	else
		parms = (IRA_Params*)&IRA_PARAMS	;		//default values

	/* To initialize the the even-odd flag to 0 signifying the initial start point is even - 0*/

	objPtr->odd_even_flag = 0;

	/* initialize state of algo. instance object */

	//dual mono support
	// [fj] : 09/02/2003 00:51
	objPtr->dualmono = (parms->nChannels > 1 ) && (parms->cplQbits == 0 ) ;

	objPtr->nsamples   = parms->nSamples;
	objPtr->nchannels  = parms->nChannels;
	objPtr->nregions   = parms->nRegions;
	objPtr->nframebits = (objPtr->dualmono) ? parms->nFrameBits / parms->nChannels : parms->nFrameBits ;

	objPtr->samprate   = parms->sampRate;
	objPtr->numCalls   = 0;
	objPtr->oldlost    = 0;

	if (objPtr->nchannels == 2) {
		objPtr->cplstart = parms->cplStart;
		objPtr->cplqbits = parms->cplQbits;
	}
	else {	/* mono */
		objPtr->cplstart = 0;
		objPtr->cplqbits = 0;
	}

	// initialize _lfsr seed for scalar dequant
	objPtr->_lfsr =  'k' | (long)'e'<<8 | (long)'n'<<16 | (long)'c'<<24;	/* well-chosen seed */

	/* store the buffers from memTab structure */

	if( memTab == NULL )	// not valid initialization : Retry !
		return IALG_EFAIL ;
	objPtr->decpcm = memTab[1].base ;

	dgainc = objPtr->dgainc;

	/* set up the decmlt buffers */
#ifdef WORKING_BUF_DMA
	// decmlt 0 - working buffer 1
	objPtr->decmlt[0] = (long*)memTab[4].base ;
	// decmlt 1 - working buffer 2
	objPtr->decmlt[1] = (long*)memTab[4].base  + 1*MAXNSAMP ;
#else
	// decmlt buffers
	objPtr->decmlt[0] = (long*)memTab[5].base;
	objPtr->decmlt[1] = objPtr->decmlt[0] + 1*MAXNSAMP ;
#endif


#ifdef WORKING_BUF_DMA		// using DMA for working buffers
	/* setup other buffers of the decoder */

	objPtr->decmlt_extmem[0] = (long*)memTab[5].base;
	objPtr->decmlt_extmem[1] = objPtr->decmlt_extmem[0] + 1*MAXNSAMP ;

	objPtr->buf = memTab[4].base ; // *must be made to point to output buffer*

	objPtr->overlap_extmem[0] = memTab[2].base ;
	objPtr->overlap_extmem[1] = memTab[3].base ;

	// points to working buffer 1
    objPtr->overlap[0] = (long*)memTab[4].base ;
     // points to working buffer 2
	objPtr->overlap[1] = (long*)memTab[4].base + 1*MAXNSAMP  ;

#else						// no DMA for working buffers
	/* setup other buffers of the decoder */
	objPtr->buf = memTab[4].base ;

	objPtr->overlap[0] = memTab[2].base ;
	objPtr->overlap[1] = memTab[3].base ;

#endif

	/* initialize all internal buffers */
	for (ch = 0; ch < parms->nChannels; ch++) {

		for (i = 0; i < 3; i++)
			dgainc[ch][i].nats = 0;

#ifdef WORKING_BUF_DMA
		for (i = 0; i < objPtr->nsamples ; i++)
			objPtr->overlap_extmem[ch][i] = 0;

#else
		for (i = 0; i < objPtr->nsamples ; i++)
			objPtr->overlap[ch][i] = 0;
#endif

	}



#ifdef 	CONST_TABLE_DMA
	// set up const table pointers
	 objPtr->fixwindow_tab_extmem = RA_TNI_fixwindow_tab[objPtr->nsamples>>9];
	 objPtr->fixwindowsize = objPtr->nsamples ;
	 objPtr->fixwindow_tab = memTab[6].base ;


	 objPtr->fixcos4_sin4tab_extmem =  RA_TNI_fixcos4tab_tab[objPtr->nsamples>>9];
	 objPtr->fixcos4sin4size = objPtr->nsamples ;		// the same value
	 objPtr->fixcos4tab = (long*)memTab[6].base ;
	 objPtr->fixsin4tab = objPtr->fixcos4tab + (objPtr->nsamples /2)  ;


  	 objPtr->fixcos1tab_extmem =  RA_TNI_fixcos1tab_tab[objPtr->nsamples>>9];
  	 objPtr->fixcos1size = objPtr->nsamples/2 + 1 ;
  	 objPtr->fixcos1tab = (long*)memTab[6].base + 1024 + 512 ; // memTab[6].base + 3072

 	 objPtr->twidtab_extmem	=  RA_TNI_twidtabs[objPtr->nsamples>>9];
     objPtr->twidtab = (long*)memTab[6].base + 1024 ;	// memTab[6].base + 2048
     objPtr->twidtabsize =  objPtr->nsamples/2 ;


	 objPtr->sqvh_tab[0] = memTab[6].base ;

	 lTemp = (long)RA_TNI_sqvh_table_1;
     lTemp = lTemp -  (long)RA_TNI_sqvh_table_0;
     objPtr->sqvh_tab[1] = (void const *)(lTemp
                           + (long)(HuffmanTable)objPtr->sqvh_tab[0]);

	 lTemp = (long)RA_TNI_sqvh_table_2;
     lTemp = lTemp -  (long)RA_TNI_sqvh_table_0;
     objPtr->sqvh_tab[2] = (void const *)(lTemp
                           + (long)(HuffmanTable)objPtr->sqvh_tab[0]);

	 lTemp = (long)RA_TNI_sqvh_table_3;
     lTemp = lTemp -  (long)RA_TNI_sqvh_table_0;
     objPtr->sqvh_tab[3] = (void const *)(lTemp
                           + (long)(HuffmanTable)objPtr->sqvh_tab[0]);

	 lTemp = (long)RA_TNI_sqvh_table_4;
     lTemp = lTemp -  (long)RA_TNI_sqvh_table_0;
     objPtr->sqvh_tab[4] = (void const *)(lTemp
                           + (long)(HuffmanTable)objPtr->sqvh_tab[0]);

	 lTemp = (long)RA_TNI_sqvh_table_5;
     lTemp = lTemp -  (long)RA_TNI_sqvh_table_0;
     objPtr->sqvh_tab[5] = (void const *)((long)lTemp
             + (long)(HuffmanTable)objPtr->sqvh_tab[0]);

	 lTemp = (long)RA_TNI_sqvh_table_6;
     lTemp = lTemp -  (long)RA_TNI_sqvh_table_0 ;
     objPtr->sqvh_tab[6] = (void const *)(lTemp
             + (long)(HuffmanTable)objPtr->sqvh_tab[0]);

	 // Initialize the DMA channel handles
	 objPtr->dmaChannel_0 = NULL ;
#ifdef WORKING_BUF_DMA
	 objPtr->dmaChannel_1 = NULL ;
	 objPtr->dmaChannel_2 = NULL ;
	 objPtr->dmaChannel_3 = NULL ;

#endif

#endif  //	CONST_TABLE_DMA

	parms->codingDelay = CODINGDELAY-1;		// return coding delay to framework

#endif	// XDAIS_IALG


	return (IALG_EOK);
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_algNumAlloc

   Description   :  This function will return the number of memory Blocks
   					that has to be allocated for the RA Decoder (5)
   					This function has to be called before RA_TNI_algAlloc
   					is called and depending on the return Value, suitable
   					number of memRec structures should be allocated.

   Input		 :  --
   Output		 :  --
   Returns		 :  Int (number of mem. blocks)

--------------------------------------------------------------------------*/
Int RA_TNI_algNumAlloc(void)
{
	return NUM_MEMORY_BLOCKS;
}





/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_algFree

   Description   :  This function will give the number of memory Blocks
   					that can be deallocated for the RA Decoder and the
   					different parameters for each block,including size,
   					alignment etc.

   Input		 :  Handle (IALG_Handle)
   Output		 :  memTab	( IALG_MemRec[] )
   Returns		 :  Int

   NOTE			 :	The RA_TNI_algNumAlloc function must be called before
   					and the memory for the IALG_MemRec structure array has
   					been allocated,
   					This function will return WORST-CASE sizes for all
   					buffers, unlike algAlloc which will return stream-
   					specific sizes for a buffer. Values returned by both may
   					not match

--------------------------------------------------------------------------*/
Int RA_TNI_algFree (IALG_Handle handle, IALG_MemRec * memTab)
{
#ifdef XDAIS_IALG			// to use XDAIS interface IALG
	RA_TNI_Obj *objPtr = (RA_TNI_Obj*)handle ;

    IRA_Params params ;	//to pass to algAlloc

	// setup from algorithm instance.
	params.size = sizeof(IRA_Params) ;
	params.nSamples = objPtr->nsamples;
	params.nChannels = objPtr->nchannels;
	params.nRegions =  objPtr->nregions;

	//let algAlloc do the dirty work
	RA_TNI_algAlloc( (const IALG_Params*) &params, NULL, memTab);

	memTab[0].base = (void*)handle;
	memTab[1].base = (void*)objPtr->decpcm ;
#ifdef WORKING_BUF_DMA
	memTab[2].base = (void*)objPtr->overlap_extmem[0]  ;
	memTab[3].base = (void*)objPtr->overlap_extmem[1] ;
	memTab[4].base = (void*)objPtr->decmlt[0] ;
	memTab[5].base = (void*)objPtr->decmlt_extmem[0] ;
#else
	memTab[2].base  = (void*)objPtr->overlap[0] ;
	memTab[3].base  = (void*)objPtr->overlap[1] ;
	memTab[4].base = (void*)objPtr->buf  ;
	memTab[5].base = (void*)objPtr->decmlt[0] ;
#endif

#ifdef 	CONST_TABLE_DMA
	memTab[6].base = (void*)objPtr->fixwindow_tab;
#endif

#endif	// XDAIS_IALG

	return NUM_MEMORY_BLOCKS;

}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_algAlloc

   Description   :  This function will give the number of memory Blocks
   					that has to be allocated for the RA Decoder and the
   					different parameters for each block,including size,
   					alignment etc.[ Refer to the IALG_MemRec structure for
   					further details]

   Input		 :  Params (parameter for creation)
   Output		 :  IALGFxns** (pointer to parent IALG_Fxn* )
   					memTab	( IALG_MemRec[] )
   Returns		 :  Int

   NOTE			 :	The RA_TNI_algNumAlloc function must be called before
   					and the memory for the IALG_MemRec structure array has
   					been allocated

   NOTE*		  : Units of size and alignment are words not bytes.

   NOTE**		  : This function will return stream specific sizes for all
   					buffers, unlike algFree which will return WORST
   					CASE sizes for a buffer. Values returned by both may
   					not match
--------------------------------------------------------------------------*/
Int	RA_TNI_algAlloc(const IALG_Params * Params, IALG_Fxns ** fxns, IALG_MemRec * memTab)
{
#ifdef XDAIS_IALG			// to use XDAIS interface IALG

    IRA_Params *params ;
    int size ;					// size of buffer in words

	/* Allocate Memory for one RA_TNI_Obj Structure */
	memTab[0].size = sizeof(RA_TNI_Obj);
	memTab[0].alignment = 2;
	memTab[0].space = IALG_SARAM0;
	memTab[0].attrs = IALG_PERSIST;

	// do u have the right parameter structure ?
	if( Params != NULL && Params->size == sizeof(IRA_Params) )
		params = (void*)Params;
	else
		params = (IRA_Params*)&IRA_PARAMS	;		//default values

	size = params->nSamples * sizeof(long) ; // size of buffers in words

	/* Allocate decpcm array */
	memTab[1].size = size*2;  		/* 2 * nSamples * sizeof(long) */
	memTab[1].alignment = size;		// alignment on 2048, 1024 and 512 words will do !!
	memTab[1].space = IALG_DARAM1;
#ifdef RA_OMAP1710_MEMTAB
	memTab[1].attrs = IALG_SCRATCH;
#else
	memTab[1].attrs = IALG_PERSIST;
#endif
	
	/* Allocate Memory for overlap_0 array */
	memTab[2].size = size  ;	 //	nSamples * sizeof(long)
	memTab[2].alignment = 2;
#ifdef WORKING_BUF_DMA                  // if using DMA for working buffers
	memTab[2].space = IALG_EXTERNAL ;	// 	place in External memory
#else                             		// else
	memTab[2].space = IALG_DARAM0;		// 	place in DARAM
#endif
	memTab[2].attrs = IALG_PERSIST;

	/* Allocate Memory for overlap_1 array - only for stereo stream*/
	memTab[3].size = (params->nChannels==2) ? size : 2 ;
	memTab[3].alignment = 2;
#ifdef WORKING_BUF_DMA                  // if using DMA for working buffers
	memTab[3].space = IALG_EXTERNAL ;	// 	place in External memory
#else                             		// else
	memTab[3].space = IALG_DARAM0;		// 	place in DARAM
#endif
	memTab[3].attrs = IALG_PERSIST;

#ifdef WORKING_BUF_DMA	// using DMA for working buffers
	/* Allocate Memory for working buffers in DARAM */
	memTab[4].size = 2048 * params->nChannels; 	//words
	memTab[4].alignment = 2;        //at least !!
	memTab[4].space = IALG_DARAM0;
	memTab[4].attrs = IALG_PERSIST;
#else
	/* Allocate Memory for temp. buf. array */
	memTab[4].size = (size > 1024 )? size : 1024;
	//atleast 1024 size needed for allocating
	// DecodeMLT locals in this buffer
	memTab[4].alignment = 2;
	memTab[4].space = IALG_DARAM0;
	memTab[4].attrs = IALG_PERSIST;
#endif

// if using DMA for working buffers
#ifdef WORKING_BUF_DMA
	/* Allocate  memory for decmlt array in extmem*/
	memTab[5].size = MAXNSAMP * params->nChannels * sizeof(long);
	memTab[5].alignment = 2;
	memTab[5].space = IALG_EXTERNAL;		// 	place in EXT
	memTab[5].attrs = IALG_PERSIST;
#else
	/* Allocate Memory for decmlt array */
	memTab[5].size = MAXNSAMP * params->nChannels * sizeof(long);
	memTab[5].alignment = 2;
	memTab[5].space = IALG_DARAM0;		// 	place in DARAM
	memTab[5].attrs = IALG_PERSIST;
#endif

#ifdef CONST_TABLE_DMA		// using DMA
    /* Allocate Memory for buffer for const tables in SARAM */
	memTab[6].size = 4098	; 		//words
	memTab[6].alignment = 2;        //at least !!
	memTab[6].space = IALG_SARAM0;
#ifdef RA_OMAP1710_MEMTAB	
	memTab[6].attrs = IALG_SCRATCH;
#else
	memTab[6].attrs = IALG_PERSIST;
#endif		
#endif

	if( fxns )
		*fxns = NULL;		/* nothing to return */

#endif	// XDAIS_IALG

	return NUM_MEMORY_BLOCKS;


}



/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_algMoved

   Description   :  This function is called whenever the application moves
   					the buffers allocated to the algorithm
   					It will read data from the params structure and initialize
   					the decoder. If params is NULL then IRA_PARAMS default values
   					will be applied.

   Input		 :  Handle (IALG_Handle)
   					mem		( IALG_MemRec[] )
   					parent
   Output		 :  --
   Returns		 :  void

--------------------------------------------------------------------------*/
Void RA_TNI_algMoved(IALG_Handle Handle,const IALG_MemRec *memTab, IALG_Handle parent, const IALG_Params *params)
{
#ifdef XDAIS_IALG			// to use XDAIS interface IALG

    RA_TNI_Obj *objPtr = (Void *)Handle;

    /* store the buffers from memTab structure */
	if( memTab == NULL )	// not valid initialization : Retry !
		return ;
	objPtr->decpcm = memTab[1].base ;

	/* set up the decmlt buffers */
#ifdef WORKING_BUF_DMA
	// decmlt 0 - working buffer 1
	objPtr->decmlt[0] = (long*)memTab[4].base ;
	// decmlt 1 - working buffer 2
	objPtr->decmlt[1] = (long*)memTab[4].base  + 1*MAXNSAMP ;
#else
	// decmlt buffers
	objPtr->decmlt[0] = (long*)memTab[5].base;
	objPtr->decmlt[1] = objPtr->decmlt[0] + 1*MAXNSAMP ;
#endif


#ifdef WORKING_BUF_DMA		// using DMA for working buffers
	/* setup other buffers of the decoder */

	objPtr->decmlt_extmem[0] = (long*)memTab[5].base;
	objPtr->decmlt_extmem[1] = objPtr->decmlt_extmem[0] + 1*MAXNSAMP ;

	objPtr->buf = memTab[4].base ; // *must be made to point to output buffer*

	objPtr->overlap_extmem[0] = memTab[2].base ;
	objPtr->overlap_extmem[1] = memTab[3].base ;

	// points to working buffer 1
    objPtr->overlap[0] = (long*)memTab[4].base ;
     // points to working buffer 2
	objPtr->overlap[1] = (long*)memTab[4].base + 1*MAXNSAMP  ;

#else						// no DMA for working buffers
	/* setup other buffers of the decoder */

	objPtr->buf = memTab[4].base ;

	objPtr->overlap[0] = memTab[2].base ;
	objPtr->overlap[1] = memTab[3].base ;

#endif


#ifdef 	CONST_TABLE_DMA
	// set up const table pointers
	 objPtr->fixwindow_tab_extmem = RA_TNI_fixwindow_tab[objPtr->nsamples>>9];
	 objPtr->fixwindow_tab = memTab[6].base ;


	 objPtr->fixcos4_sin4tab_extmem =  RA_TNI_fixcos4tab_tab[objPtr->nsamples>>9];
	 objPtr->fixcos4tab = (long*)memTab[6].base ;
	 objPtr->fixsin4tab = objPtr->fixcos4tab + (objPtr->nsamples /2)  ;


  	 objPtr->fixcos1tab_extmem =  RA_TNI_fixcos1tab_tab[objPtr->nsamples>>9];
  	 objPtr->fixcos1tab = (long*)memTab[6].base + 1024 + 512 ; // memTab[6].base + 3072

 	 objPtr->twidtab_extmem	=  RA_TNI_twidtabs[objPtr->nsamples>>9];
     objPtr->twidtab = (long*)memTab[6].base + 1024 ;	// memTab[6].base + 2048


	 objPtr->sqvh_tab[0] = memTab[6].base ;
	 objPtr->sqvh_tab[1] = (HuffmanTable)objPtr->sqvh_tab[0] + (RA_TNI_sqvh_table_1 -  RA_TNI_sqvh_table_0 ) ;
	 objPtr->sqvh_tab[2] = (HuffmanTable)objPtr->sqvh_tab[0] + (RA_TNI_sqvh_table_2 -  RA_TNI_sqvh_table_0 ) ;
	 objPtr->sqvh_tab[3] = (HuffmanTable)objPtr->sqvh_tab[0] + (RA_TNI_sqvh_table_3 -  RA_TNI_sqvh_table_0 ) ;
	 objPtr->sqvh_tab[4] = (HuffmanTable)objPtr->sqvh_tab[0] + (RA_TNI_sqvh_table_4 -  RA_TNI_sqvh_table_0 ) ;
	 objPtr->sqvh_tab[5] = (HuffmanTable)objPtr->sqvh_tab[0] + (RA_TNI_sqvh_table_5 -  RA_TNI_sqvh_table_0 ) ;
	 objPtr->sqvh_tab[6] = (HuffmanTable)objPtr->sqvh_tab[0] + (RA_TNI_sqvh_table_6 -  RA_TNI_sqvh_table_0 ) ;

#endif  //	CONST_TABLE_DMA


#endif	// XDAIS_IALG



}
