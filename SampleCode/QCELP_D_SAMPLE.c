#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#endif // _WIN32_WCE

#include <stdio.h>
#include <stdlib.h>
#include "voQCELP.h"
#include "cmnMemory.h"

#define   L_FRAME       320      //160 short samples = 320 Bytes
#define   INPUT_SIZE    4096
unsigned char InputBuf[INPUT_SIZE];

int  GetNextBuf(FILE* inFile, unsigned char* dst, int size)
{
	int size2 = (int)fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

#ifdef _WIN32_WCE
int _tmain(int argc, TCHAR **argv) 
#else // _WIN32_WCE
int main(int argc, char **argv)
#endif//_WIN32_WCE
{
	FILE *fsrc = NULL;
	FILE *fdst = NULL;
	VO_HANDLE hCodec;
	VO_AUDIO_CODECAPI AudioAPI;
	VO_MEM_OPERATOR moper;
	VO_CODEC_INIT_USERDATA useData;
	VO_CODECBUFFER inData;
	VO_CODECBUFFER outData;
	VO_AUDIO_OUTPUTINFO outFormat;
	int   Relens = 0;
	short synth[L_FRAME];
	int   frameCount = 0;
	int   eofFile = 0;
	int   returnCode, ret;
	char  *inBuf = InputBuf;
	unsigned long  start=0, end=0, total=0;

#ifdef _WIN32_WCE 
	TCHAR msg[256];
	fsrc  = fopen("/Storage Card/QCELP/EXPT1OBJ.QCELP", "rb");
	fdst  = fopen("/Storage Card/QCELP/EXPT1OBJ.QCELP.pcm", "wb");
#else	
	if (argc !=3)
	{
		printf("parameters invalid\n");
		exit(17);
	}   

	fsrc  = fopen(argv[1], "rb");
	if (NULL == fsrc)
	{
		printf("open input file error!\n");
		exit(11);
	}

	fdst = fopen(argv[2], "wb");
	if (NULL == fdst)
	{
		printf("open output file error!\n");
		exit(12);
	}
#endif   

	Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
	if(Relens != INPUT_SIZE)
	{
		eofFile = 0;
	}

	moper.Alloc = cmnMemAlloc;
	moper.Copy = cmnMemCopy;
	moper.Free = cmnMemFree;
	moper.Set = cmnMemSet;
	moper.Check = cmnMemCheck;

	useData.memflag = VO_IMF_USERMEMOPERATOR;
	useData.memData = (VO_PTR)(&moper);	

	returnCode = voGetQCELPDecAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}

	//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingQCELP13, &useData);

	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		ret = -1;
		goto safe_exit;
	}

	do {
		inData.Buffer    = (unsigned char *)inBuf;
		inData.Length    = Relens;
		outData.Buffer   = (unsigned char *)synth;
		outData.Length   = 160*2;

#ifdef _WIN32_WCE
		start = GetTickCount();
#else
		start = clock();
#endif
		/* decode one QCELP block */
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
		end = GetTickCount();
		total += end - start;
#else
		end = clock();
		total += end - start;
#endif
		if (!eofFile) {
			Relens = GetNextBuf(fsrc, InputBuf, INPUT_SIZE);
			inBuf = InputBuf;
			if (feof(fsrc) && Relens == 0)
				eofFile = 1;
		}

	} while (!eofFile && returnCode);

safe_exit:
	AudioAPI.Uninit(hCodec);

#ifdef _WIN32_WCE	
	wsprintf(msg, TEXT("Decode Time: %d clocks"), total);
	MessageBox(NULL, msg, TEXT("QCELP Decode Finished"), MB_OK);
#else
	printf("Used Time: %10d\n", total);
#endif
	fclose(fsrc);
	fclose(fdst);

	return 0;
}
