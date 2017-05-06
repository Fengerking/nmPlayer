/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: ra_tni_ialgvt.c,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 *  ======== RA_tni_ialgvt.c ========
 * 
 *  Description :	This file contains the vtable definitions for the
 *  				IALG / IRA interface implemented by the RA_TNI module.
 *  				Use the structure instance RA_TNI_IRA to initialize any vtable (IRA_Fxns) 
 *  				for the RA_TNI module
 */
 
 
#include <std.h>
#include <ialg.h>
#include <RA_tni.h>
#include "coder.h"

#define IALGFXNS 									 \
    (void*)&RA_TNI_IRA,/* implementationID  */       \
    NULL,    		   /* algActivate 		*/       \
    RA_TNI_algAlloc,   /* algAlloc 			*/       \
    NULL,              /* algControl 	 	*/  	 \
    NULL, 			   /* algDeactivate 	*/       \
    RA_TNI_algFree,    /* algFree	   		*/       \
    RA_TNI_algInit,    /* algInit 	   		*/       \
    RA_TNI_algMoved,   /* algMoved     		*/       \
    RA_TNI_algNumAlloc /* algNumAlloc  		*/


#pragma DATA_SECTION( RA_TNI_IRA, "params_section")
#pragma DATA_ALIGN( RA_TNI_IRA, 2 )
/*
 *  ======== RA_TNI_IRA ========
 *  This structure defines TNI's implementation of the IRA interface
 *  for the RA_TNI module.
 */
const IRA_Fxns RA_TNI_IRA = { 	 IALGFXNS,   		 /* IALG functions */    
								 NULL,		 		 /* getStatus 	   */
								 NULL,   	 		 /* setStatus 	   */
								 NULL,       		 /* reset 		   */
								 NULL,    	 		 /* findSync 	   */	
								 RA_TNI_decode,      /* decode 		   */ 
								 (Void*)&RA_TNI_IDMA2 /* IDMA pointer   */
							};

	
/*
 *  ======== RA_TNI_IALG ========
 *  This structure instance defines TNI's implementation of the IALG interface
 * 	for the RA_TNI module.
 */

#ifdef _TI_
/* equate RA_TNI_IALG with RA_TNI_IRA and set it as a global symbol*/
asm( "_RA_TNI_IALG .set _RA_TNI_IRA	 " );
asm( "			   .def _RA_TNI_IALG " ); 			 
#else
 
/* We duplicate the structure here to allow this code to be compiled and run on non-DSP platforms
	at the expense of data-space consumed by the re-definition below.
*/
const IALG_Fxns RA_TNI_IALG = {	 IALGFXNS 
							  };

#endif
          
          
#ifdef CONST_TABLE_DMA

#pragma DATA_SECTION( RA_TNI_IDMA2, "params_section")
#pragma DATA_ALIGN( RA_TNI_IDMA2, 2 )
/*
 *  ======== RA_TNI_IDMA ========
 *  This structure defines TNI's implementation of the IDMA2 interface
 *  for the RA_TNI module.
 */
const IDMA2_Fxns RA_TNI_IDMA2 = { (Void*) &RA_TNI_IALG,			/* implementationID   */    
								 RA_TNI_dmaChangeChannels, 	    /* dmaChangeChannles  */
								 RA_TNI_dmaGetChannelCnt,  		/* dmaGetChannelCnt	  */
								 RA_TNI_dmaGetChannels,   	    /* dmaGetChannels     */
								 RA_TNI_dmaInit	    	  		/* dmaInit 	          */	
							};
#endif  

