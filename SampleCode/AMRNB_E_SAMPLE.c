/************************************************************************
*                                                                       *
*		VisualOn, Inc. Confidential and Proprietary, 2009       *
*		                                                        *			
*************************************************************************/
#ifdef _WIN32_WCE
#include     <windows.h>
#include     <objbase.h>
#include     <Winbase.h>
#endif // _WIN32_WCE
#include     <stdio.h>
#include     <stdlib.h>
#include     <string.h>
#include     <time.h>
#include      "voAMRNB.h"
#include      "cmnMemory.h"

#define L_FRAME 160
//Assume the input file is IF1
#define MAX_SERIAL_SIZE 244 
#define AMR_MAGIC_NUMBER "#!AMR\n"
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 6)

#define  INPUT_SIZE   4096
unsigned char  InputBuf[INPUT_SIZE];
unsigned char  OutputBuf[MAX_PACKED_SIZE];

int  GetNextBuf(FILE* inFile,unsigned char* dst,int size)
{
	int size2 = fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

int encode(int mode, const char* srcfile, const char* dstfile)
{
	int		ret = 0;
	void		*hEnc = NULL;
	FILE		*fsrc = NULL;
	FILE		*fdst = NULL;
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
	int  Relens, size1;
	int  eofFile =0;
	int  frameCount = 0;
	short  allow_dtx = 0;                       // 0 --- disable VAD, 1 --- enable VAD
	VOAMRNBFRAMETYPE frameType = VOAMRNB_RFC3267;

#ifdef _WIN32_WCE
	DWORD total = 0;
	TCHAR msg[256];
	int t1, t2 = 0;	
#else
	clock_t   start, finish;
	double    duration = 0.0;
#endif
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

    returnCode = voGetAMRNBEncAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}

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
	returnCode = AudioAPI.SetParam(hCodec, VO_PID_AMRNB_DTX,&allow_dtx);
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
	printf( "\n%2.5f seconds\n", (double)duration);
#endif
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);
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
	mode = VOAMRNB_MD122;
	inFileName  = "/Storage Card/AMR/EXPT1OBJ_ref.pcm";
	outFileName = "/Storage Card/AMR/amrenctest.amr";
#else
	if (argc != 3)
	{
		fprintf(stderr, "\nUsage: %s mode srcfile.pcm dstfile.amr\n", argv[0]);
		return -1;
	}
	mode = VOAMRNB_MD122;
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
