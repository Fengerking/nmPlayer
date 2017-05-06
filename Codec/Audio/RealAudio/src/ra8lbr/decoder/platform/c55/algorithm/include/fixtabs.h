/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: fixtabs.h,v 1.1 2005/02/18 23:30:39 hfrederickson Exp $ 
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

#ifndef _RA_TNI_FIXTABS_H_

#define _RA_TNI_FIXTABS_H_

extern const long RA_TNI_fixwindow_tab_0[256];
extern const long RA_TNI_fixwindow_tab_1[512];
extern const long RA_TNI_fixwindow_tab_2[1024];

extern const long RA_TNI_fixcos4tab_tab_0[128];
extern const long RA_TNI_fixcos4tab_tab_1[256];
extern const long RA_TNI_fixcos4tab_tab_2[512];

extern const long RA_TNI_fixsin4tab_tab_0[128];
extern const long RA_TNI_fixsin4tab_tab_1[256];
extern const long RA_TNI_fixsin4tab_tab_2[512];

extern const long RA_TNI_fixcos1tab_tab_0[129];
extern const long RA_TNI_fixcos1tab_tab_1[257];
extern const long RA_TNI_fixcos1tab_tab_2[513];

extern const long *const RA_TNI_fixwindow_tab[3];
extern const long *const RA_TNI_fixcos4tab_tab[3];
extern const long *const RA_TNI_fixsin4tab_tab[3];
extern const long *const RA_TNI_fixcos1tab_tab[3];

#endif
