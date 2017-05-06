/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		AMRNB_D_SAMPLE.c

Contains:	AMR_NB API Operator Implement Code

Written by:	Huaping Liu

Change History (most recent first):
2009-05-06		LHP			Create file

*******************************************************************************/
#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#else
#ifdef LINUX
#include <dlfcn.h>
#endif
#endif // _WIN32_WCE
#include      <stdio.h>
#include      <stdlib.h>
#include      <time.h>
#include      "voAMRNB.h"
#include      "cmnMemory.h"

#define       L_FRAME   160
#define       AMR_MAGIC_NUMBER "#!AMR\n"
#define       INPUT_SIZE   32

unsigned char  InputBuf[INPUT_SIZE ];

int  GetNextBuf(FILE* inFile, unsigned char* dst, int size)
{
	int size2 = (int)fread(dst, sizeof(signed char), size,inFile);
	return size2;
}
#if 0
static int write_wav_header(FILE* fp, int channelNum,int sampleRate)//AACDecLibParam* pFrameInfo)
{
	unsigned char header[44];
	unsigned char* p = header;
	unsigned int bytes = 2;
	float data_size = (float)bytes * 0;//pFrameInfo->outputSize;//pFrameInfo->outputSamps;//aufile->total_samples;
	unsigned long word32;
	//pFrameInfo->channelNum = 1;
	//pFrameInfo->sampleRate*=2;//pFrameInfo->channelNum;
	*p++ = 'R'; *p++ = 'I'; *p++ = 'F'; *p++ = 'F';

	word32 = 0x010cd024;//word32 = (data_size + (44 - 8) < (float)MAXWAVESIZE) ?(unsigned long)data_size + (44 - 8)  :  (unsigned long)MAXWAVESIZE;

	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	*p++ = 'W'; *p++ = 'A'; *p++ = 'V'; *p++ = 'E';

	*p++ = 'f'; *p++ = 'm'; *p++ = 't'; *p++ = ' ';

	*p++ = 0x10; *p++ = 0x00; *p++ = 0x00; *p++ = 0x00;

	if (0)//aufile->outputFormat == FAAD_FMT_FLOAT)
	{
		*p++ = 0x03; *p++ = 0x00;
	} else {
		*p++ = 0x01; *p++ = 0x00;
	}

	*p++ = (unsigned char)(channelNum >> 0);
	*p++ = (unsigned char)(channelNum >> 8);

	word32 = (unsigned long)(sampleRate);
	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	word32 = sampleRate * bytes * channelNum;
	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	word32 = bytes * channelNum;
	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);

	*p++ = (unsigned char)(16 >> 0);
	*p++ = (unsigned char)(16 >> 8);

	*p++ = 'd'; *p++ = 'a'; *p++ = 't'; *p++ = 'a';
	word32 = 0x010cd000;//data_size < MAXWAVESIZE ?(unsigned long)data_size : (unsigned long)MAXWAVESIZE;


	*p++ = (unsigned char)(word32 >>  0);
	*p++ = (unsigned char)(word32 >>  8);
	*p++ = (unsigned char)(word32 >> 16);
	*p++ = (unsigned char)(word32 >> 24);

	return (int)fwrite(header, sizeof(header), 1, fp);
}
#endif

typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv)
#elif _IOS
int beginTest_amr(char *inFile, char *outFile, int time)
#else  // for gcc compiler;
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	FILE   *fsrc = NULL;
	FILE   *fdst = NULL;
	long int  t1, t2;
	char   *infileName, *outfileName;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_HANDLE hCodec;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	int    returnCode;
	int    eofFile = 0;
	int    Relens, Ma_len, ret = 0;
	char   *inBuf = InputBuf;
	int    frameType;
	short  synth[L_FRAME];             /* Synthesis                     */
	int    frameCount = 0;
	signed char  magic[8];
	double   duration = 0.0;
	void  *handle;
	void  *pfunc;
        VOGETAUDIODECAPI pGetAPI;

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
	infileName = "/Storage Card/AMR/EXPT1OBJ_ref.amr";
	outfileName = "/Storage Card/AMR/test1_ref.pcm";
#elif _IOS
	infileName = inFile;
	outfileName = outFile;
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

	Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
	if(Relens != INPUT_SIZE)
	{
		eofFile = 1;
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);	
#ifdef LINUX
	handle = dlopen("/data/local/tmp/voAMRNBDec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}
	
	pfunc = dlsym(handle, "voGetAMRNBDecAPI");	
	if(pfunc == 0)
	{
		printf("open function error......");
		return -1;
	}

	pGetAPI = (VOGETAUDIODECAPI)pfunc;

	returnCode  = pGetAPI(&AudioAPI);
	if(returnCode)
		return -1;
#else
	returnCode = voGetAMRNBDecAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif
	//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingAMRNB, &useData);

	if(returnCode < 0)
	{
		ret = -1;
		goto safe_exit;
	}

	//The frameType param should be set first,or the decoder can not be launched correctly
	frameType = VOAMRNB_RFC3267;//it can be VOAMRNB_RFC3267,VOAMRNB_IF1,or  VOAMRNB_IF2
	returnCode = AudioAPI.SetParam(hCodec,VO_PID_AMRNB_FRAMETYPE, &frameType);

	if(returnCode != 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		ret = 3;
		goto safe_exit;
	}

	if(frameType == VOAMRNB_RFC3267)
	{
		Ma_len = strlen(AMR_MAGIC_NUMBER);
		memcpy(magic, inBuf, Ma_len);
		inBuf += Ma_len;
		Relens -= Ma_len;

		if (strncmp((const char *)magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
		{
			ret = 4; //Invalid magic number
			goto safe_exit;
		}
	}
	//#######################################    Decoding Section   #########################################

	do {
        inData.Buffer    = (unsigned char *)inBuf;
		inData.Length    = Relens;
		outData.Buffer   = (unsigned char *)synth;
		outData.Length   = 160*2;

#ifdef _WIN32_WCE
		t1 = GetTickCount();
#else
		t1 = clock();
#endif
		/* decode one amr block */
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
		t2 = clock();
		duration += t2 - t1;
#endif
		if (!eofFile) {
			Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
			inBuf = InputBuf;
			if (feof(fsrc) && Relens == 0)
				eofFile = 1;
		}

	} while (!eofFile && returnCode);

#ifdef _WIN32_WCE	
	wsprintf(msg, TEXT("Decode Time: %d clocks, Decode Frames: %d, fps: %d"), total, frameCount,(1000*frameCount)/total);
	MessageBox(NULL, msg, TEXT("AMR Decoder Finished"), MB_OK);
#else
	printf( "\n%2.5f seconds\n", (double)duration);
#endif

safe_exit:
	returnCode = AudioAPI.Uninit(hCodec);

	if(returnCode != 0)
	{
		printf("#### VOI_Error4:fail to destroy the decoder ###\n");
	}	
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);
#ifdef LINUX
	dlclose(handle);
#endif
	return ret;
}
