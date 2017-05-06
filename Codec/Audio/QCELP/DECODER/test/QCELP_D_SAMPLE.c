#ifdef _WIN32_WCE
#include <windows.h>
#include <objbase.h>
#include <Winbase.h>
#else
#ifdef LINUX
#include <dlfcn.h>
#endif
#endif // _WIN32_WCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "voQCELP.h"
#include "cmnMemory.h"


#define   L_FRAME             320                /* 160 short samples = 320 Bytes */
#define   INPUT_SIZE          4096

#ifdef NXP
#define   VO_NXP_CODEC_KEY    0x4000FFFF         /* License key ID */
#endif

unsigned char InputBuf[INPUT_SIZE];

int  GetNextBuf(FILE* inFile, unsigned char* dst, int size)
{
	int size2 = (int)fread(dst, sizeof(signed char), size,inFile);
	return size2;
}

#ifdef RVDS
__asm void init_cpu() {
// Set up CPU state
	MRC p15,0,r4,c1,c0,0
	ORR r4,r4,#0x00400000 // enable unaligned mode (U=1)
	BIC r4,r4,#0x00000002 // disable alignment faults (A=0) // MMU not enabled: no page tables
	MCR p15,0,r4,c1,c0,0
#ifdef __BIG_ENDIAN
SETEND BE
#endif
	MRC p15,0,r4,c1,c0,2 // Enable VFP access in the CAR -
	ORR r4,r4,#0x00f00000 // must be done before any VFP instructions
	MCR p15,0,r4,c1,c0,2
	MOV r4,#0x40000000 // Set EN bit in FPEXC
	MSR FPEXC,r4

	IMPORT __main
	B __main
}
#endif

typedef int (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

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
#ifdef NXP
	int   KeyValue = 1;                         /* KeyValude = 1 means disable time limitation */
#endif
	unsigned long  start=0, end=0, total=0;
	void  *handle;
	void  *pfunc;
    VOGETAUDIODECAPI pGetAPI;

#ifdef _WIN32_WCE 
	TCHAR msg[256];
	fsrc  = fopen("/Storage Card/QCELP/EXPT1OBJ1.qcelp", "rb");
	fdst  = fopen("/Storage Card/QCELP/EXPT1OBJ.QCELP.pcm", "wb");
#else	
	if(argc < 3)
	{
		printf ("QCELP Reference Decoder HELP   Displays this text\n");
		printf ("\n");
		printf ("Usage:\n");
		printf ("voQCELPDec Input_file output_file \n");
		return -1;
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

#ifdef LINUX
	handle = dlopen("/data/local/tmp/voQCELPDec.so", RTLD_NOW);
	if(handle == 0)
	{
		printf("open dll error......");
		return -1;
	}
	
	pfunc = dlsym(handle, "voGetQCELPDecAPI");	
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
	returnCode = voGetQCELPDecAPI(&AudioAPI);
	if(returnCode)
	{
		ret = -1;
		goto safe_exit;
	}
#endif
	//#######################################   Init Decoding Section   #########################################
	returnCode = AudioAPI.Init(&hCodec, VO_AUDIO_CodingQCELP13, &useData);

	if(returnCode < 0)
	{
		printf("#### VOI_Error2:fail to initialize the decoder###\n");
		ret = -1;
		goto safe_exit;
	}

#ifdef NXP
	returnCode = AudioAPI.SetParam(hCodec, VO_NXP_CODEC_KEY, &KeyValue);
#endif

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

#ifdef LINUX
	dlclose(handle);
#endif
	return 0;
}
