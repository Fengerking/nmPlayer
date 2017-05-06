/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_dma_api.c,v 1.1 2005/04/05 21:22:07 hfrederickson Exp $ 
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

#ifdef MEP
#include "asp.h"
#endif

#include "typedefs.h"
//#include "../../library/video/include/rv_overlay.h"


#define REG_DCR   0x1100        // The DMA control register

#define REG_CAR0  0x1000        // The channel active register
#define REG_CCR0  0x1001        // The channel control register
#define REG_TCM0  0x1002        // The transfer command register
#define REG_SRC0  0x1003        // The source address register
#define REG_DST0  0x1004        // The destination address register
#define REG_TRC0  0x1005        // The transfer count register
#define REG_LWR0  0x1006        // The line width register
#define REG_LSR0  0x1007        // The line step register
#define REG_DTA0  0x1008        // The descriptor table register
#define REG_DTS0  0x1009        // The descriptor table size register
#define REG_DSR0  0x100a        // The DMA status register

#define REG_CAR1  0x1010        // The channel active register
#define REG_CCR1  0x1011        // The channel control register
#define REG_TCM1  0x1012        // The transfer command register
#define REG_SRC1  0x1013        // The source address register
#define REG_DST1  0x1014        // The destination address register
#define REG_TRC1  0x1015        // The transfer count register
#define REG_LWR1  0x1016        // The line width register
#define REG_LSR1  0x1017        // The line step register
#define REG_DTA1  0x1018        // The descriptor table register
#define REG_DTS1  0x1019        // The descriptor table size register
#define REG_DSR1  0x101a        // The DMA status register


#define CNT0    0x400   /* Counter */
#define CMP0    0x401   /* Compare Register */
#define TEN0    0x402   /* Timer Enable Register */
#define TCR0    0x403   /* Timer Control Register */
#define TIS0    0x404   /* Timer Interrupt Status Register */
#define TCD0    0x405   /* Timer Clock Divide Register */







/* Initialize DMA */
VOID DMAInit()
{	
   INT32 htrq,bl,dcm,nie,ccr;	
	  htrq = 0x0;       // HTRQ (hardware transfer request)
	  bl   = 0x1;       // BL (burst length: 8)
	  dcm  = 0;         // DCM (descriptor chain mode)
	  nie  = 0;         // NIE (the interrupt which finished properly
	  	                //      is enabled)
	  	                //     Make the interrupt request
	 
	 
	  ccr = (htrq << 12) | (bl << 8) | (dcm << 4) | nie ;
	  stcb(ccr, REG_CCR0);// channel 0 :: no chaining mode, and bl is 32 byte
	  
	  htrq = 0x0;        // HTRQ (hardware transfer request)
	  bl   = 0x0;        // BL (burst length: 8)
	  dcm  = 0;          // DCM (descriptor chain mode)
	  nie  = 0;          // NIE (the interrupt which finished properly
	  	                 //      is enabled)
	  ccr = (htrq << 12) | (bl << 8) | (dcm << 4) | nie ;

	 
	  stcb(ccr, REG_CCR1); // Channel 1 :: chaining mode is there (default no chaining mode), and bl is 16 byte 
	 	
}





dma_transfer_wait(UINT32 src, UINT32 dst, INT32 size)
{
	INT32 dsr=0;
	stcb(0x0,  REG_DSR0);
	stcb(src,  REG_SRC0);  // Source Address
	stcb(dst,  REG_DST0);  // Destination Adress

	stcb(size, REG_TRC0);  // Size to transfer 

	stcb(0x1, REG_CAR0);   // channel 
	
	do{
           ldcb(dsr, REG_DSR0);  // DSR is checked to confirm the proper finish
           dsr = (dsr&0x1);
          }while(dsr!=1);	
	
}


