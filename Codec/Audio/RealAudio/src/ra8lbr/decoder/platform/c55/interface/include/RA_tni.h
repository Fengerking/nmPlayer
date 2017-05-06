/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: RA_tni.h,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 *  ======== RA_tni.h ========
 *
 *	Interface header.
 *
 *  Description : Definition of RA_TNI... API for Real Audio decoder
 *					 and RA_TNI decoder types
 *			
 *
 *
 *	Author : Firdaus Janoos
 *  
 */
 
/* NOTE : When using DMA enabled version of RA_TNI module, define symbol CONST_TABLE_DMA */
 
#ifndef RA_TNI_
#define RA_TNI_

#include <std.h>
#include <ialg.h>
#include <iRA.h>  

												
/*	
 *  ======== RA_TNI_exit ========	
 *  Required module finalization function.
 */
extern Void RA_TNI_exit(Void);

/*
 *  ======== RA_TNI_init ========
 *  Required module initialization function.
 */
extern Void RA_TNI_init(Void);

/*
 *  ======== RA_TNI_IALG ========
 *  TNI's implementation of IALG interface for Real Audio (RA)
 */
extern const IALG_Fxns RA_TNI_IALG; 

/*
 *  ======== RA_TNI_IRA ========
 *  TNI's implementation of  IRA interface for Real Audio (RA)
 */
extern const IRA_Fxns RA_TNI_IRA; 


#if defined CONST_TABLE_DMA || defined WORKING_BUF_DMA		                                                             

#include <idma2.h>
#include <acpy2.h>

/*
 *  ======== RA_TNI_IDMA ========
 *  TNI's implementation of the IDMA interface for Real Audio (RA)
 */
extern const IDMA2_Fxns RA_TNI_IDMA2 ;

#endif



#endif /* RA_TNI_ */
