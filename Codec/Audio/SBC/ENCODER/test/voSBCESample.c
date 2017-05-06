//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    voSBCEncSample.cpp

Abstract:

    caller for SBC encoder.

Author:

    Witten Wen 17-December-2009

Revision History:

*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#ifndef LINUX
#include <windows.h>
#endif	//LINUX

#ifdef LINUX
#include <dlfcn.h>
#endif

#include "cmnmemory.h"
#include "voSBC.h"
#include "sbctypes.h"
#include "sbc.h"

VO_AUDIO_FORMAT voauFormat;
#define OUTBUFFERSIZE	(1<<14)
#define INSIZE	(0x478)

//#define MEMORYSIZE
#ifdef MEMORYSIZE
extern int memorysize;
#endif

#define MAXSRCSIZE	1024*16

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef LINUX
void main( int argc, char** argv )
#else
void wmain(int argc, char *argv[])
#endif
{		
	FILE *hpcm, *hsbc;
	int filesize, insize, outsize, maxoutsize, famesize, consumed=0, returnCode, FrameCount=0;
	char *pFile, *pSrc;//, *pEnd;
	VO_U8 *OutBuffer;

	/* VisualOn define */
	VO_AUDIO_FORMAT voauFormat;
	VO_CODEC_INIT_USERDATA userData;
	VO_AUDIO_CODECAPI pEncHandle;
	VO_CODECBUFFER inData, OutData;
	VO_MEM_OPERATOR *pSBCMemOP = NULL;
	VO_HANDLE hCodec = 0;
	VO_AUDIO_OUTPUTINFO pOutInfo;
	int		t1,t2, FPs;
	int		total = 0;
	double	performance;
#ifndef LINUX
	TCHAR msg[256];
#endif

#ifdef LINUX
	void  *handle;
	void  *pfunc;
	VOGETAUDIOENCAPI pGetAPI;
#endif

#ifndef LINUX
#ifdef _WIN32_WCE
	char *infile = "\\wmp\\input_01.wav";
	char *outfile = "\\wmp\\out.sbc";
#else
	//char *infile = ".\\test1_2c_44Khz.wav";
	//char *outfile = ".\\out.sbc";
#endif
	if((hpcm = fopen(argv[1], "rb"))==NULL)
		goto FailOut;
#else
	if(argc == 1 || argv[1] == NULL)
		goto FailOut;
	if((hpcm = fopen(argv[1], "rb"))==NULL)
		goto FailOut;
#endif
	
	fseek(hpcm, 0, SEEK_END);
	filesize = ftell(hpcm);
	fseek(hpcm, 0, SEEK_SET);
	if((pFile = (char *)malloc(MAXSRCSIZE))==NULL)
		goto FailOut;
//	if(fread(pFile, 1, filesize, hpcm) != filesize)
//	{
//		fclose(hpcm);
//		goto FailOut;
//	}
//	fclose(hpcm);
//	pEnd = pFile + filesize;
	pSrc = pFile;
	if(fread(pFile, 1, 44, hpcm) != 44)
	{
		goto FailOut;
	}
	voauFormat.SampleRate	= ((unsigned int *)pSrc)[6];
	voauFormat.Channels		= ((unsigned short *)pSrc)[11];
	voauFormat.SampleBits = ((unsigned short *)pSrc)[17];
	if(voauFormat.SampleBits != 16)
		goto FailOut;
	filesize -= 44;//raw data
	insize = filesize;

	cmnMemFillPointer(VO_AUDIO_CodingSBC);
	pSBCMemOP = &g_memOP;

//	psbcenc = (sbc_t *)malloc(sizeof(sbc_t));
//	pDst =(char *)malloc(OUTBUFFERSIZE);

	userData.memflag = VO_IMF_USERMEMOPERATOR;
	userData.memData = pSBCMemOP;

#ifdef LINUX
	handle = dlopen("/data/local/tmp/libvoSBCEnc.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetSBCEncAPI");	
	if(pfunc == 0)
	{
		printf("open function error......");
		return -1;
	}

	pGetAPI = (VOGETAUDIOENCAPI)pfunc;

	returnCode  = pGetAPI(&pEncHandle);
	if(returnCode)
		return -1;
#else
	returnCode = voGetSBCEncAPI(&pEncHandle);
#endif

	if(pEncHandle.Init(&hCodec, VO_AUDIO_CodingSBC, &userData)!=VO_ERR_NONE)
		goto FailOut;

	if(pEncHandle.SetParam(hCodec, VO_PID_AUDIO_FORMAT, &voauFormat)!=VO_ERR_NONE)
		goto FailOut;
	if(pEncHandle.GetParam(hCodec, VO_PID_SBC_INPUTSIZE, &famesize)!=VO_ERR_NONE)
		goto FailOut;
//	if(psbcenc->SetInput(&config) !=TPVSBCENC_SUCCESS)
//		goto FailOut;
	if(pEncHandle.GetParam(hCodec, VO_PID_SBC_MAXOUTSIZE, &maxoutsize)!=VO_ERR_NONE)
		goto FailOut;
	OutBuffer = (VO_U8 *)malloc(maxoutsize);

//	insize = famesize;

	inData.Buffer = (VO_PBYTE)pSrc;
	if(fread(inData.Buffer, 1, INSIZE, hpcm) != INSIZE)
	{
		goto FailOut;
	}
	filesize -= INSIZE;
	inData.Length = INSIZE;
	OutData.Buffer	= OutBuffer;

#ifndef LINUX
	if((hsbc = fopen(argv[2], "wb"))<=0)
		goto FailOut;
#else
	if((hsbc = fopen(argv[2], "wb"))<=0)
		goto FailOut;
#endif
	while(1)
	{		
		if(pEncHandle.SetInputData(hCodec, &inData)!=VO_ERR_NONE)
			goto FailOut;

		while(1)
		{
#ifndef LINUX
			t1 = GetTickCount();
//			t1 = clock();
#endif	
			returnCode = pEncHandle.GetOutputData(hCodec, &OutData, &pOutInfo);
#ifndef LINUX
			t2 = GetTickCount();
//			t2 = clock();
			total += t2 - t1;
//			printf("total: %dms,\n", total);
#endif
			if(returnCode == VO_ERR_NONE)
			{
				FrameCount++;
				fwrite(OutData.Buffer, 1, OutData.Length, hsbc);
			}
			else if(returnCode == VO_ERR_INPUT_BUFFER_SMALL)
				break;
			else 
				goto FailOut;
		}

//		inData.Buffer += inData.Length;
//		if((int)inData.Buffer + INSIZE <= (int)pEnd)
//			inData.Length = INSIZE;
//		else
//		{
//			if((int)inData.Buffer < (int)pEnd)
//				inData.Length = (int)pEnd - (int)inData.Buffer;
//			else
//				break;
//		}
		if(filesize >= INSIZE)
		{
			if(fread(inData.Buffer, 1, INSIZE, hpcm) != INSIZE)
			{
				goto FailOut;
			}
			inData.Length = INSIZE;
			filesize-=INSIZE;
		}
		else 
		{
			if(filesize >0)
			{
				if(fread(inData.Buffer, 1, filesize, hpcm) != filesize)
				{
					goto FailOut;
				}
				inData.Length = filesize;
				filesize = 0;
			}
			else
				break;
		}		
	}
#ifndef LINUX
	performance = 1.0*total*voauFormat.SampleRate*4/insize;//MHz = cpu_freq(MHz) * (total/1000) / ((datalength/4)/samplerate)
	FPs = (int)(FrameCount*1000.0/total);
//	wsprintf(msg, TEXT("Decode Time: %d clocks"), total);
	wsprintf(msg, TEXT("Performance: %f Mhz/s, %d frames, %d F/S"), performance, FrameCount, FPs);
	MessageBox(NULL, msg, TEXT("SBC Encode Finished"), MB_OK);
#else
	printf("encoding is finished!\n");
#endif
FailOut:
	pEncHandle.Uninit(hCodec);
#ifdef MEMORYSIZE
	printf("the size of RAM is: %d,\n", memorysize);
#endif
	if(hpcm != NULL)
		fclose(hpcm);
	if(hsbc != NULL)
		fclose(hsbc);
	if(pFile)
		free(pFile);
	if(OutBuffer)
		free(OutBuffer);
#ifdef LINUX
	dlclose(handle);
#endif
//	if(psbcenc)
//		free(psbcenc);
}