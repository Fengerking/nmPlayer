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

/* coder.h
 * --------------------------------------
 *
 * Written by:			Ken Cooke
 * Last modified by:	Jon Recker, 10/22/02
 */
#ifndef _WRAPPER_H
#define _WRAPPER_H

#include <stdio.h>

#if defined _WIN32 && defined _DEBUG && defined FORTIFY
#include "fortify.h"
#endif

#define ERR(x) LogFormattedString("\nError: %s\n", x), FreeLog(), AudioClose(), exit(-1)
#define WARNING(x) LogFormattedString("\nWarning: %s\n", x)

/* these should go away, malloc actual size instead */
#define NBINS		20		/* transform bins per region */

#define DEF_NCHANNELS		2
#define MAX_NCHANNELS		2
#define DEF_NSAMPLES		512

/* gecko2 decoder supports regular gecko and RA8 gecko */
typedef enum {Gecko1 = 1, Gecko2 = 2} geckoType;

/* indicates desired playback state based on user input */
typedef enum {Stop, Play, Pause, Quit} uiSignal;

typedef struct fileCDT *fileADT;

typedef struct {
	int nSamples;
	int nFrameBits;
	int sampRate;
	int nRegions;
	int nChannels;
	int audioFlag;
	int lossRate;
	int cplStart;
	int cplQbits;
	geckoType geckoMode;	/* GECKO1 or GECKO2 */
	int writeFlag;
	int resampRate;

	char *infileName;
	char *outfileName;

} optionT;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

/* audio.c */
int AudioOpen(int SamplingRate, int nChannels, int nSamples, int nBuffers);
int AudioClose(void);
int AudioWrite(short *SampleBuf, int nSamples);
int AudioPrime(void);
void AudioFlush(void);
void AudioWait(void);
void AudioReset(void);

/* debug.c */
void DebugMemCheckInit(void);
void DebugMemCheckStartPoint(void);
void DebugMemCheckEndPoint(void);
void DebugMemCheckFree(void);

/* fileio.c */
fileADT InitRead(optionT *optPtr, unsigned int readSize);
int ReadNextChunk(fileADT fr, unsigned char *readBuf);
int FreeRead(fileADT fr);
fileADT InitWrite(optionT *optPtr, unsigned int writeSize);
int WriteNextChunk(fileADT fw, unsigned char *writeBuf);
int FreeWrite(fileADT fw);

/* logmsg.c */
int InitLog(void);
int LogFormattedString(char *format, ...);
int FreeLog(void);

/* params.c */
int GetParameters(int argc, char *argv[], optionT *optPtr);
void Usage(char *name);

/* resample.cpp */
int InitResample(int inputRate, int outputRate, int inSamps, int nChans);
int Resample(short *inbuf, short *outbuf);
int FreeResample(void);

/* timing.c */
int InitTimer(void);
unsigned int ReadTimer(void);
int FreeTimer(void);
unsigned int GetClockFrequency(void);
unsigned int CalcTimeDifference(unsigned int startTime, unsigned int endTime);

/* userint.c */
uiSignal PollHardware(void);

#ifdef __cplusplus
}
#endif

#endif	/* _WRAPPER_H */
