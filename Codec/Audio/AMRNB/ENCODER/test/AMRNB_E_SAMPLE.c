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
#include      "voAMRNB.h"
#include      "cmnMemory.h"

//Assume the input file is IF1
#define MAX_SERIAL_SIZE 244 
#define AMR_MAGIC_NUMBER "#!AMR\n"
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 6)

#define  INPUT_SIZE   102400

typedef struct{
	int  mode;
	unsigned char *input_file;
	unsigned char *output_file;
	VOAMRNBFRAMETYPE frameType;
}AMRNBDecoderParams;

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

typedef int (VO_API * VOGETAUDIOENCAPI) (VO_AUDIO_CODECAPI * pEncHandle);

//int encode(int mode, const char* srcfile, const char* dstfile)
int encode(AMRNBDecoderParams *AMRDec)
{
	unsigned char  InputBuf[INPUT_SIZE];
	unsigned char  OutputBuf[MAX_PACKED_SIZE];
	int		ret = 0;
	int     mode;
	int dtx = 1;
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
	VOAMRNBFRAMETYPE frameType;

	void  *handle;
	void  *pfunc;
	VOGETAUDIOENCAPI pGetAPI;

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
	int t1, t2 = 0;	
#else
	clock_t   start, finish;
	double    duration = 0.0;
#endif

	mode = AMRDec->mode;
	srcfile = AMRDec->input_file;
	dstfile = AMRDec->output_file;
	frameType = AMRDec->frameType;

	//short dtx = 0;  /* enable encoder DTX */	
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

#ifdef LINUX
	handle = dlopen("/data/local/tmp/voAMRNBEnc.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}

	pfunc = dlsym(handle, "voGetAMRNBEncAPI");	
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
	returnCode = voGetAMRNBEncAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif
	//#######################################   Init Encoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAMRNB, &useData);

	if(returnCode)
	{
		ret = 3;
		goto safe_exit;
	}

	Relens = GetNextBuf(fsrc,InputBuf,INPUT_SIZE);
	if(Relens != INPUT_SIZE)
	{
		eofFile = 0;
	}
	
    frameType == VOAMRNB_RFC3267;
	if(frameType == VOAMRNB_RFC3267)
	{
		/* write magic number to indicate single channel AMR file storage format */
		size1 = strlen(AMR_MAGIC_NUMBER);
		memcpy(OutBuf, AMR_MAGIC_NUMBER, size1);
		OutBuf += size1;
	}
	/* Process speech frame by frame */
	//set parameter
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AMRNB_FRAMETYPE, &frameType);
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AMRNB_MODE, &mode);
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AMRNB_DTX, &dtx);
	//#######################################   Encoding Section   #########################################
	do{
		inData.Buffer = (unsigned char *)inBuf;
		inData.Length = Relens;
		outData.Buffer = OutBuf;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		start = clock();
#endif
		/* decode one amr block */
		returnCode = AudioAPI.SetInputData(hCodec,&inData);

		do {
			returnCode = AudioAPI.GetOutputData(hCodec,&outData, &outFormat);
			if(returnCode == 0)
			{
				frameCount++;
				if(frameCount == 1)
				{
					fwrite(OutputBuf, 1, outData.Length + size1, fdst);
					fflush(fdst);	
				}
				else
				{
					fwrite(outData.Buffer, 1, outData.Length, fdst);
					fflush(fdst);
				}
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
	wsprintf(msg, TEXT("Encode Time: %d clocks, Encode frames: %d, Encode frames per s: %f"), total, frameCount, (float)frameCount*1000/total);
	MessageBox(NULL, msg, TEXT("AMR Encode Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration/CLOCKS_PER_SEC);
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

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#elif _IOS
int beginTest_amrenc(char *inFile, char *outFile, int time)
#else// _WIN32_WCE
int main(int argc, char **argv)  // for gcc compiler;
#endif//_WIN32_WCE
{
	int   mode, r;
	char   *inFileName,*outFileName;
	AMRNBDecoderParams AMRDecParams, AMRDecParams_Reentant;

#ifdef  LINUX
	pthread_t  thread1, thread2;
	int  iret1, iret2;
#endif 

#ifdef Unit_Testing
	printf("Use help: exe input_file output_file mode filetype\n ");
	printf("mode: \n 0,	/*!< 475kbps */ \n 1,    /*!< 515kbps */   \n 2,	/*!< 590kbps */");
	printf(" \n 3,	/*!< 670kbps */ \n 4,	/*!< 740kbps */ \n 5,	/*!< 795kbps */ ");
	printf("\n 6,	/*!< 10.2kbps */ \n 7,    /*!< 12.2kbps */ \n ");
	printf("filetype: \n 0, /* RFC3267*/ \n 2, /* IF1 */ \n 3, /* IF2 */ \n");

	if(argc < 5)
	{
		printf("Use help: exe input_file output_file mode filetype");
		printf("mode: \n 0,	/*!< 475kbps */ \n 1,    /*!< 515kbps */   \n 2,	/*!< 590kbps */");
		printf(" \n 3,	/*!< 670kbps */ \n 4,	/*!< 740kbps */ \n 5,	/*!< 795kbps */ ");
		printf("\n 6,	/*!< 10.2kbps */ \n 7,    /*!< 12.2kbps */ \n ");
		printf("filetype: \n 0, /* RFC3267*/ \n 2, /* IF1 */ \n 3, /* IF2 */ \n");
		exit(0);
	}

	AMRDecParams.input_file = argv[1];
	AMRDecParams.output_file = argv[2];
	AMRDecParams.mode = atoi(*(&argv[3]));
	AMRDecParams.frameType = atoi(*(&argv[4]));

#elif _IOS
	AMRDecParams.mode = VOAMRNB_MD122;
	AMRDecParams.input_file = inFile;
	AMRDecParams.output_file = outFile;
	AMRDecParams.frameType = VOAMRNB_RFC3267;
#else
	AMRDecParams.mode = VOAMRNB_MD122;
	AMRDecParams.input_file = argv[1];
	AMRDecParams.output_file = argv[2];
	AMRDecParams.frameType = VOAMRNB_RFC3267;
#endif

#ifdef  LINUX
	AMRDecParams_Reentant.mode = VOAMRNB_MD122;
	AMRDecParams_Reentant.input_file = argv[3];
	AMRDecParams_Reentant.output_file = argv[4];
	//re-entrancy test: Greate independant threads each of which will execute the decoder function

	iret1 = pthread_create( &thread1, NULL, (void*) encode, (void*) &AMRDecParams);
	iret2 = pthread_create( &thread2, NULL, (void*) encode, (void*) &AMRDecParams_Reentant);

	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL);

#else

#ifdef _WIN32_WCE
	AMRDecParams.mode  = VOAMRNB_MD122;
	AMRDecParams.input_file  = "/Storage Card/AMR/EXPT1OBJ_ref.pcm";
	AMRDecParams.output_file = "/Storage Card/AMR/amrenctest.amr";
#endif

	r = encode(&AMRDecParams);
	if(r)
	{
		fprintf(stderr, "error: %d\n", r);
	}

#endif
	return 0;
}
