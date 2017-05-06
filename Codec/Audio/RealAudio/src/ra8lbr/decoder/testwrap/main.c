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

/* main.c
 * --------------------------------------
 *
 * Written by:			Ken Cooke
 * Last modified by:	Jon Recker, November 2002
 *
 * Command-line codec wrapper.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrapper.h"
#include "../gecko2codec.h"

#define MAX_ARM_FRAMES	 2000
#define ARMULATE_MUL_FACT	1

#if defined _WIN32 && defined _WIN32_WCE && (_WIN32_WCE < 400)
#include <windows.h>
int main(int argc, char **argv);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	/* not designed to do anything right now - just lets us build application to force linker
	 *   to run and check for link errors 
	 */
	char *testArgs[3] = {
		"testwrap.exe", 
		"\\My Documents\\file.rm",
		"nul"
	};

	main(3, testArgs);

	return 0;
}

#endif

int main(int argc, char **argv)
{
	short *outbuf;			/* sample buffers */
	unsigned char *codebuf;
	unsigned int readSize, writeSize, nread;
	int sampleCount, err, nFrames;
	int lossFlag, lossCount, lossTotal;
	int codingDelay;
	unsigned int startTime, endTime, diffTime, lastDiffTime, totalDecTime;
	fileADT fr, fw;	 /* pointers to structures for file reading and writing */
	optionT opt;
	uiSignal playStatus = Stop;
	float audioSecs, audioFrameSecs, currMHz, peakMHz;	/* for calculating percent RT */
	HGecko2Decoder gecko2Decoder;

	DebugMemCheckInit();

	if (InitLog())
		return -1;

	while (playStatus != Quit) {
		/* Validate parameters, and set any defaults. */
		if (GetParameters(argc, argv, &opt))
			ERR("invalid parameters\n");

		if (!opt.nSamples) 
			opt.nSamples = DEF_NSAMPLES;
		
		if ((opt.nSamples != 256) && (opt.nSamples != 512) && (opt.nSamples != 1024))
			ERR("opt.nSamples must be 256, 512, or 1024\n");

		if (!opt.nFrameBits) 
			opt.nFrameBits = opt.nSamples;
		opt.nFrameBits &= ~0x7;	/* force multiple of 8 */

		if (!opt.sampRate) 
			opt.sampRate = (opt.nSamples / 256) * 11025;

		if (!opt.nRegions) 
			opt.nRegions = (12*opt.nSamples)/(13*NBINS);
		if (opt.nRegions < (560*opt.nSamples)/(1024*NBINS) || opt.nRegions > (1024*opt.nSamples)/(1024*NBINS)) {
			LogFormattedString("opt.nRegions must be %d-%d for this transform\n",
				(560*opt.nSamples)/(1024*NBINS),
				(1024*opt.nSamples)/(1024*NBINS));
			exit(0);
		}

		readSize = (unsigned int)(opt.nFrameBits+7) / 8;
		writeSize = (unsigned int)(opt.nSamples * opt.nChannels * sizeof(short));

		/* malloc buffers */
		codebuf = (unsigned char *) malloc((size_t)readSize);
		outbuf = (short *) malloc((size_t)writeSize);
		if (!codebuf || !outbuf)
			ERR("malloc failed");

		/* Open audio output */
		if (opt.audioFlag) {
			if (AudioOpen(opt.sampRate,  opt.nChannels, opt.nSamples, 16))
				ERR("Cannot open audio output");
		}
		
		/* this signals dual-mono in the case of Gecko1 and nChans == 2 
		 * the geckoMode flag isn't strictly necessary (could just assume that cplStart, cplQbits == 0
		 *   on cmd-line line indicates dual-mono) but leave it in for compatibility w/old test scripts
		 */
		if (opt.geckoMode == Gecko1) {
			opt.cplStart = 0; 
			opt.cplQbits = 0;
		}

		DebugMemCheckStartPoint();

		gecko2Decoder = Gecko2InitDecoder(opt.nSamples, opt.nChannels, opt.nRegions, opt.nFrameBits, opt.sampRate, opt.cplStart, opt.cplQbits, &codingDelay);
		if (!gecko2Decoder)
			ERR("error initializing decoder");

#ifdef _DEBUG
		LogFormattedString("Heap state - from before calling InitDecoder() to after calling InitDecoder()\n");
#endif
		DebugMemCheckEndPoint();

		LogFormattedString("xform=%d bits=%d samprate=%d resp=%d bitrate=%d nchan=%d\n",
			opt.nSamples,
			opt.nFrameBits,
			opt.sampRate,
			opt.sampRate/2 * opt.nRegions * NBINS / opt.nSamples, 
			opt.sampRate * opt.nFrameBits / opt.nSamples,
			opt.nChannels
		);
		if (opt.resampRate)
			LogFormattedString("Resampling audio to %d Hz\n", opt.resampRate);

		/* rounded up to nearest byte, just in case */
		fr = InitRead(&opt, readSize);
		if (!fr)	ERR("invalid infile\n");

		fw = InitWrite(&opt, writeSize);
		if (!fw)	ERR("invalid outfile\n");

		sampleCount = 0;
		lossFlag = 0;
		lossCount = 0;
		lossTotal = 0;
		playStatus = Play;

		/* initialize timing code */
		InitTimer();
		totalDecTime = 0;
		audioFrameSecs = ((float)opt.nSamples) / ((float) opt.sampRate);
		peakMHz = 0;
		lastDiffTime = 0;

		/* Processing loop */
		err = 0;
		nFrames = 0;
		while ((nread = ReadNextChunk(fr, codebuf)) > 0 && playStatus == Play) {

			if (nread < readSize)
				break;

			sampleCount += opt.nSamples * opt.nChannels;

			if (opt.lossRate) {
				if (opt.lossRate < 0)	/* random loss */
					lossFlag = (rand() / 328) < -opt.lossRate;
				else 	/* pattern loss */
					lossFlag = (lossCount * 100) < (opt.lossRate * lossTotal);

				lossTotal++;
				lossCount += lossFlag;
				/* 
				if (lossFlag)	LogFormattedString("X");
				else			LogFormattedString(".");
				*/
			}

			/* Decode - verify that loss doesn't cheat by passing NULL */
			startTime = ReadTimer();

			err = Gecko2Decode(gecko2Decoder, codebuf, lossFlag, outbuf);

			endTime = ReadTimer();
			diffTime = CalcTimeDifference(startTime, endTime);
			totalDecTime += diffTime;
			currMHz = ARMULATE_MUL_FACT * (1.0f / audioFrameSecs) * diffTime / 1e6f;
			peakMHz = (currMHz > peakMHz ? currMHz : peakMHz);
	
			if (err)
				ERR("decode error\n");

			nFrames++;
#if defined ARM_ADS && defined MAX_ARM_FRAMES
			printf("frame %5d  [%10u - %10u = %6u ticks] .. ", nFrames, startTime, endTime, diffTime);
			printf("curr MHz = %5.2f, peak MHz = %5.2f, delta ticks = %#d\r", currMHz, peakMHz, diffTime - lastDiffTime);
			fflush(stdout);
#endif
			lastDiffTime = diffTime;

			if (opt.writeFlag) {
				if (codingDelay > 0)	
					codingDelay--;
				else
					WriteNextChunk(fw, (unsigned char *)outbuf);
			}

			/* play audio */
			if (opt.audioFlag && AudioWrite(outbuf, opt.nSamples * opt.nChannels))
				WARNING("PlayAudio failed");
			playStatus = PollHardware();

			switch (playStatus) {
			case Stop:
			case Quit:
				WARNING("Processing aborted");
				break;
			case Play:
				break;
			}
#if defined ARM_ADS && defined MAX_ARM_FRAMES
			if (nFrames >= MAX_ARM_FRAMES)
				break;
#endif
		}

		audioSecs = ((float)nFrames * opt.nSamples) / ((float) opt.sampRate);
		printf("\nTotal clock ticks = %d, MHz usage = %.2f\n", totalDecTime, ARMULATE_MUL_FACT * (1.0f / audioSecs) * totalDecTime / 1e6f);
		printf("nFrames = %d, output samps = %d, sampRate = %d, nChans = %d\n", nFrames, opt.nSamples, opt.sampRate, opt.nChannels);

		/* Clean up */
		if (opt.audioFlag && AudioClose())
			WARNING("AudioClose failed");

		Gecko2FreeDecoder(gecko2Decoder);

		FreeRead(fr);
		FreeWrite(fw);
		free(codebuf);
		free(outbuf);

#if defined (_WIN32) || defined (ARM_ADS) || defined (__GNUC__)
		playStatus = Quit;
#endif	/* _WIN32 */
	}
	
	FreeLog();

	DebugMemCheckFree();
	
	return 0;
}
