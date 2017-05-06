/************************************************************************
*                                                                       *
*		VisualOn, Inc. Confidential and Proprietary, 2009       *
*		                                                        *			
*************************************************************************/
#ifdef _WIN32_WCE
#include     <windows.h>
#include     <objbase.h>
#include     <Winbase.h>
#else
#ifdef LINUX
#include <dlfcn.h>
#endif
#endif // _WIN32_WCE

#ifdef LINUX
#include <pthread.h>
#endif


#include     <stdio.h>
#include     <stdlib.h>
#include     <string.h>
#include     <time.h>
#include      "cmnMemory.h"
#include       "voAudioSpeed.h"

//Assume the input file is IF1
#define MAX_OUTFRAME_SIZE (1024*100)  //sample_count*sample_size

#define  INPUT_SIZE   1024*5

typedef struct{
	unsigned char *input_file;
	unsigned char *output_file;
}voxPlayerInParams;

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pEncHandle);
int process(voxPlayerInParams *pInParams)
{
	float tempoRatio=20.0f;
	float rateRatio= 0.0f;
	float pitchRatio=0.0f;
	int   nBitsSample = 16;

	unsigned char  InputBuf[INPUT_SIZE];
	unsigned char  OutputBuf[MAX_OUTFRAME_SIZE];
	int		ret = 0;
	void	*hEnc = NULL;
	FILE	*fsrc = NULL;
	FILE	*fdst = NULL;
	unsigned char *srcfile;
	unsigned char *dstfile;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	unsigned char *inBuf = InputBuf;
	unsigned char *OutBuf = OutputBuf;
	int  returnCode;
	int  Relens, size1=0;
	int  eofFile =0;
	int  frameCount = 0;

	unsigned int channels;
	unsigned int sampleRate;

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
	int t1, t2 = 0;	
#else
	clock_t   start, finish;
	double    duration = 0.0;
#endif


	srcfile = pInParams->input_file;
	dstfile = pInParams->output_file;


	//short dtx = 0;  /* enable processr DTX */	
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

#if 0 //def LINUX
	handle = dlopen("/data/local/tmp/libvoAudioSpeed.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetAudioSpeedAPI");	
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
	returnCode = voGetAudioSpeedAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif
	//#######################################   Init Encoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_AudioSpeed, &useData);

	if(returnCode)
	{
		ret = 3;
		goto safe_exit;
	}

	channels = 2;
	sampleRate = 44100;

	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AUDIO_CHANNELS, &channels);
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AUDIO_SAMPLEREATE, &sampleRate);
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AUDIOSPEED_BITS, &nBitsSample);

	Relens = GetNextBuf(fsrc,InputBuf,INPUT_SIZE);
	if(Relens != INPUT_SIZE)
	{
		eofFile = 0;
	}

	/* Process speech frame by frame */
	//set parameter
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AUDIOSPEED_RATE, &tempoRatio);
	//#######################################   Processing Section   #########################################
	do{
		inData.Buffer = (unsigned char *)inBuf;
		inData.Length = Relens;
		outData.Buffer = OutBuf;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		start = clock();
#endif
		/* process one  block */
		returnCode = AudioAPI.SetInputData(hCodec,&inData);
		do {
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);
			if(returnCode == 0)
			{
				frameCount++;
				fwrite(outData.Buffer, 1, outData.Length, fdst);
				fflush(fdst);
			}
		} while(returnCode != VO_ERR_INPUT_BUFFER_SMALL);

#ifdef _WIN32_WCE
		t2 = GetTickCount();
		total += t2 - t1;
#else
		finish = clock();
		duration += finish - start;
#endif
		if (!eofFile) {
			Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
			inBuf = InputBuf;
			if (feof(fsrc) && Relens == 0)
				eofFile = 1;
		}
	} while (!eofFile && returnCode);
	//#######################################   End Encoding Section   #########################################
safe_exit:
	returnCode = AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE
	wsprintf(msg, TEXT("process Time: %d clocks, process frames: %d, process frames per s: %f"), total, frameCount, (float)frameCount*1000/total);
	MessageBox(NULL, msg, TEXT("AMR process Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration/CLOCKS_PER_SEC);
#endif
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);

#if 0 //def LINUX
	dlclose(handle);
#endif
	return ret;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#elif _IOS
int beginTest_amrenc(char *inFile, char *outFile, int time)
#else// _WIN32_WCE
int main(int argc, char **argv)  // for gcc compiler;
#endif//_WIN32_WCE
{
	int  r;
	voxPlayerInParams pInParams;

#if 0 //def  LINUX
	pthread_t  thread1, thread2;
	int  iret1, iret2;
#endif 


	pInParams.input_file = argv[1];
	pInParams.output_file = argv[2];

	printf ("inputfile name = %s\n", pInParams.input_file);
	printf ("outfile name = %s\n\n", pInParams.output_file);

	r = process(&pInParams);
	if(r)
	{
		fprintf(stderr, "error: %d\n", r); 
	}
	return 0;
}
