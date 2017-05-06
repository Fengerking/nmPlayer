/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: ra_tni_idma.c,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 *  ======== RA_TNI_iDMA.c ========
 *
 *  Description : Implementation of IDMA2 interface for Real Audio decoder algorithm
 *					for RA_TNI decoder module
 *					
 *
 *	Author : Firdaus Janoos
 *  
 */                 


        
#ifdef CONST_TABLE_DMA	
// If DMA is enabled
#pragma CODE_SECTION(RA_TNI_dmaChangeChannels, ".text:dmaChangeChannels");
#pragma CODE_SECTION(RA_TNI_dmaGetChannelCnt, ".text:dmaGetChannelCnt");
#pragma CODE_SECTION(RA_TNI_dmaGetChannels, ".text:dmaGetChannels");
#pragma CODE_SECTION(RA_TNI_dmaInit, ".text:dmaInit");


#include "coder.h"    
#include <idma2.h>

#pragma DATA_SECTION( RA_TNI_DMA_PARAMS, "params_section")
#pragma DATA_ALIGN( RA_TNI_DMA_PARAMS, 2 )
// IDMA_Params to transfer in the different tables and buffers
const IDMA2_Params RA_TNI_DMA_PARAMS = {
#ifdef RA_OMAPS00005636

	IDMA2_1D1D ,				// 1D to 1D transfer
	IDMA2_ELEM32,			// 32 bit elements
	1,						// single frame
	0+3,						// srcElemIndex
	0+3,						// dstElemIndex
	0,						// srcFrameIndex
	0                       // dstFrameIndex
							
#else	
	
	IDMA2_1D1D ,				// 1D to 1D transfer
	IDMA2_ELEM32,			// 32 bit elements
	1,						// single frame
	0,						// srcElemIndex
	0,						// dstElemIndex
	0,						// srcFrameIndex
	0                       // dstFrameIndex
#endif	
}  ;        


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_dmaChangeChannels
   
   Description   :  This function will notify the algorithm instance that DMA
   					resources have changed
   
   Input		 :  handle (IALG_Handle)
   					dmaTab[] (IDMA2_ChannelRec)
   Output		 :  --
   Returns		 :  void
   
--------------------------------------------------------------------------*/ 	   
Void RA_TNI_dmaChangeChannels(IALG_Handle handle , IDMA2_ChannelRec dmaTab[] )
{
	RA_TNI_Obj *objPtr =  (RA_TNI_Obj*)handle ;              
	// set up DMA channel handles for algo. instance
	objPtr->dmaChannel_0 = dmaTab[0].handle;
#ifdef WORKING_BUF_DMA	
	objPtr->dmaChannel_1 = dmaTab[1].handle;
	objPtr->dmaChannel_2 = dmaTab[2].handle;
	objPtr->dmaChannel_3 = dmaTab[3].handle;
#endif	
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_dmaGetChannelCnt
   
   Description   :  This function will get number of DMA resources required
   
   Input		 :  void
   Output		 :  --
   Returns		 :  numRecs (Int)
   
--------------------------------------------------------------------------*/ 	   	
Int  RA_TNI_dmaGetChannelCnt(Void)
{
	return NUM_DMA_CHANNELS ;	
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_dmaGetChannels
   
   Description   :  This function will get algorithm object's DMA
   					requirements/holdings
   
   Input		 :  handle (IALG_Handle)
   					dmaTab[] (IDMA2_ChannelRec)
   Output		 :  dmaTab[] (IDMA2_ChannelRec)
   Returns		 :  numRecs (Int)
   
--------------------------------------------------------------------------*/ 	   
Int  RA_TNI_dmaGetChannels(IALG_Handle handle , IDMA2_ChannelRec dmaTab[])
{
      RA_TNI_Obj *objPtr = (RA_TNI_Obj*)handle ;               

      dmaTab[0].queueId = 0    ; // on Queue 0
      dmaTab[0].handle = objPtr->dmaChannel_0 ;
      
#ifdef WORKING_BUF_DMA      
      dmaTab[1].queueId = 1    ; // on Queue 1
      dmaTab[1].handle = objPtr->dmaChannel_1 ;

	  dmaTab[2].queueId = 1    ; // on Queue 1
      dmaTab[2].handle = objPtr->dmaChannel_2 ;

	  dmaTab[3].queueId = 1    ; // on Queue 2
      dmaTab[3].handle = objPtr->dmaChannel_3 ;

#endif // WORKING_BUF_DMA

      return NUM_DMA_CHANNELS ;
}


/*-----------------------------------------------------------------------
   Function Name : 	RA_TNI_dmaInit
   
   Description   :  This function will grant the algorithm DMA resources
   
   Input		 :  handle (IALG_Handle)
   					dmaTab[] (IDMA2_ChannelRec)
   Output		 :  --
   Returns		 :  status (Int)
   
--------------------------------------------------------------------------*/ 	   
Int  RA_TNI_dmaInit(IALG_Handle handle , IDMA2_ChannelRec dmaTab[])
{
      
	RA_TNI_Obj	*objPtr =  (RA_TNI_Obj*)handle ;              
	
	// set up DMA channel handles for algo. instance
	objPtr->dmaChannel_0 = dmaTab[0].handle;
#ifdef WORKING_BUF_DMA	
	objPtr->dmaChannel_1 = dmaTab[1].handle;
	objPtr->dmaChannel_2 = dmaTab[2].handle;
	objPtr->dmaChannel_3 = dmaTab[3].handle;
#endif

	// configure the dma channels        
	ACPY2_configure( objPtr->dmaChannel_0, (IDMA2_Params*)&RA_TNI_DMA_PARAMS ) ; 
#ifdef WORKING_BUF_DMA	
	ACPY2_configure( objPtr->dmaChannel_1, (IDMA2_Params*)&RA_TNI_DMA_PARAMS ) ; 
	ACPY2_configure( objPtr->dmaChannel_2, (IDMA2_Params*)&RA_TNI_DMA_PARAMS ) ; 
	ACPY2_configure( objPtr->dmaChannel_3, (IDMA2_Params*)&RA_TNI_DMA_PARAMS ) ; 
#endif


	return IALG_EOK;		
}
   


#endif	// CONST_TABLE_DMA


