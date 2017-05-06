/************************************************************************
*									*
*	VisualOn, Inc. Confidential and Proprietary, 2003~2012		*			
*									*									
************************************************************************/
#ifdef _WIN32_WCE
#include 	<windows.h>
#include 	<objbase.h>
#include 	<Winbase.h>
#endif // _WIN32_WCE
#ifndef LINUX
#include 	<windows.h>
#endif
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

#define  INPUT_SIZE   1024
UWord8  InputBuf[INPUT_SIZE];
UWord8  OutputBuf[INPUT_SIZE];


int  GetNextBuf(FILE* inFile,UWord8* dst,int size)
{
	int size2 = fread(dst, sizeof(Word8), size,inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
// for gcc compiler;
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	char *infileName, *outfileName;
	int  size1,ret = 0;
	char* inBuf = InputBuf;
	int endDecode = 0;
	int lastTime = 0;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;

	FILE *fsrc = NULL;
	FILE *fdst = NULL;
	Word32 frame = 0;
	int  first = 1;
	int  returnCode;
	int  InputLen = 0;
	int  endFile = 0; 
	int  mode = 0;                       

#ifdef LINUX
	void  *handle;
	void  *pfunc;
	VOGETAUDIOENCAPI pGetAPI;
#endif

#ifdef _WIN32_WCE
	TCHAR msg[256];
	int t1, t2 = 0;	
#else
	clock_t   start, finish;
	double    duration = 0.0;
#endif

#ifdef _WIN32_WCE
	DWORD t1, t2;
	DWORD total = 0;
	TCHAR msg[256];

	infileName = "/Storage Card/G711/g711_1_enc_out_ulaw.bit";
	outfileName = "/Storage Card/G711/g711_1_dec1.out";
#else//_WIN32_WCE
	if (argc != 3)
	{
		fprintf (stderr, " Usage:\n\n   %s  src.amr dst.pcm\n\n", argv[0]);
		exit (1);
	}
	infileName = argv[1];
	outfileName = argv[2];
#endif//_WIN32_WCE

	if ((fsrc = fopen (infileName, "rb")) == NULL)
	{
		ret = 1;
		goto safe_exit;
	}

	if ((fdst = fopen (outfileName, "wb")) == NULL)
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

	InputLen = INPUT_SIZE;

#ifdef LINUX
	handle = dlopen("/data/local/tmp/libvoG711Dec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetG711DecAPI");	
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
	returnCode = voGetG711DecAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif

	/* Init G711 encoder */
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingG711, &useData);	
	if(returnCode)
	{
		ret = 3;
		goto safe_exit;
	}

	/* Set G711 Parameters */
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_G711_MODE, &mode);

	if (returnCode)
	{
		ret = 3;
		goto safe_exit;
	}


	size1 = GetNextBuf(fsrc, InputBuf, InputLen);
	if(size1 != InputLen && !feof(fsrc))
	{
		ret = 5; //Invalid magic number
		goto safe_exit;
	}

	do 
	{
		inData.Buffer = (unsigned char *)InputBuf;
		inData.Length = InputLen;
		outData.Buffer = OutputBuf;
#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		start = clock();
#endif
		/* Decoder one G711 fame block */
		returnCode = AudioAPI.SetInputData(hCodec,&inData); 
		returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);
#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		finish = clock();
		duration += finish - start;
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

safe_exit:
	returnCode = AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d, fps: %d"), total, frame, frame*1000/total);
	MessageBox(NULL, msg, TEXT("G711 Decoder Finished"), MB_OK);
#endif

	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);

#ifdef LINUX
	dlclose(handle);
#endif
	return ret;
}
