/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_wrapper.c,v 1.1 2005/04/05 21:22:09 hfrederickson Exp $ 
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "ucifio.h" /* Not available in Helix. Contact Toshiba for access */
#include "../../RA8LIB/include/gecko2codec.h"
#include "radec_wrapper.h"

#define NUMAUDREFSTREAMS 32
#define DEF_WRITEFLAG 1
#define MAX_NHIST	32

FILE *fpInput = NULL ,*fpOutput = NULL,*fpRef = NULL,fpLogFile = NULL; 

#pragma section data .geckostruct
Gecko2Info sGecko2InfoObj = {0};
#pragma section data
#pragma section area .iodata
unsigned char ucInputBuff[280];
#pragma section area
#pragma section area .raoutput 
short isOutputBuff[2048];
#pragma section area 

SAudioParams gsRAParameters[NUMAUDREFSTREAMS] = {
	/******************** MONO *********************/
	/* -n	-r	-b	 -c -q -g -m */
	{  256,	12,  256, 0, 0, 2, 1, 8000  },	/*** chor_00 ***/
	{  256,	12,  256, 0, 0, 2, 1, 11025 },  /*** eddi_01 ***/
	{  512,	18,  376, 0, 0, 2, 1, 22050 },	/*** quar_02 ***/
	{  512,	23,	 480, 0, 0, 2, 1, 22050 },	/*** cast_03 ***/
	{ 1024,	37,	 744, 0, 0, 2, 1, 44100 },	/*** gspi_04 ***/
	{ 1024,	47, 1024, 0, 0, 2, 1, 44100 },	/*** vibr_05 ***/
	{ 1024,	47, 1488, 0, 0, 2, 1, 44100 },	/*** tmpt_06 ***/
	{  512,	24,  744, 0, 0, 2, 1, 22050 },	/*** soli_07 ***/
	{  256,	9,   192, 0, 0, 2, 1, 8000  },	/*** guit_08 ***/
	{ 1024,	47, 1488, 0, 0, 2, 1, 44100 },	/*** vibr_14 ***/
	{ 1024,	47,  480, 0, 0, 2, 1, 44100 },	/*** quar_15 ***/
	{ 1024,	47,  744, 0, 0, 2, 1, 44100 },  /*** eddi_16 ***/

	/***********STEREO - DUAL MONO *****************/
	/* -n	-r	-b	 -c -q -g -m */
	{  256,	11,  464, 0, 0, 2, 2, 11025 },	/*** abba_09 ***/
	{  512,	18,  752, 0, 0, 2, 2, 22050 },	/*** quar_10 ***/
	{  512,	24, 1024, 0, 0, 2, 2, 22050 },	/*** xylo_11 ***/
	{ 1024,	37, 1488, 0, 0, 2, 2, 44100 },	/*** chor_12 ***/
	{ 1024,	47,	2224, 0, 0, 2, 2, 44100 },	/*** abba_13 ***/

	/***********STEREO - JOINT STEREO **************/
	/* -n	-r	-b	 -c -q -g -m */
	{  512,	16,  384, 1, 3, 2, 2, 22050 },	/*** tmpt_17 ***/
	{  512,	20,  480, 1, 3, 2, 2, 22050 },	/*** xylo_18 ***/
	{  512,	23,  480, 1, 3, 2, 2, 22050 },	/*** soli_19 ***/
	{  512,	24,  744, 2, 4, 2, 2, 22050 },	/*** gspi_20 ***/
	{ 1024,	32,  744, 2, 4, 2, 2, 44100 },	/*** guit_21 ***/
	{ 1024,	32, 1024, 5, 5, 2, 2, 44100 },	/*** cast_22 ***/
	{ 1024,	37, 1024, 2, 4, 2, 2, 44100 },	/*** quar_23 ***/
	{ 1024,	37, 1488, 6, 5, 2, 2, 44100 },	/*** soli_24 ***/
	{ 1024,	37,	2240, 8, 5, 2, 2, 44100 },	/*** abba_25 ***/
	{  256,	9,   288, 1, 2, 2, 2, 11025 },	/*** vibr_26 ***/

	/***********STEREO - SURROUND STEREO ***********/
	/* -n	-r	-b	 -c -q -g -m */
	{ 1024,	30, 1488,17, 5, 2, 2, 44100 },  /*** eddi_27 ***/
	{ 1024,	34, 2240,19, 5, 2, 2, 44100 },	/*** tmpt_28 ***/
	{  512,	23, 1024,17, 5, 2, 2, 22050 },	/*** guit_29 ***/
};



/******************** Input Files ***********************/

char *pbAudInpFiles[NUMAUDREFSTREAMS] =
{
/*MONO BIT STREAMS*/
"../../../AudioTestStreams/AudioInputStreams/chor_00.enc",
"../../../AudioTestStreams/AudioInputStreams/eddi_01.enc",
"../../../AudioTestStreams/AudioInputStreams/quar_02.enc",
"../../../AudioTestStreams/AudioInputStreams/cast_03.enc",
"../../../AudioTestStreams/AudioInputStreams/gspi_04.enc",
"../../../AudioTestStreams/AudioInputStreams/vibr_05.enc",
"../../../AudioTestStreams/AudioInputStreams/tmpt_06.enc",
"../../../AudioTestStreams/AudioInputStreams/soli_07.enc",
"../../../AudioTestStreams/AudioInputStreams/guit_08.enc",
"../../../AudioTestStreams/AudioInputStreams/vibr_14.enc",
"../../../AudioTestStreams/AudioInputStreams/quar_15.enc",
"../../../AudioTestStreams/AudioInputStreams/eddi_16.enc",

/*STEREO FLAVOR-DUAL MONO STREAMS*/
"../../../AudioTestStreams/AudioInputStreams/abba_09.enc",
"../../../AudioTestStreams/AudioInputStreams/quar_10.enc",
"../../../AudioTestStreams/AudioInputStreams/xylo_11.enc",
"../../../AudioTestStreams/AudioInputStreams/chor_12.enc",
"../../../AudioTestStreams/AudioInputStreams/abba_13.enc",

/*STEREO FLAVOR-JOINT STEREO STREAMS*/
"../../../AudioTestStreams/AudioInputStreams/tmpt_17.enc",
"../../../AudioTestStreams/AudioInputStreams/xylo_18.enc",
"../../../AudioTestStreams/AudioInputStreams/soli_19.enc",
"../../../AudioTestStreams/AudioInputStreams/gspi_20.enc",
"../../../AudioTestStreams/AudioInputStreams/guit_21.enc",
"../../../AudioTestStreams/AudioInputStreams/cast_22.enc",
"../../../AudioTestStreams/AudioInputStreams/quar_23.enc",
"../../../AudioTestStreams/AudioInputStreams/soli_24.enc",
"../../../AudioTestStreams/AudioInputStreams/abba_25.enc",
"../../../AudioTestStreams/AudioInputStreams/vibr_26.enc",

/*STEREO FLAVOR-SURROUND BIT STREAMS*/
"../../../AudioTestStreams/AudioInputStreams/eddi_27.enc",
"../../../AudioTestStreams/AudioInputStreams/tmpt_28.enc",
"../../../AudioTestStreams/AudioInputStreams/guit_29.enc"
};

/***********************Conformence output files *********************************/
char *pbAudRefFiles[NUMAUDREFSTREAMS] =
{
/*MONO BIT STREAMS*/
"../../../AudioTestStreams/AudioReferenceStreams/chor_00.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/eddi_01.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/quar_02.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/cast_03.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/gspi_04.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/vibr_05.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/tmpt_06.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/soli_07.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/guit_08.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/vibr_14.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/quar_15.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/eddi_16.pcm",

/*STEREO FLAVOR-DUAL MONO STREAMS*/
"../../../AudioTestStreams/AudioReferenceStreams/abba_09.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/quar_10.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/xylo_11.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/chor_12.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/abba_13.pcm",

/*STEREO FLAVOR-JOINT STEREO STREAMS*/
"../../../AudioTestStreams/AudioReferenceStreams/tmpt_17.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/xylo_18.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/soli_19.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/gspi_20.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/guit_21.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/cast_22.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/quar_23.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/soli_24.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/abba_25.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/vibr_26.pcm",

/*STEREO FLAVOR-SURROUND BIT STREAMS*/
"../../../AudioTestStreams/AudioReferenceStreams/eddi_27.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/tmpt_28.pcm",
"../../../AudioTestStreams/AudioReferenceStreams/guit_29.pcm"
};

/******************************** Generated output files ****************************/
char *pRealAudFileOut[NUMAUDREFSTREAMS] = {
	/******* MONO *****************/
"../../../AudioTestStreams/AudioOutputStreams/chor_00.pcm",
"../../../AudioTestStreams/AudioOutputStreams/eddi_01.pcm",
"../../../AudioTestStreams/AudioOutputStreams/quar_02.pcm",
"../../../AudioTestStreams/AudioOutputStreams/cast_03.pcm",	
"../../../AudioTestStreams/AudioOutputStreams/gspi_04.pcm",	
"../../../AudioTestStreams/AudioOutputStreams/vibr_05.pcm",	
"../../../AudioTestStreams/AudioOutputStreams/tmpt_06.pcm",	
"../../../AudioTestStreams/AudioOutputStreams/soli_07.pcm",	
"../../../AudioTestStreams/AudioOutputStreams/guit_08.pcm",
"../../../AudioTestStreams/AudioOutputStreams/vibr_14.pcm",	
"../../../AudioTestStreams/AudioOutputStreams/quar_15.pcm",
"../../../AudioTestStreams/AudioOutputStreams/eddi_16.pcm",
/*********DUAL MONO ************/
"../../../AudioTestStreams/AudioOutputStreams/abba_09.pcm",
"../../../AudioTestStreams/AudioOutputStreams/quar_10.pcm",
"../../../AudioTestStreams/AudioOutputStreams/xylo_11.pcm",
"../../../AudioTestStreams/AudioOutputStreams/chor_12.pcm",
"../../../AudioTestStreams/AudioOutputStreams/abba_13.pcm",
/******** JOINT STEREO *********/
"../../../AudioTestStreams/AudioOutputStreams/tmpt_17.pcm",
"../../../AudioTestStreams/AudioOutputStreams/xylo_18.pcm",
"../../../AudioTestStreams/AudioOutputStreams/soli_19.pcm",
"../../../AudioTestStreams/AudioOutputStreams/gspi_20.pcm",
"../../../AudioTestStreams/AudioOutputStreams/guit_21.pcm",
"../../../AudioTestStreams/AudioOutputStreams/cast_22.pcm",
"../../../AudioTestStreams/AudioOutputStreams/quar_23.pcm",
"../../../AudioTestStreams/AudioOutputStreams/soli_24.pcm",
"../../../AudioTestStreams/AudioOutputStreams/abba_25.pcm",
"../../../AudioTestStreams/AudioOutputStreams/vibr_26.pcm",
/******** SURROUND STEREO ******/
"../../../AudioTestStreams/AudioOutputStreams/eddi_27.pcm",
"../../../AudioTestStreams/AudioOutputStreams/tmpt_28.pcm",
"../../../AudioTestStreams/AudioOutputStreams/guit_29.pcm"
};

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
static void ClearBuffer(void *buf, int nBytes)
{
	int i;
	unsigned char *cbuf = (unsigned char *)buf;

	for (i = 0; i < nBytes; i++)
		cbuf[i] = 0;
	return;
}


int pcmcheck(FILE *file1,FILE *file2,int BLOCK_SIZE)
{
	short inbuf1[1024], inbuf2[1024],Temp = 0;
	int i, j, nread1, nread2, totalSamples;
	int  nHist, hist[MAX_NHIST + 1];
	int diff, maxDiff;
	double totalDiff = 0.0, rms = 0.0;
	totalSamples = 0;
	maxDiff = 0;
	j = BLOCK_SIZE;
	for (i = 0; i <= MAX_NHIST; i++)
		hist[i] = 0;

	while (j == BLOCK_SIZE) 
	{
		nread1 = fread(inbuf1, 2,BLOCK_SIZE ,file1);
		nread2 = fread(inbuf2, 2, BLOCK_SIZE,file2);
		if((nread1 == 0) || (nread2 == 0))
			break;		
		if(nread1 < nread2)
			j=nread1;
		else
			j=nread2;
		for (i = 0; i < j; i++)	
		{
			if(inbuf2[i] != 0)
			{
				Temp = 0;
				Temp = (Temp | (inbuf2[i] & 0xff00)) >> 8;
		        inbuf2[i] = (inbuf2[i] << 8) | Temp;
			}		
			diff = abs(inbuf1[i] - inbuf2[i]);
			totalDiff += (double)diff;
			if (diff > maxDiff) 
			{
				maxDiff = diff;
				
			}
			if (diff < MAX_NHIST)
				hist[diff]++;
			else
			{	
				hist[MAX_NHIST]++;
				printf("\n diff greater 32\n");
			}	
			rms += ((double)diff * diff);
		}
		totalSamples += j;		
	}		
	printf("\nPeak difference ............ %d\n", maxDiff);
	fprintf(fpLogFile,"\nPeak difference ............ %d\n", maxDiff);
	printf("Average difference ......... %.6f\n", totalDiff / (float)totalSamples );
	fprintf(fpLogFile,"Average difference ......... %.6f\n", totalDiff / (float)totalSamples );
	printf("RMS power of difference .... %.6f\n", sqrt(rms/totalSamples));
	fprintf(fpLogFile,"RMS power of difference .... %.6f\n", sqrt(rms/totalSamples));
	printf("\n  Total samples %10d\n", totalSamples);
	fprintf(fpLogFile,"\n  Total samples %10d\n", totalSamples);
	nHist = (maxDiff >= MAX_NHIST ? MAX_NHIST - 1: maxDiff);
	for (i = 0; i <= nHist; i++) 
	{
		printf("  diff[%3d ]    %10d   (%.2f%c)\n", i, hist[i], (100.0 * hist[i] / totalSamples), '%');
	}		
	printf("  diff[%3d+]    %10d   (%.2f%c)\n", i, hist[i], (100.0 * hist[i] / totalSamples), '%');	
	if( (maxDiff <= 4) &&  ((sqrt(rms/totalSamples) <= 0.58)))
		return 1;
	else
		return 0;		
}

/**************************************************************************************
 * Function:    Main
 *
 * Description: Main function of the Real Audio 8 Decoder.
 *
 * Inputs:      Command Line Arugments
 *
 * Outputs:     None.
 *
 * Return:      Zero on success, or error Value
 *
 ***************************************************************************************/
int main()
{

	unsigned char ucTestCase = 0,*InputBuffer = NULL,ucTotalFilesPassed =0;
	unsigned char ucTotalFilesFailed = 0;
	short int *OutputBuffer = NULL;
	int iCodingDelay =0;
	unsigned int uiInputBufSize = 0,uiOutputBufSize = 0,uiInputDataRead =0,uiFrames =0; 
	ERRCODES eStatus;
	Gecko2Info *sGecko2Info=NULL;	

	fpLogFile = fopen("../../../TestReport/log.txt","wb");
	if(!fpLogFile)
	{		
		printf("\nError in opening output log file\n");
		return 0;
	}	
	
	printf("\n**************CONFORMANCE TEST FOR REAL AUDIO 8 G2 CODEC***************\n");	 
    fprintf(fpLogFile,"\n**************CONFORMANCE TEST FOR REAL AUDIO 8 G2 CODEC***************\n");	   
	sGecko2Info = (Gecko2Info *)&sGecko2InfoObj;

	if (!sGecko2Info)
   	{
		printf("\n Memory allocation for Gecko2Info data structure failed\n");
		fprintf(fpLogFile,"\n Memory allocation for Gecko2Info data structure failed\n");		
   		return 0;
   	}	
   	fclose(fpLogFile);
	while(ucTestCase < 30)
    {
		
		fpLogFile = fopen("../../../TestReport/log.txt","ab");
		if(!fpLogFile)
		{		
			printf("\nError in opening output log file\n");
			return 0;
		}	
		printf("\n**************CONFORMANCE TEST FOR THE FILE %s ***************\n",pbAudInpFiles[ucTestCase]);	 
		fprintf(fpLogFile,"\n**************CONFORMANCE TEST FOR THE FILE %s ***************\n",pbAudInpFiles[ucTestCase]);	
		uiFrames = 0;
		ClearBuffer(sGecko2Info, sizeof(Gecko2Info));
		sGecko2Info->sHdInfo.nSamples = gsRAParameters[ucTestCase].nSamples;
		sGecko2Info->sHdInfo.nFrameBits = gsRAParameters[ucTestCase].nFrameBits;
		sGecko2Info->sHdInfo.sampRate = gsRAParameters[ucTestCase].sampRate;
		sGecko2Info->sHdInfo.nRegions = gsRAParameters[ucTestCase].nRegions;
		sGecko2Info->sHdInfo.nChannels = gsRAParameters[ucTestCase].nChannels;
		sGecko2Info->sHdInfo.geckoMode = gsRAParameters[ucTestCase].geckoMode;
		sGecko2Info->sHdInfo.cplStart = gsRAParameters[ucTestCase].cplStart;
		sGecko2Info->sHdInfo.cplQbits = gsRAParameters[ucTestCase].cplQbits;
		sGecko2Info->sHdInfo.writeFlag = DEF_WRITEFLAG;
		sGecko2Info->sHdInfo.resampRate = 0;
		sGecko2Info->sHdInfo.lossRate = 0;
		sGecko2Info->sHdInfo.nFrameBits &= ~0x7;


		uiInputBufSize = (unsigned int)(sGecko2Info->sHdInfo.nFrameBits+7) / 8;
		uiOutputBufSize = (unsigned int)(sGecko2Info->sHdInfo.nSamples * sGecko2Info->sHdInfo.nChannels * sizeof(short));

		/* malloc buffers */
		InputBuffer = (unsigned char *)ucInputBuff;
		if(!InputBuffer)
		{
			printf("\n Memory allocation for input buffer failed\n");
			fprintf(fpLogFile,"\n Memory allocation for input buffer failed\n");			
			return 0;
		}
		OutputBuffer = (short *)isOutputBuff;
		if(!OutputBuffer)
		{
			printf("\n Memory allocation for output buffer failed\n");
			fprintf(fpLogFile,"\n Memory allocation for output buffer failed\n");			
			return 0;
		}
		sGecko2Info->sHdInfo.inBuf = InputBuffer;
		sGecko2Info->sHdInfo.outbuf = OutputBuffer;

		eStatus = Gecko2InitDecoder(sGecko2Info,&iCodingDelay);
		if(eStatus != 0)
		{		
			printf("\n Error during decoder initialization \n");
		    fprintf(fpLogFile,"\n Error during decoder initialization \n");
			return 0; 
		}	

		fpInput = fopen(pbAudInpFiles[ucTestCase],"rb");
		if(!fpInput)
        {
			printf("\nError in opening input file %s\n",pbAudInpFiles[ucTestCase]);
			fprintf(fpLogFile,"\nError in opening input file %s\n",pbAudInpFiles[ucTestCase]);
			return 0;
        }
		fpOutput = fopen(pRealAudFileOut[ucTestCase],"wb");
		if(!fpOutput)
		{
			printf("\nError in opening output file %s\n",pRealAudFileOut[ucTestCase]);
			fprintf(fpLogFile,"\nError in opening output file %s\n",pRealAudFileOut[ucTestCase]);
			return 0;
		}	

		while((uiInputDataRead = fread(sGecko2Info->sHdInfo.inBuf, 1, uiInputBufSize,fpInput)) > 0 )
		{
    		if (uiInputDataRead < uiInputBufSize)
				break;						
			eStatus = Gecko2Decode(sGecko2Info, 0);
    		if (eStatus)
				printf("\n Error during decoding\n");
			uiFrames++;
			printf("\r%d: Frames Decoded", uiFrames);
			if (sGecko2Info->sHdInfo.writeFlag) 
			{
				if (iCodingDelay > 0)	
					iCodingDelay--;
				else
					fwrite(sGecko2Info->sHdInfo.outbuf, 1, uiOutputBufSize, fpOutput);					
			}		
		}
		fclose(fpInput);
		fclose(fpOutput);

		printf("\nFile Comparison in Progress\n");
		fprintf(fpLogFile,"\nFile Comparison in Progress\n");
		fpInput = fopen(pRealAudFileOut[ucTestCase],"rb");
		if(!fpInput)
        {
			printf("\nError in opening output file %s\n",pRealAudFileOut[ucTestCase]);
			fprintf(fpLogFile,"\nError in opening output file %s\n",pRealAudFileOut[ucTestCase]);
			return 0;
		}
		fpRef   = fopen(pbAudRefFiles[ucTestCase],"rb");
		if(!fpRef)
        {
			printf("\nError in opening reference input file %s\n",pbAudRefFiles[ucTestCase]);
			fprintf(fpLogFile,"\nError in opening reference input file %s\n",pbAudRefFiles[ucTestCase]);
			return 0;
		}
        if(pcmcheck(fpInput,fpRef,gsRAParameters[ucTestCase].nSamples))	
		{	
			printf("\n**************ACCEPTANCE CRITERIA FOR THE FILE %s PASSED***************\n",
			pbAudInpFiles[ucTestCase]);	 
			fprintf(fpLogFile,"\n**************ACCEPTANCE CRITERIA FOR THE FILE %s PASSED***************\n",
			pbAudInpFiles[ucTestCase]);	 
			ucTotalFilesPassed++;
        }
		else
        {
			printf("\n**************ACCEPTANCE CRITERIA FOR THE FILE %s FAILED***************\n",
			pbAudInpFiles[ucTestCase]);	
			fprintf(fpLogFile,"\n**************ACCEPTANCE CRITERIA FOR THE FILE %s FAILED***************\n",
			pbAudInpFiles[ucTestCase]);	
			ucTotalFilesFailed++;
        }
		fclose(fpInput);
		fclose(fpRef);
		ucTestCase++;    
	    fclose(fpLogFile);
	}
	printf("\nTotal number of files tested for conformance = %d\n",ucTestCase);
	fprintf(fpLogFile,"\nTotal number of files tested for conformance = %d\n",ucTestCase);
	printf("Total number of files passed conformance test = %d\n",ucTotalFilesPassed);
    fprintf(fpLogFile,"Total number of files passed conformance test = %d\n",ucTotalFilesPassed);
	printf("Total number of files failed conformance test = %d\n",ucTotalFilesFailed);
	fprintf(fpLogFile,"Total number of files failed conformance test = %d\n",ucTotalFilesFailed);
	printf("\n**************CONFORMANCE TEST FOR REAL AUDIO 8 G2 CODEC COMPLETE***************\n");	 
	fprintf(fpLogFile,"\n**************CONFORMANCE TEST FOR REAL AUDIO 8 G2 CODEC COMPLETE***************\n");	 
	return 0;
}
	





















