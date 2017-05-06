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

/* fileio.c
 * --------------------------------------
 *
 * Jon Recker, November 2002
 */

#include "wrapper.h"

#if defined (_WIN32) || defined (ARM_ADS) || defined (__GNUC__)

#include <stdlib.h>
#include <stdio.h>

typedef struct fileCDT {
	unsigned int nBytes;
	FILE *f;
} fileCDT;

fileADT InitRead(optionT *optPtr, unsigned int readSize)
{
	fileADT fr;
	fr = (fileADT)malloc(sizeof(struct fileCDT));

	fr->nBytes = readSize;
	fr->f = fopen(optPtr->infileName, "rb");
	if (!fr->f)
		return 0;
		
	return fr;
}

int ReadNextChunk(fileADT fr, unsigned char *readBuf)
{
	int nRead;
	nRead = fread(readBuf, sizeof(unsigned char), (size_t)fr->nBytes, fr->f);
	return nRead;
}

int FreeRead(fileADT fr)
{
	fclose(fr->f);
	free(fr);
	return 0;
}

fileADT InitWrite(optionT *optPtr, unsigned int writeSize)
{
	fileADT fw;
	fw = (fileADT)malloc(sizeof(struct fileCDT));
	fw->nBytes = writeSize;
	fw->f = fopen(optPtr->outfileName, "wb");
	if (!fw->f)
		return 0;
		
	return fw;
}

int WriteNextChunk(fileADT fw, unsigned char *writeBuf)
{
	int nWritten;
	nWritten = fwrite(writeBuf, sizeof(unsigned char), (size_t)fw->nBytes, fw->f);
	return nWritten;
}

int FreeWrite(fileADT fw)
{
	fclose(fw->f);
	free(fw);
	return 0;
}

#endif
