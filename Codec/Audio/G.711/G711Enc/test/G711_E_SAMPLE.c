/*
*  Copyright 2008 by Visualon software Incorporated.
*  All rights reserved. Property of Visualon software Incorporated.
*  Restricted rights to use, duplicate or disclose this code are
*  granted through contract.
*  
*/

/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    11-25-2008        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#ifdef _WIN32_WCE
#include 	<windows.h>
#include 	<objbase.h>
#include 	<Winbase.h>
#endif // _WIN32_WCE
#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#ifdef LINUX
#include <dlfcn.h>
#endif
#include     "voG711.h"
#include     "cmnMemory.h"

typedef unsigned char UWord8;
typedef signed char Word8;
typedef unsigned short UWord16;
typedef short Word16;
typedef unsigned int UWord32;
typedef int Word32;

#ifdef VOANDROID
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"
static unsigned long timeGetTime(){
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
}
#endif

#define INPUT_SIZE   2048
#define OUTPUT_SIZE  2048
UWord8  InputBuf[INPUT_SIZE];
UWord8  OutputBuf[OUTPUT_SIZE];

int  GetNextBuf(FILE* inFile, UWord8* dst, int size)
{
	int size2 = fread(dst, sizeof(Word8), size, inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pDecHandle);

int encode(int mode, const char* srcfile, const char* dstfile)
{
	int			ret = 0;
	void		*hEnc = NULL;
	FILE		*fsrc = NULL;
	FILE		*fdst = NULL;
	int			endEncode=0;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	unsigned char *inBuf = InputBuf;
	unsigned char *OutBuf = OutputBuf;

	int  samplerate = 1;               // sample code set audio sample rate is 7KHz
	int  returnCode;
	int  InputLen = 1024;
	int  endFile = 0; 
	int  size1;

#ifdef LINUX
	void  *handle;
	void  *pfunc;
	VOGETAUDIOENCAPI pGetAPI;
#endif

#ifdef _WIN32_WCE
	TCHAR msg[256];
	int t1, t2 = 0;	
#else
	unsigned long   start, finish;
	double   duration;
#endif
	if ((fsrc = fopen (srcfile, "rb")) == NULL)
	{
		ret = 1;
		goto safe_exit;
	}

	if ((fdst = fopen (dstfile, "wb")) == NULL)
	{
		ret = 2;
		goto safe_exit;
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;
	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);

	//#######################################   Init Encoding Section   #########################################
#ifdef LINUX
	handle = dlopen("/data/local/tmp/libvoG711Enc.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetG711EncAPI");	
	if(pfunc == 0)
	{
		printf("open function error......");
		return -1;
	}

	pGetAPI = (VOGETAUDIOENCAPI)pfunc;

	returnCode  = pGetAPI(&AudioAPI);
	if(returnCode)
		return -1;
#else
	returnCode = voGetG711EncAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif
	/*  Init G711 encoder */
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingG711, &useData);	
	if(returnCode)
	{
		ret = 3;
		goto safe_exit;
	}


	/* Set Codec bitrate */
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_G711_MODE, &mode);
	if (returnCode)
	{
		ret = 3;
		goto safe_exit;
	}

	size1 = GetNextBuf(fsrc,InputBuf,InputLen);

	if(size1 != InputLen && !feof(fsrc))
	{
		ret = 5; //Invalid magic number
		goto safe_exit;
	}


	//#######################################   Encoding Section   #########################################
	do 
	{
		inData.Buffer = (unsigned char *)InputBuf;
		inData.Length = InputLen;
		outData.Buffer = OutputBuf;
#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		start = timeGetTime();
#endif
		/* Encoder one G711 fame block */
		returnCode = AudioAPI.SetInputData(hCodec,&inData); 
		returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		finish = timeGetTime();
		duration += (double)(finish - start);
#endif
		if(returnCode==0)
		{	
			fwrite(outData.Buffer, 1, outData.Length, fdst);
		}

		size1 = GetNextBuf(fsrc,InputBuf,InputLen);
		if(size1 != InputLen)
		{
			endFile = 1;
		}
	} while (!endFile);

	//#######################################   End Encoding Section   #########################################

safe_exit:
	returnCode = AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Encode Time: %d clocks, Encode frames: %d"), t2, frame);
	MessageBox(NULL, msg, TEXT("AMR Encode Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)(duration/1000));
#endif
	//fclose(log_file);
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);
#ifdef LINUX
	dlclose(handle);
#endif

	return ret;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)  // for gcc compiler;
#endif//_WIN32_WCE
{
	int   mode, r;
	char   *inFileName,*outFileName;

#ifdef _WIN32_WCE
	mode = VOG711_MDMU_LAW;
	inFileName  = "/Storage Card/G711/g711_1_enc_in.pcm";
	outFileName = "/Storage Card/G711/g711_1_enc_in1.out";
#else
	if (argc != 3)
	{
		fprintf(stderr, "\nUsage: %s mode srcfile.pcm dstfile.amr\n", argv[0]);
		return -1;
	}
	mode = VOG711_MDMU_LAW;
	inFileName  = argv[1];
	outFileName = argv[2];
#endif
	r = encode(mode, inFileName, outFileName);
	if(r)
	{
		fprintf(stderr, "error: %d\n", r);
	}
	return r;
}

