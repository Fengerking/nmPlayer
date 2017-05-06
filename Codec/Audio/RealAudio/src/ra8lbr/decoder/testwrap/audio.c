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

/* audio.c
 * --------------------------------------
 *
 * Simple Audio Output library.
 * 6/7/2000 Ken Cooke
 *
 * TODO: proper error handling.
 *
 * Notes:
 * Assumes 16-bit PCM audio.
 * link with winmm.lib (in Windows)
 */

#include "wrapper.h"

#if defined (_WIN32)

#include <windows.h>
#include <mmsystem.h>

/* globals */
static HWAVEOUT	g_hWave;
static int		g_nBuffers;
static int		g_Index;
static int		g_HaveBuffer;
static HANDLE	g_hSem;
static WAVEHDR	*g_Hdr;
static int		*g_nBytes;

static void CALLBACK
WaveCallback(HWAVEOUT hWave, UINT32 uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if (uMsg == WOM_DONE) {
		ReleaseSemaphore((HANDLE)dwUser, 1, NULL);
	}
}

int AudioOpen(int SamplingRate, int nChannels, int nSamples, int nBuffers)
{
	WAVEFORMATEX Format;
	int nBytes = nSamples * sizeof(short);
	int i;

	g_hWave = NULL;
	g_nBuffers = nBuffers;
	g_Index = 0;
	g_HaveBuffer = FALSE;
	g_hSem = CreateSemaphore(NULL, nBuffers, nBuffers, NULL);

	/* Open the wave device */
	Format.wFormatTag = WAVE_FORMAT_PCM;
	Format.nChannels = (unsigned short)nChannels;
	Format.nSamplesPerSec = SamplingRate;
	Format.nAvgBytesPerSec = SamplingRate * nChannels * sizeof(short);
	Format.nBlockAlign = (unsigned short)(nChannels * sizeof(short));
	Format.wBitsPerSample = 8 * sizeof(short);
	Format.cbSize = 0;

	if (MMSYSERR_NOERROR != waveOutOpen(&g_hWave, WAVE_MAPPER, &Format,
		(DWORD)WaveCallback, (DWORD)g_hSem, CALLBACK_FUNCTION)) {
		CloseHandle(g_hSem);
		return -1;
	}

	/* Initialize the wave buffers */
	g_Hdr = (WAVEHDR *) calloc(nBuffers, sizeof(WAVEHDR));
	g_nBytes = (int *) calloc(nBuffers, sizeof(int));

	for (i = 0; i < nBuffers; i++) {
		g_nBytes[i] = 0;

		/* Allocate a data buffer and initialize the header */
		g_Hdr[i].lpData = (LPSTR) calloc(nBytes, sizeof(char));
		g_Hdr[i].dwBufferLength  = nBytes;
		g_Hdr[i].dwBytesRecorded = 0;
		g_Hdr[i].dwUser = 0;
		g_Hdr[i].dwFlags = 0;
		g_Hdr[i].dwLoops = 0;
		g_Hdr[i].lpNext = 0;
		g_Hdr[i].reserved = 0;

		/* Prepare it */
		waveOutPrepareHeader(g_hWave, &g_Hdr[i], sizeof(WAVEHDR));
	}
	return 0;
}

int AudioClose(void)
{
	int i;

	if (!g_hWave)
		return -1;

	/* Get the buffers back */
	waveOutReset(g_hWave);

	/* Free all the buffers */
	for (i = 0; i < g_nBuffers; i++) {
		if (g_Hdr[i].lpData) {
			waveOutUnprepareHeader(g_hWave, &g_Hdr[i], sizeof(WAVEHDR));
			free(g_Hdr[i].lpData);
		}
	}
	free(g_Hdr);
	free(g_nBytes);

	/* Close the wave device */
	waveOutClose(g_hWave);

	/* Free the semaphore */
	CloseHandle(g_hSem);

	return 0;
}
#define HX_MIN(a, b)  (((a) < (b)) ? (a) : (b))

int AudioWrite(short *SampleBuf, int nSamples)
{
	BYTE *pData = (BYTE *) SampleBuf;
	int nBytes = nSamples * sizeof(short);
	int nWritten;

	while (nBytes != 0) {

		/* Get a buffer, if necessary */
		if (!g_HaveBuffer) {
			WaitForSingleObject(g_hSem, INFINITE);
			g_HaveBuffer = TRUE;
		}

		/* Copy into the buffer */
		nWritten = HX_MIN((int)g_Hdr[g_Index].dwBufferLength - g_nBytes[g_Index], nBytes);
		memcpy(g_Hdr[g_Index].lpData + g_nBytes[g_Index], pData, nWritten);
		g_nBytes[g_Index] += nWritten;

		/* If the buffer is full, send it off */
		if (g_nBytes[g_Index] == (int)g_Hdr[g_Index].dwBufferLength) {

			waveOutWrite(g_hWave, &g_Hdr[g_Index], sizeof(WAVEHDR));
			g_nBytes[g_Index] = 0;
			g_HaveBuffer = FALSE;
			g_Index = (g_Index + 1) % g_nBuffers;

			nBytes -= nWritten;
			pData += nWritten;

		} else {
			break;	/* Not full yet */
		}
	}
	return 0;
}

void AudioFlush(void)
{
	int nEmpty;

	/* Flush any partially-filled buffer */
	if (g_HaveBuffer) {

		/* Pad with silence */
		nEmpty = (int)g_Hdr[g_Index].dwBufferLength - g_nBytes[g_Index];
		memset(g_Hdr[g_Index].lpData + g_nBytes[g_Index], 0, nEmpty);

		waveOutWrite(g_hWave, &g_Hdr[g_Index], sizeof(WAVEHDR));
		g_nBytes[g_Index] = 0;
		g_HaveBuffer = FALSE;
		g_Index = (g_Index + 1) % g_nBuffers;
	}
}

/*
 * Prime all available buffers with silence.
 */
int AudioPrime(void)
{
	/* Fill any partially-filled buffer */
	AudioFlush();

	/* Fill all remaining buffers */
	while (WaitForSingleObject(g_hSem, 0) == WAIT_OBJECT_0) {

		g_HaveBuffer = TRUE;
		memset(g_Hdr[g_Index].lpData, 0, g_Hdr[g_Index].dwBufferLength);
		
		waveOutWrite(g_hWave, &g_Hdr[g_Index], sizeof(WAVEHDR));
		g_nBytes[g_Index] = 0;
		g_HaveBuffer = FALSE;
		g_Index = (g_Index + 1) % g_nBuffers;
	}
	return 0;
}

void AudioWait(void)
{
	int i;

	AudioFlush();

	/* Wait for all the buffers back */
	for (i = 0; i < g_nBuffers; i++) {
		WaitForSingleObject(g_hSem, INFINITE);
	}
	ReleaseSemaphore(g_hSem, g_nBuffers, NULL);
}

void AudioReset(void)
{
	AudioFlush();

	waveOutReset(g_hWave);
}

#else

int AudioOpen(int SamplingRate, int nChannels, int nSamples, int nBuffers)
{
	return -1;
}

int AudioClose(void)
{
	return -1;
}

int AudioWrite(short *SampleBuf, int nSamples)
{
	return -1;
}

int AudioPrime(void)
{
	return -1;
}

void AudioFlush(void)
{
	return;
}

void AudioWait(void)
{
	return;
}

void AudioReset(void)
{
	return;
}

#endif
