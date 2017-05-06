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

/* params.c
 * --------------------------------------
 *
 * Jon Recker, November 2002
 *
 * General functions to read in codec parameters from cmd-line or file
 *  (depending on platform)
 */

#include "wrapper.h"

#include <string.h>	/* for strcpy() */
#include <stdlib.h>	/* for atoi(), malloc() */

#define DEF_AUDIOFLAG	0
#define DEF_WRITEFLAG	1
#define DEF_GECKOMODE	Gecko1

int GetParameters(int argc, char *argv[], optionT *optPtr)
{

	int i;
	char *argPtr, tmpString[10];

	/* set defaults (main will correctly initialize things the user doesn't) */
	optPtr->nSamples = 0;
	optPtr->nFrameBits = 0;
	optPtr->sampRate = 0;
	optPtr->nRegions = 0;
	optPtr->nChannels = DEF_NCHANNELS;
	optPtr->audioFlag = DEF_AUDIOFLAG;
	optPtr->lossRate = 0;
	optPtr->cplStart = 0;
	optPtr->cplQbits = 0;
	optPtr->geckoMode = DEF_GECKOMODE;
	optPtr->writeFlag = DEF_WRITEFLAG;
	optPtr->resampRate = 0;

	for (i = 1; i < argc; i++)
	{
		argPtr = argv[i];
		if (*argPtr == '-')		/* all options except file names must start with - */
		{
			strcpy(tmpString, ++argPtr + 1);	/* read in what comes after -x (if anything) */
			switch(*argPtr) {
			case 'a':
				optPtr->audioFlag = 1;
				break;
			case 'b':
				optPtr->nFrameBits = atoi(tmpString);
				if (!optPtr->nFrameBits)	Usage(argv[0]);
				break;
			case 'c':
				optPtr->cplStart = atoi(tmpString);
				if (!optPtr->cplStart) Usage(argv[0]);
				break;
			case 'g':
				optPtr->geckoMode = (geckoType)atoi(tmpString);
				if (optPtr->geckoMode != 1 && optPtr->geckoMode != 2) Usage(argv[0]);
				break;
			case 'l':
				optPtr->lossRate = atoi(tmpString);
				if ((optPtr->lossRate < -100) || (optPtr->lossRate > 100))  Usage(argv[0]);
				break;
			case 'm':
				optPtr->nChannels = 1;
				break;
			case 'n':
				optPtr->nSamples = atoi(tmpString);
				if (!optPtr->nSamples)  Usage(argv[0]);
				break;
			case 'q':
				optPtr->cplQbits = atoi(tmpString);
				if (!optPtr->cplQbits) Usage(argv[0]);
				break;
			case 'r':
				optPtr->nRegions = atoi(tmpString);
				if (!optPtr->nRegions)  Usage(argv[0]);
				break;
			case 's':
				optPtr->sampRate = atoi(tmpString);
				if (!optPtr->sampRate)  Usage(argv[0]);
				break;
			case 'z':
				optPtr->resampRate = atoi(tmpString);
				if (optPtr->resampRate < 0)  Usage(argv[0]);
				break;
			case '?':
			default:
				Usage(argv[0]);
			}
		}
		else if (i == argc - 2)	{	/* arg has no -, so expect filenames */
			optPtr->infileName = argv[i++];
			optPtr->outfileName = argv[i++];
			return 0;
		}
	}
	Usage(argv[0]);
	return -1;	/* never get here, Usage() calls exit(0) */
}

/*
 * Print usage info.
 */
void Usage(char *name)
{
	char *s;

	if ((s = strrchr(name, '\\')) != 0)	/* strip to basename */
		name = s + 1;
	LogFormattedString("                                                           \n");
	LogFormattedString("Usage: %s [options] infile outfile                         \n", name);
	LogFormattedString("       -a            audio output                          \n");
	LogFormattedString("       -b BITS       bits per frame                        \n");
	LogFormattedString("       -c CPLSTART   coupling start region                 \n");
	LogFormattedString("       -g GECKOMODE  type of stereo, {1=G2, 2=RA8} (def=1) \n");
	LogFormattedString("       -l LOSSRATE   simulated loss, in percent            \n");
	LogFormattedString("       -m            mono (def=stereo)                     \n");
	LogFormattedString("       -n NSAMPLES   transform size                        \n");
	LogFormattedString("       -q CPLQBITS   coupling quant bits                   \n");
	LogFormattedString("       -r REGIONS    region limit                          \n");
	LogFormattedString("       -s SAMPRATE   sampling rate                         \n");
	LogFormattedString("       -z RESAMPRATE resampling rate (windows only)        \n");
	LogFormattedString(" *** This is a decoder only ***                            \n");
	exit(0);
}


