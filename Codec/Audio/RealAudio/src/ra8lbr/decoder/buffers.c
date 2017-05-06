/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point RealAudio 8 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * October 2003
 *
 * buffers.c - allocation and freeing of internal RA8 decoder buffers
 *
 * All memory allocation for the codec is done in this file, so if you want 
 *   to use something other the default malloc() and free() this is the only file 
 *   you'll need to change.
 **************************************************************************************/

//#include "stdlib.h"
#include "coder.h"
#include "VORA_sdk.h"
/**************************************************************************************
 * Function:    ClearBuffer
 *
 * Description: fill buffer with 0's
 *
 * Inputs:      pointer to buffer
 *              number of bytes to fill with 0
 *
 * Outputs:     cleared buffer
 *
 * Return:      none
 *
 * Notes:       slow, platform-independent equivalent to memset(buf, 0, nBytes)
 **************************************************************************************/
//static void ClearBuffer(void *buf, int nBytes)
//{
//	int i;
//	unsigned char *cbuf = (unsigned char *)buf;
//
//	for (i = 0; i < nBytes; i++)
//		cbuf[i] = 0;
//
//	return;
//}

/**************************************************************************************
 * Function:    AllocateBuffers
 *
 * Description: allocate all the memory needed for the RA8 decoder
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      pointer to Gecko2Info structure, set to all 0's
 **************************************************************************************/
Gecko2Info *AllocateBuffers(VO_MEM_OPERATOR	  *pMemOP)
{
	Gecko2Info *gi;

	/* create new Gecko2Info structure */
	gi = (Gecko2Info *)rm_memory_malloc(pMemOP, 0, sizeof(Gecko2Info));
	if (!gi)
		return 0;

	return gi;
}

#define SAFE_FREE(x)	{if (x)	rm_memory_free(pMemOP, 0, x);	(x) = 0;}	/* helper macro */

/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: free all the memory used by the RA8 decoder
 *
 * Inputs:      pointer to initialized Gecko2Info structure
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void FreeBuffers(Gecko2Info *gi, VO_MEM_OPERATOR  *pMemOP)
{
	if (!gi)
		return;

	SAFE_FREE(gi);
}
